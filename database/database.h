// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>
#include <iostream>

class Database{
    private:
        sqlite3* db;
    public:
        Database(std::string filename);
        ~Database();
        void CheckTables();
        void RunSQLCommand(std::string command);
        int SQLSingleInt(std::string command);
        std::string SQLSingleString(std::string command);
        bool AddUser(std::string name, std::string e, std::string n, std::string password, std::string email);
        void RemoveUser(int userID);
        std::string GetUsername(int userID);
        std::string GetUserPasswd(int userID);
        std::string GetUserE(int userID);
        std::string GetUserN(int userID);
        int GetCountToUser(int userID);
        int GetCountFromUser(int userID);
        std::string GetMessage(int messageID);
        void DeleteMessage(int messageID);
        int UserExists(std::string name);
        std::vector<std::vector<std::string>> GetMessageCaptionsToUser(int userID);
        std::vector<std::vector<std::string>> GetMessageCaptionsFromUser(int userID);
};