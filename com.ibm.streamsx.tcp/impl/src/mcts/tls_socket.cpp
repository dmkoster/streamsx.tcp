/*
#######################################################################
# Copyright (C)2016, International Business Machines Corporation and
# others. All Rights Reserved.
#######################################################################
*/

#include "mcts/tls_socket.h"

#include <SPL/Runtime/Common/RuntimeDebug.h>

using namespace mcts;

TLSSocket::TLSSocket(streams_boost::asio::io_service & ioService, const std::string & certifcate) :
  Socket(ioService)
{
  SPLAPPTRC(L_TRACE,"TLS Client Constructor", "TLSSocket");

  streams_boost::asio::ssl::context ctx(streams_boost::asio::ssl::context::tlsv12);
  ctx.set_options(streams_boost::asio::ssl::context::default_workarounds);
  ctx.set_verify_mode(streams_boost::asio::ssl::verify_peer);
  ctx.load_verify_file(certifcate);

  ssl_.reset(
    new streams_boost::asio::ssl::stream<streams_boost::asio::ip::tcp::socket&>(
      getUnderlyingSocket(), ctx));
}

TLSSocket::TLSSocket(streams_boost::asio::io_service & ioService, const std::string & certifcate, const std::string & key) :
  Socket(ioService)
{
  SPLAPPTRC(L_TRACE,"TLS Server Constructor", "TLSSocket");

  streams_boost::asio::ssl::context ctx(streams_boost::asio::ssl::context::tlsv12);
  ctx.set_options(streams_boost::asio::ssl::context::default_workarounds);
  ctx.set_verify_mode(streams_boost::asio::ssl::verify_peer);
  ctx.use_certificate_file(certifcate, streams_boost::asio::ssl::context::pem);
  ctx.use_private_key_file(key, streams_boost::asio::ssl::context::pem);

  ssl_.reset(
    new streams_boost::asio::ssl::stream<streams_boost::asio::ip::tcp::socket&>(
      getUnderlyingSocket(), ctx));
}

void TLSSocket::async_write(streams_boost::asio::const_buffers_1 buffer, async_complete_func handler)
{
  SPLAPPTRC(L_TRACE,"TLS Async Write", "TLSSocket");
  streams_boost::asio::async_write(*ssl_, buffer, strand_.wrap(handler));
}

void TLSSocket::async_write(streams_boost::array<streams_boost::asio::const_buffer, 2> buffers, async_complete_func handler)
{
  SPLAPPTRC(L_TRACE,"TLS Async Write (2)", "TLSSocket");
  streams_boost::asio::async_write(*ssl_, buffers, strand_.wrap(handler));
}

void TLSSocket::async_read(streams_boost::asio::mutable_buffers_1 buffer, async_complete_func handler)
{
  SPLAPPTRC(L_TRACE,"TLS Async Read", "TLSSocket");
  streams_boost::asio::async_read(*ssl_, buffer, handler);
}

void TLSSocket::async_read_some(streams_boost::asio::mutable_buffers_1 buffer, async_complete_func handler)
{
  SPLAPPTRC(L_TRACE,"TLS Async Read Some", "TLSSocket");
  ssl_->async_read_some(buffer, handler);
}

void TLSSocket::handleConnect(connect_complete_func handler, const streams_boost::system::error_code & ec)
{
  SPLAPPTRC(L_TRACE,"Handle Connect " << ec.message(), "TLSSocket");
  if(!ec)
  {
    streams_boost::system::error_code handshakeEc;
    ssl_->handshake(streams_boost::asio::ssl::stream_base::client, handshakeEc);
    SPLAPPTRC(L_TRACE,"Handshake (Client) " << ec.message(), "TLSSocket");
    if(handshakeEc)
        handler(handshakeEc);
  }
  handler(ec);
}

void TLSSocket::handleAccept(accept_complete_func handler, const streams_boost::system::error_code & ec)
{
  SPLAPPTRC(L_TRACE,"Handle Accept " << ec.message(), "TLSSocket");
  if(!ec)
  {
    streams_boost::system::error_code handshakeEc;
    ssl_->handshake(streams_boost::asio::ssl::stream_base::server, handshakeEc);
    SPLAPPTRC(L_TRACE,"Handshake (Server) " << ec.message(), "TLSSocket");
    if(handshakeEc)
      handler(handshakeEc);
  }
  Socket::handleAccept(handler, ec);
  
}

void TLSSocket::shutdown(streams_boost::asio::ip::tcp::socket::shutdown_type what, streams_boost::system::error_code & ec)
{
  if(what != streams_boost::asio::ip::tcp::socket::shutdown_both)
  {
    // TODO: Throw or set error code
    // TLS won't work with only half a connection
    // Or skip all of the shutdown?
    
    // SSL Stream Shutdown
    ssl_->shutdown(ec);
  }
  
  // If there are no issues, shutdown the underlying socket
  if(!ec)
  {
    Socket::shutdown(what, ec);
  }
}
