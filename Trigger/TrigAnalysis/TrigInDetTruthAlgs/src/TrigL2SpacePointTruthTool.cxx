/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigL2SpacePointTruthTool tool
// -------------------------------
// ATLAS Collaboration
//
// 21.09.2011 Tool created
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: D.Emeliyanov@rl.ac.uk
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>

#include "TrigInDetEvent/TrigSiSpacePoint.h"

#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/PixelCluster.h"

// detector geometry and helper classes
#include "InDetIdentifier/SCT_ID.h" 
#include "InDetIdentifier/PixelID.h"

// monte carlo edm
#include "GenzModuleEvent/KineHepMcmap.h"
#include "AtlasHepMC/GenParticle.h"

#include "TrigInDetToolInterfaces/ITrigL2SpacePointTruthTool.h"
#include "TrigL2SpacePointTruthTool.h"

TrigL2SpacePointTruthTool::TrigL2SpacePointTruthTool(const std::string& t, 
					     const std::string& n,
					     const IInterface*  p ): AthAlgTool(t,n,p)
{
  declareInterface< ITrigL2SpacePointTruthTool >( this );
  declareProperty("PixelSDO_MapName", m_pixel_SDO_Map = "PixelSDO_Map");
  declareProperty("SCT_SDO_MapName", m_SCT_SDO_Map = "SCT_SDO_Map");
}

StatusCode TrigL2SpacePointTruthTool::initialize()
{
  StatusCode sc = AlgTool::initialize();

  ATH_MSG_INFO("In initialize..."); 

 // Get SCT & pixel Identifier helpers

  if (detStore()->retrieve(m_PIXid, "PixelID").isFailure()) { 
     ATH_MSG_FATAL("Could not get Pixel ID helper");
     return StatusCode::FAILURE;
  }
  if (detStore()->retrieve(m_SCTid, "SCT_ID").isFailure()) {  
     ATH_MSG_FATAL("Could not get SCT ID helper");
     return StatusCode::FAILURE;
  }

  //   Get StoreGate service
  //

  ATH_MSG_INFO("TrigL2SpacePointTruthTool constructed ");
  return sc;
}

StatusCode TrigL2SpacePointTruthTool::finalize()
{
  StatusCode sc = AlgTool::finalize(); 
  return sc;
}

TrigL2SpacePointTruthTool::~TrigL2SpacePointTruthTool()
{

}

void TrigL2SpacePointTruthTool::getBarCodes(std::vector<TrigSiSpacePoint*>& vsp, std::vector<int>& vBar) {

  vBar.clear();
  if(!GetTruthMaps()) {
    for(std::vector<TrigSiSpacePoint*>::const_iterator it  = vsp.begin();it!=vsp.end();++it) {
      vBar.push_back(-1);
    }
    return;
  }
  for(std::vector<TrigSiSpacePoint*>::const_iterator it  = vsp.begin();it!=vsp.end();++it) {
    int nBestBarCode=-1;
    TrigSiSpacePoint* pSP = (*it);
    if(pSP->clusters().second==NULL) {
      std::vector<HepMcParticleLink> vPL;
      const Trk::PrepRawData* pPIXclus = pSP->clusters().first;
      PIX_ClusterTruth(pPIXclus,vPL);
      nBestBarCode = analyzeClusterMatchings(vPL);
      
    }
    else {
      std::vector<HepMcParticleLink> vPL; 
      const Trk::PrepRawData* pSCTclus;
      pSCTclus = pSP->clusters().first;
      SCT_ClusterTruth(pSCTclus,vPL);
      int uCode = analyzeClusterMatchings(vPL);
      pSCTclus = pSP->clusters().second;
      vPL.clear();
      SCT_ClusterTruth(pSCTclus,vPL);
      int vCode = analyzeClusterMatchings(vPL);
      if(vCode!=uCode) {
	nBestBarCode=-2;
      }
      else {
	nBestBarCode=vCode;
      }
    }
    pSP->barCode(nBestBarCode);
    vBar.push_back(nBestBarCode);
  }
}

