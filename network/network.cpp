// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#include "network.h"

void Session::handle_package(Package* package){
   byte_t cmdID = package->GetCmdID();
   bool isOk = true;
   int userID = -1;
   resultPackage = new Package(CMD_NONE);
   if(cmdID != CMD_CREATE_USER){
      userID = package->GetIntValue();
   }/*

      //Handle Signature
      std::string signature = package->GetStrValue();
      RSA_Encryptor rsa;

      RSA_Pub_Key userKey;
      mpz_init_set_str(userKey.e, database->GetUserE(userID).c_str(), 16);
      mpz_init_set_str(userKey.n, database->GetUserN(userID).c_str(), 16);


      RSA_Priv_Key signKey;
      mpz_init_set(signKey.d, userKey.e);
      mpz_init(signKey.p);
      mpz_init(signKey.q);
      if(rsa.decryptString(signature, userKey, signKey) != L"Signature OK"){
         isOk = false;
      }
   }*/
   if(isOk){
   switch(cmdID){
      case CMD_CREATE_USER:
         {
            std::string name = package->GetStrValue();
            std::string e = package->GetStrValue();
            std::string n = package->GetStrValue();
            std::string password = package->GetStrValue();
            std::string email = package->GetStrValue();
            if(!database->AddUser(name, e, n, password, email)){
               resultPackage = new Package(CMD_USER_ALREADY_EXISTS);
            }
         }
      break;
      case CMD_DELETE_USER:
         {
            database->RemoveUser(userID);
         }
      break;
      case CMD_GET_USER_ID:
         {
            std::string name = package->GetStrValue();
            resultPackage = new Package(CMD_GET_USER_ID);
            resultPackage->Append(database->UserExists(name));
         }
      break;
      case CMD_GET_USER_DATA:
         {
            resultPackage = new Package(CMD_GET_USER_DATA);
            resultPackage->Append(database->GetUsername(userID));
            resultPackage->Append(database->GetUserPasswd(userID));
            resultPackage->Append(database->GetUserE(userID));
            resultPackage->Append(database->GetUserN(userID));
         }
      break;
      case CMD_GET_COUNT_TO_USER:
         {
            resultPackage = new Package(CMD_GET_COUNT_TO_USER);
            resultPackage->Append(database->GetCountToUser(userID));
         }
      break;
      case CMD_GET_COUNT_FROM_USER:
         {
            resultPackage = new Package(CMD_GET_COUNT_FROM_USER);
            resultPackage->Append(database->GetCountFromUser(userID));
         }
      break;
      case CMD_GET_VERSION:
         {
            resultPackage = new Package(CMD_GET_VERSION);
            resultPackage->Append(server_version);
         }
      break;
      case CMD_GET_MESSAGE_CAPTIONS_TO_USER:
         {
            resultPackage = new Package(CMD_GET_MESSAGE_CAPTIONS_TO_USER);
            //Data
            std::vector<std::vector<std::string>> captions = database->GetMessageCaptionsToUser(userID);
            resultPackage->Append(captions.size());
            for(int i = 0; i < captions.size(); i++){
               resultPackage->Append(std::stoi(captions.at(i).at(0)));
               resultPackage->Append(std::stoi(captions.at(i).at(1)));
               resultPackage->Append(captions.at(i).at(2));
               resultPackage->Append(std::stoi(captions.at(i).at(3)));
               resultPackage->Append(captions.at(i).at(4));
               resultPackage->Append(captions.at(i).at(5));
            }
         }
      break;
      case CMD_GET_MESSAGE_CAPTIONS_FROM_USER:
         {
            resultPackage = new Package(CMD_GET_MESSAGE_CAPTIONS_FROM_USER);
            //Data
            std::vector<std::vector<std::string>> captions = database->GetMessageCaptionsFromUser(userID);
            resultPackage->Append(captions.size());
            for(int i = 0; i < captions.size(); i++){
               resultPackage->Append(std::stoi(captions.at(i).at(0)));
               resultPackage->Append(std::stoi(captions.at(i).at(1)));
               resultPackage->Append(captions.at(i).at(2));
               resultPackage->Append(std::stoi(captions.at(i).at(3)));
               resultPackage->Append(captions.at(i).at(4));
               resultPackage->Append(captions.at(i).at(5));
            }
         }
      break;
      case CMD_GET_MESSAGE:
         {
            int messageID = package->GetIntValue();
            resultPackage = new Package(CMD_GET_MESSAGE);
            resultPackage->Append(database->GetMessage(messageID));
         }
      break;
      case CMD_DELETE_MESSAGE:
         {
            int messageID = package->GetIntValue();
            database->DeleteMessage(messageID);
         }
      break;
   }
   } else {
      std::cout << "Falsche Signatur!" << std::endl;
   }
}

void Session::handle_read(const boost::system::error_code& err, size_t bytes_transferred)
{
    if (err) {
        std::cout << "error: " << err.message() << std::endl;
    }

    Package package;
    package.SetByteStream(readdata, bytes_transferred);
    
    handle_package(&package);

    if(resultPackage->GetByteStream().at(0) != CMD_NONE){
      socket.async_write_some(boost::asio::buffer(resultPackage->GetByteStream()), boost::bind(&Session::handle_write,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    }

    free(readdata);
}

void Session::handle_write(const boost::system::error_code& err, size_t bytes_transferred){

}

void Session::start(){
    readdata = (byte_t*)malloc(1000000000);
    socket.async_read_some(boost::asio::buffer(readdata, 1000000000), boost::bind(&Session::handle_read,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void Server::async_accept()
{
    acceptor.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
    {
        if (!ec)
        {
            std::make_shared<Session>(std::move(socket), database)->start();
        }

        async_accept();
    });
}