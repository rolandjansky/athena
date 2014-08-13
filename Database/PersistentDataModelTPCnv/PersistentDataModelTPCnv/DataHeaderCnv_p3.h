/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENTDATAMODELTPCNV_DATAHEADERCNV_P3_H
#define PERSISTENTDATAMODELTPCNV_DATAHEADERCNV_P3_H

/**
 *  @file DataHeaderCnv_p3.h
 *  @brief This file contains the class definition for the DataHeaderCnv_p3 and DataHeaderElementCnv_p3 classes.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: DataHeaderCnv_p3.h,v 1.3 2009-04-21 21:48:34 gemmeren Exp $
 **/

#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModelTPCnv/DataHeader_p3.h"

/** @class DataHeaderElementCnv_p3
 *  @brief This class provides the converter to customize the saving of DataHeaderElement_p3.
 **/
class DataHeaderElementCnv_p3 {
public:
   DataHeaderElementCnv_p3();
   virtual ~DataHeaderElementCnv_p3();

   void persToTrans(const DataHeaderElement_p3* pers, DataHeaderElement* trans, const std::vector<std::string>& map);
   void transToPers(const DataHeaderElement* trans, DataHeaderElement_p3* pers, std::vector<std::string>& map);

friend class DataHeaderCnv_p3;
};

/** @class DataHeaderCnv_p3
 *  @brief This class provides the converter to customize the saving of DataHeader_p3.
 **/
class DataHeaderCnv_p3 {
public:
   DataHeaderCnv_p3();
   virtual ~DataHeaderCnv_p3();

   DataHeader* createTransient(const DataHeader_p3* persObj);
   void persToTrans(const DataHeader_p3* pers, DataHeader* trans);
   DataHeader_p3* createPersistent(const DataHeader* transObj);
   void transToPers(const DataHeader* trans, DataHeader_p3* pers);

   void insertDHRef(DataHeader_p3* pers, const std::string& key, const std::string& token);

private:
   DataHeaderElementCnv_p3 m_elemCnv;
};

inline DataHeader* DataHeaderCnv_p3::createTransient(const DataHeader_p3* persObj) {
   DataHeader* trans = new DataHeader();
   persToTrans(persObj, trans);
   return(trans);
}
inline DataHeader_p3* DataHeaderCnv_p3::createPersistent(const DataHeader* transObj){
   DataHeader_p3* pers = new DataHeader_p3();
   transToPers(transObj, pers);
   return(pers);
}

#endif
