/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "DetailedMuonPatternTruthBuilder.h"

#include "MuonPattern/MuonPattern.h"
#include "MuonPattern/MuonPatternCollection.h"
#include "MuonPattern/MuonPatternCombination.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPattern/MuonPatternChamberIntersect.h"
#include "MuonPattern/DetailedMuonPatternTruthCollection.h"
#include "TrkTruthData/DetailedTrackTruth.h"
#include "TrkTruthData/DetailedSegmentTruth.h"
#include "TrkTruthData/TruthTrajectory.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "AthContainers/DataVector.h"
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "MuonSimData/MuonSimData.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "MuonPrepRawData/sTgcPrepData.h"
#include "TrkParameters/TrackParameters.h" 
#include "TrkTrack/Track.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonRIO_OnTrack/MMClusterOnTrack.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "MuonPrepRawData/sTgcPrepData.h"
#include "MuonPrepRawData/MuonCluster.h"
#include "MuonSegment/MuonSegment.h"
#include "AtlasHepMC/GenParticle.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonTrackToSegmentTool.h"
#include "MuonTrackMakerUtils/SortMeasurementsByPosition.h"

#include <map>
#include <queue>
#include <list>

#include <iostream>
#include <iterator>
#include <algorithm>

const MuonSimDataCollection* retrieveTruthCollection( std::string colName );
const MuonSimData::Deposit* getDeposit( const MuonSimDataCollection& simCol, const HepMC::GenParticle& genPart, const Identifier& id );

struct DetectorLayer {
  DetectorLayer() : stIndex(Muon::MuonStationIndex::StUnknown), isEndcap(false), minPos(1e9),maxPos(-1e9),nnsw(0),nmdtS(0),nmdtL(0),nphi(0) {}
  Muon::MuonStationIndex::StIndex stIndex;
  bool isEndcap;
  double minPos; // flag whether first and second globalpos have been filled
  double maxPos;
  int nnsw;
  int nmdtS;
  int nmdtL;
  int nphi;
  Amg::Vector3D first3D;
  Amg::Vector3D last3D;
  std::vector<const Trk::MeasurementBase*> meas;
};

namespace {

  template<class Map>  void printMap(const Map& m) {
    std::cout<<"printMap(): [";
    for(typename Map::const_iterator i=m.begin(); i!=m.end(); i++) {
      std::cout<<"("<<i->first<<","<<i->second<<"), ";
    }
    std::cout<<"]"<<std::endl;
  }

  struct SubDetPRDs {
    std::set<Identifier> subDetHits[SubDetHitStatistics::NUM_SUBDETECTORS];
  };

  SubDetPRDs& operator+=(SubDetPRDs& a, const SubDetPRDs& b) {
    for(unsigned i=0; i<SubDetHitStatistics::NUM_SUBDETECTORS; i++) {
      const std::set<Identifier>& bset = b.subDetHits[i];
      for(std::set<Identifier>::const_iterator pb=bset.begin(); pb!=bset.end(); pb++) {
	a.subDetHits[i].insert(*pb);
      }
    }
    return a;
  }

  SubDetHitStatistics makeSubDetHitStatistics(const SubDetPRDs& prds) {
    SubDetHitStatistics res;
    for(unsigned i=0; i<SubDetHitStatistics::NUM_SUBDETECTORS; i++) {
      res[SubDetHitStatistics::SubDetType(i)] = prds.subDetHits[i].size();
    }
    return res;
  }


  // Truth trajectory sprouts
  class Sprout : public std::list<const HepMC::GenParticle*> {
  public:
    SubDetPRDs stat;
  };

}


namespace Trk {

//================================================================
DetailedMuonPatternTruthBuilder::DetailedMuonPatternTruthBuilder(const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type,name,parent) {
  declareInterface<IDetailedMuonPatternTruthBuilder>(this);
}

//================================================================
StatusCode DetailedMuonPatternTruthBuilder::initialize() {
  ATH_CHECK(m_truthTrackBuilder.retrieve());
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_mdtCreator.retrieve());
  ATH_CHECK(m_muonClusterCreator.retrieve());
  return StatusCode::SUCCESS;
}

//================================================================
void DetailedMuonPatternTruthBuilder:: 
buildDetailedMuonPatternTruth(DetailedMuonPatternTruthCollection *output, 
                        const MuonPatternCombinationCollection& patterns, 
                        const std::vector<const PRD_MultiTruthCollection*>& prdTruth) 
{ 
  ATH_MSG_VERBOSE("DetailedMuonPatternTruthBuilder::buildDetailedMuonPatternTruth() "); 
   
  if(!output) {  
    return; 
  } 
 
  //---------------------------------------------------------------- 
  // The caller can pass PRD truth collections in any order. Sort them out. 

  std::vector<const PRD_MultiTruthCollection*> orderedPRD_Truth(SubDetHitStatistics::NUM_SUBDETECTORS); 
  PRD_InverseTruth inverseTruth; 
 
  for(std::vector<const PRD_MultiTruthCollection*>::const_iterator i = prdTruth.begin(); i!=prdTruth.end(); i++) { 
    if(*i) { 
      if (!(*i)->empty()) { 
        SubDetHitStatistics::SubDetType subdet = findSubDetType((*i)->begin()->first) ; 
                 
        if(subdet != SubDetHitStatistics::NUM_SUBDETECTORS) { 
          orderedPRD_Truth[subdet] = *i; 
          addToInverseMultiMap(&inverseTruth, **i); 
        } 
        else { 
          ATH_MSG_WARNING("Got unknown SubDetType in prdTruth "); 
        } 
      } 
      else { 
        ATH_MSG_DEBUG("Empty truth ???"); 
      } 
    } 
  } 
 	   
  //---------------------------------------------------------------- 
  // Find associated truth for each track 
	 
  for(unsigned itrack=0; itrack<patterns.size(); itrack++) { 
    ElementLink<DataVector<Muon::MuonPatternCombination> > ptrack(patterns, itrack); 
    addTrack(output, ptrack, orderedPRD_Truth, inverseTruth); 
  } 
	   
  // DEBUG 
  // FIXME: in normal production jobs that does *a lot of* formatting only to discard the result... 
  ATH_MSG_DEBUG("Dumping output collection.\n" 
                " Entries with TruthTrajectories of more then one particle shown at the DEBUG level.\n" 
                " Use VERBOSE level for complete dump."); 
 
  for(DetailedMuonPatternTruthCollection::const_iterator i=output->begin(); i!=output->end(); i++) { 
    bool interesting = (i->second.trajectory().size() > 1); 
	 
  // TODO:  Reinsert the following code once I understand the compile-time error 
  //    msg(interesting  ? MSG::DEBUG : MSG::VERBOSE) 
  //      <<"out: trk="<<i->first.index()<<" => "<<i->second<<endmsg; 
	 
    if(interesting) { 
      const TruthTrajectory& t = i->second.trajectory(); 
      msg(MSG::VERBOSE)<<"Particles on the trajectory:\n"; 
      for(unsigned k=0; k<t.size(); ++k) { 
        msg(MSG::VERBOSE)<<*t[k]<<"\n"; 
      } 
      msg(MSG::VERBOSE)<<"\n"<<endmsg; 
    }        
  } 	   
} 
	 