void TrigL2SpacePointTruthTool::getBarCodes(std::vector<TrigSiSpacePointBase>& vsp, std::vector<int>& vBar) {

  vBar.clear();
  if(!GetTruthMaps()) {
    for(std::vector<TrigSiSpacePointBase>::const_iterator it  = vsp.begin();it!=vsp.end();++it) {
      vBar.push_back(-1);
    }
    return;
  }
  for(std::vector<TrigSiSpacePointBase>::const_iterator it  = vsp.begin();it!=vsp.end();++it) {
    int nBestBarCode=-1;
    TrigSiSpacePointBase& pSP = const_cast<TrigSiSpacePointBase&>(*it);
    if(pSP.offlineSpacePoint()->clusterList().second==NULL) {
      std::vector<HepMcParticleLink> vPL;
      const Trk::PrepRawData* pPIXclus = pSP.offlineSpacePoint()->clusterList().first;
      PIX_ClusterTruth(pPIXclus,vPL);
      nBestBarCode = analyzeClusterMatchings(vPL);
      
    }
    else {
      std::vector<HepMcParticleLink> vPL; 
      const Trk::PrepRawData* pSCTclus;
      pSCTclus = pSP.offlineSpacePoint()->clusterList().first;
      SCT_ClusterTruth(pSCTclus,vPL);
      int uCode = analyzeClusterMatchings(vPL);
      pSCTclus = pSP.offlineSpacePoint()->clusterList().second;
      vPL.clear();
      SCT_ClusterTruth(pSCTclus,vPL);
      int vCode = analyzeClusterMatchings(vPL);
      if(vCode!=uCode) {
	nBestBarCode=-2;
      }
      else {
	nBestBarCode=vCode;
      }
    }
    pSP.barCode(nBestBarCode);
    vBar.push_back(nBestBarCode);
  }
}

void TrigL2SpacePointTruthTool::SCT_ClusterTruth(const Trk::PrepRawData* pSCTclus,std::vector<HepMcParticleLink>& pl) {
  
  if(pSCTclus==NULL) return;

  ATH_MSG_DEBUG("SCTspTruth() : cluster-id: " << m_SCTid->print_to_string((*pSCTclus).identify()));
    
  // get list of Raw Data Objects identifiers in cluster
  const std::vector<Identifier> clusRDOids = pSCTclus->rdoList();
    
  ATH_MSG_DEBUG("SCTspTruth() : cluster has " << clusRDOids.size() << " RDO identifiers"); 
    
  std::vector<Identifier>::const_iterator rdoIter=clusRDOids.begin();
  std::vector<Identifier>::const_iterator lastRDO=clusRDOids.end();
    
  // loop over RDO identifiers and get collection of InDetSimData
  for (int iRDO=0; rdoIter != lastRDO; ++rdoIter, ++iRDO) {
    ATH_MSG_DEBUG("SCTspTruth() : Doing RDO nr " << iRDO); 
    
    // find InDetSimData object corresponding to this RDO from the
    // SCT InDetSimDataCollection map; a InDetSimData is basically a 
    // vector of deposits (typedef of pair<HepMcParticleLink,float>) 
    // a InDetSimDataCollection is a map<Identifier,InDetSimData>
    InDetSimDataCollection::const_iterator clusSimData = m_id2SimDataMapSCT->find(*rdoIter);
    
    // check if the InDetSimData corresponding to the RDO was found
    if (clusSimData == m_id2SimDataMapSCT->end()) {
      ATH_MSG_DEBUG("SCTspTruth() : Cannot find simulation info for " <<  m_SCTid->print_to_string(*rdoIter));
      continue;
    } else {
      // add all GenParticles which contributed to this cluster into vector
      updatePLvector(pl,clusSimData);
    }
  }
}

