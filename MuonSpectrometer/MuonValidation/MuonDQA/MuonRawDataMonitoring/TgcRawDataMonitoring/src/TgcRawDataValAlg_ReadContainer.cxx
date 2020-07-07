/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package: TgcRawDataValAlg
// Authors:  N. Benekos(Illinois), M. Ishino(ICEPP), H.Kurashige (Kobe), A.Ishikawa (Kobe)  akimasa.ishikawa@cern.ch, M.King(Kobe) kingmgl@stu.kobe-u.ac.jp
// Original: Apr. 2008
// Modified: June 2011
//
// DESCRIPTION:
// Subject: TGCLV1-->Offline Muon Data Quality/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"

// GeoModel
#include "MuonReadoutGeometry/TgcReadoutParams.h"

#include "Identifier/Identifier.h"

// MuonRDO
#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoIdHash.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
 
#include "TgcRawDataMonitoring/TgcRawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include <inttypes.h> 

#include <sstream>
#include <cmath>

void
TgcRawDataValAlg::clearVectorsArrays(){
  // Reset Counters
  m_nColl = 0; // Number of TGC chambers with hits
  m_nPrd = 0;  // Total number of TGC prd digits
  
  // clear Wire Strip vectors
  for(int ac=0;ac<2;ac++)
    for(int ws=0;ws<2;ws++){
      m_nHits[ac][ws]=0;
      m_hitStation[ac][ws].clear();
      m_hitLayer[ac][ws].clear();
      m_hitChannel[ac][ws].clear();
      m_hitSector[ac][ws].clear();
      m_hitSectorPhi[ac][ws].clear();
      m_hitEtaIndex[ac][ws].clear();
      m_hitPhiIndex[ac][ws].clear();
      m_hitPhi48[ac][ws].clear();
      m_hitPhi4[ac][ws].clear();
      m_hitPosR[ac][ws].clear();
      m_hitPosPhi[ac][ws].clear();
      
      for(int pcn=0;pcn<3;pcn++)
        for(int eta=0;eta<6;eta++)
          for(int phi48=0;phi48<48;phi48++)
            for(int layer=0;layer<9;layer++)
              m_hitIdVects[pcn][ac][ws][eta][phi48][layer].clear();
    }// ws
  
}// EOF