//================================================================ 

void DetailedMuonPatternTruthBuilder::
buildDetailedTrackTruth(std::vector<DetailedTrackTruth> *output,
			const Muon::MuonPatternCombination &pattern,
			const std::vector<const PRD_MultiTruthCollection*>& prdTruth)
{
  ATH_MSG_VERBOSE("DetailedMuonPatternTruthBuilder::buildDetailedTrackTruth() ");
  
  if(!output) { 
    return;
  }

  //----------------------------------------------------------------
  // The caller can pass PRD truth collections in any order. Sort them out.

  std::vector<const PRD_MultiTruthCollection*> orderedPRD_Truth(SubDetHitStatistics::NUM_SUBDETECTORS);
  PRD_InverseTruth inverseTruth;

  for(std::vector<const PRD_MultiTruthCollection*>::const_iterator i = prdTruth.begin(); i!=prdTruth.end(); i++) {
    if(*i) {
      if (!(*i)->empty()) {
	SubDetHitStatistics::SubDetType subdet = findSubDetType((*i)->begin()->first) ;
		
	if(subdet != SubDetHitStatistics::NUM_SUBDETECTORS) {
	  orderedPRD_Truth[subdet] = *i;
	  addToInverseMultiMap(&inverseTruth, **i);
	}
	else {
	  ATH_MSG_WARNING("Got unknown SubDetType in prdTruth ");
	}
      }
      else {
	ATH_MSG_DEBUG("Empty truth ???");
      }
    }
  }
  
  //----------------------------------------------------------------
  // Find associated truth for each track
  addDetailedTrackTruth(output, pattern, orderedPRD_Truth, inverseTruth);
  
  // DEBUG
  // FIXME: in normal production jobs that does *a lot of* formatting only to discard the result...
  ATH_MSG_DEBUG("Dumping output collection.\n"
		" Entries with TruthTrajectories of more then one particle shown at the DEBUG level.\n"
		" Use VERBOSE level for complete dump.");

/*
  for(TrackTruthCollection::const_iterator i=output->begin(); i!=output->end(); i++) {
    bool interesting = (i.trajectory().size() > 1);

  // TODO:  Reinsert the following code once I understand the compile-time error
//    msg(interesting  ? MSG::DEBUG : MSG::VERBOSE)
//      <<"out: trk="<<i->first.index()<<" => "<<i->second<<endmsg;

    if(interesting) {
      const TruthTrajectory& t = i.trajectory();
      msg(MSG::VERBOSE)<<"Particles on the trajectory:\n";
      for(unsigned k=0; k<t.size(); ++k) {
	msg(MSG::VERBOSE)<<*t[k]<<"\n";
      }
      msg(MSG::VERBOSE)<<"\n"<<endmsg;
    }
	  
  }
*/
  
}
//================================================================


//================================================================
// Plagiarized from 
// Tracking/TrkTools/TrkTruthCreatorTools/src/TrackTruthMaker.cxx
// (no way to reuse its private function PrepRawDataCollectionType() )

SubDetHitStatistics::SubDetType 
DetailedMuonPatternTruthBuilder::findSubDetType(Identifier id) {
  if (!m_idHelperSvc->isMuon(id)) {
    if (m_idHelperSvc->mdtIdHelper().is_pixel(id)) return SubDetHitStatistics::Pixel;
    else if (m_idHelperSvc->mdtIdHelper().is_sct(id)) return SubDetHitStatistics::SCT;
    else if (m_idHelperSvc->mdtIdHelper().is_trt(id)) return SubDetHitStatistics::TRT;
  } else {
    if (m_idHelperSvc->isMdt(id)) return SubDetHitStatistics::MDT;
    else if (m_idHelperSvc->isRpc(id)) return SubDetHitStatistics::RPC;
    else if (m_idHelperSvc->isTgc(id)) return SubDetHitStatistics::TGC;
    else if (m_idHelperSvc->isCsc(id)) return SubDetHitStatistics::CSC;
    else if (m_idHelperSvc->isMM(id)) return SubDetHitStatistics::MM;
    else if (m_idHelperSvc->issTgc(id)) return SubDetHitStatistics::STGC;
  }

  ATH_MSG_WARNING("findSubDetType(): UNKNOWN subdet for id="<<id);

  return SubDetHitStatistics::NUM_SUBDETECTORS;
}