void TrigL2SpacePointTruthTool::PIX_ClusterTruth(const Trk::PrepRawData* pPIXclus,std::vector<HepMcParticleLink>& pl) {
  
  if (!pPIXclus) return;
    
    ATH_MSG_DEBUG("PIXspTruth() : cluster-id: " << m_PIXid->print_to_string((*pPIXclus).identify()));
    
  // get list of Raw Data Objects identifiers in cluster
  const std::vector<Identifier> clusRDOids = pPIXclus->rdoList(); 
    
  ATH_MSG_DEBUG("PIXspTruth() : cluster has " << clusRDOids.size() << " RDO identifiers"); 
    
  std::vector<Identifier>::const_iterator rdoIter = clusRDOids.begin();
  std::vector<Identifier>::const_iterator lastRDO = clusRDOids.end();
    
  // loop over RDO identifiers and get collection of InDetSimData
  for (int iRDO=0; rdoIter != lastRDO; ++rdoIter, ++iRDO) {
      
    ATH_MSG_DEBUG("PIXspTruth() : Doing RDO nr " << iRDO); 
    
    if ( ! (*rdoIter).is_valid() ) {
	ATH_MSG_WARNING("Invalid identifier from pixel SP->rdoList() !!");
      continue;
    } else {
	ATH_MSG_DEBUG("Pixel identifier: " << m_PIXid->print_to_string(*rdoIter));
    }

    // find InDetSimData object corresponding to this RDO from the
    // PIXel InDetSimDataCollection map; a InDetSimData is basically a 
    // vector of deposits (typedef of pair<HepMcParticleLink,float>) 
    // a InDetSimDataCollection is a map<Identifier,InDetSimData>
    InDetSimDataCollection::const_iterator clusSimData = m_id2SimDataMapPIX->find(*rdoIter);
      
    if (clusSimData == m_id2SimDataMapPIX->end()) {
      // we didn't find the ID in the map - check if it is a ganged pixel
      ATH_MSG_DEBUG("PIXspTruth() : Cannot find simulation info for " 
		  << m_PIXid->print_to_string(*rdoIter) <<" : looking for ganged pixel");
      
      const unsigned int phi_index = m_PIXid->phi_index(*rdoIter);
      unsigned int ganged_phi_index=0;
      
      switch (phi_index) {
      case 153: ganged_phi_index = 160; break;
      case 155: ganged_phi_index = 161; break;
      case 157: ganged_phi_index = 162; break;
      case 159: ganged_phi_index = 163; break;
      case 168: ganged_phi_index = 164; break;
      case 170: ganged_phi_index = 165; break;
      case 172: ganged_phi_index = 166; break;
      case 174: ganged_phi_index = 167; break;
      default: ganged_phi_index =0;
      }

      if ( ganged_phi_index != 0) {
        // it is a ganged pixel : get the ID of the partner.
        const Identifier new_rdoID = m_PIXid->pixel_id(m_PIXid->barrel_ec(*rdoIter),
						       m_PIXid->layer_disk(*rdoIter),
						       m_PIXid->phi_module(*rdoIter),
						       m_PIXid->eta_module(*rdoIter),
						       ganged_phi_index, 
						       m_PIXid->eta_index(*rdoIter));
    
        ATH_MSG_DEBUG("PIXspTruth() : ganged to pixel " <<  m_PIXid->print_to_string(new_rdoID));

        clusSimData = m_id2SimDataMapPIX->find(new_rdoID);
      }
      if(clusSimData == m_id2SimDataMapPIX->end() )  {
        // check again, in case ganged pixel is also not in map
        ATH_MSG_DEBUG("SCTspTruth() : Cannot find simulation info for " 
  		    <<  m_PIXid->print_to_string(*rdoIter));
        continue;
      }
    }
    // add all GenParticles which contributed to this cluster into vector
    updatePLvector(pl,clusSimData);
  }
}

int TrigL2SpacePointTruthTool::analyzeClusterMatchings(std::vector<HepMcParticleLink>& vPL) {

  int bestCode=-1;
  std::map<int,int> codeMap;
  int nTotalParticles = vPL.size();
  if(nTotalParticles==0) 
    return bestCode;

  nTotalParticles=0;

  for(std::vector<HepMcParticleLink>::iterator pIt = vPL.begin(); pIt!=vPL.end();++pIt) {
    HepMcParticleLink partLink =  (*pIt);
    int barCode = partLink.barcode();
    int eventIndex = partLink.eventIndex();
    if(eventIndex!=0) continue;
    nTotalParticles++;
    std::map<int,int>::iterator it = codeMap.find(barCode);
    if(it==codeMap.end()) {
      codeMap.insert(std::pair<int,int>(barCode,1));
    }
    else {
      (*it).second++;
    }
  }
  if(nTotalParticles==0) 
    return bestCode;
  int max=-1;
  for(std::map<int,int>::iterator it=codeMap.begin();it!=codeMap.end();++it) {
    if((*it).second>max) {
      max=(*it).second;
      bestCode=(*it).first;
    }
  }
  float P = (1.0*max)/nTotalParticles;
  if(P>0.5) return bestCode;
  else return -1;
}


