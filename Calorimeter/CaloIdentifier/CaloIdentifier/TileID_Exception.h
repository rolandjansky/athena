/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEID_EXCEPTION_H
#define TILEID_EXCEPTION_H

#include <string>

/**
* @class TileID_Exception
* @brief Exception class for Tile Identifiers 
* @author Alexander Solodkov
*/

class TileID_Exception {
public:

    /**
     * default constructor 
     */
    TileID_Exception() : m_message("No error message") , m_code( 999 ) {  }

    /**
     * constructor to be used 
     */
    TileID_Exception(const std::string&  l_message , int l_code) : m_message ( l_message ) , m_code ( l_code ) { }

    /**
     * set error message 
     */
    void message(const std::string& l_message) { m_message = l_message ;}

    /**
     * return error message  
     */
    std::string message() const { return m_message;}

    /**
     * set error code number 
     */
    void code(int l_code) { m_code = l_code ;}

    /**
     * return error code 
     * 
     * error codes : 
     * 0 : no error 
     * 1 : Logical ID is out of range
     * 2 : Hardware ID is out of range
     * 3 : No PMT for this logical ID (invalid logical ID)
     * 4 : No PMT for this channel (not connected)
     * 9 : ID compactification Error 
     *11 : id is not an TileCal logical identifier 
     *12 : id is not an TileCal hardware identifier 
     *19 : no IDCompactor available ( null pointer ) 
     * 999 : undefined error
     */
    int code() const { return m_code;}

    operator std::string();


private:

    /**
     * error message 
     */
    std::string m_message;

    /**
     * error code 
     */
    int m_code;
};
#endif //TILEID_EXCEPTION_H

