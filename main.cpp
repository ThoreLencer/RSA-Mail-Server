// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#include <iostream>
#include "database/database.h"
#include "network/package.h"
#include "network/network.h"
#include "Encryptor/rsa_encryption.h"

int main(){
   //Start Database
   Database* database = new Database("mail.db");
   database->CheckTables();
   //Start Server
   boost::asio::io_context io_context;
   Server srv(io_context, 40000, database);
   srv.async_accept();
   io_context.run();
   return 0;
}