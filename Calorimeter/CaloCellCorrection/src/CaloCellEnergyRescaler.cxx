/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCellEnergyRescaler.h" 
#include "CaloEvent/CaloCellContainer.h"
#include "CaloCondBlobObjs/CaloCondBlobFlt.h"


CaloCellEnergyRescaler::CaloCellEnergyRescaler (const std::string& type, 
						const std::string& name, 
						const IInterface* parent) :
  base_class (type, name, parent),
  m_corrValues(0) { 
  declareProperty("Folder",m_key="/CALO/Ofl/CellEnergyCorr",
		  "Key (=foldername) of AttrListCollection");
}
                                                                                

CaloCellEnergyRescaler::~CaloCellEnergyRescaler() {}


StatusCode CaloCellEnergyRescaler::initialize() {

  ATH_CHECK( detStore()->regFcn(&CaloCellEnergyRescaler::load,
                                dynamic_cast<CaloCellEnergyRescaler*>(this),
                                m_attrList,m_key) );
  return StatusCode::SUCCESS;
}


StatusCode CaloCellEnergyRescaler::load(IOVSVC_CALLBACK_ARGS) {

  ATH_MSG_INFO("in IOV callback function");
  if (m_corrValues) {
    ATH_MSG_INFO("Delete previously loaded correction values");
    delete m_corrValues;
    m_corrValues=0;
  }

  const coral::Blob& blob = (*m_attrList)["CaloCondBlob16M"].data<coral::Blob>();
  if (blob.size()<3) {
    ATH_MSG_INFO("Found empty blob, no corretion needed :-)");
  }
  else {
    m_corrValues=CaloCondBlobFlt::getInstance(blob);
    ATH_MSG_INFO("Database folder has values for " << m_corrValues->getNChans() << " channels and " << m_corrValues->getNGains() << " gains.");
  }
 
  return StatusCode::SUCCESS;
}


StatusCode
CaloCellEnergyRescaler::process (CaloCellContainer* theCaloCellContainer,
                                 const EventContext& /*ctx*/) const
{
  if (m_corrValues) {
    CaloCellContainer::iterator it=theCaloCellContainer->begin();
    CaloCellContainer::iterator it_e=theCaloCellContainer->end();
    for(;it!=it_e;++it) {
      CaloCell* theCell=(*it);
      const IdentifierHash& hash_id=theCell->caloDDE()->calo_hash();
      if (hash_id<m_corrValues->getNChans()) {
	const float& scale= m_corrValues->getData(hash_id,0,0);
	theCell->setEnergy(theCell->energy()*scale);
      }//end if hash_id<NChans
    }//end loop over cells
  }//end if have correction values
  return StatusCode::SUCCESS;
}
