/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENTDATAMODELTPCNV_DATAHEADERCNV_P4_H
#define PERSISTENTDATAMODELTPCNV_DATAHEADERCNV_P4_H

/**
 *  @file DataHeaderCnv_p4.h
 *  @brief This file contains the class definition for the DataHeaderCnv_p4 and DataHeaderElementCnv_p4 classes.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: DataHeaderCnv_p4.h,v 1.1 2009-04-21 21:48:34 gemmeren Exp $
 **/

#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModelTPCnv/DataHeader_p4.h"

/** @class DataHeaderElementCnv_p4
 *  @brief This class provides the converter to customize the saving of DataHeaderElement_p4.
 **/
class DataHeaderElementCnv_p4 {
public:
   DataHeaderElementCnv_p4();
   virtual ~DataHeaderElementCnv_p4();

   void persToTrans(const DataHeaderElement_p4* pers, DataHeaderElement* trans, const std::vector<std::string>& map);
   void transToPers(const DataHeaderElement* trans, DataHeaderElement_p4* pers, std::vector<std::string>& map);

friend class DataHeaderCnv_p4;
};

/** @class DataHeaderCnv_p4
 *  @brief This class provides the converter to customize the saving of DataHeader_p4.
 **/
class DataHeaderCnv_p4 {
public:
   DataHeaderCnv_p4();
   virtual ~DataHeaderCnv_p4();

   DataHeader* createTransient(const DataHeader_p4* persObj);
   void persToTrans(const DataHeader_p4* pers, DataHeader* trans);
   DataHeader_p4* createPersistent(const DataHeader* transObj);
   void transToPers(const DataHeader* trans, DataHeader_p4* pers);

   void insertDHRef(DataHeader_p4* pers, const std::string& key, const std::string& token);

private:
   DataHeaderElementCnv_p4 m_elemCnv;
};

inline DataHeader* DataHeaderCnv_p4::createTransient(const DataHeader_p4* persObj) {
   DataHeader* trans = new DataHeader();
   persToTrans(persObj, trans);
   return(trans);
}
inline DataHeader_p4* DataHeaderCnv_p4::createPersistent(const DataHeader* transObj){
   DataHeader_p4* pers = new DataHeader_p4();
   transToPers(transObj, pers);
   return(pers);
}

#endif
