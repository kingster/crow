#include "gtest/gtest.h"

#include "crow/common.h"
using namespace crow;

#include <string>

TEST(common, method_name) {
  EXPECT_EQ(method_name(HTTPMethod::Delete), "DELETE");
  EXPECT_EQ(method_name(HTTPMethod::Get), "GET");
  EXPECT_EQ(method_name(HTTPMethod::Head), "HEAD");
  EXPECT_EQ(method_name(HTTPMethod::Post), "POST");
  EXPECT_EQ(method_name(HTTPMethod::Put), "PUT");
  EXPECT_EQ(method_name(HTTPMethod::Connect), "CONNECT");
  EXPECT_EQ(method_name(HTTPMethod::Options), "OPTIONS");
  EXPECT_EQ(method_name(HTTPMethod::Trace), "TRACE");
  EXPECT_EQ(method_name(HTTPMethod::Patch), "PATCH");
  EXPECT_EQ(method_name(HTTPMethod::Purge), "PURGE");

  EXPECT_EQ(method_name(static_cast<HTTPMethod>(999)), "invalid");
}

#ifndef CROW_MSVC_WORKAROUND
TEST(common, methodStringLiteral) {
  EXPECT_EQ("DELETE"_method, HTTPMethod::Delete);
  EXPECT_EQ("GET"_method, HTTPMethod::Get);
  EXPECT_EQ("HEAD"_method, HTTPMethod::Head);
  EXPECT_EQ("POST"_method, HTTPMethod::Post);
  EXPECT_EQ("PUT"_method, HTTPMethod::Put);
  EXPECT_EQ("CONNECT"_method, HTTPMethod::Connect);
  EXPECT_EQ("OPTIONS"_method, HTTPMethod::Options);
  EXPECT_EQ("TRACE"_method, HTTPMethod::Trace);
  EXPECT_EQ("PATCH"_method, HTTPMethod::Patch);
  EXPECT_EQ("PURGE"_method, HTTPMethod::Purge);
}
#endif

TEST(routing_params, get_int64_t) {
  routing_params rp;
  rp.int_params.push_back(42);
  rp.int_params.push_back(84);
  EXPECT_EQ(rp.get<int64_t>(0), 42);
  EXPECT_EQ(rp.get<int64_t>(1), 84);
}

TEST(routing_params, get_uint64_t) {
  routing_params rp;
  rp.uint_params.push_back(42);
  rp.uint_params.push_back(84);
  EXPECT_EQ(rp.get<uint64_t>(0), 42);
  EXPECT_EQ(rp.get<uint64_t>(1), 84);
}

TEST(routing_params, get_double) {
  routing_params rp;
  rp.double_params.push_back(42.0);
  rp.double_params.push_back(84.0);
  EXPECT_DOUBLE_EQ(rp.get<double>(0), 42.0);
  EXPECT_DOUBLE_EQ(rp.get<double>(1), 84.0);
}

TEST(routing_params, get_string) {
  routing_params rp;
  rp.string_params.push_back("a");
  rp.string_params.push_back("b");
  EXPECT_EQ(rp.get<std::string>(0), "a");
  EXPECT_EQ(rp.get<std::string>(1), "b");
}
