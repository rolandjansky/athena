/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
// TopInputs - container to hold inputs for Top Reconstrution Algorithms
// author - Dustin Urbaniec <durbanie@nevis.columbia.edu>


#ifndef TOP_INPUTS_H
#define TOP_INPUTS_H

#include "TopInputs/TopMuonMap.h"
#include "TopInputs/TopElectronMap.h"
#include "TopInputs/TopPhotonMap.h"
#include "TopInputs/TopTauJetMap.h"
#include "TopInputs/TopJetMap.h"
#include "TopInputs/TopBJetMap.h"
#include "TopInputs/TopTrackMap.h"
#include "MissingETEvent/MissingET.h"
#include "DataModel/DataLink.h"
#include "SGTools/CLASS_DEF.h"
#include <cstdio>

namespace TopRec {

  class TopInputs {
  public:
    
    TopInputs()  {;}
    ~TopInputs() {;}

    // Retrieve the maps
    const TopMuonMap& muonMap()          const {return m_muonMap;}
    const TopElectronMap& electronMap()  const {return m_electronMap;}
    const TopPhotonMap& photonMap()      const {return m_photonMap;}
    const TopTauJetMap& tauJetMap()      const {return m_tauJetMap;}
    const TopJetMap& jetMap()            const {return m_jetMap;}
    const TopBJetMap& bJetMap()          const {return m_bJetMap;}
    const TopTrackMap& trackMap()        const {return m_trackMap;}
    const DataLink<MissingET>& metLink() const {return m_metLink;}
    const MissingET * metObjPtr()        const {return m_metLink.cptr();}

    void retrieve(TopMuonMap& muonMap)          const {muonMap=m_muonMap;}
    void retrieve(TopElectronMap& electronMap)  const {electronMap=m_electronMap;}
    void retrieve(TopPhotonMap& photonMap)      const {photonMap=m_photonMap;}
    void retrieve(TopTauJetMap& tauJetMap)      const {tauJetMap=m_tauJetMap;}
    void retrieve(TopJetMap& jetMap)            const {jetMap=m_jetMap;}
    void retrieve(TopTrackMap& trackMap)        const {trackMap=m_trackMap;}
    void retrieve(DataLink<MissingET>& metLink) const {metLink=m_metLink;}

    // Insert individual pairs
    void insertMuonOverlapPair(TopMuonOverlapPair muonPair)             {m_muonMap.insert(muonPair);}
    void insertElectronOverlapPair(TopElectronOverlapPair electronPair) {m_electronMap.insert(electronPair);}
    void insertPhotonOverlapPair(TopPhotonOverlapPair photonPair)       {m_photonMap.insert(photonPair);}
    void insertTauJetOverlapPair(TopTauJetOverlapPair tauJetPair)       {m_tauJetMap.insert(tauJetPair);}
    void insertJetOverlapPair(TopJetOverlapPair jetPair)                {m_jetMap.insert(jetPair);}
    void insertBJetOverlapPair(TopBJetOverlapPair bJetPair)             {m_bJetMap.insert(bJetPair);}    
    void insertTrackOverlapPair(TopTrackOverlapPair trackPair)          {m_trackMap.insert(trackPair);}    

    // Set the whole map
    void setMuonMap(const TopMuonMap& muonMap)             {m_muonMap=muonMap;}
    void setElectronMap(const TopElectronMap& electronMap) {m_electronMap=electronMap;}
    void setPhotonMap(const TopPhotonMap& photonMap)       {m_photonMap=photonMap;}
    void setTauJetMap(const TopTauJetMap& tauJetMap)       {m_tauJetMap=tauJetMap;}
    void setJetMap(const TopJetMap& jetMap)                {m_jetMap=jetMap;}
    void setBJetMap(const TopBJetMap& bJetMap)             {m_bJetMap=bJetMap;}
    void setTrackMap(const TopTrackMap& trackMap)          {m_trackMap=trackMap;}
    void setMETLink(const DataLink<MissingET>& metLink)    {m_metLink=metLink;}
    
    // Set the event flags
    void setElectronTrigger(bool isElectronTrigger) {m_isElectronTrigger=isElectronTrigger;}
    void setMuonTrigger(bool isMuonTrigger)         {m_isMuonTrigger=isMuonTrigger;}
    void setEleMuOverlap(bool isEleMuOverlap)       {m_isEleMuOverlap=isEleMuOverlap;}
    
    // Retrieve the event flags
    bool isElectronTrigger() const {return m_isElectronTrigger;}
    bool isMuonTrigger()     const {return m_isMuonTrigger;}
    bool isEleMuOverlap()    const {return m_isEleMuOverlap;}

  private:
    
    TopMuonMap          m_muonMap;
    TopElectronMap      m_electronMap;
    TopPhotonMap        m_photonMap;
    TopTauJetMap        m_tauJetMap;
    TopJetMap           m_jetMap;
    TopBJetMap          m_bJetMap;
    TopTrackMap         m_trackMap;
    DataLink<MissingET> m_metLink;
    
    bool m_isElectronTrigger;
    bool m_isMuonTrigger;
    bool m_isEleMuOverlap; 
    
  }; // TopInputs

} // namespace

CLASS_DEF(TopRec::TopInputs, 43798243, 1)

#endif // TOP_INPUTS_H
