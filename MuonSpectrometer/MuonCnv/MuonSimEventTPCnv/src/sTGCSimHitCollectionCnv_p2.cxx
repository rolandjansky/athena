/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/sTGCSimHit.h"
#include "MuonSimEventTPCnv/sTGCSimHitCollectionCnv_p2.h"
#include "MuonSimEventTPCnv/sTGCSimHitCollection_p2.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"

//#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// Athena
#include "StoreGate/StoreGateSvc.h"


//** This function is not supported in master because HepMcParticleLinkCnv_p2::transToPers(..) 
//** is not supported either.
void sTGCSimHitCollectionCnv_p2::transToPers(const sTGCSimHitCollection* , Muon::sTGCSimHitCollection_p2* , MsgStream &/*log*/ )
{
  throw std::runtime_error("sTGCSimHitCollectionCnv_p2::transToPers is not supported in this release!");
}


sTGCSimHitCollection* sTGCSimHitCollectionCnv_p2::createTransient(const Muon::sTGCSimHitCollection_p2* persObj, MsgStream &log) {
  std::auto_ptr<sTGCSimHitCollection> trans(new sTGCSimHitCollection("DefaultCollectionName",persObj->m_globalTime.size()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}


void sTGCSimHitCollectionCnv_p2::persToTrans(const Muon::sTGCSimHitCollection_p2* persCont, sTGCSimHitCollection* transCont, MsgStream &log) 
{
  HepMcParticleLinkCnv_p2 hepMcPLCnv;
    
  //transCont->reserve(persCont->m_sTGCId.size());//done in createTransient
  for (unsigned int i = 0; i < persCont->m_sTGCId.size(); i++) { 
    //post
    Amg::Vector3D position(persCont->m_stX[i], persCont->m_stY[i], persCont->m_stZ[i]);
    Amg::Vector3D direction(persCont->m_ptX[i], persCont->m_ptY[i], persCont->m_ptZ[i]); 

    HepMcParticleLink link;
    hepMcPLCnv.persToTrans(&persCont->m_partLink[i],&link, log);   

    transCont->Emplace(persCont->m_sTGCId[i], persCont->m_globalTime[i], position, persCont->m_particleEncoding[i], direction, persCont->m_depositEnergy[i], link);
  }
}

