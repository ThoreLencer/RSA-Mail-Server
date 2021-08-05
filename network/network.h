// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once
#include "package.h"
#include <vector>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include "../database/database.h"
#include "../Encryptor/rsa_encryption.h"

const int server_version = 21;

enum {
   CMD_CREATE_USER = 0,
   CMD_DELETE_USER,
   CMD_GET_USER_ID,
   CMD_GET_USER_DATA,
   CMD_GET_COUNT_TO_USER,
   CMD_GET_COUNT_FROM_USER,
   CMD_GET_VERSION,
   CMD_GET_MESSAGE_CAPTIONS_TO_USER,
   CMD_GET_MESSAGE_CAPTIONS_FROM_USER,
   CMD_GET_MESSAGE,
   CMD_DELETE_MESSAGE,
   CMD_USER_ALREADY_EXISTS,
   CMD_NONE
};

class Session : public std::enable_shared_from_this<Session>
{
public:

    Session(boost::asio::ip::tcp::socket&& socket, Database* database)
    : socket(std::move(socket))
    {
        this->database=database;
    }

    void handle_read(const boost::system::error_code& err, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& err, size_t bytes_transferred);

    void handle_package(Package* package);

    void start();

private:

    boost::asio::ip::tcp::socket socket;
    Package* resultPackage;
    byte_t* readdata;
    Database* database;
};

class Server{
    public:
        Server(boost::asio::io_context& io_context, std::uint16_t port, Database* database)
        : io_context(io_context)
        , acceptor  (io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
        {
            this->database = database;
        }

        void async_accept();
    private:
        boost::asio::io_context& io_context;
        boost::asio::ip::tcp::acceptor acceptor;
        Database* database;
};