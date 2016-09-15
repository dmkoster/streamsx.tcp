/*
#######################################################################
# Copyright (C)2016, International Business Machines Corporation and
# others. All Rights Reserved.
#######################################################################
*/

#include "mcts/tls_socket.h"

using namespace mcts;

TLSSocket::TLSSocket(streams_boost::asio::io_service & ioService) :
  Socket(ioService)
{
  streams_boost::asio::ssl::context ctx(streams_boost::asio::ssl::context::tlsv12);
  ctx.set_options(streams_boost::asio::ssl::context::default_workarounds);
  ctx.set_verify_mode(streams_boost::asio::ssl::verify_peer);

  // TODO: Load Verification File / Certificate Chain / Private Key
  //ctx.load_verify_file(AsioConnectionUtils::getTransportCertificatePath());

  ssl_.reset(
    new streams_boost::asio::ssl::stream<streams_boost::asio::ip::tcp::socket&>(
      getUnderlyingSocket(), ctx));
}

void TLSSocket::async_write(streams_boost::asio::const_buffers_1 buffer, async_complete_func handler)
{
  streams_boost::asio::async_write(*ssl_, buffer, strand_.wrap(handler));
}

void TLSSocket::async_write(streams_boost::array<streams_boost::asio::const_buffer, 2> buffers, async_complete_func handler)
{
  streams_boost::asio::async_write(*ssl_, buffers, strand_.wrap(handler));
}

void TLSSocket::async_read(streams_boost::asio::mutable_buffers_1 buffer, async_complete_func handler)
{
  streams_boost::asio::async_read(*ssl_, buffer, handler);
}

void TLSSocket::async_read_some(streams_boost::asio::mutable_buffers_1 buffer, async_complete_func handler)
{
  ssl_->async_read_some(buffer, handler);
}

void TLSSocket::connect()
{

}

void TLSSocket::accept()
{

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
