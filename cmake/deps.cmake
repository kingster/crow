find_package(Tcmalloc)
find_package(Threads)
find_package(OpenSSL)

if (OPENSSL_FOUND)
  include_directories(SYSTEM ${OPENSSL_INCLUDE_DIR})
endif()

if (MSVC)
  set(Boost_USE_STATIC_LIBS "On")
  find_package( Boost 1.52 COMPONENTS system thread regex REQUIRED )
else()
  find_package( Boost 1.52 COMPONENTS system thread REQUIRED )
endif()

include_directories(SYSTEM "${Boost_INCLUDE_DIR}")