void
TgcRawDataValAlg::readTgcPrepDataContainer(const Muon::TgcPrepDataContainer* tgc_prep_container, int pcn){
  ///////////////////////////////////////////////////////////////////////////
  // Loop over TgcPrepDataContainer
  if(pcn!=TOTA){
    // MuonDetectorManager from the conditions store
    SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
    const MuonGM::MuonDetectorManager* MuonDetMgr = DetectorManagerHandle.cptr(); 
    if(MuonDetMgr==nullptr){
      ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
      return; 
    } 

    // PREV/CURR/NEXT timing
    Muon::TgcPrepDataContainer::const_iterator container_end=tgc_prep_container->end();
    for(Muon::TgcPrepDataContainer::const_iterator containerIt=tgc_prep_container->begin();
        containerIt!=container_end;
        ++containerIt){
      if(containerIt==container_end || (*containerIt)->size()==0)continue; //check if there are counts  
      ATH_MSG_DEBUG( "size of tgc collection is " << (*containerIt) -> size()  );
      // Increment Collection counter
      m_nColl++;
      
      // Get chamber ID
      Identifier chamberID = (*containerIt)->identify();
      
      // Get chamber indexes
      int stationName = int(m_idHelperSvc->tgcIdHelper().stationName(chamberID));//unsigned integer 41:T1F 42:T1E 43:T2F 44:T2E 45:T3F 46:T3E 47:T4F 48:T4E (T4 means inner small wheel TGCs, EI/FI)
      int stationEta  = int(m_idHelperSvc->tgcIdHelper().stationEta(chamberID)); //backward:[-5,-1], forward:[1,5], (1 or -1 at lowest R)
      int stationPhi  = int(m_idHelperSvc->tgcIdHelper().stationPhi(chamberID)); //forward:[1:24], endcap:[1:48], EI:[1:21], FI:[1:24]
      
      int fe = (stationName==42||stationName==44||stationName==46||stationName==48);// 0:forward, 1:endcap
      int ac = (stationEta<0);
      
      // Get isEIFI index
      int isEIFI=0;
      if(stationName==47||stationName==48) isEIFI=1;
      
      // Get chamber phi indexes
      int phi48   = stationPhi-1;//[0:47]
      int pseudoSectorPhiEIFI =-1;//[0:3]  for EIFI
      int pseudoSectorEIFI    =-1;//[1:12] for EIFI
      if(isEIFI){
        if(fe==1) phi48 = EIphi21toEIFIphi24(phi48);//EI [0:20] -> EIFI [0:23]
        
        phi48*=2;//[0:2:46]
        // phi48ToSectorPhi4 is designed for the midstation.
        // The EIFI actually has 4 sectors with 6 phi indexes per sector, however it can be forced into the sector12-phi4 scheme for convenience
        phi48ToSectorPhi4(phi48,pseudoSectorEIFI, pseudoSectorPhiEIFI);
      }
      else if(fe==0){// Midstation Forward
        phi48*=2;//[0:2:46]
      }
      
      // loop over TgcPrepDataCollection
      Muon::TgcPrepDataCollection::const_iterator collection_end=(*containerIt)->end();
      for(Muon::TgcPrepDataCollection::const_iterator collectionIt=(*containerIt)->begin();
          collectionIt!= collection_end;
          ++collectionIt){
	if(pcn<0){
	  if (((*collectionIt)->getBcBitMap()&Muon::TgcPrepData::BCBIT_PREVIOUS)==Muon::TgcPrepData::BCBIT_PREVIOUS) pcn=PREV;
	  if (((*collectionIt)->getBcBitMap()&Muon::TgcPrepData::BCBIT_CURRENT)==Muon::TgcPrepData::BCBIT_CURRENT) pcn=CURR;
	  if (((*collectionIt)->getBcBitMap()&Muon::TgcPrepData::BCBIT_NEXT)==Muon::TgcPrepData::BCBIT_NEXT) pcn=NEXT;
	}
        // Increment Prd Counter
        if(pcn==CURR)m_nPrd++;
        
        // Get element ID
        Identifier elementID = (*collectionIt)->identify();
        
        // Get element indexes
        int gasGap  = int(m_idHelperSvc->tgcIdHelper().gasGap(elementID)); //increase with |Z|, doublet:[1:2], triplet:[1:3]
        int channel = int(m_idHelperSvc->tgcIdHelper().channel(elementID));//channel in a chamber
        int ws      = int(m_idHelperSvc->tgcIdHelper().isStrip(elementID));//[0:1]
        
        // Get indexes for EtaPhi numbering scheme
        // tgc[0:3] TGC index, gasgap[0:2] GasGap index,
        // eta[0:5] StationEta index, phi[0:47](MidEndcap) [0:23](MidForward) [0:23](EIFI) StationPhi index
        int tgc, gasgap, eta, phi;
        getEtaPhiBasedNumbering(stationName,gasGap,stationEta, stationPhi,
                                tgc,gasgap,eta,phi);
        // Get indexes for Sector numbering scheme
        // layer[0:8] Layer index, sector[0:11](Mid)[0:3](EIFI) Sector index
        // dummy1[0:5] StationEta index, phiInSector[0:4](MidEndcap) [0:2](MidForward) [0:5](EIFI) Phi index inside sector
        int layer,sector,dummy1,sectorPhi;
        getSectorBasedNumbering(stationName, gasGap, stationEta, stationPhi,
                                layer, sector, dummy1, sectorPhi);
                
        // Get channel position
        const MuonGM::TgcReadoutElement* pReadoutElementTGC = MuonDetMgr->getTgcReadoutElement(elementID);
        const Amg::Vector3D channelPos = pReadoutElementTGC->channelPos(elementID); // attention
	//const HepGeom::Point3D<double> channelPos = pReadoutElementTGC->channelPos(elementID);        

        // Increment hit counter for histograms
        m_nHits[ac][ws]++;
        
        // Check whether current channel already exists in ID vector
        int ssize = m_hitIdVects[pcn][ac][ws][eta][phi48][layer].size();
        bool multihit_inchannel=false;
        for(int hit=0;hit<ssize;hit++){
          int ch=m_idHelperSvc->tgcIdHelper().channel(m_hitIdVects[pcn][ac][ws][eta][phi48][layer].at(hit));
          if(ch == channel){
            multihit_inchannel=true;
            break;
          }
        }
        
        // Only add current ID to vector if there is currently no hit from this channel
        if(multihit_inchannel==false ){
          m_hitIdVects[pcn][ac][ws][eta][phi48][layer].push_back(elementID);
        }
        
        if(pcn==CURR){
          // Fill hit variable vectors
          m_hitStation[ac][ws].push_back(stationName);
          m_hitLayer[ac][ws].push_back(layer);
          m_hitChannel[ac][ws].push_back(channel);
          m_hitSector[ac][ws].push_back(sector);
          m_hitSectorPhi[ac][ws].push_back(sectorPhi);
          m_hitEtaIndex[ac][ws].push_back(eta);
          m_hitPhiIndex[ac][ws].push_back(phi);
          m_hitPhi48[ac][ws].push_back(phi48);
          m_hitPhi4[ac][ws].push_back(isEIFI?pseudoSectorPhiEIFI:sectorPhi);
          m_hitPosR[ac][ws].push_back(channelPos.perp()); 
          m_hitPosPhi[ac][ws].push_back(channelPos.phi());
        }
        
        ATH_MSG_DEBUG( " TgcPrepData number:  " << m_nPrd  );
        if(m_nPrd>maxPrd-1){
          ATH_MSG_WARNING( "Maximum number of TgcPrepData in the Histograms reached: " << maxPrd  );
          return;
        }
      }// Collection
      
      ATH_MSG_DEBUG( " TgcCollection number  " << m_nColl  );
      if(m_nColl>maxColl-1){
        ATH_MSG_WARNING( "Maximum number of TgcPrepDataCollection in the TGCRawData Monitoring Histos reached: "
                         << maxColl  );
        return;
      } 
    }// Container
  }// if PCN
  
  return;
}
