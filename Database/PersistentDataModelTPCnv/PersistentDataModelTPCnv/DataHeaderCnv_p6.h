/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENTDATAMODELTPCNV_DATAHEADERCNV_P6_H
#define PERSISTENTDATAMODELTPCNV_DATAHEADERCNV_P6_H

/**
 *  @file DataHeaderCnv_p6.h
 *  @brief This file contains the class definition for DataHeaderCnv_p6 
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

class DataHeader;
class DataHeader_p6;
class DataHeaderForm_p6;

/** @class DataHeaderCnv_p6
 *  @brief This class provides the converter to customize the saving of DataHeader_p6.
 **/
class DataHeaderCnv_p6 {
public:
   DataHeaderCnv_p6() {}

   DataHeader* createTransient(const DataHeader_p6* persObj, const DataHeaderForm_p6& form);
   DataHeader_p6* createPersistent(const DataHeader* transObj, DataHeaderForm_p6& form);

   /// convert single DH element to persistent represenation
   void elemToPers(const DataHeaderElement* trans, DataHeader_p6* pers, DataHeaderForm_p6& form);
   /// restore single DH element from persistent represenation
   void persToElem(const DataHeader_p6* pers, unsigned p_idx, DataHeaderElement* trans,
                   const DataHeaderForm_p6& form );
  
   void insertDHRef( DataHeader_p6* pers,
                     const std::string& key, const std::string& strToken,
                     DataHeaderForm_p6& form );
};

#endif