//================================================================
void DetailedMuonPatternTruthBuilder::addTrack(DetailedMuonPatternTruthCollection *output,
					 const ElementLink<DataVector<Muon::MuonPatternCombination> > &MuPatternCombo,
					 const std::vector<const PRD_MultiTruthCollection*>& orderedPRD_Truth,
					 const PRD_InverseTruth& inverseTruth
					 )
{
  SubDetHitStatistics trackStat;
  std::map<HepMcParticleLink,SubDetPRDs> pairStat; // stats for (track,GenParticle) for the current track

  // Loop over MuonPatternChamberIntersect
  const std::vector< Muon::MuonPatternChamberIntersect >& MPCIV = (*MuPatternCombo)->chamberData();
  for(unsigned int i_MPCI = 0; i_MPCI < MPCIV.size(); i_MPCI++) {
    if(MPCIV.size() == 0) continue;

    //get the PrepRawData from the MuonPatternChamberIntersect
    std::vector<const Trk::PrepRawData*> PRDV = MPCIV.at(i_MPCI).prepRawDataVec();

    // Loop over the PRDV
    for(unsigned int j_PRD = 0; j_PRD < PRDV.size(); j_PRD++) {
      if(PRDV.size() == 0) continue;

      Identifier id = PRDV.at(j_PRD)->identify();
      SubDetHitStatistics::SubDetType subdet = findSubDetType(id);

      if(subdet != SubDetHitStatistics::NUM_SUBDETECTORS) {
        // if PRD truth collection is missing, ignore subdet in track stat calculation as well.
        if(orderedPRD_Truth[subdet]) {
          ++trackStat[subdet];
    
          typedef PRD_MultiTruthCollection::const_iterator iprdt;
          std::pair<iprdt, iprdt> range = orderedPRD_Truth[subdet]->equal_range(id);
    
          int n=0;
          // Loop over particles contributing to this cluster
          for(iprdt i = range.first; i != range.second; i++) {
            if(!i->second.isValid()) {
              ATH_MSG_WARNING("Unexpected invalid HepMcParticleLink in PRD_MultiTruthCollection");
            }
            else {
              pairStat[i->second].subDetHits[subdet].insert(id);
              n += 1;
              ATH_MSG_VERBOSE("PRD-ID:"<<id<<" subdet:"<<subdet<<" number:"<<n<<" particle link:"<<i->second);
            }
          }
          if (n == 0) {
            ATH_MSG_VERBOSE("--> no link, noise ? PRD-ID:"<<id<<" subdet:"<<subdet);
            // add barcode 0 to pairs, we like to keep track of fake fakes
            unsigned int BC(0),EV(0);
            pairStat[HepMcParticleLink(BC,EV)].subDetHits[subdet].insert(id);
          }
        } // orderedPRD_Truth[] available
      } // subdet type check, warning in findSubDetType()
    }
  }  

  if(msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE)<<"PRD truth particles = ";
    for(std::map<HepMcParticleLink,SubDetPRDs>::const_iterator i=pairStat.begin(); i!=pairStat.end(); i++) {
      msg(MSG::VERBOSE)<<i->first<<",";
    }
    msg(MSG::VERBOSE)<<endmsg;
  }

  //----------------------------------------------------------------
  // The stat structures are ready.
  // Build truth trajectories for the track
  std::set<HepMcParticleLink> seeds;
  for(std::map<HepMcParticleLink,SubDetPRDs>::const_iterator i=pairStat.begin(); i!=pairStat.end(); i++) {
    if(i->first.isValid()) {
      seeds.insert(i->first);
    } 
    else {
      // add barcode 0 particles, we like to keep track of fake fakes
      TruthTrajectory traj;
      traj.reserve(1);
      traj.push_back(i->first);
      ATH_MSG_VERBOSE("addTrack(): add barcode 0 hits - noise ?");

      // noise/no truth hits on this track
      SubDetHitStatistics noiseStat = makeSubDetHitStatistics(i->second);

      // Only valid HepMcParticleLink make it into seeds and then into sprouts, and 
      // stored in the loop over sprouts below.
      // Store output for noise/no truth particles here.
      output->insert(std::make_pair(MuPatternCombo,
				    DetailedTrackTruth(traj,
						       noiseStat,
						       trackStat,
						       noiseStat) ));
    }
  }
  
  // Grow sprouts from the seeds
  typedef std::map<HepMcParticleLink, Sprout> SproutMap;
  SproutMap sprouts;

  while(!seeds.empty()) {
    HepMcParticleLink link = *seeds.begin();
    
    Sprout current_sprout;
    std::queue<const HepMC::GenParticle*> tmp;
    unsigned eventIndex = link.eventIndex();
    const HepMC::GenParticle *current = link.cptr();
    
    do {
      HepMcParticleLink curlink(current->barcode(), eventIndex);

      // remove the current particle from the list of particles to consider (if it is still there)
      seeds.erase(curlink);
      
      // Have we worked on this particle before?
      SproutMap::iterator p_old = sprouts.find(curlink);
      if(p_old != sprouts.end()) {
	//merge the old sprout with the current one.
	current_sprout.splice(current_sprout.end(), p_old->second);
	current_sprout.stat += p_old->second.stat;
	// and remove the entry for the old
	sprouts.erase(p_old);
	break; // the do-while(getMother()) loop
      }
      else { // No, this is a new particle.  Try to extend the current truth trajectory.
	
	// Add the particle to the current truth trajectory.
	// New: with the current stricter cuts on mother-daughter 
	// we don't have to require that ancestors produce hits.

	current_sprout.push_back(current);

	std::map<HepMcParticleLink,SubDetPRDs>::iterator p_newstat = pairStat.find(curlink);
	if(p_newstat != pairStat.end()) {
	  current_sprout.stat += p_newstat->second;
	}

      }
    } while( (current = m_truthTrackBuilder->getMother(current)) );
    
    // Add the grown sprout to the list
    sprouts.insert(std::make_pair(link, current_sprout));
    
  } // while(!seeds.empty())
  
  //----------------
  // All seeds have been processed, and the upstream extensions of the
  // sprouts are done.  Extend the sprouts downstream to get the final
  // truth trajectories and store the result.
  //
  // Note: so far the "sprouts" object mapped {last particle ==> sprout}
  // Extending a sprout downstream will break this relationship,
  // but at this point we don't care about it and will only use the 
  // value of the map, not the key.

  for(SproutMap::iterator s=sprouts.begin(); s!=sprouts.end(); ++s) {

    // Attempt to extend the TruthTrajectory sprout to the "outside".
    // This may add only hits that are *not* on the current track.
    // Thus no need to update stats track and stats common.

    const HepMC::GenParticle* current = *s->second.begin();
    while( (current = m_truthTrackBuilder->getDaughter(current)) ) {
      s->second.push_front(current);
    }

    // Now we have info to build the final TruthTrajectory.
    // FIXME: what is the current average size?
    TruthTrajectory traj;
    traj.reserve(2); // The average size is about 1.05.  Hardcode that instead of using slow list::size().
    for(Sprout::const_iterator ppart=s->second.begin(); ppart!=s->second.end(); ppart++) {
      traj.push_back(HepMcParticleLink((*ppart)->barcode(), s->first.eventIndex()));
    }

    // Count PRDs on the TruthTrajectory
    std::set<Muon::MuonStationIndex::ChIndex> tempSet;
    SubDetHitStatistics truthStat = countPRDsOnTruth(traj, inverseTruth, tempSet);

    ATH_MSG_VERBOSE("addTrack(): sprout length = "<<traj.size());
    output->insert(std::make_pair(MuPatternCombo,
				  DetailedTrackTruth(traj, 
						     makeSubDetHitStatistics(s->second.stat),
						     trackStat,
						     truthStat) ));
  }

  ATH_MSG_VERBOSE("addTrack(): #sprouts = "<<sprouts.size()<<", output->size() = "<<output->size());

}

