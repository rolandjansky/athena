/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENTDATAMODELTPCNV_DATAHEADERCNV_P5_H
#define PERSISTENTDATAMODELTPCNV_DATAHEADERCNV_P5_H

/**
 *  @file DataHeaderCnv_p5.h
 *  @brief This file contains the class definition for the DataHeaderCnv_p5 and DataHeaderElementCnv_p5 classes.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: DataHeaderCnv_p5.h,v 1.1 2009-04-21 21:48:34 gemmeren Exp $
 **/

#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModelTPCnv/DataHeader_p5.h"

#include <map>

/** @class DataHeaderElementCnv_p5
 *  @brief This class provides the converter to customize the saving of DataHeaderElement_p5.
 **/
class DataHeaderElementCnv_p5 {
public:
   DataHeaderElementCnv_p5();
   virtual ~DataHeaderElementCnv_p5();

   void persToTrans(const DataHeaderElement_p5* pers, DataHeaderElement* trans, const DataHeaderForm_p5& form);
   void transToPers(const DataHeaderElement* trans, DataHeaderElement_p5* pers, DataHeaderForm_p5& form);

friend class DataHeaderCnv_p5;
};

/** @class DataHeaderCnv_p5
 *  @brief This class provides the converter to customize the saving of DataHeader_p5.
 **/
class DataHeaderCnv_p5 {
public:
   DataHeaderCnv_p5();
   virtual ~DataHeaderCnv_p5();

   DataHeader* createTransient(const DataHeader_p5* persObj);
   void persToTrans(const DataHeader_p5* pers, DataHeader* trans);
   DataHeader_p5* createPersistent(const DataHeader* transObj);
   void transToPers(const DataHeader* trans, DataHeader_p5* pers);

   void insertDHRef(DataHeader_p5* pers, const std::string& key, const std::string& strToken);

private:
   DataHeaderElementCnv_p5 m_elemCnv;
};

inline DataHeader* DataHeaderCnv_p5::createTransient(const DataHeader_p5* persObj) {
   DataHeader* trans = new DataHeader();
   persToTrans(persObj, trans);
   return(trans);
}
inline DataHeader_p5* DataHeaderCnv_p5::createPersistent(const DataHeader* transObj){
   DataHeader_p5* pers = new DataHeader_p5();
   transToPers(transObj, pers);
   return(pers);
}

#endif
