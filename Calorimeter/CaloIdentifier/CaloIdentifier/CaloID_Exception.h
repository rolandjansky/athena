/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/* date of creation : 10/X/2002 */

/* date of last modification : 10/X/2002 */

#ifndef CALOID_EXCEPTION_H
#define CALOID_EXCEPTION_H

#include <string>

/**
 * @brief Exception class for Calo Identifiers 

 * @author Johann Collot , Fabienne Ledroit (cloning  LArID_Exception)
 */

class CaloID_Exception {
public:

    /**
     * default constructor 
     */
  CaloID_Exception() ;

    /**
     * constructor to be used 
     */
    CaloID_Exception(const std::string&  lMessage , int lCode);

    /**
     * set error message 
     */
   void message(const std::string& lMessage);

    /**
     * return error message <br> 
     */
    std::string message() const ;

    /**
     * set error code number<br> 
     */
    void code(int lCode) ;

    /**
     * return error code <br><br>
     * 
     * error codes : <br>
     * 0 : no error <br>
     * 1 : CaloLVL1_ID::region_id Error <br>
     * 2 : CaloLVL1_ID::channel_id Error <br>
     * 3 : LArDM::region_id Error <br>
     * 4 : LArDM::zone_id Error <br>
     * 999 : undefined error <br>
     */
    int code() const ;

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


#endif //CALOID_EXCEPTION_H