//================================================================
SubDetHitStatistics DetailedMuonPatternTruthBuilder::countPRDsOnTruth(const TruthTrajectory& traj,
								const PRD_InverseTruth& inverseTruth,
                                                                std::set<Muon::MuonStationIndex::ChIndex> chIndices)
{
  // Different particles from the same TruthTrajectory can contribute to the same cluster.
  // We should be careful to avoid double-counting in such cases.

  SubDetPRDs prds;
  for(TruthTrajectory::const_iterator p = traj.begin(); p != traj.end(); ++p) {
    typedef PRD_InverseTruth::const_iterator iter;
    std::pair<iter,iter> range = inverseTruth.equal_range(*p);
    for(iter i = range.first; i != range.second; ++i) {
      if(chIndices.find(m_idHelperSvc->chamberIndex(i->second)) != chIndices.end()) {
        SubDetHitStatistics::SubDetType subdet = findSubDetType(i->second);
	if(subdet==SubDetHitStatistics::NUM_SUBDETECTORS) continue;
        prds.subDetHits[subdet].insert(i->second);
      }
    }
  }

  return makeSubDetHitStatistics(prds);
}


//================================================================
Amg::Vector3D DetailedMuonPatternTruthBuilder::getPRDTruthPosition(const Muon::MuonSegment &segment,
								   std::list<const HepMC::GenParticle*> genPartList,
								   int truthPos,
								   std::set<Muon::MuonStationIndex::ChIndex> chIndices)
{
  double minPos = 2e8, maxPos = 0;
  Amg::Vector3D first3D(0,0,0), last3D(0,0,0);

  if(genPartList.size() == 0) {
    ATH_MSG_WARNING("No GenParticles associated to this PRD_TruthTrajectory. Exiting segment creation.");
    return Amg::Vector3D(0, 0, 0);
  }  

  const MuonSimDataCollection* mdtSimDataMap = retrieveTruthCollection( "MDT_SDO" );
  if( !mdtSimDataMap) {
    ATH_MSG_WARNING(" failed to retrieve MuonSimDataCollection: " << "MDT_SDO" );
    return Amg::Vector3D(0, 0, 0);
  }
  const MuonSimDataCollection* mmSimDataMap = retrieveTruthCollection( "MM_SDO" );
  if( !mmSimDataMap) {
    ATH_MSG_WARNING(" failed to retrieve MuonSimDataCollection: " << "MM_SDO" );
    return Amg::Vector3D(0, 0, 0);
  }
  const MuonSimDataCollection* stgcSimDataMap = retrieveTruthCollection( "STGC_SDO" );
  if( !stgcSimDataMap) {
    ATH_MSG_WARNING(" failed to retrieve MuonSimDataCollection: " << "STGC_SDO" );
    return Amg::Vector3D(0, 0, 0);
  }

  // First sort the hits per detector layer
  std::map<Muon::MuonStationIndex::StIndex,DetectorLayer> hitsPerLayer;
  std::map< const Trk::TrkDetElementBase*, std::pair< std::list<const Trk::PrepRawData*>,std::list<const Trk::PrepRawData*> > > clustersPerDetEl;

  // Loop over containedROTs in segment
  for(unsigned int i_cROTv = 0; i_cROTv < segment.numberOfContainedROTs(); i_cROTv++) {

    const Trk::RIO_OnTrack* rot=segment.rioOnTrack(i_cROTv);

    // get the PrepRawData from the ROT
    const Trk::PrepRawData* prd = rot->prepRawData();

    Identifier id = prd->identify();

    Muon::MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(id);
    bool isEndcap = m_idHelperSvc->isEndcap(id);

    if(chIndices.find(m_idHelperSvc->chamberIndex(id)) == chIndices.end()) {
      ATH_MSG_DEBUG("Muon station doesn't match segment.  Continuing");
      continue;
    }

    // some patches until MuonIdHelper is updated
    const Muon::MMPrepData* mm = dynamic_cast<const Muon::MMPrepData*>(prd);
    if( mm ) {
      isEndcap = true;
      stIndex = Muon::MuonStationIndex::EI;
    }
    const Muon::sTgcPrepData* stgc = dynamic_cast<const Muon::sTgcPrepData*>(prd);
    if( stgc ) {
      isEndcap = true;
      stIndex = Muon::MuonStationIndex::EI;
    }
// BEGIN NEW WORK

    DetectorLayer& detLayer = hitsPerLayer[stIndex];
    detLayer.isEndcap = isEndcap;
    detLayer.stIndex = stIndex;

    //const Trk::MeasurementBase* meas = 0;
    if( m_idHelperSvc->isMdt(id) ){
      const Muon::MdtPrepData* mprd = dynamic_cast<const Muon::MdtPrepData*>(prd);
      if( !mprd ) {
        ATH_MSG_WARNING(" MDT PRD not of type MdtPrepData " << m_idHelperSvc->toString(id) );
        continue;
      }
      
      const MuonSimData::Deposit* deposit = 0;
      for (std::list<const HepMC::GenParticle*>::const_iterator it = genPartList.begin(); it != genPartList.end() && !deposit; ++it) {
        deposit = getDeposit(*mdtSimDataMap, **it ,id);
      }
      if( !deposit ){
        ATH_MSG_WARNING(" Deposit for GenParticle not found " << m_idHelperSvc->toString(id) );
        continue;
      }

      Amg::Vector2D lp(deposit->second.firstEntry(),deposit->second.secondEntry());
      const Amg::Vector3D* gpos = prd->detectorElement()->surface(id).localToGlobal(lp);
      if( !gpos ) {
        ATH_MSG_WARNING(" LocalToGlobal failed " << m_idHelperSvc->toString(id) );
        continue;
      }

      double val = isEndcap ? fabs(gpos->z()) : gpos->perp();

      // nasty comparisons to figure out which MDT hit comes first
      if( val < detLayer.minPos ){
        if( detLayer.maxPos < -1e8 && detLayer.minPos < 1e8 ){
          detLayer.last3D = detLayer.first3D;
          detLayer.maxPos = detLayer.minPos;
        }
        detLayer.first3D = *gpos;
        detLayer.minPos = val;        
      }else if( val > detLayer.maxPos ){
        detLayer.last3D = *gpos;
        detLayer.maxPos = val;
      }

      if(val < minPos) {
        if(maxPos < -1e8 && minPos < 1e8) {
          last3D = first3D;
          maxPos = minPos;
        }
        minPos = val;
        first3D = *gpos;
      } else if (val > maxPos) {
        maxPos = val;
        last3D = *gpos;
      }

      const Muon::MdtDriftCircleOnTrack* mdt = m_mdtCreator->createRIO_OnTrack(*mprd,*gpos);
      delete gpos;
      if( !mdt ) {
        ATH_MSG_WARNING(" ROT creation failed " << m_idHelperSvc->toString(id) );
        continue;
      }
      Trk::DriftCircleSide side = deposit->second.firstEntry() < 0 ? Trk::LEFT : Trk::RIGHT;
      m_mdtCreator->updateSign(*const_cast<Muon::MdtDriftCircleOnTrack*>(mdt),side);
      double pull = (mdt->driftRadius()-deposit->second.firstEntry())/mdt->localCovariance()(Trk::locR) ;
      ATH_MSG_VERBOSE(" new MDT    " << m_idHelperSvc->toString(id) << " radius " << mdt->driftRadius() 
                      << " true radius " << deposit->second.firstEntry()
                      << " pull " << pull );
      if( fabs(pull)>3. ) ATH_MSG_VERBOSE(" hit with large pull ");
      detLayer.meas.push_back(mdt);
      if( m_idHelperSvc->isSmallChamber(id) ) ++detLayer.nmdtS;
      else                                 ++detLayer.nmdtL;
      //meas = mdt;
    }else if( mm ){

      const MuonSimData::Deposit* deposit = 0;
      for (std::list<const HepMC::GenParticle*>::const_iterator it = genPartList.begin(); it != genPartList.end() && !deposit; ++it) {
        deposit = getDeposit(*mmSimDataMap, **it ,id);
      }
      if( !deposit ){
        ATH_MSG_WARNING(" Deposit for GenParticle not found " << m_idHelperSvc->toString(id) );
        continue;
      }

      Amg::Vector2D lp(deposit->second.firstEntry(),deposit->second.secondEntry());
      const Amg::Vector3D* gpos = prd->detectorElement()->surface(id).localToGlobal(lp);
      if( !gpos ) {
        ATH_MSG_WARNING(" LocalToGlobal failed " <<  m_idHelperSvc->toString(id) );
        continue;
      }

      // double val = isEndcap ? fabs(gpos->z()) : gpos->perp();
      // micormegas are always endcap
      double val = fabs(gpos->z());
      // nasty comparisons to figure out which MDT hit comes first
      if( val < detLayer.minPos ){
        if( detLayer.maxPos < -1e8 && detLayer.minPos < 1e8 ){
          detLayer.last3D = detLayer.first3D;
          detLayer.maxPos = detLayer.minPos;
        }
        detLayer.first3D = *gpos;
        detLayer.minPos = val;
      }else if( val > detLayer.maxPos ){
        detLayer.last3D = *gpos;
        detLayer.maxPos = val;
      }

      if(val < minPos) {
        if( maxPos < -1e8 && detLayer.minPos < 1e8 ){
          last3D = first3D;
          maxPos = minPos;
        }
        minPos = val;
        first3D = *gpos;
      } else if (val > maxPos) {
        maxPos = val;
        last3D = *gpos;
      }

      const Muon::MuonClusterOnTrack* rot = m_muonClusterCreator->createRIO_OnTrack(*mm,*gpos);
      if( !rot ) {
        delete gpos;
        ATH_MSG_WARNING(" ROT creation failed " << m_idHelperSvc->toString(id) );
        continue;
      }
      double residual = rot->localParameters().get(Trk::locX)-lp.x();
      double pull = residual / rot->localCovariance()(Trk::locX);
      ATH_MSG_DEBUG( "Adding r " << gpos->perp() << " z " << gpos->z() << "  " << m_idHelperSvc->toString(id) << " " << residual << " pull " << pull  );
      detLayer.meas.push_back(rot);
      //meas = rot;
      ++detLayer.nnsw;
      delete gpos;

    }else if( stgc ) {

      // skip pads in outer most two chambers as here the wires are more precise
      if( m_idHelperSvc->stgcIdHelper().channelType(id) == 0 && abs(m_idHelperSvc->stationEta(id)) > 2 ) continue;

      // there is already a check for this at the beginning of the method
      // if( !stgcSimDataMap ) continue;
     
      const MuonSimData::Deposit* deposit = 0;
      for (std::list<const HepMC::GenParticle*>::const_iterator it = genPartList.begin(); it != genPartList.end() && !deposit; ++it) {
        deposit = getDeposit(*stgcSimDataMap, **it ,id);
      }
      if( !deposit ){
        ATH_MSG_WARNING(" Deposit for GenParticle not found " << m_idHelperSvc->toString(id) );
        continue;
      }


      Amg::Vector2D lp(deposit->second.firstEntry(),deposit->second.secondEntry());
      const Amg::Vector3D* gpos = prd->detectorElement()->surface(id).localToGlobal(lp);
      if( !gpos ) {
        ATH_MSG_WARNING(" LocalToGlobal failed " <<  m_idHelperSvc->toString(id) );
        continue;
      }

      // double val = isEndcap ? fabs(gpos->z()) : gpos->perp();
      // stgcs are always endcap
      double val = fabs(gpos->z());
      // // nasty comparisons to figure out which STGC hit comes first
      if( val < detLayer.minPos ){
        if( detLayer.maxPos < -1e8 && detLayer.minPos < 1e8 ){
          detLayer.last3D = detLayer.first3D;
          detLayer.maxPos = detLayer.minPos;
        }
        detLayer.first3D = *gpos;
        detLayer.minPos = val;
      }else if( val > detLayer.maxPos ){
        detLayer.last3D = *gpos;
        detLayer.maxPos = val;
      }

      if(val < minPos) {
        if( maxPos < -1e8 && minPos < 1e8 ){
          last3D = first3D;
          maxPos = minPos;
        }
        minPos = val;
        first3D = *gpos;
      } else if (val > maxPos) {
        maxPos = val;
        last3D = *gpos;
      }

      const Muon::MuonClusterOnTrack* rot = m_muonClusterCreator->createRIO_OnTrack(*stgc,*gpos);
      if( !rot ) {
        delete gpos;
        ATH_MSG_WARNING(" ROT creation failed " << m_idHelperSvc->toString(id) );
        continue;
      }
      double residual = rot->localParameters().get(Trk::locX) - lp.x();
      double pull = residual / rot->localCovariance()(Trk::locX);
      ATH_MSG_DEBUG( "Adding r " << gpos->perp() << " z " << gpos->z() << "  " << m_idHelperSvc->toString(id) << " " << residual << " pull " << pull  );
      detLayer.meas.push_back(rot);
      //meas = rot;
      ++detLayer.nnsw;
      delete gpos;
    }
  }

// END NEW WORK
  if(minPos == 2e8 || maxPos == 0) {
    ATH_MSG_WARNING("Min and max positions not found.  Filling with meaningless position");
    return Amg::Vector3D(-15000, -15000, -15000);
  }

  if(truthPos)
    return (first3D + last3D) / 2;
  else {
    // else truth direction
    return (last3D - first3D);
  }

}

