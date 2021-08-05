// Copyright 2021 Thore Lencer
// SPDX-License-Identifier: AGPL-3.0-only

#include "database.h"

Database::Database(std::string filename){
    sqlite3_open(filename.c_str(), &db);
}

Database::~Database(){
    sqlite3_close(db);
}

void Database::RunSQLCommand(std::string command){
    sqlite3_stmt* statement;
    sqlite3_prepare (db, command.c_str(), -1, &statement, 0);

    int smth= sqlite3_step (statement);
    sqlite3_reset (statement);
    sqlite3_finalize (statement);
}

int Database::SQLSingleInt(std::string command){
    sqlite3_stmt* statement;
    sqlite3_prepare (db, command.c_str(), -1, &statement, 0);

    int smth= sqlite3_step (statement);
    int result = -1;
    if(smth == SQLITE_ROW){
        result = sqlite3_column_int(statement, 0);
    }

    sqlite3_reset (statement);
    sqlite3_finalize (statement);

    return result;
}

std::string Database::SQLSingleString(std::string command){
    sqlite3_stmt* statement;
    sqlite3_prepare (db, command.c_str(), -1, &statement, 0);

    int smth= sqlite3_step (statement);
    std::string result = "";
    if(smth == SQLITE_ROW){
        result = std::string((const char*)sqlite3_column_text(statement, 0));
    }

    sqlite3_reset (statement);
    sqlite3_finalize (statement);

    return result;
}
        
void Database::CheckTables(){
    RunSQLCommand("CREATE TABLE if not exists `Users` (`ID` INTEGER PRIMARY KEY AUTOINCREMENT,`Name` text NOT NULL,`E` text NOT NULL,`N` text NOT NULL,`Password` text NOT NULL,`Email` text DEFAULT 'None');");
    RunSQLCommand("CREATE TABLE if not exists `Messages` (`ID` INTEGER PRIMARY KEY AUTOINCREMENT,`Sender` INTEGER NOT NULL,`Receiver` INTEGER NOT NULL,`Caption` text NOT NULL,`Message` longtext NOT NULL,`Signature` mediumtext NOT NULL,`Read` TINYINTEGER NOT NULL DEFAULT 0,`SenderCaption` text NOT NULL,`SenderMessage` longtext NOT NULL,`Date` text NOT NULL,`ReceiveDate` text NOT NULL DEFAULT '0000-00-00 00:00:00',`SenderDate` text NOT NULL DEFAULT '0000-00-00',`SenderReceiveDate` text NOT NULL DEFAULT '0000-00-00',`AttachInfo` text DEFAULT '0;',`SenderAttachInfo` text DEFAULT '0;',`Attachment` longblob DEFAULT NULL,`AttachmentKey` text DEFAULT NULL);");

}

bool Database::AddUser(std::string name, std::string e, std::string n, std::string password, std::string email){
    if(UserExists(name) == -1){
        RunSQLCommand(std::string("INSERT INTO Users (Name, E, N, Password, Email) VALUES('"+name+"', '"+e+"', '"+n+"', '"+password+"', '"+email+"');"));
        return true;
    } else {
        return false;
    }
}

int Database::UserExists(std::string name){
    int result = SQLSingleInt(std::string("SELECT ID, Name FROM Users WHERE Name = '"+name+"'"));

    return result;
}

void Database::RemoveUser(int userID){
    RunSQLCommand(std::string("DELETE FROM Messages WHERE Sender=" + std::to_string(userID)));
    RunSQLCommand(std::string("DELETE FROM Messages WHERE Receiver=" + std::to_string(userID)));
    RunSQLCommand(std::string("DELETE FROM Users WHERE ID=" + std::to_string(userID)));
}

std::string Database::GetUsername(int userID){
    return SQLSingleString(std::string("SELECT Name, ID FROM Users WHERE ID="+ std::to_string(userID)));
}

std::string Database::GetUserPasswd(int userID){
    return SQLSingleString(std::string("SELECT Password, ID FROM Users WHERE ID="+ std::to_string(userID)));
}

std::string Database::GetUserE(int userID){
    return SQLSingleString(std::string("SELECT E, ID FROM Users WHERE ID="+ std::to_string(userID)));
}

std::string Database::GetUserN(int userID){
    return SQLSingleString(std::string("SELECT N, ID FROM Users WHERE ID="+ std::to_string(userID)));
}

int Database::GetCountToUser(int userID){
    return SQLSingleInt(std::string("SELECT COUNT(ID), Receiver FROM Messages WHERE Receiver="+ std::to_string(userID)));
}
        
int Database::GetCountFromUser(int userID){
    return SQLSingleInt(std::string("SELECT COUNT(ID), Sender FROM Messages WHERE Sender="+ std::to_string(userID)));
}

std::vector<std::vector<std::string>> Database::GetMessageCaptionsToUser(int userID){
    std::vector<std::vector<std::string>> result;
    
    sqlite3_stmt* statement;
    sqlite3_prepare (db, std::string("SELECT ID, Sender, Caption, Read, Date, ReceiveDate, Receiver FROM Messages WHERE Receiver="+std::to_string(userID)).c_str(), -1, &statement, 0);
    int smth;
    do {
        smth = sqlite3_step (statement);
        if(smth == SQLITE_ROW){
            std::vector<std::string> row;
            row.push_back(std::to_string(sqlite3_column_int(statement, 0)));
            row.push_back(std::to_string(sqlite3_column_int(statement, 1)));
            row.push_back(std::string((const char*)sqlite3_column_text(statement, 2)));
            row.push_back(std::to_string(sqlite3_column_int(statement, 3)));
            row.push_back(std::string((const char*)sqlite3_column_text(statement, 4)));
            row.push_back(std::string((const char*)sqlite3_column_text(statement, 5)));
            result.push_back(row);
        }
    }while(smth == SQLITE_ROW);

    sqlite3_reset (statement);
    sqlite3_finalize (statement);

    return result;
}

std::vector<std::vector<std::string>> Database::GetMessageCaptionsFromUser(int userID){
    std::vector<std::vector<std::string>> result;
    
    sqlite3_stmt* statement;
    sqlite3_prepare (db, std::string("SELECT ID, Receiver, Caption, Read, Date, ReceiveDate, Sender FROM Messages WHERE Sender="+std::to_string(userID)).c_str(), -1, &statement, 0);
    int smth;
    do {
        smth = sqlite3_step (statement);
        if(smth == SQLITE_ROW){
            std::vector<std::string> row;
            row.push_back(std::to_string(sqlite3_column_int(statement, 0)));
            row.push_back(std::to_string(sqlite3_column_int(statement, 1)));
            row.push_back(std::string((const char*)sqlite3_column_text(statement, 2)));
            row.push_back(std::to_string(sqlite3_column_int(statement, 3)));
            row.push_back(std::string((const char*)sqlite3_column_text(statement, 4)));
            row.push_back(std::string((const char*)sqlite3_column_text(statement, 5)));
            result.push_back(row);
        }
    }while(smth == SQLITE_ROW);

    sqlite3_reset (statement);
    sqlite3_finalize (statement);

    return result;
}

std::string Database::GetMessage(int messageID){
    return SQLSingleString(std::string("SELECT Message, ID FROM Messages WHERE ID="+std::to_string(messageID)));
}

void Database::DeleteMessage(int messageID){
    RunSQLCommand(std::string("DELETE FROM Messages WHERE ID=" + std::to_string(messageID)));
}