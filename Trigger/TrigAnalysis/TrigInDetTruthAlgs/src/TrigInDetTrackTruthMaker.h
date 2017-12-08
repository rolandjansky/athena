/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         TrigInDetTrackTruthMaker.h        
 **
 **   Description:  - Finds GenParticles which match a TrigInDetTrack,
 **                   creates a TrigInDetTrackTruth for each track and 
 **                   fills a TrigInDetTrackTruthMap with them.
 **
 **   Author:       R.Goncalo    
 **
 **   Created:      Sat Jan  24 19:55:56 GMT 2006
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/

#ifndef TRIGINDETTRACK_TRUTH_MAKER_H
#define TRIGINDETTRACK_TRUTH_MAKER_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"

// core stuff
// #include "GaudiKernel/MsgStream.h"
// #include "StoreGate/StoreGateSvc.h"

// tracking stuff 
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetTruthEvent/TrigInDetTrackTruthMap.h"
class TrigIDHitStats;

// offline inner detector truth EDM
class DetailedTrackTruth;
class DetailedTrackTruthCollection;
class TruthTrajectory;

// detector geometry and helper classes
#include "InDetSimData/InDetSimDataCollection.h"
class SCT_ID;
class PixelID;
class TRT_ID;

// monte carlo edm
class KineHepMcmap;
class GenParticle;


using namespace std;

/** @class TrigInDetTrackTruthMaker
 *  This is TrigInDetTrackTruthMaker */
class TrigInDetTrackTruthMaker : public AthAlgorithm
{

 public:
  TrigInDetTrackTruthMaker(const std::string &name, ISvcLocator *pSvcLocator); //!< Algorithm constructor
  ~TrigInDetTrackTruthMaker(); //!< Algorithm deconstructor

  /** IAlgorithm virtual methods to implement */
  StatusCode initialize(); //!< Initialisation
  StatusCode execute();   //!< Execution
  StatusCode finalize(); //!< Finalisation

 private:

  /** StoreGate key for truth map and flags to be set by properties */
  std::string m_trackTruthMapKey;
  unsigned int m_minNrMatchHits;
  std::vector<std::string> m_TrigInDetTrackCollectionKeys;

  /** SDO maps for */
  std::string m_pixel_SDO_Map; //!< Pixel
  std::string m_SCT_SDO_Map; //!< SCT
  std::string m_TRT_SDO_Map; //!< TRT

  /** Internal properties */
  bool m_havePIXmap; //!< set for each event
  bool m_haveSCTmap;
  bool m_haveTRTmap;

  /** Pointers to Identifier helper classes for */
  const PixelID* m_PIXid; //!< Pixel detector data
  const SCT_ID*  m_SCTid; //!< SCT detector data
  const TRT_ID*  m_TRTid; //!< TRT detector data

  /** Maps: std::map<Identifier,InDetSimData> */
  const InDetSimDataCollection* m_id2SimDataMapPIX;
  const InDetSimDataCollection* m_id2SimDataMapSCT;
  const InDetSimDataCollection* m_id2SimDataMapTRT;

  // to retrieve GenParticles from kine (G3 only): G3 not considered!
  //  const KineHepMcmap* m_KineHepMcmap;

  /** Find all the GenParticles which contribute to this TrigInDetTrack */
  bool TrackTruth(const TrigInDetTrack* p_trk, TrigInDetTrackTruth* p_truth) const;
  /** Method to find GenParticles which contributed to a spacepoint;
   *  fills vector of GenParticles : specific to Space Points from SCT */
  void SCTspTruth(const TrigSiSpacePoint* p_SP,std::vector<HepMcParticleLink>* p_GP) const;
  /** Method to find GenParticles which contributed to a spacepoint;
   *  fills vector of GenParticles : specific to Space Points from SCT */
  void PIXspTruth(const TrigSiSpacePoint* p_SP,std::vector<HepMcParticleLink>* p_GP) const;
  /** Method to find GenParticles which contributed to a spacepoint;
   *  fills vector of GenParticles : specific to Space Points from SCT */
  void TRTdcTruth(const InDet::TRT_DriftCircle* p_DC,std::vector<HepMcParticleLink>* p_GP) const;

  /** Method to retrieve the <Identifier, InDetSimData> maps for Pixel,SCT,TRT;
   *  returns true if at least one map was found */
  bool GetTruthMaps();
  /** Add to map (hits_map) of <GenParticle,nr.of hits> all the GenParticles from vector (p_vec) which are not yet in the map; for those which are, increment numbers of hits */
  void updatePLmap(std::vector<HepMcParticleLink>* p_vec,std::map<HepMcParticleLink, unsigned int>& hits_map) const;
  /** Private helper method to update temporary vector of all the GenParticles which contributed to a cluster or drift circle InDetSimData */
  void updatePLvector(std::vector<HepMcParticleLink>* p_GP, InDetSimDataCollection::const_iterator& simData) const;
};

#endif 
