/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAHEADERCNV_H
#define DATAHEADERCNV_H

/**
 *  @file DataHeaderCnv.h
 *  @brief This file contains the class definition for the DataHeaderCnv class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModelTPCnv/DataHeader_p5.h"
#include "PersistentDataModelTPCnv/DataHeaderCnv_p5.h"
#include "PersistentDataModelTPCnv/DataHeaderCnv_p6.h"

#include "GaudiKernel/IIncidentListener.h"

#include <map>

class  DataHeaderForm_p6;

typedef  T_AthenaPoolCustomCnv<DataHeader, DataHeader_p5>   DataHeaderCnvBase;

/** @class DataHeaderCnv
 *  @brief This class provides the converter to customize the saving of DataHeader.
 **/
class DataHeaderCnv : public DataHeaderCnvBase, virtual public IIncidentListener
{
   friend class CnvFactory<DataHeaderCnv>;
public:
   DataHeaderCnv(ISvcLocator* svcloc);
   ~DataHeaderCnv();

   virtual StatusCode initialize() override;

   /// Extend base-class conversion methods
   virtual StatusCode updateRep(IOpaqueAddress* pAddress, DataObject* pObject);
   virtual StatusCode updateRepRefs(IOpaqueAddress* pAddress, DataObject* pObject);

   virtual StatusCode DataObjectToPool(DataObject* pObj, const std::string& tname);

   DataHeader_p5* poolReadObject_p5();
   DataHeader_p6* poolReadObject_p6( DataHeaderForm_p6*& );

   virtual DataHeader_p5* createPersistent(DataHeader* transObj);
   virtual DataHeader* createTransient();

private:
   /// Incident service handle listening for EndFile.
   virtual void handle(const Incident& incident) override;
 
protected:
   DataHeaderCnv_p6 m_tpInConverter_p6;
  
   /// Local DHForm cache indexed by filename or reference
   std::map<std::string,  DataHeaderForm_p6*>    m_persFormMap;

   DataHeaderCnv_p5 m_TPconverter;
   RootType m_mapClassDesc;
   mutable std::string m_dhFormMdx;
   mutable DataHeaderForm_p5* m_dhForm;
};

#endif