//================================================================
void DetailedMuonPatternTruthBuilder::addDetailedTrackTruth(std::vector<DetailedTrackTruth> *output,
					 const Muon::MuonPatternCombination& pattern,
					 const std::vector<const PRD_MultiTruthCollection*>& orderedPRD_Truth,
					 const PRD_InverseTruth& inverseTruth
					 )
{
  SubDetHitStatistics trackStat;
  std::map<HepMcParticleLink,SubDetPRDs> pairStat; // stats for (track,GenParticle) for the current track

  // Loop over MuonPatternChamberIntersect
  const std::vector< Muon::MuonPatternChamberIntersect >& MPCIV = pattern.chamberData();
  for(unsigned int i_MPCI = 0; i_MPCI < MPCIV.size(); i_MPCI++) {
    if(MPCIV.size() == 0) continue;

    //get the PrepRawData from the MuonPatternChamberIntersect
    std::vector<const Trk::PrepRawData*> PRDV = MPCIV.at(i_MPCI).prepRawDataVec();

    // Loop over the PRDV
    for(unsigned int j_PRD = 0; j_PRD < PRDV.size(); j_PRD++) {
      if(PRDV.size() == 0) continue;

      Identifier id = PRDV.at(j_PRD)->identify();
      SubDetHitStatistics::SubDetType subdet = findSubDetType(id);

      if(subdet != SubDetHitStatistics::NUM_SUBDETECTORS) {
        // if PRD truth collection is missing, ignore subdet in track stat calculation as well.
        if(orderedPRD_Truth[subdet]) {
          ++trackStat[subdet];
    
          typedef PRD_MultiTruthCollection::const_iterator iprdt;
          std::pair<iprdt, iprdt> range = orderedPRD_Truth[subdet]->equal_range(id);
    
          int n=0;
          // Loop over particles contributing to this cluster
          for(iprdt i = range.first; i != range.second; i++) {
            if(!i->second.isValid()) {
              ATH_MSG_WARNING("Unexpected invalid HepMcParticleLink in PRD_MultiTruthCollection");
            }
            else {
              pairStat[i->second].subDetHits[subdet].insert(id);
              n += 1;
              ATH_MSG_VERBOSE("PRD-ID:"<<id<<" subdet:"<<subdet<<" number:"<<n<<" particle link:"<<i->second);
            }
          }
          if (n == 0) {
            ATH_MSG_VERBOSE("--> no link, noise ? PRD-ID:"<<id<<" subdet:"<<subdet);
            // add barcode 0 to pairs, we like to keep track of fake fakes
            unsigned int BC(0),EV(0);
            pairStat[HepMcParticleLink(BC,EV)].subDetHits[subdet].insert(id);
          }
        } // orderedPRD_Truth[] available
      } // subdet type check, warning in findSubDetType()
    }
  }  

  if(msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE)<<"PRD truth particles = ";
    for(std::map<HepMcParticleLink,SubDetPRDs>::const_iterator i=pairStat.begin(); i!=pairStat.end(); i++) {
      msg(MSG::VERBOSE)<<i->first<<",";
    }
    msg(MSG::VERBOSE)<<endmsg;
  }

  //----------------------------------------------------------------
  // The stat structures are ready.
  // Build truth trajectories for the track

  std::set<HepMcParticleLink> seeds;
  for(std::map<HepMcParticleLink,SubDetPRDs>::const_iterator i=pairStat.begin(); i!=pairStat.end(); i++) {
    if(i->first.isValid()) {
      seeds.insert(i->first);
    } 
    else {
      // add barcode 0 particles, we like to keep track of fake fakes
      TruthTrajectory traj;
      traj.reserve(1);
      traj.push_back(i->first);
      ATH_MSG_VERBOSE("addTrack(): add barcode 0 hits - noise ?");

      // noise/no truth hits on this track
      SubDetHitStatistics noiseStat = makeSubDetHitStatistics(i->second);

      // Only valid HepMcParticleLink make it into seeds and then into sprouts, and 
      // stored in the loop over sprouts below.
      // Store output for noise/no truth particles here.
      output->push_back(DetailedTrackTruth(traj, noiseStat, trackStat, noiseStat));
    }
  }
  
  // Grow sprouts from the seeds
  typedef std::map<HepMcParticleLink, Sprout> SproutMap;
  SproutMap sprouts;

  while(!seeds.empty()) {
    HepMcParticleLink link = *seeds.begin();
    
    Sprout current_sprout;
    std::queue<const HepMC::GenParticle*> tmp;
    unsigned eventIndex = link.eventIndex();
    const HepMC::GenParticle *current = link.cptr();
    
    do {
      HepMcParticleLink curlink(current->barcode(), eventIndex);

      // remove the current particle from the list of particles to consider (if it is still there)
      seeds.erase(curlink);
      
      // Have we worked on this particle before?
      SproutMap::iterator p_old = sprouts.find(curlink);
      if(p_old != sprouts.end()) {
	//merge the old sprout with the current one.
	current_sprout.splice(current_sprout.end(), p_old->second);
	current_sprout.stat += p_old->second.stat;
	// and remove the entry for the old
	sprouts.erase(p_old);
	break; // the do-while(getMother()) loop
      }
      else { // No, this is a new particle.  Try to extend the current truth trajectory.
	
	// Add the particle to the current truth trajectory.
	// New: with the current stricter cuts on mother-daughter 
	// we don't have to require that ancestors produce hits.

	current_sprout.push_back(current);

	std::map<HepMcParticleLink,SubDetPRDs>::iterator p_newstat = pairStat.find(curlink);
	if(p_newstat != pairStat.end()) {
	  current_sprout.stat += p_newstat->second;
	}

      }
    } while( (current = m_truthTrackBuilder->getMother(current)) );
    
    // Add the grown sprout to the list
    sprouts.insert(std::make_pair(link, current_sprout));
    
  } // while(!seeds.empty())
  
  //----------------
  // All seeds have been processed, and the upstream extensions of the
  // sprouts are done.  Extend the sprouts downstream to get the final
  // truth trajectories and store the result.
  //
  // Note: so far the "sprouts" object mapped {last particle ==> sprout}
  // Extending a sprout downstream will break this relationship,
  // but at this point we don't care about it and will only use the 
  // value of the map, not the key.
  for(SproutMap::iterator s=sprouts.begin(); s!=sprouts.end(); ++s) {

    // Attempt to extend the TruthTrajectory sprout to the "outside".
    // This may add only hits that are *not* on the current track.
    // Thus no need to update stats track and stats common.

    const HepMC::GenParticle* current = *s->second.begin();
    while( (current = m_truthTrackBuilder->getDaughter(current)) ) {
      s->second.push_front(current);
    }

    // Now we have info to build the final TruthTrajectory.
    // FIXME: what is the current average size?
    TruthTrajectory traj;
    traj.reserve(2); // The average size is about 1.05.  Hardcode that instead of using slow list::size().
    for(Sprout::const_iterator ppart=s->second.begin(); ppart!=s->second.end(); ppart++) {
      traj.push_back(HepMcParticleLink((*ppart)->barcode(), s->first.eventIndex()));
    }

    // Count PRDs on the TruthTrajectory
    std::set<Muon::MuonStationIndex::ChIndex> tempSet;
    SubDetHitStatistics truthStat = countPRDsOnTruth(traj, inverseTruth, tempSet);

    ATH_MSG_VERBOSE("addTrack(): sprout length = "<<traj.size());
    output->push_back(DetailedTrackTruth(traj, makeSubDetHitStatistics(s->second.stat), trackStat, truthStat));
  }

  ATH_MSG_VERBOSE("addTrack(): #sprouts = "<<sprouts.size()<<", output->size() = "<<output->size());

}


