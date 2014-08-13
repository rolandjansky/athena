/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENTDATAMODELTPCNV_DATAHEADERCNV_P6_H
#define PERSISTENTDATAMODELTPCNV_DATAHEADERCNV_P6_H

/**
 *  @file DataHeaderCnv_p6.h
 *  @brief This file contains the class definition for the DataHeaderCnv_p6 and DataHeaderElementCnv_p6 classes.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: DataHeaderCnv_p6.h,v 1.1 2009-04-21 21:48:34 gemmeren Exp $
 **/

#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModelTPCnv/DataHeader_p6.h"

#include <map>

/** @class DataHeaderElementCnv_p6
 *  @brief This class provides the converter to customize the saving of DataHeaderElement_p6.
 **/
class DataHeaderElementCnv_p6 {
public:
   DataHeaderElementCnv_p6();
   virtual ~DataHeaderElementCnv_p6();

   void persToTrans(const DataHeaderElement_p6* pers, DataHeaderElement* trans, const DataHeaderForm_p6& form);
   void transToPers(const DataHeaderElement* trans, DataHeaderElement_p6* pers, DataHeaderForm_p6& form);

friend class DataHeaderCnv_p6;
};

/** @class DataHeaderCnv_p6
 *  @brief This class provides the converter to customize the saving of DataHeader_p6.
 **/
class DataHeaderCnv_p6 {
public:
   DataHeaderCnv_p6();
   virtual ~DataHeaderCnv_p6();

   DataHeader* createTransient(const DataHeader_p6* persObj);
   void persToTrans(const DataHeader_p6* pers, DataHeader* trans);
   DataHeader_p6* createPersistent(const DataHeader* transObj);
   void transToPers(const DataHeader* trans, DataHeader_p6* pers);

   void insertDHRef(DataHeader_p6* pers, const std::string& key, const std::string& strToken);

private:
   DataHeaderElementCnv_p6 m_elemCnv;
};

inline DataHeader* DataHeaderCnv_p6::createTransient(const DataHeader_p6* persObj) {
   DataHeader* trans = new DataHeader();
   persToTrans(persObj, trans);
   return(trans);
}
inline DataHeader_p6* DataHeaderCnv_p6::createPersistent(const DataHeader* transObj){
   DataHeader_p6* pers = new DataHeader_p6();
   transToPers(transObj, pers);
   return(pers);
}

#endif