void TrigL2SpacePointTruthTool::updatePLvector(std::vector<HepMcParticleLink>& pl, 
					       InDetSimDataCollection::const_iterator& simData)
{

  // get the vector of deposits for each GenParticle from the InDetSimData 
  const std::vector< std::pair<HepMcParticleLink,float> >& simDeposits = ((*simData).second).getdeposits();
  
  ATH_MSG_DEBUG("updatePLvector() : RDO has " 
               << simDeposits.size() << " deposits"); 
  
  // loop over the deposits and get info from HepMcParticleLinks
  std::vector< std::pair<HepMcParticleLink,float> >::const_iterator depIter = simDeposits.begin();
  std::vector< std::pair<HepMcParticleLink,float> >::const_iterator lastDep = simDeposits.end();
  
  for (int iDep=0 ; depIter != lastDep; ++depIter, ++iDep) {
      
    ATH_MSG_DEBUG("updatePLvector() : Doing deposit " << iDep); 
      
    // get the HepMcParticleLink from the Deposit particle link and check it's ok
    HepMcParticleLink partLink =  (*depIter).first;

    ATH_MSG_DEBUG("updatePLvector() : Deposit " << iDep 
           << ": kine " << partLink.barcode() 
           << ", event index " << partLink.eventIndex() 
           << ", energy deposit " << (*depIter).second); 
    
    if (partLink.isValid()) {
        
      // add the GenParticle to vector if it's not there yet
      std::vector<HepMcParticleLink>::iterator gpIter = pl.begin();
      std::vector<HepMcParticleLink>::iterator lastGP = pl.end();
      bool putGPinVec = true;
      for (; gpIter != lastGP; ++gpIter) {
        if (partLink == (*gpIter)) {
	  putGPinVec = false;
	  break;
	}
      }
      if (putGPinVec) {
        pl.push_back(partLink);
        ATH_MSG_DEBUG("updatePLvector() : Added particle to vector: " 
               << pl.size() << " matches so far");
      }
    }
  }
  return;
}

bool TrigL2SpacePointTruthTool::GetTruthMaps() {

  m_havePIXmap=false;
  m_haveSCTmap=false;

   if (evtStore()->contains<InDetSimDataCollection>(m_pixel_SDO_Map)) {
     // retrieve Pixel SDO map for this event
     StatusCode sc = evtStore()->retrieve(m_id2SimDataMapPIX, m_pixel_SDO_Map);
     if (sc.isFailure()) {
        ATH_MSG_DEBUG("GetTruthMaps() : Could not retrieve " << m_pixel_SDO_Map << "!");
     } else {
       m_havePIXmap=true;
     }
   } else {
     ATH_MSG_DEBUG("GetTruthMaps() : Could not retrieve " 
					      << m_pixel_SDO_Map << "!");
   }
  
   if (evtStore()->contains<InDetSimDataCollection>(m_SCT_SDO_Map)) {
     // retrieve SCT SDO map for this event
     StatusCode sc = evtStore()->retrieve(m_id2SimDataMapSCT, m_SCT_SDO_Map);
     if (sc.isFailure()) {
       ATH_MSG_DEBUG("GetTruthMaps() : Could not retrieve " << m_SCT_SDO_Map << "!");
     } else {
       m_haveSCTmap=true;
     }
   } else {
     ATH_MSG_DEBUG("GetTruthMaps() : Could not retrieve " << m_SCT_SDO_Map << "!");
   }
   
   return (m_havePIXmap && m_haveSCTmap);

 }


