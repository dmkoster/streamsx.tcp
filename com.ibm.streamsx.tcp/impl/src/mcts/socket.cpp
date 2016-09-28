/*
#######################################################################
# Copyright (C)2016, International Business Machines Corporation and
# others. All Rights Reserved.
#######################################################################
*/

#include "mcts/socket.h"

#include <SPL/Runtime/Common/RuntimeDebug.h>

using namespace mcts;

Socket::Socket(streams_boost::asio::io_service & ioService) : 
  strand_(ioService), 
  socket_(ioService)
{
  SPLAPPTRC(L_ERROR,"Constructor", "Socket");
}

void Socket::async_write(streams_boost::asio::const_buffers_1 buffer, async_complete_func handler)
{
  streams_boost::asio::async_write(socket_, buffer, strand_.wrap(handler));
}

void Socket::async_write(streams_boost::array<streams_boost::asio::const_buffer, 2> buffers, async_complete_func handler)
{
  streams_boost::asio::async_write(socket_, buffers, strand_.wrap(handler));
}

void Socket::async_read(streams_boost::asio::mutable_buffers_1 buffer, async_complete_func handler)
{
  streams_boost::asio::async_read(socket_, buffer, handler);
}

void Socket::async_read_some(streams_boost::asio::mutable_buffers_1 buffer, async_complete_func handler)
{
  socket_.async_read_some(buffer, handler);
}

void Socket::handleConnect(connect_complete_func handler, const streams_boost::system::error_code & ec)
{
  handler(ec);
}

void Socket::handleAccept(accept_complete_func handler, const streams_boost::system::error_code & ec)
{
  handler(ec);
}

void Socket::shutdown(streams_boost::asio::ip::tcp::socket::shutdown_type what, streams_boost::system::error_code & ec)
{
  socket_.shutdown(what, ec);
}

void Socket::cancel(streams_boost::system::error_code & ec)
{
  socket_.cancel(ec);
}