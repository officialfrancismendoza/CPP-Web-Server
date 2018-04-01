/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#ifndef oatpp_network_http_Protocol_hpp
#define oatpp_network_http_Protocol_hpp

#include "oatpp/network/Connection.hpp"

#include "oatpp/core/parser/ParsingCaret.hpp"

#include "oatpp/core/data/stream/Delegate.hpp"
#include "oatpp/core/collection/ListMap.hpp"
#include "oatpp/core/base/String.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http {
  
class Status{
public:
  
  static const Status CODE_100;// Continue
  static const Status CODE_101;// Switching
  static const Status CODE_102;// Processing
  
  static const Status CODE_200;// OK
  static const Status CODE_201;// Created
  static const Status CODE_202;// Accepted
  static const Status CODE_203;// Non-Authoritative Information
  static const Status CODE_204;// No Content
  static const Status CODE_205;// Reset Content
  static const Status CODE_206;// Partial Content
  static const Status CODE_207;// Multi-Status
  static const Status CODE_226;// IM Used
  
  static const Status CODE_300;// Multiple Choices
  static const Status CODE_301;// Moved Permanently
  static const Status CODE_302;// Moved Temporarily
  static const Status CODE_303;// See Other
  static const Status CODE_304;// Not Modified
  static const Status CODE_305;// Use Proxy
  static const Status CODE_306;// Reserved
  static const Status CODE_307;// Temporary Redirect
  
  static const Status CODE_400;// Bad Request
  static const Status CODE_401;// Unauthorized
  static const Status CODE_402;// Payment Required
  static const Status CODE_403;// Forbidden
  static const Status CODE_404;// Not Found
  static const Status CODE_405;// Method Not Allowed
  static const Status CODE_406;// Not Acceptable
  static const Status CODE_407;// Proxy Authentication Required
  static const Status CODE_408;// Request Timeout
  static const Status CODE_409;// Conflict
  static const Status CODE_410;// Gone
  static const Status CODE_411;// Length Required
  static const Status CODE_412;// Precondition Failed
  static const Status CODE_413;// Request Entity Too Large
  static const Status CODE_414;// Request-URI Too Large
  static const Status CODE_415;// Unsupported Media Type
  static const Status CODE_416;// Requested Range Not Satisfiable
  static const Status CODE_417;// Expectation Failed
  static const Status CODE_422;// Unprocessable Entity
  static const Status CODE_423;// Locked
  static const Status CODE_424;// Failed Dependency
  static const Status CODE_425;// Unordered Collection
  static const Status CODE_426;// Upgrade Required
  static const Status CODE_428;// Precondition Required
  static const Status CODE_429;// Too Many Requests
  static const Status CODE_431;// Request Header Fields Too Large
  static const Status CODE_434;// Requested host unavailable
  static const Status CODE_444;// Close connection withot sending headers
  static const Status CODE_449;// Retry With
  static const Status CODE_451;// Unavailable For Legal Reasons
  
  static const Status CODE_500;// Internal Server Error
  static const Status CODE_501;// Not Implemented
  static const Status CODE_502;// Bad Gateway
  static const Status CODE_503;// Service Unavailable
  static const Status CODE_504;// Gateway Timeout
  static const Status CODE_505;// HTTP Version Not Supported
  static const Status CODE_506;// Variant Also Negotiates
  static const Status CODE_507;// Insufficient Storage
  static const Status CODE_508;// Loop Detected
  static const Status CODE_509;// Bandwidth Limit Exceeded
  static const Status CODE_510;// Not Extended
  static const Status CODE_511;// Network Authentication Required
  
  Status()
    : code(0)
    , description(nullptr)
  {}
  
  Status(v_int32 pCode, const char* pDesc)
    : code(pCode)
    , description(pDesc)
  {}
  
  v_int32 code;
  const char* description;
  
};
  
class Header {
public:
  class Value {
  public:
    static const char* const CONNECTION_CLOSE;
    static const char* const CONNECTION_KEEP_ALIVE;
    
    static const char* const SERVER;
    static const char* const USER_AGENT;
    
