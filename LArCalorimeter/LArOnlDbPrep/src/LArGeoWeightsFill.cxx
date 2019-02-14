/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArOnlDbPrep/LArGeoWeightsFill.h"
#include "LArIdentifier/LArOnlineID.h"
#include "StoreGate/StoreGate.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include <fstream>

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "CoolKernel/StorageType.h"

LArGeoWeightsFill::LArGeoWeightsFill(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_onlineID(0),
  m_ttService("CaloTriggerTowerService")
{
  declareProperty("Key",m_key="GeoWeights");
  declareProperty("Dump",m_dump=false);
  declareProperty("OutFile",m_outFileName="out.txt");
  //declareProperty("InFile",m_inFileName="");

  declareProperty("Fill",m_fill=true);
}

LArGeoWeightsFill::~LArGeoWeightsFill() {}


StatusCode LArGeoWeightsFill::initialize() {

  ATH_MSG_DEBUG ( "start initialize()" );
  ATH_CHECK( detStore()->retrieve(m_onlineID,"LArOnlineID") );
  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( m_ttService.retrieve() );
  return StatusCode::SUCCESS;
}   

StatusCode LArGeoWeightsFill::stop() {

  ATH_MSG_DEBUG ( "start stop()" );

  const unsigned hashMax=m_onlineID->channelHashMax();

  if (m_fill) {

    ATH_MSG_INFO ( "Filling database" );

    coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
    //cool::RecordSpecification* spec = new cool::RecordSpecification(); 
    spec->extend("costheta", "blob");
    spec->extend("sinthetacosphi", "blob");
    spec->extend("sinthetasinphi", "blob");
    spec->extend("offlineTTid","blob");
  
    AthenaAttributeList* attr=new AthenaAttributeList(*spec);
    coral::Blob& costhetaBlob = (*attr)["costheta"].data<coral::Blob>();
    coral::Blob& sinthetacosphiBlob = (*attr)["sinthetacosphi"].data<coral::Blob>();
    coral::Blob& sinthetasinphiBlob = (*attr)["sinthetasinphi"].data<coral::Blob>();
    coral::Blob& offlineTTidBlob = (*attr)["offlineTTid"].data<coral::Blob>();


    costhetaBlob.resize(hashMax*sizeof(float));
    sinthetacosphiBlob.resize(hashMax*sizeof(float));
    sinthetasinphiBlob.resize(hashMax*sizeof(float));
    offlineTTidBlob.resize(hashMax*sizeof(uint32_t));

    float* pcostheta=static_cast<float*>(costhetaBlob.startingAddress());
    float* psinthetacosphi=static_cast<float*>(sinthetacosphiBlob.startingAddress());
    float* psinthetasinphi=static_cast<float*>(sinthetasinphiBlob.startingAddress());
    uint32_t* pofflineTTid=static_cast<uint32_t*>(offlineTTidBlob.startingAddress());

    ATH_CHECK( detStore()->record(attr,m_key) );

    const CaloDetDescrManager *theCaloDDM = CaloDetDescrManager::instance();
    if(!theCaloDDM){
      ATH_MSG_ERROR ( "Failed to return CaloDetDescrManager" );
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO ( "theCaloDDM retrieved" );
    SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
    const LArOnOffIdMapping* cabling{*cablingHdl};
    if(!cabling) {
        ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key());
        return StatusCode::FAILURE;
    }

    for (unsigned hs=0;hs<hashMax;++hs) {
      const HWIdentifier chid=m_onlineID->channel_Id(hs);

      if(!cabling->isOnlineConnected(chid)){
	ATH_MSG_DEBUG ( "cell chid: " << chid.get_compact() << " not connected channel, skip  " );
	//Set values for disconnected cells to 0
	pcostheta[hs]=0.0; 
	psinthetacosphi[hs]=0.0;
	psinthetasinphi[hs]=0.0;
	pofflineTTid[hs]=0;
	continue;
      }
      const Identifier id=cabling->cnvToIdentifier(chid);
      const CaloDetDescrElement *caloDDE = theCaloDDM->get_element(id);
      if(!caloDDE){
	ATH_MSG_ERROR ( "Failed to return CaloDetDescrElement" );
	return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG ( "hash, eta, phi: " << caloDDE->calo_hash() << ", " << caloDDE->eta() << ", " << caloDDE->phi() );

      const float eta = caloDDE->eta();
      const float phi = caloDDE->phi();
      const float v = 1./cosh(eta); // sintheta
      const float costheta=tanh(eta);
      const float sinthetacosphi=v*cos(phi);
      const float sinthetasinphi=v*sin(phi);

      pcostheta[hs]=costheta;
      psinthetacosphi[hs]=sinthetacosphi;
      psinthetasinphi[hs]=sinthetasinphi;
      pofflineTTid[hs]=m_ttService->whichTTID(id).get_identifier32().get_compact();
    } // end loop over hash ids
  }//end if FILL 
  
  
  if (m_dump) {
  
    const AthenaAttributeList* attr=0;

    ATH_CHECK( detStore()->retrieve(attr,m_key) );
    
    const coral::Blob& costhetaBlob = (*attr)["costheta"].data<coral::Blob>();
    const coral::Blob& sinthetacosphiBlob = (*attr)["sinthetacosphi"].data<coral::Blob>();
    const coral::Blob& sinthetasinphiBlob = (*attr)["sinthetasinphi"].data<coral::Blob>();
    const coral::Blob& offlineTTidBlob = (*attr)["offlineTTid"].data<coral::Blob>();

    const float* pcostheta=static_cast<const float*>(costhetaBlob.startingAddress());
    const float* psinthetacosphi=static_cast<const float*>(sinthetacosphiBlob.startingAddress());
    const float* psinthetasinphi=static_cast<const float*>(sinthetasinphiBlob.startingAddress());
    const uint32_t* pofflineTTid=static_cast<const uint32_t*>(offlineTTidBlob.startingAddress());


    std::ostream *out = &(std::cout); 
    std::ofstream outfile;
    if (m_outFileName.size()) {
      outfile.open(m_outFileName.c_str(),std::ios::out);
      if (outfile.is_open()) {
	ATH_MSG_INFO ( "Writing to file " << m_outFileName );
	out = &outfile;
      }
      else
	ATH_MSG_ERROR ( "Failed to open file " << m_outFileName );
    }

    for (unsigned hs=0;hs<hashMax;++hs) {
      const HWIdentifier chid=m_onlineID->channel_Id(hs);
      (*out) << std::hex << chid.get_identifier32().get_compact() << std::dec << " " 
	     << pcostheta[hs] << " " << psinthetacosphi[hs] << " " << psinthetasinphi[hs] 
	     << " 0x"<< std::hex << pofflineTTid[hs] << std::dec << std::endl;
    }
	

    if (outfile.is_open())
      outfile.close();
  }// end if m_dump
  
  return StatusCode::SUCCESS;
}