void DetailedMuonPatternTruthBuilder::
buildDetailedTrackTruthFromSegments(std::vector<DetailedSegmentTruth> *output,
                                    const Muon::MuonSegment &segment,
                                    const std::vector<const PRD_MultiTruthCollection*>& prdTruth) 
{
  ATH_MSG_VERBOSE("DetailedMuonPatternTruthBuilder::buildDetailedTrackTruthFromSegments() ");
  
  if(!output) { 
    return;
  }

  //----------------------------------------------------------------
  // The caller can pass PRD truth collections in any order. Sort them out.

  std::vector<const PRD_MultiTruthCollection*> orderedPRD_Truth(SubDetHitStatistics::NUM_SUBDETECTORS);
  PRD_InverseTruth inverseTruth;

  for(std::vector<const PRD_MultiTruthCollection*>::const_iterator i = prdTruth.begin(); i!=prdTruth.end(); i++) {
    if(*i) {
      if (!(*i)->empty()) {
	SubDetHitStatistics::SubDetType subdet = findSubDetType((*i)->begin()->first) ;
	
	if(subdet != SubDetHitStatistics::NUM_SUBDETECTORS) {
	  orderedPRD_Truth[subdet] = *i;
	  addToInverseMultiMap(&inverseTruth, **i);
	}
	else {
	  ATH_MSG_WARNING("Got unknown SubDetType in prdTruth ");
	}
      }
      else {
	ATH_MSG_DEBUG("Empty truth ???");
      }
    }
  }
  
  //----------------------------------------------------------------
  // Find associated truth for each track
  addDetailedTrackTruthFromSegment(output, segment, orderedPRD_Truth, inverseTruth);
  
  // DEBUG
  // FIXME: in normal production jobs that does *a lot of* formatting only to discard the result...
  ATH_MSG_DEBUG("Dumping output collection.\n"
		" Entries with TruthTrajectories of more then one particle shown at the DEBUG level.\n"
		" Use VERBOSE level for complete dump.");

/*
  for(TrackTruthCollection::const_iterator i=output->begin(); i!=output->end(); i++) {
    bool interesting = (i.trajectory().size() > 1);

  // TODO:  Reinsert the following code once I understand the compile-time error
//    msg(interesting  ? MSG::DEBUG : MSG::VERBOSE)
//      <<"out: trk="<<i->first.index()<<" => "<<i->second<<endmsg;

    if(interesting) {
      const TruthTrajectory& t = i.trajectory();
      msg(MSG::VERBOSE)<<"Particles on the trajectory:\n";
      for(unsigned k=0; k<t.size(); ++k) {
	msg(MSG::VERBOSE)<<*t[k]<<"\n";
      }
      msg(MSG::VERBOSE)<<"\n"<<endmsg;
    }
	  
  }
*/
  

}