    static const char* const TRANSFER_ENCODING_CHUNKED;
    static const char* const CONTENT_TYPE_APPLICATION_JSON;
  };
public:
  static const char* const ACCEPT;              // "Accept"
  static const char* const AUTHORIZATION;       // "Authorization"
  static const char* const CONNECTION;          // "Connection"
  static const char* const TRANSFER_ENCODING;   // "Transfer-Encoding"
  static const char* const CONTENT_ENCODING;    // "Content-Encoding"
  static const char* const CONTENT_LENGTH;      // "Content-Length"
  static const char* const CONTENT_TYPE;        // "Content-Type"
  static const char* const CONTENT_RANGE;       // "Content-Range"
  static const char* const RANGE;               // "Range"
  static const char* const HOST;                // "Host"
  static const char* const USER_AGENT;          // "User-Agent"
  static const char* const SERVER;              // "Server"
};
  
class Range {
public:
  constexpr static const char* UNIT_BYTES = "bytes";
private:
  Range()
    : units(nullptr)
  {}
public:
  
  Range(const oatpp::base::String::PtrWrapper& pUnits,
        const oatpp::os::io::Library::v_size& pStart,
        const oatpp::os::io::Library::v_size& pEnd)
    : units(pUnits)
    , start(pStart)
    , end(pEnd)
  {}
  
  oatpp::base::String::PtrWrapper units;
  oatpp::os::io::Library::v_size start;
  oatpp::os::io::Library::v_size end;
  
  oatpp::base::String::PtrWrapper toString() const;
  
  bool isValid() const {
    return units.get() != nullptr;
  }
  
  static Range parse(oatpp::parser::ParsingCaret& caret);
  static Range parse(const std::shared_ptr<oatpp::base::String>& str);
  
};
  
class ContentRange {
public:
  constexpr static const char* UNIT_BYTES = "bytes";
private:
  ContentRange()
    : units(nullptr)
  {}
public:
  
  ContentRange(const oatpp::base::String::PtrWrapper& pUnits,
               const oatpp::os::io::Library::v_size& pStart,
               const oatpp::os::io::Library::v_size& pEnd,
               const oatpp::os::io::Library::v_size& pSize,
               bool pIsSizeKnown)
    : units(pUnits)
    , start(pStart)
    , end(pEnd)
    , size(pSize)
    , isSizeKnown(pIsSizeKnown)
  {}
  
  oatpp::base::String::PtrWrapper units;
  oatpp::os::io::Library::v_size start;
  oatpp::os::io::Library::v_size end;
  oatpp::os::io::Library::v_size size;
  bool isSizeKnown;
  
  oatpp::base::String::PtrWrapper toString() const;
  
  bool isValid() const {
    return units.get() != nullptr;
  }
  
  static ContentRange parse(oatpp::parser::ParsingCaret& caret);
  static ContentRange parse(const std::shared_ptr<oatpp::base::String>& str);
  
};
  
class RequestStartingLine : public base::Controllable {
public:
  OBJECT_POOL(RequestStartingLine_Pool, RequestStartingLine, 32)
  SHARED_OBJECT_POOL(Shared_RequestStartingLine_Pool, RequestStartingLine, 32)
public:
  RequestStartingLine()
  {}
public:
  
  static std::shared_ptr<RequestStartingLine> createShared(){
    return Shared_RequestStartingLine_Pool::allocateShared();
  }
  
  std::shared_ptr<base::String> method; // GET, POST ...
  std::shared_ptr<base::String> path;
  std::shared_ptr<base::String> protocol;
  
};
  
class ResponseStartingLine : public base::Controllable {
public:
  OBJECT_POOL(ResponseStartingLine_Pool, ResponseStartingLine, 32)
  SHARED_OBJECT_POOL(Shared_ResponseStartingLine_Pool, ResponseStartingLine, 32)
public:
  ResponseStartingLine()
  {}
public:
  
  static std::shared_ptr<ResponseStartingLine> createShared(){
    return Shared_ResponseStartingLine_Pool::allocateShared();
  }
  
  std::shared_ptr<base::String> protocol;
  v_int32 statusCode;
  std::shared_ptr<base::String> description;
  
};
  
class Protocol {
public:
  typedef oatpp::collection::ListMap<base::String::PtrWrapper, base::String::PtrWrapper> Headers;
private:
  static std::shared_ptr<oatpp::base::String> parseHeaderName(oatpp::parser::ParsingCaret& caret);
public:
  
  static std::shared_ptr<RequestStartingLine> parseRequestStartingLine(oatpp::parser::ParsingCaret& caret);
  static std::shared_ptr<ResponseStartingLine> parseResponseStartingLine(oatpp::parser::ParsingCaret& caret);
  static std::shared_ptr<Headers> parseHeaders(oatpp::parser::ParsingCaret& caret, Status& error);
  
};
  
}}}}

#endif /* oatpp_network_http_Protocol_hpp */
