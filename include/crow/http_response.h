#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <ios>
#include <fstream>
#include <sstream>

#include "crow/json.h"
#include "crow/http_request.h"
#include "crow/ci_map.h"

#include "crow/socket_adaptors.h"
#include "crow/logging.h"
#include "crow/mime_types.h"
#if !defined(_WIN32)
#include <sys/stat.h>
#endif

namespace crow
{
    template <typename Adaptor, typename Handler, typename ... Middlewares>
    class Connection;
    struct response
    {
        template <typename Adaptor, typename Handler, typename ... Middlewares>
        friend class crow::Connection;

        int code{200};
        std::string body;
        std::vector<char> bytes;
        json::wvalue json_value;

        // `headers' stores HTTP headers.
        ci_map headers;

        void set_header(std::string key, std::string value)
        {
            headers.erase(key);
            headers.emplace(std::move(key), std::move(value));
        }
        void add_header(std::string key, std::string value)
        {
            headers.emplace(std::move(key), std::move(value));
        }

        const std::string& get_header_value(const std::string& key)
        {
            return crow::get_header_value(headers, key);
        }


        response() {}
        explicit response(int code) : code(code) {}
        response(int code, std::vector<char> &data) : code(code), bytes(std::move(data)) {}
        response(std::vector<char> &data) : bytes(std::move(data)) {}
        response(std::string body) : body(std::move(body)) {}
        response(json::wvalue&& json_value) : json_value(std::move(json_value))
        {
            json_mode();
        }
        response(int code, std::string body) : code(code), body(std::move(body)) {}
        response(const json::wvalue& json_value) : body(json::dump(json_value))
        {
            json_mode();
        }
        response(int code, const json::wvalue& json_value) : code(code), body(json::dump(json_value))
        {
            json_mode();
        }

        response(response&& r)
        {
            *this = std::move(r);
        }

        response& operator = (const response& r) = delete;

        response& operator = (response&& r) noexcept
        {
            body = std::move(r.body);
            bytes = std::move(r.bytes);
            json_value = std::move(r.json_value);
            code = r.code;
            headers = std::move(r.headers);
            completed_ = r.completed_;
            return *this;
        }

        bool is_completed() const noexcept
        {
            return completed_;
        }

        void clear()
        {
            body.clear();
            json_value.clear();
            bytes.clear();
            code = 200;
            headers.clear();
            completed_ = false;
        }

        void redirect(const std::string& location)
        {
            code = 301;
            set_header("Location", location);
        }

        void write(const std::vector<char>& byte_part)
        {
            bytes.insert(std::end(bytes), std::begin(byte_part), std::end(byte_part));
        }

        void write(const std::string& body_part)
        {
            body += body_part;
        }

        void end()
        {
            if (!completed_)
            {
                completed_ = true;

                if (complete_request_handler_)
                {
                    complete_request_handler_();
                }
            }
        }

        void end(const std::string& body_part)
        {
            body += body_part;
            end();
        }

        bool is_alive()
        {
            return is_alive_helper_ && is_alive_helper_();
        }
 /* adding static file support here
  * middlware must call res.set_static_file_info(filename)
  * you must add route starting with /your/restricted/path/<string>
  */
#if !defined(_WIN32)
        struct static_file_info{
            std::string path = "";
            struct stat statbuf;
            int statResult;
        };
        static_file_info file_info;

        ///Return a static file as the response body
        void set_static_file_info(std::string path){
            file_info.path = path;
            file_info.statResult = stat(file_info.path.c_str(), &file_info.statbuf);
            if (file_info.statResult == 0)
            {
                std::size_t last_dot = path.find_last_of(".");
                std::string extension = path.substr(last_dot+1);
                std::string mimeType = "";
                code = 200;
                this->add_header("Content-length", std::to_string(file_info.statbuf.st_size));

                if (extension != ""){
                    mimeType = mime_types[extension];
                    if (mimeType != "")
                        this-> add_header("Content-Type", mimeType);
                    else
                        this-> add_header("content-Type", "text/plain");
                }
            }
            else
            {
                code = 404;
                this->end();
            }
        }

        template<typename Adaptor>
        void do_stream_file(Adaptor& adaptor)
        {
            if (file_info.statResult == 0)
            {
                std::ifstream is(file_info.path.c_str(), std::ios::in | std::ios::binary);
                write_streamed(is, adaptor);
            }
        }

        template<typename Adaptor>
        void do_stream_body(Adaptor& adaptor)
        {
            if (body.length() > 0)
            {
                std::istringstream is(body);
                write_streamed(is, adaptor);
            }
        }
#endif
/* static file support end */
        private:
            bool completed_{};
            std::function<void()> complete_request_handler_;
            std::function<bool()> is_alive_helper_;

            //In case of a JSON object, set the Content-Type header
            void json_mode()
            {
                set_header("Content-Type", "application/json");
            }

            template<typename Stream, typename Adaptor>
            void write_streamed(Stream& is, Adaptor& adaptor)
            {
                char buf[16384];
                while (is.read(buf, sizeof(buf)).gcount() > 0)
                {
                    std::vector<asio::const_buffer> buffers;
                    buffers.push_back(boost::asio::buffer(buf));
                    boost::asio::write(adaptor.socket(), buffers, [this](std::error_code ec, std::size_t)
                    {
                        if (!ec)
                        {
                            return false;
                        }
                        else
                        {
                            CROW_LOG_ERROR << ec << " - happened while sending body";
                            this->end();
                            return true;
                        }
                    });
                }
            }

    };
}
