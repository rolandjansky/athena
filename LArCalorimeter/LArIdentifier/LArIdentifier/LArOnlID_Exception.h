/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHARDWAREID_EXCEPTION_H
#define LARHARDWAREID_EXCEPTION_H

#include <string>

/**
 * @brief Exception class for LAr online Identifiers 
 * @author Johann Collot , Brinick Simmons
 * @version 00-00-00
 * @since 00-00-00 
 */
class LArOnlID_Exception {
public:

    /**
     * @brief default constructor 
     */
    LArOnlID_Exception() : m_message("No error message") , m_code( 999 ) {  }

    /**
     * constructor to be used 
     */
    LArOnlID_Exception(const std::string&  l_message , int l_code) : m_message ( l_message ) , m_code ( l_code ) { }

    /**
     * set error message 
     */
    void message(const std::string& l_message) { m_message = l_message ;}

    /**
     * return error message <br> 
     */
    virtual std::string message() const { return m_message;}

    /**
     * set error code number<br> 
     */
    void code(int l_code) { m_code = l_code ;}

    /**
     * return error code <br><br>
     * 
     * error codes : <br>
     * 0 : no error <br>
     * 1 : at least 1 field was given a crazy value <br>
     * 2 : at least 1 field was given a value out of the allowed range <br>
     * 3 : wrong febID <br>
     * 4 : wrong feedthroughID <br>
     * 5 : channel value out of range
     * 6 : slot and/or channel value out of range
     * 999 : undefined error <br>
     */
    virtual int code() const { return m_code;}

    virtual operator std::string();

    /**
     * destructor 
     */
    virtual ~LArOnlID_Exception() {}

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
#endif //LARHARDWAREID_EXCEPTION_H


