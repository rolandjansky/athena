/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCondBlobAlgs/CaloCoolIdTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "Identifier/Identifier.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloCondBlobObjs/CaloCondUtils.h"

//_______________________________________________________________
CaloCoolIdTool::CaloCoolIdTool(const std::string& type,
			       const std::string& name, 
			       const IInterface* parent) : 
  AthAlgTool(type, name, parent),
  m_calo_id(0)
{
declareInterface<ICaloCoolIdTool>( this ); 
}

//
//_______________________________________________________________
CaloCoolIdTool::~CaloCoolIdTool()
{
}

//
//_______________________________________________________________
StatusCode
CaloCoolIdTool::initialize()
{
  StatusCode sc=detStore()->retrieve(m_calo_id,"CaloCell_ID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retrive CaloCell_ID" << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}

//
//_______________________________________________________________
unsigned int
CaloCoolIdTool::getCoolChannelId(unsigned int cellHash, unsigned int& subHash) const
{
  int subCalo(0);
  subHash = m_calo_id->subcalo_cell_hash(cellHash, subCalo);
  //std::cout << "Starting with hash=" << cellHash << ", SubCalo=" << subCalo << " subHash=" << subHash << std::endl;
  //=== EM calo
  if (subCalo==CaloCell_ID::LAREM) {
    //Sub-Calo hash gymastics to get sub-sub-calo hash (EMECA, EMBA, EMBC EMEC) 
    //(faster than converting hash to identfier and dealing with identifier)
    //Here are the magic numbers:
    // Hash order in LAREMCAL: 
    //Calo           Offset     
    //EMEC NEG       0              
    //EMB NEG        31872
    //EMB POS        31872 + 54784 = 86656
    //EMEC POS       31872 + 54784 + 54784 =141440

    if (subHash >= 141440) {
      subHash-=141440;
      return static_cast<unsigned int>(CaloCondUtils::EMECZPOS);
    }
    else if (subHash>=86656) {
      subHash-=86656;
      return static_cast<unsigned int>(CaloCondUtils::EMBZPOS);
    }
    else if (subHash>=31872) {
      subHash-=31872;
      return static_cast<unsigned int>(CaloCondUtils::EMBZNEG);
    }
    else
      return static_cast<unsigned int>(CaloCondUtils::EMECZNEG);
  }// end if LAr EM
  else if(subCalo==CaloCell_ID::LARHEC){  
    return static_cast<unsigned int>(CaloCondUtils::HEC);
  }
  //=== FCAL
  else if(subCalo==CaloCell_ID::LARFCAL){
    return static_cast<unsigned int>(CaloCondUtils::FCAL);
  }
  //=== TILE
  else if(subCalo==CaloCell_ID::TILE) {
    return static_cast<unsigned int>(CaloCondUtils::TILE);
  }
  
  //=== we should never go here

  msg(MSG::ERROR) << "cellHash is neither LAr nor Tile, something is deeply wrong" << endmsg;
  std::abort();
}