//================================================================
void DetailedMuonPatternTruthBuilder::addDetailedTrackTruthFromSegment(std::vector<DetailedSegmentTruth> *output,
					 const Muon::MuonSegment &segment,
					 const std::vector<const PRD_MultiTruthCollection*>& orderedPRD_Truth,
					 const PRD_InverseTruth& inverseTruth
					 )
{

  SubDetHitStatistics trackStat;
  std::map<HepMcParticleLink,SubDetPRDs> pairStat; // stats for (track,GenParticle) for the current track

  std::set<Muon::MuonStationIndex::ChIndex> chIndices;

  // Loop over containedROTs in segment
  for(unsigned int i_cROTv = 0; i_cROTv < segment.numberOfContainedROTs(); i_cROTv++) {
    const Trk::RIO_OnTrack* rot=segment.rioOnTrack(i_cROTv);

    // get the PrepRawData from the ROT
    const Trk::PrepRawData* prd = rot->prepRawData();

    Identifier id = prd->identify();
    chIndices.insert(m_idHelperSvc->chamberIndex(id));

    SubDetHitStatistics::SubDetType subdet = findSubDetType(id);

    if(subdet != SubDetHitStatistics::NUM_SUBDETECTORS) {
      // if PRD truth collection is missing, ignore subdet in track stat calculation as well.
      if(orderedPRD_Truth[subdet]) {
        ++trackStat[subdet];
    
        typedef PRD_MultiTruthCollection::const_iterator iprdt;
        std::pair<iprdt, iprdt> range = orderedPRD_Truth[subdet]->equal_range(id);
    
        int n=0;
        // Loop over particles contributing to this cluster
        for(iprdt i = range.first; i != range.second; i++) {
          if(!i->second.isValid()) {
            ATH_MSG_WARNING("Unexpected invalid HepMcParticleLink in PRD_MultiTruthCollection");
          }
          else {
            pairStat[i->second].subDetHits[subdet].insert(id);
            n += 1;
            ATH_MSG_VERBOSE("PRD-ID:"<<id<<" subdet:"<<subdet<<" number:"<<n<<" particle link:"<<i->second);
          }
        }
        if (n == 0) {
          ATH_MSG_VERBOSE("--> no link, noise ? PRD-ID:"<<id<<" subdet:"<<subdet);
          // add barcode 0 to pairs, we like to keep track of fake fakes
          unsigned int BC(0),EV(0);
          pairStat[HepMcParticleLink(BC,EV)].subDetHits[subdet].insert(id);
        }
      } // orderedPRD_Truth[] available
    } // subdet type check, warning in findSubDetType()
  }

  if(msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE)<<"PRD truth particles = ";
    for(std::map<HepMcParticleLink,SubDetPRDs>::const_iterator i=pairStat.begin(); i!=pairStat.end(); i++) {
      msg(MSG::VERBOSE)<<i->first<<",";
    }
    msg(MSG::VERBOSE)<<endmsg;
  }

  //----------------------------------------------------------------
  // The stat structures are ready.
  // Build truth trajectories for the track

  std::set<HepMcParticleLink> seeds;
  for(std::map<HepMcParticleLink,SubDetPRDs>::const_iterator i=pairStat.begin(); i!=pairStat.end(); i++) {
    if(i->first.isValid()) {
      seeds.insert(i->first);
    } 
    else {
      // add barcode 0 particles, we like to keep track of fake fakes
      TruthTrajectory traj;
      traj.reserve(1);
      traj.push_back(i->first);
      ATH_MSG_VERBOSE("addTrack(): add barcode 0 hits - noise ?");

      // noise/no truth hits on this track
      SubDetHitStatistics noiseStat = makeSubDetHitStatistics(i->second);

      // Only valid HepMcParticleLink make it into seeds and then into sprouts, and 
      // stored in the loop over sprouts below.
      // Store output for noise/no truth particles here.
      output->push_back(DetailedSegmentTruth(traj, noiseStat, trackStat, noiseStat, Amg::Vector3D(0, 0, 0), Amg::Vector3D(0, 0, 0)));
    }
  }
  
  // Grow sprouts from the seeds
  typedef std::map<HepMcParticleLink, Sprout> SproutMap;
  SproutMap sprouts;

  while(!seeds.empty()) {
    HepMcParticleLink link = *seeds.begin();
    
    Sprout current_sprout;
    std::queue<const HepMC::GenParticle*> tmp;
    unsigned eventIndex = link.eventIndex();
    const HepMC::GenParticle *current = link.cptr();
    
    do {
      HepMcParticleLink curlink(current->barcode(), eventIndex);

      // remove the current particle from the list of particles to consider (if it is still there)
      seeds.erase(curlink);
      
      // Have we worked on this particle before?
      SproutMap::iterator p_old = sprouts.find(curlink);
      if(p_old != sprouts.end()) {
	//merge the old sprout with the current one.
	current_sprout.splice(current_sprout.end(), p_old->second);
	current_sprout.stat += p_old->second.stat;
	// and remove the entry for the old
	sprouts.erase(p_old);
	break; // the do-while(getMother()) loop
      }
      else { // No, this is a new particle.  Try to extend the current truth trajectory.
	
	// Add the particle to the current truth trajectory.
	// New: with the current stricter cuts on mother-daughter 
	// we don't have to require that ancestors produce hits.

	current_sprout.push_back(current);

	std::map<HepMcParticleLink,SubDetPRDs>::iterator p_newstat = pairStat.find(curlink);
	if(p_newstat != pairStat.end()) {
	  current_sprout.stat += p_newstat->second;
	}

      }
    } while( (current = m_truthTrackBuilder->getMother(current)) );
    
    // Add the grown sprout to the list
    sprouts.insert(std::make_pair(link, current_sprout));
    
  } // while(!seeds.empty())
  
  //----------------
  // All seeds have been processed, and the upstream extensions of the
  // sprouts are done.  Extend the sprouts downstream to get the final
  // truth trajectories and store the result.
  //
  // Note: so far the "sprouts" object mapped {last particle ==> sprout}
  // Extending a sprout downstream will break this relationship,
  // but at this point we don't care about it and will only use the 
  // value of the map, not the key.
  for(SproutMap::iterator s=sprouts.begin(); s!=sprouts.end(); ++s) {

    // Attempt to extend the TruthTrajectory sprout to the "outside".
    // This may add only hits that are *not* on the current track.
    // Thus no need to update stats track and stats common.

    const HepMC::GenParticle* current = *s->second.begin();
    while( (current = m_truthTrackBuilder->getDaughter(current)) ) {
      s->second.push_front(current);
    }

    // Now we have info to build the final TruthTrajectory.
    // FIXME: what is the current average size?
    TruthTrajectory traj;
    traj.reserve(2); // The average size is about 1.05.  Hardcode that instead of using slow list::size().
    for(Sprout::const_iterator ppart=s->second.begin(); ppart!=s->second.end(); ppart++) {
      traj.push_back(HepMcParticleLink((*ppart)->barcode(), s->first.eventIndex()));
    }

    // Count PRDs on the TruthTrajectory
    SubDetHitStatistics truthStat = countPRDsOnTruth(traj, inverseTruth, chIndices);
    Amg::Vector3D pos = getPRDTruthPosition(segment, s->second, 1, chIndices);
    Amg::Vector3D dir = getPRDTruthPosition(segment, s->second, 0, chIndices);

    ATH_MSG_VERBOSE("addTrack(): sprout length = "<<traj.size());
    output->push_back(DetailedSegmentTruth(traj, makeSubDetHitStatistics(s->second.stat), trackStat, truthStat, pos, dir));
  }

  ATH_MSG_VERBOSE("addTrack(): #sprouts = "<<sprouts.size()<<", output->size() = "<<output->size());

}


  const MuonSimData::Deposit* DetailedMuonPatternTruthBuilder::getDeposit( const MuonSimDataCollection& simCol, const HepMC::GenParticle& genPart, const Identifier& id ) 
  {
    MuonSimDataCollection::const_iterator it = simCol.find(id);
    if( it == simCol.end() ) {
      ATH_MSG_WARNING(" Truth PRD not found in simdata collection: " << m_idHelperSvc->toString(id) );
      return 0;
    }
   
    const MuonSimData& simData = it->second;
    const MuonSimData::Deposit* deposit = 0;
    std::vector<MuonSimData::Deposit>::const_iterator dit = simData.getdeposits().begin();
    std::vector<MuonSimData::Deposit>::const_iterator dit_end = simData.getdeposits().end();
    for( ; dit!=dit_end;++dit ){
      const HepMC::GenParticle* gp = dit->first;
      if( gp == &genPart ){
        deposit = &*dit;
        break;
      }
    }
    return deposit;
  }

  const MuonSimDataCollection* DetailedMuonPatternTruthBuilder::retrieveTruthCollection( std::string colName ) {
    // Retrieve SDO map for this event
    if(!evtStore()->contains<MuonSimDataCollection>(colName)) return 0;

    const MuonSimDataCollection* truthCol(0);
    if(!evtStore()->retrieve(truthCol, colName).isSuccess()) {
      ATH_MSG_VERBOSE("Could NOT find the MuonSimDataMap map key = "<< colName);
    }   else {
      ATH_MSG_VERBOSE("Retrieved MuonSimDataCollection for key = " << colName);
    }
    return truthCol;
  }

} // namespace Trk
