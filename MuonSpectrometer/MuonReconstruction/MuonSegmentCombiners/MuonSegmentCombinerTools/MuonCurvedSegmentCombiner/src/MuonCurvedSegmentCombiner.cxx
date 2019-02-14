/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MuonCurvedSegmentCombiner.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MuonCurvedSegmentCombiner/MuonCurvedSegmentCombiner.h"

#include <iostream>
#include <sstream>
#include <vector>

#include "Identifier/Identifier.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentCombination.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonSegment/MuonSegmentQuality.h"
#include "MuonPattern/MuonPatternCombination.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"

#include "TrkParameters/TrackParameters.h"

#include "TrkPrepRawData/PrepRawData.h"

#include "TrkEventPrimitives/FitQuality.h"

#include "StoreGate/StoreGateSvc.h"


//================ Constructor =================================================

Muon::MuonCurvedSegmentCombiner::MuonCurvedSegmentCombiner(const std::string& t,
    const std::string& n,
    const IInterface*  p )
    :
    AthAlgTool(t,n,p),
    m_detMgr(0),
    m_rpcIdHelper(0),
    m_tgcIdHelper(0),
    m_cscIdHelper(0),
    m_mdtIdHelper(0),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool")
{
    declareInterface<IMuonCurvedSegmentCombiner>(this);
    declareInterface<IMuonSegmentPairFittingTool>(this);

    declareProperty("DoDebug",m_debug = false);

    declareProperty("DoSummary",m_summary = false);

    declareProperty("DoCosmics",m_doCosmics = false);


  // ATLAS default use Csc segments
  // Cosmics default no do NOT use Csc segments

    declareProperty( "UseCscSegments",m_useCscSegments = true );

    declareProperty( "AddAll2DCscs",m_addAll2DCscs = true);

    declareProperty("MergePatterns",m_mergePatterns = false);

    declareProperty("MissedHitsCut",m_missedHitsCut = 100);

    declareProperty("PullAssociationCut",m_pullAssociationCut = 5.);

    declareProperty("PhiAssociationCut1",m_phiAssociationCut1 = 0.5);

    declareProperty("PhiAssociationCut2",m_phiAssociationCut2 = 0.25);

    declareProperty("AddUnassociatedMiddleEndcapSegments",m_addUnassociatedMiddleEndcapSegments = false) ;

}

//================ Destructor =================================================

Muon::MuonCurvedSegmentCombiner::~MuonCurvedSegmentCombiner()
    {}


//================ Initialisation =================================================

StatusCode Muon::MuonCurvedSegmentCombiner::initialize()
{

    StatusCode sc = AlgTool::initialize();
    if (sc.isFailure()) return sc;

    StoreGateSvc* detStore=0;
    sc = serviceLocator()->service("DetectorStore", detStore);

    if ( sc.isSuccess() ) {
        sc = detStore->retrieve( m_detMgr );
        if ( sc.isFailure() ) {
            ATH_MSG_ERROR(" Cannot retrieve MuonDetDescrMgr ");
        } else {
            m_mdtIdHelper = m_detMgr->mdtIdHelper();
            m_cscIdHelper = m_detMgr->cscIdHelper();
            m_rpcIdHelper = m_detMgr->rpcIdHelper();
            m_tgcIdHelper = m_detMgr->tgcIdHelper();
            ATH_MSG_DEBUG(" Retrieved IdHelpers: (mdt, csc, rpc and tgc) ");
        }
    } else {
        ATH_MSG_ERROR(" MuonDetDescrMgr not found in DetectorStore ");
    }

    sc = m_printer.retrieve();
    if (sc.isSuccess()){
        ATH_MSG_DEBUG("Retrieved " << m_printer );
    }else{
        ATH_MSG_FATAL("Could not get " << m_printer ); 
        return sc;
    }
    
    ATH_MSG_INFO("initialize() successful in " << name() );
    return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Muon::MuonCurvedSegmentCombiner::finalize()
{
    StatusCode sc = AlgTool::finalize();
    return sc;
}

//============================================================================================

std::unique_ptr<MuonSegmentCombinationCollection>
Muon::MuonCurvedSegmentCombiner::combineSegments(   const MuonSegmentCombinationCollection& mdtCombiColl, 
                                                    const MuonSegmentCombinationCollection& csc4DCombiColl, 
                                                    const MuonSegmentCombinationCollection& csc2DCombiColl,
						    MuonSegmentCombPatternCombAssociationMap* segPattMap) 
{
    m_segInfoMap.clear();
    m_seg2DCscInfoMap.clear();
    m_seg4DCscInfoMap.clear();
    m_segMdtInfoMap.clear();
    m_cscIdSet.clear();
    m_segAssoMap.clear(); 

    m_segmentIndex = 0;
 
    if( msgLvl(MSG::DEBUG) || m_summary) {
        if( mdtCombiColl.empty() ) ATH_MSG_INFO(" summarizing input: Mdt MuonSegment combinations empty" );
        else ATH_MSG_INFO(" summarizing input: Mdt MuonSegment combinations " << std::endl << m_printer->print( mdtCombiColl ) );
    }



      // Csc segments
    if( m_useCscSegments ){

        ATH_MSG_DEBUG(" Retrieved CSC 4D MuonSegmentCombinationCollection "  << csc4DCombiColl.size() );

        if( msgLvl(MSG::DEBUG) || m_summary) {
            if( csc4DCombiColl.empty() ) ATH_MSG_INFO(" summarizing input: Csc MuonSegment combinations empty" );
            else ATH_MSG_INFO(" summarizing input: Csc MuonSegment combinations " << std::endl << m_printer->print( csc4DCombiColl ) );
        }
        processCscCombinationCollection(csc4DCombiColl,segPattMap);

        // Csc 2D segments

        if( msgLvl(MSG::DEBUG) || m_summary) {
            if( csc2DCombiColl.empty() ) ATH_MSG_INFO(" summarizing input: Csc 2D MuonSegment combinations empty" );
            else ATH_MSG_INFO(" summarizing input: Csc 2D MuonSegment combinations " << std::endl << m_printer->print( csc2DCombiColl ) );
        }
        process2DCscCombinationCollection(csc2DCombiColl);
    }

    return processCombinationCollection(mdtCombiColl,segPattMap);

}

std::unique_ptr<MuonSegmentCombinationCollection> Muon::MuonCurvedSegmentCombiner::processCombinationCollection(const MuonSegmentCombinationCollection& mdtCol, 
														MuonSegmentCombPatternCombAssociationMap* segPattMap){

  std::unique_ptr<MuonSegmentCombinationCollection> curvedCombiCol(new MuonSegmentCombinationCollection);

    int nmdtsegments = 0;
    int ncscsegments =  m_seg2DCscInfoMap.size() + m_seg4DCscInfoMap.size();

    if( m_debug )  std::cout << " in processCombinationCollection " << std::endl; 
    
    MuonSegmentCombinationCollection::const_iterator cit = mdtCol.begin();
    MuonSegmentCombinationCollection::const_iterator cit_end = mdtCol.end();
    for(; cit!=cit_end;++cit ){
        const Muon::MuonSegmentCombination* combi = *cit;
        if( !combi ) {
            ATH_MSG_DEBUG(" empty Mdt MuonSegmentCombination!!! " );
            continue;
        }

  // get association to pattern

	std::pair<MuonSegmentCombPatternCombAssociationMap::const_iterator, MuonSegmentCombPatternCombAssociationMap::const_iterator> range = segPattMap->equal_range(combi);
        if ((range.first)==(range.second)) {
            ATH_MSG_WARNING("MDT Combination missing from the map - something is wrong! Skip combination");
            continue;
        }

        
        const MuonPatternCombination* pattern = (range.first)->second;

	if( m_debug )  std::cout << " handling combi " << combi << " associated pattern " << pattern << std::endl; 

//        Muon::MuonSegPatAssMap::object_list combiAssos; 
        // if (assMap) {
        //     assMap->getObjects( combi, combiAssos );
        if( segPattMap->count(combi) != 1 ){
            ATH_MSG_INFO(" This MuonSegPatAssMap for MDTs should only have one entry!! ");
        }
// Take 2D Csc segments
        m_segInfoMap = m_seg2DCscInfoMap;
        SegInfoIt seg_it = m_seg4DCscInfoMap.begin();
        SegInfoIt seg_it_end = m_seg4DCscInfoMap.end();
        for(; seg_it != seg_it_end; ++seg_it){
           Muon::MuonSegment* segm = seg_it->first;
           if( m_seg4DCscInfoMap[segm].contains(pattern)) {
// Add 4D Csc segment associated to this pattern
             Muon::MCSCSegmentInfo info = seg_it->second;
             info.patPoint = pattern;
             m_segInfoMap[segm] = info;
           }
        }
        bool addedMdtSegments = false;
	for(unsigned int i=0;i<combi->numberOfStations();++i){
	  const std::vector<std::unique_ptr<Muon::MuonSegment> >& segs = *combi->stationSegments(i);
	  if (m_debug) std::cout << " station " << i << " has MDT segments: " << segs.size() << std::endl;
	  for(unsigned int si=0;si<segs.size();++si){
	    if(m_debug) std::cout << " Store Mdt segment " << si << " pointer " << segs[si].get() << " Rios " << segs[si]->numberOfContainedROTs() << std::endl; 
	    bool is_mdt=true;
	    for (unsigned int ri=0;ri<segs[si]->numberOfContainedROTs(); ri++){
	      if (m_mdtIdHelper->is_mdt(segs[si]->rioOnTrack(ri)->identify()) == true) break;
	      if (m_cscIdHelper->is_csc(segs[si]->rioOnTrack(ri)->identify()) == true) {is_mdt = false; break;}
	    }
	    if (is_mdt == false) continue; 
	    std::map<Muon::MuonSegment*, const MuonPatternCombination* >::iterator it = m_segAssoMap.find(segs[si].get());
	    nmdtsegments++;
	    if( it == m_segAssoMap.end() ){
	      m_segAssoMap[segs[si].get()] = pattern;
	    }else{
	      if( it->second != pattern ) ATH_MSG_INFO(" MDT Segment pointers should only be found once!! ");
	      it->second=pattern;
	    }
	    Muon::MCSCSegmentInfo info = segInfo(segs[si].get());
	    // Add Mdt segment associated to this pattern
            m_segmentIndex++;
            info.index = m_segmentIndex;
	    m_segInfoMap[segs[si].get()] = info;
	    m_segMdtInfoMap[segs[si].get()] = info;
	    addedMdtSegments = true;
	  }
	}
        
        if ( addedMdtSegments ) muonCurvedSegmentCombinations(curvedCombiCol.get(),segPattMap);
    }
    if (m_debug) std::cout << " First stage muonCurvedSegmentCombinations " << curvedCombiCol->size() << " Mdt segments " << nmdtsegments << " Csc segments " << ncscsegments << std::endl;
   
    bool mergePatterns = false;
    if (m_mergePatterns) mergePatterns = true;
    if (m_doCosmics) mergePatterns = true;
    if (m_doCosmics && (curvedCombiCol->size()  > 4 || nmdtsegments + ncscsegments > 20)) mergePatterns = false;

    if (mergePatterns) {

      if( m_debug )  std::cout << " merging patterns " << std::endl;
	
// Clear segment association map
     m_segAssoMap.clear(); 
     cit = mdtCol.begin();
     for(; cit!=cit_end;++cit ){
        const Muon::MuonSegmentCombination* combi = *cit;
        if( !combi ) continue;
	if (combi->numberOfStations() == 0) continue;
  // get association to pattern
	std::pair<MuonSegmentCombPatternCombAssociationMap::const_iterator, MuonSegmentCombPatternCombAssociationMap::const_iterator> range = segPattMap->equal_range(combi);
        if ((range.first)==(range.second)) {
            continue;
        }
        const MuonPatternCombination* pattern = (range.first)->second;
        if( segPattMap->count(combi) != 1 ){
            ATH_MSG_INFO(" This MuonSegPatAssMap for MDTs should only have one entry!! ");
        }

// Leave out 2D Csc segments
//        m_segInfoMap = m_seg2DCscInfoMap;
        m_segInfoMap.clear(); 
 
        MuonSegmentCombinationCollection::const_iterator cit1 = cit;
        for(; cit1!=cit_end;++cit1 ){

          const Muon::MuonSegmentCombination* combi1 = *cit1;
          if( !combi1 ) continue;
	  if (combi1->numberOfStations() == 0) continue;

          if (combi1 == combi) continue;
  // get association to pattern
	  std::pair<MuonSegmentCombPatternCombAssociationMap::const_iterator, MuonSegmentCombPatternCombAssociationMap::const_iterator> range1 = segPattMap->equal_range(combi1);
          if ((range1.first)==(range1.second)) {
              continue;
          }
          const MuonPatternCombination* pattern1 = (range1.first)->second;
          if( segPattMap->count(combi1) != 1 ){
            ATH_MSG_INFO(" This MuonSegPatAssMap for MDTs should only have one entry!! ");
         } 

	  if( m_debug )  std::cout << " handling combi " << combi << " associated pattern " << pattern1 << std::endl; 

          double patternPhi, patternPhi1 ;
          double patternTheta, patternTheta1;
          double patternMom = 0., patternMom1 = 0.;
	  Amg::Vector3D patternDir;
	  Amg::Vector3D patternDir1;

          const Trk::TrackParameters* trkparameters = pattern->trackParameter();
          if (trkparameters) {
             const Trk::Perigee* perigee = 0;
             perigee = dynamic_cast<const Trk::Perigee*>(trkparameters);
             if (perigee) {
// Get theta and phi values
               patternPhi = perigee->momentum().phi();
               patternTheta = perigee->momentum().theta();
               patternMom = perigee->momentum().mag();
	       patternDir = perigee->momentum();
             } else {continue;}
          } else {continue;}
          

          trkparameters = pattern1->trackParameter();
          if (trkparameters) {
             const Trk::Perigee* perigee = 0;
             perigee = dynamic_cast<const Trk::Perigee*>(trkparameters);
             if (perigee) {
// Get theta and phi values
               patternPhi1 = perigee->momentum().phi();
               patternTheta1 = perigee->momentum().theta();
               patternMom1 = perigee->momentum().mag();
	       patternDir1 = perigee->momentum();
             } else {continue;}
          } else {continue;}

// Cuts on angular association of the two patterns

          // if(acos (cos(patternPhi)*cos(patternPhi1) + sin(patternPhi)*sin(patternPhi1))  > 0.3) continue; 
	  // that is the same as: (acos(x)>0.3 <--> x < cos (0.3)= 0.955)
	  if ((cos(patternPhi)*cos(patternPhi1) + sin(patternPhi)*sin(patternPhi1)) < 0.955) continue; 


	  // for (cosmics) patterns that are split and have flipped direction:
          if(fabs(patternTheta-patternTheta1) > 1.5) continue; 

// Cosmics can be rotated in theta 

	  // if(acos (cos(patternTheta)*cos(patternTheta1) + sin(patternTheta)*sin(patternTheta1))  > 0.3 && m_mergePatterns && !m_doCosmics) continue; 
	  // same as:

          if((cos(patternTheta)*cos(patternTheta1) + sin(patternTheta)*sin(patternTheta1)) < 0.955 && m_mergePatterns && !m_doCosmics) continue; 

	  // only merge patterns in same hemisphere (only for cosmics)

	  if (m_doCosmics) {

	    // check if any 2 stations in combis are compatible, else combi's completely in other hemisphere:
	    // could probably be implemented faster (loop over all stations of 1 combi first)
	    bool compatible = false;

	    for(unsigned int i=0;i<combi->numberOfStations();++i){
	      const std::vector<std::unique_ptr<Muon::MuonSegment> >& segs = *combi->stationSegments(i);
	      if (!segs.empty()) {
		const Amg::Vector3D& segpos = segs[0]->globalPosition();
		double dotprod = segpos.x()*patternDir.x() + segpos.y()*patternDir.y() + segpos.z()*patternDir.z();
		
		for(unsigned int j=0;j<combi1->numberOfStations();++j){
		  const std::vector<std::unique_ptr<Muon::MuonSegment> >& segs1 = *combi1->stationSegments(j);
		  if (!segs1.empty()) {
		    const Amg::Vector3D& segpos1 = segs1[0]->globalPosition();
		    double dotprod1 = segpos1.x()*patternDir1.x() + segpos1.y()*patternDir1.y() + segpos1.z()*patternDir1.z();
		    
		    // if patterns are in opposite hemisphere (assuming directions are more or less same) 
		    if ((dotprod < 0 && dotprod1 < 0) || (dotprod > 0 && dotprod1 > 0)) {
		      compatible = true;
		    }
		  }
		  if (compatible == true) break;
		}
	      }
	      if (compatible == true) break;
	    }
	    if (compatible == false) continue;
	  }
	  
          const MuonPatternCombination* patternSelect = pattern; 
          if (!m_doCosmics && fabs(patternMom1)> fabs(patternMom)) patternSelect = pattern1; 
          if ( m_doCosmics && pattern1->chamberData().size() > pattern->chamberData().size() ) patternSelect = pattern1;


// Take 2D Csc segments
          m_segInfoMap = m_seg2DCscInfoMap;
          SegInfoIt seg_it = m_seg4DCscInfoMap.begin();
          SegInfoIt seg_it_end = m_seg4DCscInfoMap.end();
          for(; seg_it != seg_it_end; ++seg_it){
           Muon::MuonSegment* segm = seg_it->first;
           if( m_seg4DCscInfoMap[segm].contains(pattern) || m_seg4DCscInfoMap[segm].contains(pattern1)) {
// Add 4D Csc segment associated to this pattern
             Muon::MCSCSegmentInfo info = seg_it->second;
             info.patPoint = patternSelect;
             m_segInfoMap[segm] = info;
           }
          }
          bool addedMdtSegments = false;
	  for(unsigned int i=0;i<combi->numberOfStations();++i){
	    const std::vector<std::unique_ptr<Muon::MuonSegment> >& segs = *combi->stationSegments(i);
	    if (m_debug) std::cout << " station " << i << " has MDT segments: " << segs.size() << std::endl;
	    for(unsigned int si=0;si<segs.size();++si){
	      if(m_debug) std::cout << " Store Mdt segment " << si << " pointer " << segs[si].get() << " Rios " << segs[si]->numberOfContainedROTs() << std::endl; 
	      bool is_mdt=true;
	      for (unsigned int ri=0;ri<segs[si]->numberOfContainedROTs(); ri++){
		if (m_mdtIdHelper->is_mdt(segs[si]->rioOnTrack(ri)->identify()) == true) break;
		if (m_cscIdHelper->is_csc(segs[si]->rioOnTrack(ri)->identify()) == true) {is_mdt = false; break;}
	      }
	      if (is_mdt == false)continue; 
	      
	      std::map<Muon::MuonSegment*, const MuonPatternCombination* >::iterator it = m_segAssoMap.find(segs[si].get());
	      if( it == m_segAssoMap.end() ){
		m_segAssoMap[segs[si].get()] = patternSelect;
	      }else{
		  it->second=patternSelect;
	      }
	      Muon::MCSCSegmentInfo info = segInfo(segs[si].get());
		// Add Mdt segment associated to this pattern
	      m_segInfoMap[segs[si].get()] = info;
	      m_segMdtInfoMap[segs[si].get()] = info;
	      addedMdtSegments = true;
	      }
	  }
	
	  for(unsigned int i=0;i<combi1->numberOfStations();++i){
	    const std::vector<std::unique_ptr<Muon::MuonSegment> >& segs = *combi1->stationSegments(i);
	    if (m_debug) std::cout << " station " << i << " has MDT segments: " << segs.size() << std::endl;
	    for(unsigned int si=0;si<segs.size();++si){
	      if(m_debug) std::cout << " Store Mdt segment " << si << " pointer " << segs[si].get() << " Rios " << segs[si]->numberOfContainedROTs() << std::endl; 
	      bool is_mdt=true;
	      for (unsigned int ri=0;ri<segs[si]->numberOfContainedROTs(); ri++){
		if (m_mdtIdHelper->is_mdt(segs[si]->rioOnTrack(ri)->identify()) == true) break;
		if (m_cscIdHelper->is_csc(segs[si]->rioOnTrack(ri)->identify()) == true) {is_mdt = false; break;}
	      }
	      if (is_mdt == false)continue; 
	      std::map<Muon::MuonSegment*, const MuonPatternCombination* >::iterator it = m_segAssoMap.find(segs[si].get());
	      if( it == m_segAssoMap.end() ){
		m_segAssoMap[segs[si].get()] = patternSelect;
	      }else{
		it->second=patternSelect;
	      }
	      Muon::MCSCSegmentInfo info = segInfo(segs[si].get());
	      // Add Mdt segment associated to this pattern
	      m_segInfoMap[segs[si].get()] = info;
	      m_segMdtInfoMap[segs[si].get()] = info;
	      addedMdtSegments = true;
	    }
	  }
	  
          if ( addedMdtSegments ) muonCurvedSegmentCombinations(curvedCombiCol.get(),segPattMap);
        }
     }
     if (m_debug) std::cout << " Second stage muonCurvedSegmentCombinations " << curvedCombiCol->size() << std::endl;
    } // end mergePatterns

    if( msgLvl(MSG::DEBUG) || m_summary) {
            if( curvedCombiCol->empty() ) ATH_MSG_INFO(" summarizing output: MuonCurvedSegment combinations empty");
            else ATH_MSG_INFO(" summarizing output: MuonCurvedSegment combinations " << std::endl << m_printer->print( *curvedCombiCol ) );
    }
    return curvedCombiCol;
}

void
Muon::MuonCurvedSegmentCombiner::processCscCombinationCollection( const MuonSegmentCombinationCollection& combiCol, MuonSegmentCombPatternCombAssociationMap* segPattMap) {
    MuonSegmentCombinationCollection::const_iterator cit = combiCol.begin();
    MuonSegmentCombinationCollection::const_iterator cit_end = combiCol.end();
    for(; cit!=cit_end;++cit ){
        const Muon::MuonSegmentCombination* combi = *cit;
        if( !combi ) {
            ATH_MSG_DEBUG(" empty Csc 4D MuonSegmentCombination!!! ");
            continue;
        }
        if(combi->numberOfStations() > 0 ){
      // get association to pattern
            // Muon::MuonSegPatAssMap::object_list combiAssos; 
            // if (m_assCscMap) {
            //     m_assCscMap->getObjects( combi, combiAssos );
            // }
	  std::pair<MuonSegmentCombPatternCombAssociationMap::const_iterator, MuonSegmentCombPatternCombAssociationMap::const_iterator> range = segPattMap->equal_range(combi);
            if ((range.first)==(range.second)) {
                ATH_MSG_DEBUG("CSC Combination missing from the map - can happen");
                return;
            }
            
            const MuonPatternCombination* pattern = (*(range.first)).second;

            for(unsigned int i=0;i<combi->numberOfStations();++i){
	      const std::vector<std::unique_ptr<Muon::MuonSegment> >& segs = *combi->stationSegments(i);
                if(m_debug) std::cout << " station " << i << " has Csc segments: " << segs.size() << std::endl;
                for(unsigned int si=0;si<segs.size();++si){
                    if(m_debug) std::cout << " Store Csc segment " << si << std::endl; 
                    bool is_csc = false;
                    for (unsigned int ri=0;ri<segs[si]->numberOfContainedROTs(); ri++){
                        if (m_cscIdHelper->is_csc(segs[si]->rioOnTrack(ri)->identify())) { 
                            is_csc = true;
                            m_cscIdSet.insert(segs[si]->rioOnTrack(ri)->identify());
                            if (m_debug) std::cout << " csc hits on 4d segment " << m_cscIdSet.size() << std::endl;
                        }
                    }
                    if (is_csc == false) continue; 
                    std::map<Muon::MuonSegment*, const MuonPatternCombination* >::iterator it = m_segAssoMap.find(segs[si].get());
                    if( it == m_segAssoMap.end() ){
		      m_segAssoMap[segs[si].get()] = pattern;
		    }else{
		      it->second=pattern;
		    }
//                 4D Csc segments can be on more than 1 pattern 
                    Muon::MCSCSegmentInfo info = segInfo(segs[si].get());
                    m_segmentIndex++;
                    info.index = m_segmentIndex;
                    MuonSegmentCombPatternCombAssociationMap::const_iterator ia_it = range.first;
                    MuonSegmentCombPatternCombAssociationMap::const_iterator ia_it_end = range.second;
                    for(; ia_it != ia_it_end; ++ia_it){
                      info.patPointers.push_back((*ia_it).second);
                    }
                    m_seg4DCscInfoMap[segs[si].get()] = info;
                    if(m_debug) std::cout << " Store Csc segment " << " associated pattern size " << info.patPointers.size() << std::endl; 
                }
            }
        }
    }
}

void
Muon::MuonCurvedSegmentCombiner::process2DCscCombinationCollection( const MuonSegmentCombinationCollection& combiCol) {
    MuonSegmentCombinationCollection::const_iterator cit = combiCol.begin();
    MuonSegmentCombinationCollection::const_iterator cit_end = combiCol.end();
    for(; cit!=cit_end;++cit ){
        const Muon::MuonSegmentCombination* combi = *cit;
        if( !combi ) {
            ATH_MSG_DEBUG(" empty Csc 2D MuonSegmentCombination!!! ");
            continue;
        }

	for(unsigned int i=0;i<combi->numberOfStations();++i){
	  const std::vector<std::unique_ptr<Muon::MuonSegment> >& segs = *combi->stationSegments(i);
	  if(m_debug) std::cout << " station " << i << " has Csc segments: " << segs.size() << std::endl;
	  for(unsigned int si=0;si<segs.size();++si){
	    if(m_debug) std::cout << " Store Csc segment? " << si << std::endl; 
	    bool is_csc = false; 
	    int ncsc = 0;
	    int ncscmatched = 0;
	    for (unsigned int ri=0;ri<segs[si]->numberOfContainedROTs(); ri++){
	      if (m_cscIdHelper->is_csc(segs[si]->rioOnTrack(ri)->identify()) && ! m_cscIdHelper->measuresPhi(segs[si]->rioOnTrack(ri)->identify()) ) {
		is_csc = true; 
		ncsc++; 
		if (m_cscIdSet.find(segs[si]->rioOnTrack(ri)->identify())!=m_cscIdSet.end()) ncscmatched++;
	      }
	    }
	    if (is_csc == false) continue; 
	    if(m_debug) std::cout << " Csc eta hits " << ncsc << " Matched with 4D " << ncscmatched << std::endl; 
	    if (ncsc == ncscmatched && !m_addAll2DCscs) continue; 

	    // 2D Csc segments are NOT associated to Pattern

	    m_segAssoMap[segs[si].get()] = 0;
	    Muon::MCSCSegmentInfo info = segInfo(segs[si].get());
            m_segmentIndex++;
            info.index = m_segmentIndex;
	    m_seg2DCscInfoMap[segs[si].get()] = info;
	    //if(m_debug) std::cout << " Store Csc eta segment " << " associated pattern size " << combiAssos.size() << std::endl; 
	  }
	}
    }
}

void Muon::MuonCurvedSegmentCombiner::muonCurvedSegmentCombinations(MuonSegmentCombinationCollection* curvedCombiCol, MuonSegmentCombPatternCombAssociationMap* segPattMap) {

  // Strategy for makening trackcandidates: combinations of segments
  // Fill vectors with segment information MCSCSegmentInfo: 
  //      e.g. station, missed hits, and crude momentum estimate
  // Make ordering of segments according to multiplicity (per chamber), missed hits and momentum orderSegments
  // For high multiplicities: take only first 10 segments per chamber

  // Search for combinations is done pair wise
  // Short and Long chambers in same type (e.g. BOS and BOL )-> use Straightline fit to two segments
  // Different chambers (B vs E/F or I vs M vs O vs E) -> use Curved Fit to two segments   

  // Association cuts: a) Same pattern (pointer)  
  //                   b) Cosmics: dotprod phi1 phi2 > 0
  //                   c) Add 2D segment CSC (not associated to pattern); extrapolate pattern < 500 mm
  //                   d) Add no phi hits - phi hits  within m_phiAssociationCut1 (0.25)
  //                             both have phi hits  and same phi angle on pattern 
  // If all residuals in Pull space are within m_pullAssociatioCut (5) 

  // The pair is added to the list of combination
  // if one of the segments is already associated the other segment is added this combination


  // Multiplicities of segments per unique Chamber code and per Pattern
  // Count segments: MDT -> one per pattern
  //                 CSC -> 0 or more patterns
    std::map<int , int> segmentsChamber;

    SegInfoIt seg_it = m_segInfoMap.begin();
    SegInfoIt seg_it_end = m_segInfoMap.end();
    int nSegments = 0;
    for(; seg_it != seg_it_end; ++seg_it){
        Muon::MuonSegment* segm = seg_it->first;
        int npSize = 0;
        if (m_segAssoMap.count(segm)>0) {
            npSize = npSize + m_segAssoMap.count(segm);
        } else {
            npSize = 1;
        }
        nSegments += npSize;  
    }

  // Vectors for storing segment information
    std::vector<Muon::MuonSegment*> segment(nSegments);
    std::vector<std::string> station(nSegments); // station string 
    std::vector<int> iPhiPatHits(nSegments); // phi hits on pattern
    std::vector<double> patPhi(nSegments); // phi value of pattern  
    std::vector<double> patTheta(nSegments); // theta value of pattern  
    std::vector<int> multi(nSegments); // multiplicity in chamber
    std::vector<int> missedHits(nSegments); // missed hits in MDT 
    std::vector<int> missedLay(nSegments); // missed layers hit in trigger Chambers
    std::vector<int> nHots(nSegments); // hit on track for MDT or CSC segments 
    std::vector<double> momentum(nSegments); // momentum estimate
    std::vector<int> drop(nSegments); // drop segment = 1 ; keep it 0
    std::vector<const Muon::MuonPatternCombination*> patPoint(nSegments); // phi measurement on segment

//
// Order the segments according to stationcode, missed (trigger) hits, and chi2 
//
    std::map<double , Muon::MuonSegment*> checkSegments;
    std::map<int , Muon::MuonSegment*> checkSegmentsIndex;
//
// assign an integer to each pattern needed to count segments per pattern per chamber 
//
    std::map <const Muon::MuonPatternCombination*, int > patIndex;

    seg_it = m_segInfoMap.begin();
    seg_it_end = m_segInfoMap.end();
    int patIn = -1;
    int patInlast = -1;
    for(; seg_it != seg_it_end; ++seg_it){
       Muon::MuonSegment* segm = seg_it->first;
       const Muon::MCSCSegmentInfo& info = m_segInfoMap[segm];
       if (info.patPoint!=0) {
         if (patIndex.count(info.patPoint)!=1) {
          patInlast++;
          patIndex[info.patPoint] = patInlast;
          patIn = patInlast;
         } else {
          patIn = patIndex[info.patPoint];
         }
       } else {
          patIn = 0;
       }
//
// Built key to distinguish segments  
//
       int code = info.stationCode;
       int missedh = info.nMissedHits;
       if ( missedh < 0 ) missedh = 0;
       if ( missedh > 10 ) missedh = 10;
       double layMissed  = info.layMissed;
       int ntrigFrac = (int) layMissed*10; 
       double theta =  segm->globalDirection().theta();
       double lchi2 =  info.chi2/info.nHots;
       int patternHits = info.patHits;
       std::string st = info.station;
       if (st[0] == 'C') patternHits = 0;
       int ntot = info.nHots;
       if ( ntot < 5 ) ntot = 5;

       int miscorrect = 0;
       if(info.phiPatHits == 0) miscorrect = 2;
       if(info.nLayTrig > 0) miscorrect = 2;
       if(info.nLayTrig > 2) miscorrect = 3;
       
       int missedHitsCut = m_missedHitsCut;
       if( info.station[1] == 'I' ) ++missedHitsCut;

       if( 5*(missedh-miscorrect) > ntot*missedHitsCut) {
            if (m_debug) std::cout << " Drop this segment with missed hits " << missedh << " correction " << miscorrect << " phi pat hits " << info.phiPatHits << " trigger layers " << info.nLayTrig << std::endl; 
            continue;
       }
       int indexSegment = info.index;
       double key = 10000*(code+1)+missedh*100+ntrigFrac*10 +10*lchi2 - (indexSegment/10000.) - patternHits - 100*info.nHots ;
       if(checkSegmentsIndex.count(indexSegment)==1) {
         if (m_debug) std::cout << " PROBLEM with Segments Index is already in the checkSegmentsIndex collection " << indexSegment << std::endl;
       }
       checkSegmentsIndex[indexSegment] = segm;
       if (checkSegments.count(key)==1) { 
         const Muon::MCSCSegmentInfo& info2 = m_segInfoMap[checkSegments[key]];
         double theta2 =  checkSegments[key]->globalDirection().theta();
         std::string st = info.station;
         if (m_debug) std::cout << " PROBLEM this key is already in the checkSegments collection " << key << " station " << st <<  " pointer1 " << info.patPoint << " pointer2 " << info2.patPoint << " theta " << theta << " theta2 " << theta2 << std::endl;
       }
       checkSegments[key] = segm;
       if (info.patPoint!=0) {
         segmentsChamber[1000*code+patIn] += 1;
       } else {
         segmentsChamber[1000*code] += 1;
       }
    }

    std::map<double , Muon::MuonSegment*>::iterator ic_it = checkSegments.begin();
    std::map<double , Muon::MuonSegment*>::iterator ic_it_end= checkSegments.end();

    if (m_debug) {
      std::cout << " Check Input Segments " << checkSegments.size() << std::endl ; 
      for(; ic_it != ic_it_end; ++ic_it){
        Muon::MuonSegment* seg = ic_it->second;
        const Muon::MCSCSegmentInfo& info = m_segInfoMap[seg];
        std::string st = info.station;
        int code = info.stationCode;
        double theta =  seg->globalDirection().theta();
        if (m_debug) std::cout << " Check Station " << st << " code " << code << " theta " << theta << " key " << ic_it->first << std::endl;
      }
    }
 
  // Order segments according to key including multiplicity per pattern and chamber
 
    std::map<int , Muon::MuonSegment*> orderSegments;
    int i = 0;
    ic_it = checkSegments.begin();
    for(; ic_it != ic_it_end; ++ic_it){
       Muon::MuonSegment* segm = ic_it->second;
       const Muon::MCSCSegmentInfo& info = m_segInfoMap[segm];

//     Built key from ordered segments per station and missed hits

        int code = info.stationCode;
        int missedh = info.nMissedHits;
        int nHots = info.nHots;
        if ( missedh < 0 ) missedh = 0;
        if ( missedh > 10 ) missedh = 10;

        double invc = info.invcurvature;
        double p = fabs(1./invc)/10.;
        int ip = (int) sqrt( p/1000.);
        if (ip > 10 ) ip = 10;
        ip = 10 - ip;
        int nLayTrig  = info.nLayTrig;
        std::string st = info.station;
        double layMissed  = info.layMissed;
        int ntrigFrac = (int) layMissed*10; 
        if (m_debug) std::cout << " station " << info.station << " missed hits " << missedh << " nLayTrig " << nLayTrig << " layers missed " << layMissed << " ntrigFrac " << ntrigFrac << std::endl;  
//        int ntot = nHots;
//        if ( ntot < 5 ) ntot = 5;
//        if( 5*missedh > ntot*m_missedHitsCut) {
//            if (m_debug) std::cout << " Drop this segment with missed hits " << missedh << std::endl; 
//            continue;
//        }
        int patIn = 0;
        if (info.patPoint!=0) {
          if (patIndex.count(info.patPoint)!=1)  if (m_debug) std::cout << " Pattern pointer not in PatIndex " << std::endl;
          patIn = patIndex[info.patPoint];
        } 
        int key = segmentsChamber[1000*code+patIn]*100000+missedh*10000+ntrigFrac*1000+1000*ip+i;
        if (info.nCsc == 1) key = key + 10000*(int) log(1.+info.chi2/nHots);
        if (info.nCsc == 1 && nHots < 7) key = key + 100000;
        if (m_debug) std::cout << " segments station " << segmentsChamber[1000*code+patIn] << " missed hits " << missedh << " ntrigFrac " << ntrigFrac << " ip " << ip << " i " << i << " key " << key << std::endl;  
        if (m_debug && info.nCsc == 1 ) std::cout << " Csc Segment " << info.nHots << " log 1 + chi2 dof " << log(1.+info.chi2/nHots) << std::endl; 

        if (orderSegments.count(key)==1) { 
         if (m_debug) std::cout << " PROBLEM this key is already in the orderSegments collection " << key << std::endl;
        }
        orderSegments[key] = segm;
        i++;
    }


  // Reset nSegments 

    std::map<int , Muon::MuonSegment*>::iterator io_it = orderSegments.begin();
    std::map<int , Muon::MuonSegment*>::iterator io_it_end= orderSegments.end();

  // Store segments and stations 
    if (m_debug) std::cout << " Summary Input Segments " << nSegments << std::endl ; 
    i = 0;
    int j0 =0;
    int code0 =0;
    for(; io_it != io_it_end; ++io_it){
        Muon::MuonSegment* seg = io_it->second;
        const Muon::MCSCSegmentInfo& info = m_segInfoMap[seg];
        std::string st = info.station;
        segment[i] = seg;
        station[i] = st;
        iPhiPatHits[i] = info.phiPatHits;
        patPhi[i] = info.patPhi;
        patTheta[i] = info.patTheta;
        patPoint[i] = info.patPoint;
        int code = info.stationCode;
        int patIn = 0;
        if (info.patPoint!=0) {
          patIn = patIndex[info.patPoint];
        } 
        multi[i] = segmentsChamber[1000*code+patIn];
        int miscorrect = 0;
        if(info.phiPatHits == 0) miscorrect = 2;
        if(info.nLayTrig > 0) miscorrect = 2;
        if(info.nLayTrig > 2) miscorrect = 3;
        missedHits[i] = info.nMissedHits - miscorrect;
        if(missedHits[i] < 0) missedHits[i];
        missedLay[i]  =  (int) info.layMissed;
        nHots[i] = info.nHots;
        double invc = info.invcurvature;
        double p = fabs(1./invc)/10.;
        momentum[i] = p;

        if (m_debug){
//            double phisp  = atan2( seg->globalPosition().y(), seg->globalPosition().x());
//            std::cout << " station " << st << " Segment position x " << seg->globalPosition().x() << " y "  << seg->globalPosition().y() << " z " << seg->globalPosition().z()<< " phi "  << seg->globalDirection().phi() << " phi space " << phisp <<" phi pat " << patPhi[i] << " theta " << seg->globalDirection().theta() << " theta pat " << patTheta[i] <<std::endl; 
//            std::cout << info << " pointer " << patPoint[i] << " MDT missed " << info.nMissedHits << " Trigger Missed " << info.layMissed  <<" missed Hits " << missedHits[i] << std::endl;
            std::cout << " Key station " << st <<  " theta " << seg->globalDirection().theta() << " key " << io_it->first << " patPhi " << patPhi[i] << " patTheta " << patTheta[i] << std::endl; 
        } 
        drop[i] = 0;
        if (multi[i] > 9) {
            j0++;
            if (j0 == 1) code0 = code;
            if (code == code0 ) {
                if (j0 > 9 ) drop[i] = 1;
            } else {
                j0 = 1;
                code0 = code;
            }
        }
        if (m_debug && drop[i] ==1 ) std::cout << "Drop this segment with multiplicity " << multi[i] << " drop  " << drop[i] << " station " << st << " theta " <<  seg->globalDirection().theta()  << std::endl; 
        i++;
    } 

    nSegments = i;
    if(m_debug) std::cout << " nsegments " << nSegments << " index " << i << std::endl ;   

    std::list <std::vector<int> > combiSegmentList(0);

    for (int i = 0; i <nSegments;++i) { 
        if ( drop[i] == 1 ) continue;
        std::string st1 = station[i];
        double theta1 = segment[i]->globalDirection().theta();
        bool segmentAdded = false; 
        for (int j = i+1; j <nSegments;++j) { 
            if ( drop[j] == 1 ) continue;
            std::string st2 = station[j];
            double theta2 = segment[j]->globalDirection().theta(); 
            if ( st1[0] == st2[0] && st1[1] == st2[1] && st1[2] == st2[2] ) continue;
//            if (m_debug) std::cout << " segment nr i begin  " << i << std::endl; 
//            if (m_debug) std::cout << " segment nr j begin  " << j << std::endl; 

            if (patPoint[i]!=0&&patPoint[j]!=0) {
                // Combine segments from same pattern
               // Cosmics: Combine all segments (cuts on angle applied later)
//  Also Cosmics on Pattern ...               
//                if (patPoint[i]!=patPoint[j]&&!m_doCosmics) {
                if (patPoint[i]!=patPoint[j]) {
                    bool selectPair = true;
                // Do not combine segments if NOT on same pattern and NO phi measurements  
                    if (iPhiPatHits[i]+iPhiPatHits[j] == 0) selectPair = false;
// Csc 4D segments check all patternpointers   
                    if ( st1[0] == 'C' ) {
                      selectPair = false; 
                      if( m_segInfoMap[segment[i]].contains(patPoint[j])) {
// Overwrite = store correct pointer!
                        patPoint[i] = patPoint[j];
                        selectPair = true; 
                      }
                    } else if ( st2[0] == 'C' ) {
                      selectPair = false; 
                      if( m_segInfoMap[segment[j]].contains(patPoint[i])) {
// Overwrite  = store correct pointer!
                        patPoint[j] = patPoint[i];
                        selectPair = true;
                      }
                    } else {
                      if (patPoint[i]!=patPoint[j]) selectPair = false; 
                    }  
                    if(!selectPair) {
                       if (m_debug) std::cout << "  Pair not selected i   "  << i << " j " << j << " station 1 " << st1 << " theta " << theta1 << " station 2 " << st2 << " theta " << theta2<<  " point i " << patPoint[i] << "point j " << patPoint[j] << " phipat i " << iPhiPatHits[i] << " phipat j " << iPhiPatHits[j] << std::endl; 
                      continue;
                    }
                }
                if (m_debug) std::cout << " OK pattern pointer i " << patPoint[i] << " j " << patPoint[j] << " i "  << i << " j " << j << " station 1 " << st1 << " theta " << theta1 << " station 2 " << st2 << " theta " << theta2<< std::endl; 
            } else {
                // If 2D CSC segments not on a pattern 
// if(m_assCscMap && st1[0] =='C'&&patPoint[i]!=0) continue;
// if(m_assCscMap && st2[0] =='C'&&patPoint[j]!=0) continue;
                if( st1[0] =='C'&&patPoint[i]!=0) continue;
                if( st2[0] =='C'&&patPoint[j]!=0) continue;
                if (m_debug) std::cout << " pattern pointer i " << patPoint[i] << " j " << patPoint[j] << std::endl; 
                // Extrapolate pattern Forward in CSC region
                if (st1[0] == 'C' && st2[0] == 'C') continue; 

                double hitz = 0.;
                double hitr = 0.;
                double r0 = 0.;
                double z0 = 0.;
                double theta = 0.;
                double invcurvature = 0.;
                int k = -1;
                if (st1[0] == 'C' ) {
                    hitz = segment[i]->globalPosition().z(); 
                    hitr = segment[i]->globalPosition().perp(); 
                    theta = segment[j]->globalDirection().theta(); 
                    z0 = segment[j]->globalPosition().z(); 
                    r0 = segment[j]->globalPosition().perp(); 
                    k = j;
                } 
                if( st2[0] == 'C') {
                    hitz = segment[j]->globalPosition().z(); 
                    hitr = segment[j]->globalPosition().perp(); 
                    theta = segment[i]->globalDirection().theta(); 
                    z0 = segment[i]->globalPosition().z(); 
                    r0 = segment[i]->globalPosition().perp(); 
                    k = i; 
                }   
                if (m_debug) std:: cout << " station " << st1 << " station j " << st2 << std::endl;
                if (m_debug) std::cout << " CSC  segment r " << hitr << " segment z " << hitz << " segment theta " << theta <<std::endl; 
                if (k > -1) {
                    if (patPoint[k] != 0) {
                        const Trk::TrackParameters* trkparameters = patPoint[k]->trackParameter(); 
                        if (trkparameters) {
                            const Trk::Perigee* perigee = 0;
                            perigee = dynamic_cast<const Trk::Perigee*>(trkparameters);
                            if (perigee) {
			      const Amg::VectorX parameters = perigee->parameters();
                                double phi = parameters[Trk::phi];
                                theta = parameters[Trk::theta];
                                r0 = - perigee->position().y()*std::cos(phi) + perigee->position().x()*std::sin(phi) ;
                                double charge = 1.;
                                if ( r0 < 0) charge = -1.;
                                if (m_debug) std::cout << " Pattern Momentum " << perigee->momentum().mag() << std::endl;
                                invcurvature = charge/(perigee->momentum().mag());
                                z0 = perigee->position().z();
                            }
                        }
                    } 
                } 
                if (fabs(cos(theta)) < 0.2) continue;
                double z_cylinder = 5000.; 
                int sign = 1;
                if (hitz < 0) sign = -1;
                double rext = r0 + (hitz-z0)*std::tan(theta) - (hitz-sign*z_cylinder)*(hitz-sign*z_cylinder)*invcurvature/std::cos(theta);
                double sdistance = (rext - hitr);
                if (m_debug) std::cout << " CSC extrapolation r0 " << r0 << " z0 " << z0 << " theta " << theta << " inv curvature " << invcurvature << " distance " << sdistance  << std::endl; 
                if (fabs(sdistance) > 500.) continue;
            }

// Skip segment combi with too many missed hits 

            int ntot = nHots[i]+nHots[j];
            if (ntot < 10 ) ntot = 10;
            if( st1[0]!=st2[0] || st1[1]!=st2[1] ) {
              bool dropmissed = false;
              if( 10 *(missedHits[i]+missedHits[j]) > ntot*m_missedHitsCut ) dropmissed = true;
              if ( st1[0]!=st2[0] || st1[2]!=st2[2] ) {
                int missedL = missedLay[i];
                if(missedLay[j] < missedL) missedL = missedLay[j];
                if( 10 *(missedHits[i]+missedHits[j]+missedL) > ntot*(m_missedHitsCut+1) ) dropmissed = true;
              } else {
                if( 10 *(missedHits[i]+missedHits[j]+missedLay[i]+missedLay[j]) > ntot*(m_missedHitsCut+1) ) dropmissed = true;
              }
              if (dropmissed) {
                if (m_debug) std::cout << " DROP MISSED i " << i << st1 << " missed i " << missedHits[i] << " missedLay i " <<  missedLay[i] <<  " j " << j << st2 << " missed j " << missedHits[j]  <<  " missedLay j " <<  missedLay[j] << " nHots i " << nHots[i] << " nHots j " << nHots[j] << std::endl; 
                 continue;
              }
            }
                //
            Amg::VectorX Res(4);
	    Res.setZero();
            Amg::VectorX Pull(4); 
	    Pull.setZero();
            double phisp  = atan2( segment[i]->globalPosition().y(), segment[i]->globalPosition().x());
            double phispe  = atan2( segment[j]->globalPosition().y(), segment[j]->globalPosition().x());
                //      double psi = ( cos(phisp)*cos(phispe) + sin(phisp)*sin(phispe) ); 
                //      double dphis = sin (segment[i]->globalDirection().phi())*sin (segment[j]->globalDirection().phi()) + cos (segment[i]->globalDirection().phi())*cos (segment[j]->globalDirection().phi()); 

                      // select closest in phi according to: angular phi or space phi
            double dphis = ( cos(patPhi[i])*cos(patPhi[j]) + sin(patPhi[i])*sin(patPhi[j]) ); 

            if (m_debug) {
	      // fpe protection: 
	      if (dphis>1) dphis=1.;
	      else if (dphis<-1) dphis=-1;
	      double cdphis = acos(dphis);
	      std::cout << " acos dphis  " << cdphis << " iphi1 " << iPhiPatHits[i] << " iphi2 " << iPhiPatHits[j] << std::endl; 
	      std::cout <<  " Phi space 1 " << phisp << " Phi space 2 " << phispe << " Phi 1 " << segment[i]->globalDirection().phi() << " Phi 2 " << segment[j]->globalDirection().phi() << " pattern phi 1 " << patPhi[i] << " pattern phi2 " << patPhi[j] << std::endl; 
	    }

            bool selectPair = false;
            if (patPoint[i]==patPoint[j] && patPoint[i]!=0)  {
                      // Accept all pairs from same pattern
                selectPair = true ;
            }
//            if (m_doCosmics && patPoint[i]!=patPoint[j]){
//                selectPair = true ;
//                double phii = segment[i]->globalDirection().phi();
//                double phij = segment[j]->globalDirection().phi();
//                double thetai = segment[i]->globalDirection().theta();
//                double thetaj = segment[j]->globalDirection().theta();
//                double dotprod = sin(thetai)*sin(thetaj)*cos(phii)*cos(phij) +  sin(thetai)*sin(thetaj)*sin(phii)*sin(phij) +  cos(thetai)*cos(thetaj);
//                double psicosmics = acos(dotprod);
//                if (psicosmics > 0.1) selectPair = false;
//           }
            if (patPoint[i]!=patPoint[j]){
                selectPair = false;
                if (patPoint[i]== 0 || patPoint[j] ==0)  {
                      // Accept all pairs with no link to pattern == 2D CSC segments 
                    selectPair = true ;
                }
                // Recuperate split patterns
                      // Phi measured for both segments
//                double pest = fabs(momentum[i]);
//                if (fabs(momentum[j]) < pest ) pest = fabs(momentum[j]);
//                if( pest < 5000) pest = 5000.;
//                      // phi cut not used for association
//                double phicut = m_phiAssociationCut2 * 5000. / pest;
//                if (patPhi[i] == patPhi[j] ) selectPair = true;
//                      // Phi measured for one of the segment 
//                if (fabs (cdphis) <   m_phiAssociationCut2 && (iPhiPatHits[i] == 0 || iPhiPatHits[j] ==  0) ) selectPair = true;
//                if (selectPair && m_debug) std::cout<< " Split pattern dphi " << cdphis << " momentum estimate " << pest << " phi cut " << phicut << std::endl; 
            }

            if (!selectPair&&m_debug) std::cout << " Pair not selected i " << i << " j " << j << std::endl;  
            if (selectPair) { 
                if ( st1[2] != st2[2] && st1[0] == st2[0] && st1[1] == st2[1] ) {
                    if (m_debug) std::cout << " Start SL fit 0 " << std::endl; 

                    fit2SegmentsSL(*segment[i],*segment[j], Res, Pull);
                //    if (m_doCosmics) fit2SegmentsC(segment[i],segment[j], Res, Pull);
                    } else if ( (st1[0] == 'E' || st1[0] == 'F') && ( st2[0] == 'E' || st2[0] == 'F') && 
                    ((st1[1] == 'O' && st2[1] == 'M')||(st2[1] == 'O' && st1[1] == 'M')) ) {
                        if (m_debug) std::cout << " Start SL fit 1 " << std::endl; 
                        fit2SegmentsSL(*segment[i],*segment[j], Res, Pull);
                //    if (m_doCosmics) fit2SegmentsC(segment[i],segment[j], Res, Pull); 
                    } else {
		      if (m_debug) std::cout << " Start full fit " << std::endl; 
		      if (!m_doCosmics) fit2Segments(*segment[i],*segment[j], Res, Pull);
		      if (m_doCosmics ){
			if( dphis > 0.) {
			  fit2SegmentsC(*segment[i],*segment[j], Res, Pull); 
			  if (fabs(Pull[0]) > m_pullAssociationCut || fabs(Pull[1]) > m_pullAssociationCut || 
			      fabs(Pull[2]) > m_pullAssociationCut || fabs(Pull[3]) > m_pullAssociationCut) {
			    fit2SegmentsSL(*segment[i],*segment[j], Res, Pull); 
			  }
			}else{
			  if( m_debug ) std::cout << " not doing any fit " << std::endl;
			  continue;
			}
		      }
		    }
                    if (m_debug) std::cout << " End fits Pull " << Pull[0] << " station 1 " << st1<< " station 2 " << st2 << " theta " << theta1 << " station 2 " << st2 << " theta " << theta2 << std::endl; 

                    // Good match
                    if (fabs(Pull[0]) < m_pullAssociationCut && fabs(Pull[1]) < m_pullAssociationCut && fabs(Pull[2]) < m_pullAssociationCut && fabs(Pull[3]) < m_pullAssociationCut) {
                        if (m_debug) std::cout << " Segment nr i,j associated  " << i << " " << " station " << st1 << " j " << j << " station " << st2  << " theta " << theta1 << " station 2 " << st2 << " theta " << theta2 << std::endl; 
                        segmentAdded = true;
                        std::list < std::vector<int> >::iterator it=combiSegmentList.begin();
                        std::list < std::vector<int> >::iterator it_begin=combiSegmentList.begin();
                        std::list < std::vector<int> >::iterator it_end=combiSegmentList.end();
                        std::list < std::vector<int> > addCombiSegmentList(0);

                        bool add = true;
                        if (combiSegmentList.size() == 0) add = false;

                        int itermax = combiSegmentList.size();
                        if( itermax > 5 && m_debug ) std::cout << " combinations " << itermax << std::endl;
                        if (itermax > 100) break; 
                        int iter = 0; 
                        for (; it!=it_end ; ++it) {
                            ++iter;
                            if (iter>itermax) {
                                if (m_debug) std::cout << "Problem should not occur iter " << iter << " itermax " << itermax <<std::endl;  
                                break;
                            }
                            if (m_debug) std::cout << " Combination length "<< std::distance(it_end,it_begin) << " index " << std::distance(it,it_begin) 
                                << " length " << std::distance(combiSegmentList.end(),it) << " len2 " << std::distance(combiSegmentList.end(),combiSegmentList.begin())<< std::endl; 
                            std::vector<int>& combi = *it;
                            bool iFound = false;
                            bool jFound = false;
                // reject combinations if on different patterns
                            bool reject = false;
                            for (unsigned int k = 0 ; k < combi.size() ; ++k ) {
                                if (m_debug) std::cout << " k " << k << " combi k " << combi[k] << std::endl; 
                                if (i == combi[k]) iFound = true;
                                if (j == combi[k]) jFound = true;
                                std::string stk = station[combi[k]];
                                if (patPoint[j]!=0 && patPoint[combi[k]]!=0) {
                                    if (patPoint[j]!= patPoint[combi[k]]) {
                                       if(st2[0]!='C' && stk[0]!='C') reject = true;
                                    }
                                }
                                if (patPoint[i]!=0 && patPoint[combi[k]]!=0) {
                                    if (patPoint[i]!= patPoint[combi[k]]) {
                                       if(st1[0]!='C' && stk[0]!='C') reject = true;
                                    }
                                }
                            }
                            if (m_debug && iFound) std::cout <<  " iFound  i " << i << " station " << station[i] << std::endl;
                            if (m_debug && jFound) std::cout <<  " jFound  j " << j << " station " << station[j] << std::endl;

                // Don't add segments to combination if they have different phi pattern values
                // Be carefull not to add same segments (if associated differently)
                            if ((iFound && !jFound) ||(!iFound && jFound) ) {
                //              if (phimaxi > 0.001) iFound = false;
                //              if (phimaxj > 0.001) jFound = false;
                              if (reject && m_debug) std::cout <<  " Try to add segment with different pattern pointer phi i " << patPhi[i] << " phi j " << patPhi[j] << " theta i " << patTheta[i] << " theta j " << patTheta[j] << std::endl;
                              if (reject) iFound = false;
                              if (reject) jFound = false;
                            }

                            add =true;
                            if (!iFound && !jFound ) add =false;

                            if (iFound && !jFound ) {
                                combi.push_back(j);
                                if (m_debug) std::cout << " Add to existing set j " << j << std::endl;
                            } 
                            if (jFound && !iFound ) {
                                combi.push_back(i);
                                if (m_debug) std::cout << " Add to existing set i " << i << std::endl;

                            }
                            if (m_debug) std::cout << " Loop combis " << i << std::endl;

                        //    write Information out 
                            if (m_debug) {
                                std::cout << " Check this combination " ;
                                std::vector<int>& combi0 = *it;
                                for (unsigned int k = 0 ; k < combi0.size() ; ++k ) {
                                    std::cout <<" segment nr " << combi0[k] << " Station " << station[combi0[k]] << std::endl ;
                                }
                            }
                        } // end loop Segmentcombis
                        if (!add ) {
                            std::vector<int> combi2(2);
                            combi2[0]= i;
                            combi2[1]= j;
                            combiSegmentList.push_back(combi2);
                            if (m_debug) std::cout << " End Make combination i " << i << " j " << j << std::endl;
                        }
                        if (m_debug) std::cout << " add segments to list " << addCombiSegmentList.size() << std::endl;

                        combiSegmentList.insert( combiSegmentList.end(),addCombiSegmentList.begin(),addCombiSegmentList.end() );

                        if (m_debug) {
                            int ncombi = 0;
                            std::list < std::vector<int> >::iterator it0=combiSegmentList.begin();
                            std::list < std::vector<int> >::iterator it0_end=combiSegmentList.end();
                            for (; it0!=it0_end ; ++it0) {
                                ncombi++;
                                std::cout << " Check after ADD combinations " << ncombi ;
                                std::vector<int>& combi0 = *it0;
                                for (unsigned int k = 0 ; k < combi0.size() ; ++k ) {
                                    std::cout <<" segment nr " << combi0[k] << " Station " << station[combi0[k]] << std::endl ;
                                }
                            }
                        }
                    } // end goodmatch m_pullAssociationCut standard deviations 
                }// end select pair 

//                if (m_debug) std::cout << " segment nr j end " << j << std::endl; 
            } // end segment j
//            if (m_debug) std::cout << " segment nr i end " << i << std::endl; 
            if (m_addUnassociatedMiddleEndcapSegments) {
              if (st1[0] == 'E' && st1[1] == 'M' ) {
                if (!segmentAdded) {
                  std::vector<int> combi(1);
                  combi[0] = i; 
                  combiSegmentList.insert(combiSegmentList.end(), combi );
                  if(m_debug) std::cout << " addUnassociatedMiddleEndcapSegment succesfull " << i  << " station " << st1 << std::endl;
                }
              }
            }
        } // end segment i

    // removal of residual subsets
        std::list < std::vector<int> >::iterator it_begin=combiSegmentList.begin();
        std::list < std::vector<int> >::iterator it=combiSegmentList.begin();
        std::list < std::vector<int> >::iterator it2=combiSegmentList.begin();
        std::list < std::vector<int> >::iterator it_end=combiSegmentList.end();
        while ( it!=it_end ) {
            std::vector<int>& combi1 = *it;
    //    if (m_debug) std::cout << " pos first "<< std::distance(it,it_begin)  << std::endl;
            bool erased = false; 
            it2 = it;
            ++it2;
            while (it2!=it_end ) {
      //        if (m_debug) std::cout << " pos second " << std::distance(it2,it_begin) << std::endl; 
                unsigned int overlap = 0;
                std::vector<int>& combi2 = *it2;
                for (unsigned int i = 0 ; i < combi1.size() ; ++i ) {
                    for (unsigned int j = 0 ; j < combi2.size() ; ++j ) {
                        if (combi1[i]==combi2[j]) {
                            overlap++;
                            break;
                        }
                    }
                }
      //        if (m_debug) std::cout << " Removing Overlaps " << overlap << " size 2: " << combi2.size() << " size 1:  " << combi1.size() << std::endl; 
                if (overlap == combi2.size()) {
                    it2 = combiSegmentList.erase(it2);
                    if (m_debug) std::cout << " erased second " << std::endl;    
                    if (m_debug) std::cout << " pos second " << std::distance(it2,it_begin) << std::endl; 
                    continue;
                } else if (overlap == combi1.size()) {    
                    it = combiSegmentList.erase(it);
                    erased = true;
                    if (m_debug) std::cout << " erased first " << std::endl;
                    break;
                }
                ++it2;
            } // it2
            if (!erased) ++it;
        } // it

  // Write out  MuonSegmentCombination for track candidates

  //Muon::MuonSegPatAssMap* assCurvedMap = new Muon::MuonSegPatAssMap();

        if (m_debug) std::cout << " Summary Segment Combinations found " << combiSegmentList.size() << std::endl;
        if (combiSegmentList.size()>0) {
            it =   combiSegmentList.begin();
            it_end=combiSegmentList.end();  
            int i = 0;           
            for (; it!=it_end ; ++it) {
                i++;
      // create MuonSegmentCombination for one track candidate 
                Muon::MuonSegmentCombination* combination = new Muon::MuonSegmentCombination();
                typedef std::map <std::string , std::unique_ptr<std::vector<std::unique_ptr<Muon::MuonSegment> > > > ChamberSort;
                typedef ChamberSort::iterator ChamberSortIt;
                ChamberSort chamberSort;

                std::vector<int>& combi = *it;
                if (m_debug) std::cout << "Segment combination " << i; 

                const Muon::MuonPatternCombination* patc = 0;
                for (unsigned int k = 0 ; k < combi.size() ; ++k ) {
                    if (patPoint[combi[k]]) patc = patPoint[combi[k]]; 
                    if (m_debug){
		      Muon::MuonSegment* seg = segment[combi[k]];
		      //const Muon::MCSCSegmentInfo& info = m_segInfoMap[seg];
		      double phisp  = atan2( seg->globalPosition().y(), seg->globalPosition().x());
		      std::cout << " Segment position x " << seg->globalPosition().x() << " y "  << seg->globalPosition().y() << " z " << seg->globalPosition().z()<< " phi "  << seg->globalDirection().phi() << " phi space " << phisp << " theta " << seg->globalDirection().theta() <<std::endl; 
		      //std::cout << info << std::endl;
                    } 
                    std::string st = station[combi[k]];
                    ChamberSortIt pos = chamberSort.find(st);
                    if (pos == chamberSort.end()) {
		      chamberSort[st] = std::make_unique<std::vector<std::unique_ptr<Muon::MuonSegment> > >();
		      chamberSort[st]->push_back(std::unique_ptr<Muon::MuonSegment>(segment[combi[k]]->clone()));
      //          if (m_debug) std::cout << " First segment in Station " << st << std::endl;
                    } else {
		      pos->second->push_back(std::unique_ptr<Muon::MuonSegment>(segment[combi[k]]->clone()));
      //          if (m_debug) std::cout << " Next segment in Station " << st << " size " << segPerStation->size() << std::endl;
                    }
                }
                ChamberSortIt cit = chamberSort.begin();
                ChamberSortIt cit_end = chamberSort.end();
                for (;cit !=cit_end; ++cit) {
		  combination->addSegments( std::move(cit->second) );
                }

                // if (patc) assCurvedMap->addAssociation( patternCol, patc, combiCol, combination );
                segPattMap->insert(std::make_pair(combination, patc));
                
                curvedCombiCol->push_back( combination );
            }
        }

        if( msgLvl(MSG::DEBUG) || m_summary) {
            if( curvedCombiCol->empty() ) ATH_MSG_INFO(" summarizing output: MuonCurvedSegment combinations empty");
            else ATH_MSG_INFO(" summarizing output: MuonCurvedSegment combinations " << std::endl << m_printer->print( *curvedCombiCol ) );
        }
    }

unsigned int 
Muon::MuonCurvedSegmentCombiner::missedHits( Muon::MuonSegment* seg) {

  // try to dynamic_cast to MdtSegmentQuality in order to obtain quality
    const Muon::MuonSegmentQuality* q = dynamic_cast<const Muon::MuonSegmentQuality*>(seg->fitQuality());

    if( !q ) {
        ATH_MSG_DEBUG("dynamic_cast MuonSegmentQuality failed");
        return 0;
    }

    ATH_MSG_DEBUG("Got MuonSegmentQuality "
            << " hots " << q->numberDoF()+2 << " number of holes + out of time hits " << q->channelsWithoutHit().size() );

    //  unsigned int hots = q->hots();
    //unsigned int crossedtubes = q->crossedTubesMl1()+q->crossedTubesMl2() ;
    //unsigned int deltas = q->deltas();

        unsigned int missed = q->channelsWithoutHit().size();
    //  if(missed < 0) missed = 0;
        return missed;
}

Muon::MCSCSegmentInfo 
Muon::MuonCurvedSegmentCombiner::segInfo( Muon::MuonSegment* seg ){
    
  Muon::MCSCSegmentInfo info;

  // MDT information 
  info.index = -1;

  info.nMult1 = 0;
  info.nMult2 = 0;
  info.nLayTrig = 0;
  info.nMissedHits = 0;
  info.layMissed = 0.;
  // Phi measurement
  info.phiPatHits = 0;
  info.patHits = 0;

  // Hits on segment (CSC or MDT)
  int nHots = 0;
  // station code and string 
  int code = 0;
  std::string name;
  bool ismdt = false;

  info.chi2 = 0.;
  if( seg->fitQuality()) {
   info.chi2 = (seg->fitQuality())->chiSquared();
  }
  
  const MdtDriftCircleOnTrack* mdtShortest = 0;
  double shortestTube = 1e9;
  Identifier id;
  std::vector<const Trk::MeasurementBase*> mbs = seg->containedMeasurements();
  std::vector<const Trk::MeasurementBase*>::const_iterator mit = mbs.begin();
  std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = mbs.end();
  for (;mit!=mit_end;++mit) {
      Identifier idr ;      
      const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*> (*mit);
      if (!rot){
       const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*> (*mit);
       if (crot) rot  = &crot->rioOnTrack(0);
      }	
      if (rot)  idr = rot->identify();
      if (!rot) continue; 
      if(m_cscIdHelper->is_csc(idr)){
        id = idr; 
      }
      if(m_mdtIdHelper->is_mdt(idr)){
	
	const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(*mit);
	if( mdt ){
	  int lay = m_mdtIdHelper->tubeLayer(mdt->identify());
	  int tube = m_mdtIdHelper->tube(mdt->identify());
	  double tubelen = mdt->prepRawData()->detectorElement()->getActiveTubeLength(lay,tube);
	  if( tubelen < shortestTube ){
	    mdtShortest = mdt;
	    shortestTube = tubelen;
	  }
	}
	ismdt = true;
        id = idr; 
        break;
      }
  }
 

  if (ismdt) {
    
    bool closeToChamberEdge = false;
    double posAlongTube = 0;
    if( mdtShortest ){
      posAlongTube = (mdtShortest->associatedSurface().transform().inverse()*seg->globalPosition()).z();
      if( 0.5*shortestTube - fabs(posAlongTube) < 100. ) closeToChamberEdge = true;
    }else{
      ATH_MSG_WARNING(" shorest tube not set ");
    }
    
    if (m_debug) std::cout << " new seg in " << m_mdtIdHelper->print_to_string(id) << std::endl;
    // MDT or CSC segment
    info.nCsc = 0;
    info.nMissedHits = closeToChamberEdge ? 0 : missedHits(seg);
     
    int stName = m_mdtIdHelper->stationName( id );
    name = m_mdtIdHelper->stationNameString( stName );
    int stEta = m_mdtIdHelper->stationEta( id );
    int stPhi = m_mdtIdHelper->stationPhi( id );
    code = stName*1000+100*stPhi+stEta;

    int nMult1 = 0;
    int nMult2 = 0;
    std::map < int, int > triggerLayers;
    mit = mbs.begin();
    for (;mit!=mit_end;++mit) {
      Identifier idr ;      
      const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*> (*mit);
      if (!rot){
       const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*> (*mit);
       if (crot) rot  = &crot->rioOnTrack(0);
      }	
      if (rot)  idr = rot->identify();
      if (!rot) continue; 

      if(m_mdtIdHelper->is_mdt(idr)){
	if( m_mdtIdHelper->multilayer(idr) == 1 ) nMult1++;
	if( m_mdtIdHelper->multilayer(idr) == 2 ) nMult2++;   
	nHots++;
      }
      int layerCode = 0;
      if(m_rpcIdHelper->is_rpc(idr)){
        layerCode = 1000000*(m_rpcIdHelper->stationName(idr))+10000*(m_rpcIdHelper->stationPhi(idr))+ 100* ((m_rpcIdHelper->stationEta(idr))+10);
        layerCode = layerCode + 2*((m_rpcIdHelper->doubletR(idr))-1)+16*((m_rpcIdHelper->gasGap(idr))-1);
        if (m_rpcIdHelper->measuresPhi(idr)) layerCode += 100;
        triggerLayers[layerCode] = 1;
        if (m_debug) std::cout << " RPC hit phi" << m_rpcIdHelper->measuresPhi(idr) << " R " << m_rpcIdHelper->doubletR(idr) << " gas " << m_rpcIdHelper->gasGap(idr) << std::endl;
      }
      if (m_tgcIdHelper->is_tgc( idr )) {
        layerCode = 1000000*(m_tgcIdHelper->stationName(idr))+10000*(m_tgcIdHelper->stationPhi(idr))+ 100* ((m_tgcIdHelper->stationEta(idr))+10);
        layerCode = layerCode + m_tgcIdHelper->gasGap(idr);
        if (m_tgcIdHelper->isStrip(idr)) layerCode += 100;
        triggerLayers[layerCode] = 1;
      }
    } 
    info.nLayTrig = triggerLayers.size();
//
    std::string st = name;
    int nLayTrig =  triggerLayers.size();
    int nLayMax = 0;  
    if (st[0] == 'B') {
     if (st[1] == 'M') nLayMax = 4;
     if (st[1] == 'O') nLayMax = 2;
    } else if (st[0] == 'E' || st[0] == 'F') {
       if (st[1] == 'I') nLayMax = 2;
       if (st[1] == 'M') nLayMax = 7;
    }
//
// Model quality:  maximal 3 missed hits = no TGCs in Middle (7)
//                         1 missed hit  = no TGCs Inner     (2)
//                         2 missed hits = no RPC Middle     (4)
//                         1 missed hit  = no RPC Outer      (2) 
        
    double trigFrac = 0.;  
    if (nLayTrig > nLayMax && nLayMax > 0 ) {
     nLayTrig = nLayMax;
    }
    if ( nLayMax > 0 ) {
     trigFrac = (nLayMax - nLayTrig)/2.;
    }
    if (m_debug) std::cout << " nLayTrig " << nLayTrig << " nLayMax " << nLayMax << " trigFrac " << trigFrac << std::endl;
    if (trigFrac > 3.) trigFrac = 3.;
//  
//  Station witn NO RPCs or partial coverage
//
    if (st[0] == 'B' && st[1] =='E' && st[2] =='E') trigFrac = 0.;
    if (st[0] == 'B' && st[1] =='O' && st[2] =='G') trigFrac = 0.;
    if (st[0] == 'B' && st[1] =='O' && st[2] =='L') trigFrac =  trigFrac/2.;
    if (st[0] == 'B' && st[1] =='M' && st[2] =='L') trigFrac =  trigFrac/2.;
    if (st[0] == 'B' && st[1] =='M' && st[2] =='F') trigFrac =  trigFrac/2.;
//
//  Endcap stations no TGC coverage for station eta = +-1
//  reduced TGC coverage in Barrel-Endcap around station eta 4,5
//
    if (st[0] == 'E') {
      if(abs(stEta) == 1 ) trigFrac = 0.;
      if(abs(stEta) == 5 && st[1] =='M' && st[2] =='S' ) trigFrac = trigFrac/4.;
      if(abs(stEta) == 5 && st[1] =='I' && st[2] =='L' ) trigFrac = trigFrac/4.;
      if(abs(stEta) == 4 && st[1] =='I' && st[2] =='L' ) trigFrac = trigFrac/4.;
    } 
    info.layMissed = trigFrac;
    
    info.nMult1 = nMult1;
    info.nMult2 = nMult2;

  } else if (m_cscIdHelper->is_csc(id)) {
    if (m_debug) std::cout << " new seg in " << m_cscIdHelper->print_to_string(id) << std::endl;
    info.nCsc = 1;
    int stName = m_cscIdHelper->stationName( id );
    name = m_cscIdHelper->stationNameString( stName );
    int steta = m_cscIdHelper->stationEta( id );
    code = stName*1000+steta;
    mit = mbs.begin();
    for (;mit!=mit_end;++mit) {
      Identifier idr ;      
      const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*> (*mit);
      if (!rot){
       const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*> (*mit);
       if (crot) rot  = &crot->rioOnTrack(0);
      }	
      if (rot)  idr = rot->identify();
      if (!rot) continue; 
      if(m_cscIdHelper->is_csc(idr)) {
	nHots++;
      }
    }
  }

// Start values for pattern information

    int nphi = 0;
    int nall = 0;
    double patternPhi = -1000.;
    double patternTheta = -1000.;
    double patternMomentum = -1.; 
    info.patPoint = 0;

// Navigate to FIRST pattern stored in m_segAssoMap
    const Muon::MuonPatternCombination* patP = m_segAssoMap[seg];
    if (patP != 0 ) {
// Set pattern pointer
         info.patPoint = patP;
         const Trk::TrackParameters* trkparameters = patP->trackParameter();
         if (trkparameters) {
             const Trk::Perigee* perigee = 0;
             perigee = dynamic_cast<const Trk::Perigee*>(trkparameters);
             if (perigee) {
// Get momentum and phi value  
               patternMomentum = perigee->momentum().mag();
               patternPhi = perigee->momentum().phi();
               patternTheta = perigee->momentum().theta();
//               if (m_debug) std::cout << " Associated Pattern Momentum " << perigee->momentum().mag() << std::endl;
             }
         }
// Count phi hits
         std::vector< Muon::MuonPatternChamberIntersect >::const_iterator it = patP->chamberData().begin();
         for (; it!=patP->chamberData().end(); ++it )  {
           std::vector< const Trk::PrepRawData* >::const_iterator pit = (*it).prepRawDataVec().begin();
           for (; pit!=(*it).prepRawDataVec().end(); ++pit) {
             const Trk::PrepRawData* hit = *pit ;
             nall++; 
//             if (m_debug) std::cout << " Prep data on pattern " << std::endl; 
             Identifier id = hit->identify() ;
             if ( m_rpcIdHelper -> is_rpc(id) ) {
               if( m_rpcIdHelper->measuresPhi(id) ) nphi++;
             } else if( m_tgcIdHelper -> is_tgc(id) ) {
               if( m_tgcIdHelper -> isStrip(id)  ) nphi++;
             } else if( m_cscIdHelper -> is_csc(id) ) {
               if( m_cscIdHelper->measuresPhi(id) ) nphi++;
             }
           }
         }

    }
//  if (m_debug) std::cout << " Seg Info Phi hits  on pattern " << nphi << std::endl; 
  info.phiPatHits = 0;
  info.patHits = nall;
  if(nphi>0) info.phiPatHits = nphi;
  info.patPhi = patternPhi;
  info.patTheta = patternTheta;
  info.patMomentum = patternMomentum;

  info.nHots = nHots;
  info.stationCode = code;
  info.station = name;

  if (m_debug) std::cout << " Station name " << name << " Station code " << code << std::endl;
 
  double theta;
  double invcurvature;
  int imeth; 
  trackParameters(*seg,theta,invcurvature,imeth);
  info.invcurvature = invcurvature;

  return info;
}

void 
Muon::MuonCurvedSegmentCombiner::trackParameters( Muon::MuonSegment& seg ,double &theta,double &invcurvature,int &imeth ){

    // Determine track parameters in Rz plane (theta and inverse curvature) from the input segment

    Amg::MatrixX Model(2,2);// H projection matrix PDG 
    Model.setZero();
    Amg::MatrixX CovT(2,2); // Covariance or Track parameters here 2
    CovT.setZero();
    Amg::MatrixX CovTI(2,2); // Covariance Inverse of Track parameters
    CovTI.setZero();
    Amg::MatrixX V(2,1);// V = H^T Cov^-1 r   Cov-1 = Covariance of measurements  
    V.setZero();
    Amg::MatrixX T(2,1);// Track parameters   
    T.setZero();
    Amg::MatrixX ym(2,1); // Vector of measurememns
    ym.setZero();
    Amg::MatrixX Res(2,1); // Vector of residuals 
    Res.setZero();
    double radius_cylinder = 4000.;
    double z_cylinder = 6000.;
    double z_end = 15000.;

    double thetas = seg.globalDirection().theta();
    double rs = seg.globalPosition().perp();
    double zs = seg.globalPosition().z();
    double ers2 = 0.1*0.1; //error squared position 
    double ets2 = 0.0001*0.0001; // error squared angle 
    double sign = 1;
    if ( zs < 0 ) sign = -1.;

    const Muon::MCSCSegmentInfo& info = m_segInfoMap[&seg];
    std::string st = info.station;
    bool barrel = false;
    if (st[0] == 'B') barrel = true;


    if (barrel) {
        imeth = 0;
    // Barrel Track Model Matrix
        Model(0,0) = rs;
        Model(0,1) = (rs-radius_cylinder)*(rs-radius_cylinder);
        Model(1,0) = 1;
        Model(1,1) = 2*(rs-radius_cylinder);
    // Measurements ym
    // correspondig squared errors: zs -> ers2  cot(thetas) ->ets2
        ym(0,0) = zs; 
        ym(1,0)= cos(thetas)/sin(thetas);
    } else {
        imeth = 1;
    // Forward Track Model Matrix
        Model(0,0) = zs;
        Model(0,1) = sign*(zs-sign*z_cylinder)*(zs-sign*z_cylinder);
        Model(1,0) = 1;
        Model(1,1) = 2*sign*(zs-sign*z_cylinder);

        if (fabs(zs) > z_end) {
            Model(0,1) = -z_end*z_end + z_cylinder*z_cylinder + 2*zs*sign*(z_end-z_cylinder);
            Model(1,1) = 2*(sign*z_end-sign*z_cylinder);
        }  
    // Measurements ym
    // correspondig squared errors: rs -> ers2  tan(thetas) ->ets2
        ym(0,0) = rs; 
        ym(1,0)= tan(thetas);
        if (m_debug) std::cout << " Forward zs " << zs << std::endl; 
    }

    for(int i = 0; i <2 ; ++i )  {
        V(i,0)= Model(0,i)*ym(0,0)/ers2 + Model(1,i)*ym(1,0)/ets2;
    }    

    for(int i = 0; i <2 ; ++i )  {
        for(int j = 0; j <2 ; ++j )  {
	  CovTI(i,j) += Model(0,i)*Model(0,j)/ers2;
	  CovTI(i,j) += Model(1,i)*Model(1,j)/ets2;
        }
    }
    // Solution for Track parameters
    CovT = CovTI.inverse(); 
    T = CovT*V;

    // calculate residuals and chi2
    for(int i = 0; i <2 ; ++i )  {
        Res(i,0)= Model(i,0)*T(0,0) + Model(i,1)*T(1,0) - ym(i,0);
    }    
    //   double chi2 = Res(0,0)*Res(0,0)/ers2 + Res(1,0)*Res(1,0)/ets2;
    //   std::cout << " residuals matrix method  z " << Res(0,0) << " cot(theta) " << Res(1,0) << " chi2 " << chi2 << std::endl;   
    //   std:: cout<< " Track parameters Matrix T00 " << T(0,0) << " T10 " << T(1,0) << std::endl;     

    Amg::MatrixX ModelP(2,2);// Matrix for predicition H 
    Model.setZero();
    Amg::MatrixX ModelPT(2,2);// Transposed  
    ModelPT.setZero();
    for(int i = 0; i <2 ; ++i )  {
        for(int j = 0; j <2 ; ++j )  {
            ModelP(i,j)=Model(i,j);
            ModelPT(i,j)=Model(j,i);
        }
    }
    Amg::MatrixX CovP(2,2);// Covariance of predicition 
    CovP.setZero();
    CovP = ModelP*CovT*ModelPT;

    if ( m_debug ) std::cout << " covariance at measurement 00 " << CovP(0,0) << " 01 " << CovP(0,1) << " 10 " << CovP(1,0) << " 11 " << CovP(1,1) << std::endl; 
    if ( m_debug ) std::cout << " covariance at track 00 " << CovT(0,0) << " 01 " << CovT(0,1) << " 10 " << CovT(1,0) << " 11 " << CovT(1,1) << std::endl; 



    if (imeth == 0) {
        theta = atan2(1.,T(0,0));
        invcurvature = T(1,0)*sin(theta);
    } else if (imeth == 1) {
        theta = atan2(T(0,0),1.);
        if (T(0,0) < 0) theta = atan2 ( -T(0,0),-1.);
        invcurvature = T(1,0)*cos(theta);
    }  

    //    double dis = signedDistance(0.,theta,invcurvature,0.,rs,zs);
    //    StandardAlgs standardAlgs;
    //    double diss = standardAlgs.signedDistanceCurvedToHit(0.,theta,invcurvature,0.,rs,zs);
    if(m_debug) std:: cout<< " Track parameters theta " << theta << " invcurvature " << invcurvature << " Momentum " << 0.1/invcurvature << std::endl;     


}

void 
Muon::MuonCurvedSegmentCombiner::fit2SegmentsSL( Muon::MuonSegment& seg , Muon::MuonSegment& sege , Amg::VectorX & Res, Amg::VectorX & Pull) {


    // Fit 2 segments to a straightline (no Interaction Point constraint)

    // Tracking matrices 
    Amg::MatrixX Model(4,2);// H projection matrix PDG 
    Model.setZero();
    Amg::MatrixX CovT(2,2); // Covariance or Track parameters here 3: z beam; r/z segment; tan/cot theta segment;
    CovT.setZero();
    Amg::MatrixX V(2,1);// V = H^T Cov^-1 r   Cov-1 = Covariance of measurements  
    V.setZero();
    Amg::MatrixX T(2,1);// Track parameters   
    T.setZero();
    Amg::MatrixX ym(4,1); // Vector of measurememns
    ym.setZero();
    // Extrapolation Matrices 
    Amg::MatrixX Resi(4,1); // Residuals
    Resi.setZero();
    // Geometry conventions

    //int imeth;

    // First Segment  
    const Muon::MCSCSegmentInfo& info1 = m_segInfoMap[&seg];
    double thetas = seg.globalDirection().theta();
    double rs = seg.globalPosition().perp();
    double zs = seg.globalPosition().z();
    // angular resolution

    double era1 = 0.0002;
    if( info1.nMult1 == 0 ||  info1.nMult2 == 0 ) era1 = 0.002;
    if (info1.nCsc > 0) era1 = 0.002;
    // Second Segment
    const Muon::MCSCSegmentInfo& info2 = m_segInfoMap[&sege];
    double thetase = sege.globalDirection().theta();
    if (m_debug) std::cout << " thetase " << thetase <<std::endl;
    double rse = sege.globalPosition().perp();
    double zse = sege.globalPosition().z();

    if (m_doCosmics) { // translate line to IP for cosmics
      double dist = sqrt((rse-rs)*(rse-rs)+(zse-zs)*(zse-zs));
      int is = 1;
      if(sege.globalPosition().y() > seg.globalPosition().y()) is = -1; 
      double phiSegments = atan2(is*sege.globalPosition().y()-is*seg.globalPosition().y(),is*sege.globalPosition().x()-is*seg.globalPosition().x()); 
      double dphi = phiSegments - seg.globalDirection().phi();   
      double dphie = phiSegments - sege.globalDirection().phi();   
      if(m_debug) std::cout << " phiSegments " << phiSegments << " segment phi " << sege.globalDirection().phi() << " dist " << dist << " dphi " << dphi << " dphie " << dphie << std::endl; 

      rse = sege.globalPosition().x() * std::cos(sege.globalDirection().phi()) + sege.globalPosition().y() * std::sin(sege.globalDirection().phi());
      rs = seg.globalPosition().x() * std::cos(seg.globalDirection().phi()) + seg.globalPosition().y() * std::sin(seg.globalDirection().phi());
      if(fabs(dphi)>0.5||fabs(dphie)>0.5) {
// Phi direction unreliable
        rse = sege.globalPosition().x() * std::cos(phiSegments) + sege.globalPosition().y() * std::sin(phiSegments);
        rs = seg.globalPosition().x() * std::cos(phiSegments) + seg.globalPosition().y() * std::sin(phiSegments);
        if(m_debug) std::cout << " Phi direction Unreliable " << std::endl; 
      } 
    }

    double era2 = 0.0002;
    if( info2.nMult1 == 0 ||  info2.nMult2 == 0 ) era2 = 0.002;
    if (info2.nCsc > 0) era2 = 0.002;
    // Get momenta 
    double ic1 = info1.invcurvature;
    double ic2 = info2.invcurvature;
    if (m_doCosmics) ic1 = 1./15000.;
    if (m_doCosmics) ic2 = 1./15000.;
    double scf = 20*fabs(ic1)*50000.;
    if (fabs(ic2) > fabs(ic1) ) scf = 20*fabs(ic2)*50000.;
    if (scf < 1. ) scf = 1.; 
    if (scf > 100. ) scf = 100.;
    if (m_debug) std::cout << " error scaling in SL fit " << scf << std::endl; 

    std::string st1 = info1.station;
    std::string st2 = info2.station;
    bool barrel = false;
    if (st1[0] == 'B' || st2[0] == 'B' ) barrel = true;

// additional error scaling 
    double scfn = 0.;
    double phiHitScale = 1.;
    if ( info1.phiPatHits == 0 || info2.phiPatHits == 0 ) phiHitScale = 2.; 
    // Barrel - Endcap 
    if ( (st1[0] == 'E' && st2[0] == 'B' ) || (st2[0] == 'E' && st1[0] == 'B' ) )  scfn = phiHitScale*25.;
    // Small - Large
    if ( st1[2] != st2[2] ) scfn = phiHitScale*25.;
    // Extended 
    if ( st1[1] == 'E' || st2[1] == 'E' ) scfn = phiHitScale*25.;

    // Error definitions
    //  double scf = 1; // pt = 100 GeV scf = 1;  pt 5 GeV scf = 20*20
    double ers2 = 0.1*0.1+scf*scf+scfn*scfn; //error squared position 
    double ets21 = era1*era1 + 0.002*0.002*(scf*scf+scfn*scfn); // error squared angle 
    double ets22 = era2*era2 + 0.002*0.002*(scf*scf+scfn+scfn); // error squared angle 
    // double sign = 1;
    // if ( zs < 0 ) sign = -1.;

    if (m_debug) std::cout << " Fit2SegmentsSL  Nofield " << std::endl;

    if (barrel) {
      //imeth = 0;
    // Barrel Track Model Matrix
        Model(0,0) = 1.;
        Model(0,1) = rs;
        Model(1,1) = 1.;
        Model(2,0) = 1.;
        Model(2,1) = rse;
        Model(3,1) = 1.;
    // Measurements ym
    // correspondig squared errors: zs -> ers2  cot(thetas) ->ets2
        ym(0,0) = zs; 
        ym(1,0)= cos(thetas)/sin(thetas);
        ym(2,0) = zse; 
        ym(3,0)= cos(thetase)/sin(thetase);
    } else {
      //imeth = 1;
        if (m_debug) std::cout << " forward fit " << cos(thetas) << std::endl;
    // Forward Track Model Matrix
        Model(0,0) = 1.;
        Model(0,1) = zs;
        Model(1,1) = 1.;
        Model(2,0) = 1.;
        Model(2,1) = zse;
        Model(3,1) = 1.;
    // Measurements ym
    // correspondig squared errors: rs -> ers2  tan(thetas) ->ets2
        ym(0,0) = rs; 
        ym(1,0)= tan(thetas);
        ym(2,0) = rse; 
        ym(3,0)= tan(thetase);
    //   std::cout << " Forward zs " << zs << std::endl; 
    }
    if (m_debug) std::cout << " distance segments " << sqrt((zs-zse)*(zs-zse)+(rs-rse)*(rs-rse))<< std::endl;
    for(int i = 0; i <2 ; ++i )  {
        V(i,0)= Model(0,i)*ym(0,0)/ers2 + Model(1,i)*ym(1,0)/ets21
            + Model(2,i)*ym(2,0)/ers2 + Model(3,i)*ym(3,0)/ets22;
    }    

    Amg::MatrixX CovTI(2,2); // Covariance Inverse of Track parameters
    CovTI.setZero();
    for(int i = 0; i <2 ; ++i )  {
        for(int j = 0; j <2 ; ++j )  {
            CovTI(i,j) += Model(0,i)*Model(0,j)/ers2;
            CovTI(i,j) += Model(1,i)*Model(1,j)/ets21;
            CovTI(i,j) += Model(2,i)*Model(2,j)/ers2;
            CovTI(i,j) += Model(3,i)*Model(3,j)/ets22;
        }
    }
    // Solution for Track parameters
    CovT = CovTI.inverse(); 
    //   std::cout << " ierr " << ierr << std::endl;
    T = CovT*V;
    //   std::cout << " covariance at track 00 " << CovT(0,0) << " 01 " << CovT(0,1) << " 11 " << CovT(1,1) << std::endl; 
    //   std:: cout<< " Track parameters Matrix T00 " << T(0,0) << " T10 " << T(1,0) << std::endl;     

    //double theta=0.; 
    // if (imeth == 0) {
    //     theta = atan2(1.,T(1,0));
    // } else if (imeth == 1) {
    //     theta = atan(T(1,0));
    // }  

    // calculate residuals and chi2
    for(int i = 0; i <4 ; ++i )  {
        Resi(i,0)= Model(i,0)*T(0,0) + Model(i,1)*T(1,0) - ym(i,0);
    }    
    double chi2 = Resi(0,0)*Resi(0,0)/ers2 + Resi(1,0)*Resi(1,0)/ets21
        +  Resi(2,0)*Resi(2,0)/ers2 + Resi(3,0)*Resi(3,0)/ets22;

    if (m_debug) std::cout << " resi 00 " << Resi(0,0) << " chi2 " << chi2 << std::endl;   



    Res = Amg::VectorX(4); 
    Res.setZero();
    for(int i = 0; i <4 ; ++i )  {
        Res[i]= Resi(i,0);
    }    

    Pull = Amg::VectorX(4); 
    Pull.setZero();

    // do scaling 
    bool toobig = false; 
    for(int i = 0; i <4 ; ++i )  {
        if ( i==0 ) Pull[i] = Res[i]/sqrt(ers2);
        if ( i==1 ) Pull[i] = Res[i]/sqrt(ets21);
        if ( i==2 ) Pull[i] = Res[i]/sqrt(ers2);
        if ( i==3 ) Pull[i] = Res[i]/sqrt(ets22);
        if ( fabs( Pull[i] ) > 5 ) toobig = true;
    }
    if (toobig && m_debug) {
        std::cout << " SL Pull too BIG " << " rad pos1 " << rs << " rad pos2 " << rse << " ang1 " << thetas << " ang2 " << thetase << std::endl;

        std::string st1 = info1.station;
        std::string st2 = info2.station;

        const Amg::Vector3D& d1  = seg.globalDirection();
        const Amg::Vector3D& d2  = sege.globalDirection();
        std::cout << st1 << " " << st2 << " diff phi " << d1.x()*d2.y() - d1.y()*d2.x() << std::endl ;
    }

    if (m_debug) std::cout << " Fit2SegmentsSL: Residual 0 " << Res[0] << " pull 0 " << Pull[0] << " Residual 1 " << Res[1] << " pull 1 " << Pull[1] << std::endl;   
    if (m_debug) std::cout << " Fit2SegmentsSL: Residual 2 " << Res[2] << " pull 2 " << Pull[2] << " Residual 3 " << Res[3] << " pull 3 " << Pull[3] << std::endl;   

}
        
void 
Muon::MuonCurvedSegmentCombiner::fit2Segments( Muon::MuonSegment& seg , Muon::MuonSegment& sege , Amg::VectorX & Res, Amg::VectorX & Pull) {


    // Fit 2 segments to one track and determine residuals and pulls 

    // Tracking matrices 
    Amg::MatrixX Model(5,3);// H projection matrix PDG 
    Model.setZero();
    Amg::MatrixX CovT(3,3); // Covariance or Track parameters here 3: z beam; r/z segment; tan/cot theta segment;
    CovT.setZero();
    Amg::MatrixX V(3,1);// V = H^T Cov^-1 r   Cov-1 = Covariance of measurements  
    V.setZero();
    Amg::MatrixX T(3,1);// Track parameters   
    T.setZero();
    Amg::MatrixX ym(5,1); // Vector of measurememns
    ym.setZero();
    // Extrapolation Matrices 
    Amg::MatrixX Resi(5,1); // Residuals
    Resi.setZero();

    // Geometry conventions 
    double radius_cylinder = 4000.;
    double z_cylinder = 6000.;
    double z_end = 15000.; 
    double cos_barrel = std::cos(std::atan2(11430.,14000.));  
    int imeth;

    // First Segment  
    const Muon::MCSCSegmentInfo& info1 = m_segInfoMap[&seg];
    double thetas = seg.globalDirection().theta();
    double rs = seg.globalPosition().perp();
    double zs = seg.globalPosition().z();
    //  double phi  = seg.globalDirection().phi(); 
    //  double phisp  = atan2( seg.globalPosition().y(), seg.globalPosition().x());
    double thetasp  = atan2(rs,zs);
    // angular resolution
    double era1 = 0.0002;
    if( info1.nMult1 == 0 ||  info1.nMult2 == 0 ) era1 = 0.002;
    if (info1.nCsc > 0) era1 = 0.002;
    // Second Segment
    const Muon::MCSCSegmentInfo& info2 = m_segInfoMap[&sege];
    double thetase = sege.globalDirection().theta();
    double rse = sege.globalPosition().perp();
    double zse = sege.globalPosition().z();
    //  double phie  = sege.globalDirection().phi(); 
    //  double phispe  = atan2( sege.globalPosition().y(), sege.globalPosition().x());
    double thetaspe  = atan2(rse,zse);


    double era2 = 0.0002;
    if( info2.nMult1 == 0 ||  info2.nMult2 == 0 ) era2 = 0.002;
    if (info2.nCsc > 0) era2 = 0.002;
    // Get momenta 
    double ic1 = info1.invcurvature;
    double ic2 = info2.invcurvature;
    double scf = 20*fabs(ic1)*50000.;
    if (fabs(ic2) > fabs(ic1) ) scf = 20*fabs(ic2)*50000.;
    if (scf < 1. ) scf = 1.; 
    if (scf > 100. ) scf = 100.;
    if (m_debug) std::cout << " error scaling in Curved fit " << scf << std::endl; 
    if (std::abs(std::cos(thetasp))>cos_barrel || std::abs(std::cos(thetaspe))>cos_barrel ) scf = 2*scf;

    std::string st1 = info1.station;
    std::string st2 = info2.station;
    bool barrel = false;
    if (st1[0] == 'B' || st2[0] == 'B' ) barrel = true;

// additional error scaling 
    double scfn = 0.;
    double phiHitScale = 1.;
    if ( info1.phiPatHits == 0 || info2.phiPatHits == 0 ) phiHitScale = 2.; 
    // Barrel - Endcap 
    if ( (st1[0] == 'E' && st2[0] == 'B' ) || (st2[0] == 'E' && st1[0] == 'B' ) )  scfn = phiHitScale*25.;
    // Small - Large
    if ( st1[2] != st2[2] ) scfn = phiHitScale*25.;
    // Extended 
    if ( st1[1] == 'E' || st2[1] == 'E' ) scfn = phiHitScale*25.;

    // Error definitions
    //  double scf = 1; // pt = 100 GeV scf = 1;  pt 5 GeV scf = 20*20
    double ers2 = 0.1*0.1+scf*scf+scfn*scfn; //error squared position 
    double ebs2 = 50*50*fabs(sin(thetas)*sin(thetase))+ers2; //error squared beam position 
    double ets21 = era1*era1 + 0.002*0.002*(scf*scf+scfn*scfn); // error squared angle 
    double ets22 = era2*era2 + 0.002*0.002*(scf*scf+scfn+scfn); // error squared angle 

    double sign = 1;
    if ( zs < 0 ) sign = -1.;

    if (barrel) {
        imeth = 0;
    // Barrel Track Model Matrix
        Model(0,0) = 1.;
        Model(1,0) = 1.;
        Model(1,1) = rs;
        Model(1,2) = (rs-radius_cylinder)*(rs-radius_cylinder);
        Model(2,1) = 1.;
        Model(2,2) = 2*(rs-radius_cylinder);
        Model(3,0) = 1.;
        Model(3,1) = rse;
        Model(3,2) = (rse-radius_cylinder)*(rse-radius_cylinder);
        Model(4,1) = 1.;
        Model(4,2) = 2*(rse-radius_cylinder);
    // Measurements ym
    // correspondig squared errors: zs -> ers2  cot(thetas) ->ets2
        ym(0,0) = 0.; 
        ym(1,0) = zs; 
        ym(2,0)= cos(thetas)/sin(thetas);
        ym(3,0) = zse; 
        ym(4,0)= cos(thetase)/sin(thetase);
    } else {
        imeth = 1;
        if (m_debug) std::cout << " forward fit " << cos(thetas) << std::endl;
    // Forward Track Model Matrix
        Model(0,0) = 1.;
        Model(1,0) = 1.;
        Model(1,1) = zs;
        Model(1,2) = sign*(zs-sign*z_cylinder)*(zs-sign*z_cylinder);
        Model(2,1) = 1;
        Model(2,2) = sign*2*(zs-sign*z_cylinder);
        Model(3,0) = 1.;
        Model(3,1) = zse;
        Model(3,2) = sign*(zse-sign*z_cylinder)*(zse-sign*z_cylinder);
        Model(4,1) = 1;
        Model(4,2) = sign*2*(zse-sign*z_cylinder);

        if (fabs(zs) > z_end+2000) {
            Model(1,2) = sign*(-z_end*z_end + z_cylinder*z_cylinder + 2*zs*sign*(z_end-z_cylinder)
                +      (zs-sign*z_end)*(zs-sign*z_end)/5.);
            Model(2,2) = sign*(2*(sign*z_end-sign*z_cylinder)
                +     (zs-sign*z_end)/5.);
        }
        if (fabs(zse) > z_end+2000) {
            Model(3,2) = sign*(-z_end*z_end + z_cylinder*z_cylinder + 2*zse*sign*(z_end-z_cylinder) 
                +      (zse-sign*z_end)*(zse-sign*z_end)/5.);
            Model(4,2) = sign*(2*(sign*z_end-sign*z_cylinder)
                +     (zse-sign*z_end)/5.);
        }  
    // Measurements ym
    // correspondig squared errors: rs -> ers2  tan(thetas) ->ets2
        ym(0,0) = 0.; 
        ym(1,0) = rs; 
        ym(2,0)= tan(thetas);
        ym(3,0) = rse; 
        ym(4,0)= tan(thetase);

    //   std::cout << " Forward zs " << zs << std::endl; 
    }
    if (m_debug) std::cout << " distance segments " << sqrt((zs-zse)*(zs-zse)+(rs-rse)*(rs-rse))<< std::endl;

    for(int i = 0; i <3 ; ++i )  {
        V(i,0)= Model(0,i)*ym(0,0)/ebs2 + Model(1,i)*ym(1,0)/ers2 + Model(2,i)*ym(2,0)/ets21
            + Model(3,i)*ym(3,0)/ers2 + Model(4,i)*ym(4,0)/ets22;
    }    

    Amg::MatrixX CovTI(3,3); // Covariance Inverse of Track parameters
    CovTI.setZero();
    for(int i = 0; i <3 ; ++i )  {
        for(int j = 0; j <3 ; ++j )  {
            CovTI(i,j) += Model(0,i)*Model(0,j)/ebs2;
            CovTI(i,j) += Model(1,i)*Model(1,j)/ers2;
            CovTI(i,j) += Model(2,i)*Model(2,j)/ets21;
            CovTI(i,j) += Model(3,i)*Model(3,j)/ers2;
            CovTI(i,j) += Model(4,i)*Model(4,j)/ets22;
        }
    }
    // Solution for Track parameters
    CovT = CovTI.inverse(); 
    T = CovT*V;
    //   std::cout << " covariance at track 00 " << CovT(0,0) << " 11 " << CovT(1,1) << " 12 " << CovT(1,2) << " 22 " << CovT(2,2) << std::endl; 

    double theta=0.; double invcurvature=0.;
    if (imeth == 0) {
        theta = atan2(1.,T(1,0));
        invcurvature = T(2,0)*sin(theta);
    } else if (imeth == 1) {
        theta = atan2(1.,1./T(1,0));
        invcurvature = -T(2,0)*fabs(cos(theta));
    }  

    if (m_debug) std::cout << " Momentum in MeV " << (1./invcurvature)/10. << " theta fit " << theta << std::endl;  

    // calculate residuals and chi2
    for(int i = 0; i <5 ; ++i )  {
        Resi(i,0)= Model(i,0)*T(0,0) + Model(i,1)*T(1,0) + Model(i,2)*T(2,0) - ym(i,0);
    }    
    double chi2 = Resi(0,0)*Resi(0,0)/ebs2 + Resi(1,0)*Resi(1,0)/ers2 + Resi(2,0)*Resi(2,0)/ets21
        + Resi(3,0)*Resi(3,0)/ers2 + Resi(4,0)*Resi(4,0)/ets22;
    if (m_debug) std::cout << " resi 00 " << Resi(0,0) << " chi2 " << chi2 << std::endl;   
    //   std:: cout<< " Track parameters Matrix T00 " << T(0,0) << " T10 " << T(1,0) << std::endl;     

    // Reshuffle residuals  Res(0) -> segment position in Model (1,x)
    //                      Res(1) -> segment angle in Model (2,x)

    Res = Amg::VectorX(4); 
    Res.setZero();
    for(int i = 1; i <5 ; ++i )  {
      Res[i-1]= Resi(i,0);
    }    

    Pull = Amg::VectorX(4); 
    Pull.setZero();

    bool toobig = false; 
    for(int i = 0; i <4 ; ++i )  {
        if ( i==0 ) Pull[i] = Res[i]/sqrt(ers2);
        if ( i==1 ) Pull[i] = Res[i]/sqrt(ets21);
        if ( i==2 ) Pull[i] = Res[i]/sqrt(ers2);
        if ( i==3 ) Pull[i] = Res[i]/sqrt(ets22);
        if ( fabs( Pull[i] ) > 5 ) toobig = true;
    }

    if (toobig && m_debug) {
        std::cout << " Pull too BIGFIT " << " rad pos1 " << rs << " rad pos2 " << rse << " ang1 " << thetas << " ang2 " << thetase << std::endl;
        double phisp  = atan2( seg.globalPosition().y(), seg.globalPosition().x());
        double phispe  = atan2( sege.globalPosition().y(), sege.globalPosition().x());
        std::cout << " z pos1 " << zs << " z pos2 " << zse << " phi pos 1 " << phisp << " phi pos 2 " << phispe << std::endl;

        const Amg::Vector3D& d1  = seg.globalDirection();
        const Amg::Vector3D& d2  = sege.globalDirection();
        std::cout << st1 << " " << st2 << " diff phi " << d1.x()*d2.y() - d1.y()*d2.x() << std::endl ;
    }
    if (m_debug) std::cout << " Fit2Segments: Residual 0 " << Res[0] << " pull 0 " << Pull[0] << " Residual 1 " << Res[1] << " pull 1 " << Pull[1] << std::endl;   
    if (m_debug) std::cout << " Fit2Segments: Residual 2 " << Res[2] << " pull 2 " << Pull[2] << " Residual 3 " << Res[3] << " pull 3 " << Pull[3] << std::endl;   
    if (m_debug) std::cout << " radius 1  " << ym(1,0) << " cottan theta 1 " << ym(2,0) << " radius 2  " << ym(3,0) 
        << " cottan theta 2 " << ym(4,0) << std::endl; 
    if (m_debug) std::cout << " radius fit 1  " << -(Res[0]+ym(1,0)) << " cottan theta fit 1 " << -(Res[1]+ym(2,0)) << " radius 2 fit  " << -(Res[2]+ym(3,0)) 
        << " cottan theta 2 fit " << -(Res[3]+ym(4,0)) << std::endl; 
}

void Muon::MuonCurvedSegmentCombiner::fit2SegmentsC( Muon::MuonSegment& seg , Muon::MuonSegment& sege , Amg::VectorX & Res, Amg::VectorX & Pull) {

    // Fit two segments for cosmics with loose IP constraint and free curvature 

    // Tracking matrices 
    Amg::MatrixX Model(5,3);// H projection matrix PDG 
    Model.setZero();
    Amg::MatrixX CovT(3,3); // Covariance or Track parameters here 3: z beam; r/z segment; tan/cot theta segment;
    CovT.setZero();
    Amg::MatrixX V(3,1);// V = H^T Cov^-1 r   Cov-1 = Covariance of measurements  
    V.setZero();
    Amg::MatrixX T(3,1);// Track parameters   
    T.setZero();
    Amg::MatrixX ym(5,1); // Vector of measurememns
    ym.setZero();
    // Extrapolation Matrices 
    Amg::MatrixX Resi(5,1); // Residuals
    Resi.setZero();

    // Geometry conventions 
    double radius_cylinder = 4000.;
    double z_cylinder = 6000.;
    double z_end = 15000.;
    double cos_barrel = std::cos(std::atan2(11430.,14000.));  
    int imeth;

    // First Segment  
    const Muon::MCSCSegmentInfo& info1 = m_segInfoMap[&seg];
    double thetas = seg.globalDirection().theta();
    double rs = seg.globalPosition().perp();
    double zs = seg.globalPosition().z();
    double thetasp  = atan2(rs,zs);
    // angular resolution
    double era1 = 0.0002;
    if( info1.nMult1 == 0 ||  info1.nMult2 == 0 ) era1 = 0.002;
    if (info1.nCsc > 0) era1 = 0.002;
    // Second Segment
    const Muon::MCSCSegmentInfo& info2 = m_segInfoMap[&sege];
    double thetase = sege.globalDirection().theta();
    double rse = sege.globalPosition().perp();
    double zse = sege.globalPosition().z();
    double thetaspe  = atan2(rse,zse);
    double era2 = 0.0002;
    if( info2.nMult1 == 0 ||  info2.nMult2 == 0 ) era2 = 0.002;
    if (info2.nCsc > 0) era2 = 0.002;

    double rsec = rse;
    double rsc = rs;

    if (m_doCosmics) { // translate line to IP for cosmics
      double dist = sqrt((rse-rs)*(rse-rs)+(zse-zs)*(zse-zs));
      int is = 1;
      if(sege.globalPosition().y() > seg.globalPosition().y()) is = -1; 
      double phiSegments = atan2(is*sege.globalPosition().y()-is*seg.globalPosition().y(),is*sege.globalPosition().x()-is*seg.globalPosition().x()); 
      double dphi = phiSegments - seg.globalDirection().phi();   
      double dphie = phiSegments - sege.globalDirection().phi();   
      if(m_debug) std::cout << " phiSegments " << phiSegments << " segment phi " << sege.globalDirection().phi() << " dist " << dist << " dphi " << dphi << " dphie " << dphie << std::endl; 

      rsec = sege.globalPosition().x() * std::cos(sege.globalDirection().phi()) + sege.globalPosition().y() * std::sin(sege.globalDirection().phi());
      rsc = seg.globalPosition().x() * std::cos(seg.globalDirection().phi()) + seg.globalPosition().y() * std::sin(seg.globalDirection().phi());
     if(fabs(dphi)>0.5||fabs(dphie)>0.5) {
// Phi direction unreliable
        rsec = sege.globalPosition().x() * std::cos(phiSegments) + sege.globalPosition().y() * std::sin(phiSegments);
        rsc = seg.globalPosition().x() * std::cos(phiSegments) + seg.globalPosition().y() * std::sin(phiSegments);
        if(m_debug) std::cout << " Phi direction Unreliable " << std::endl; 
     } 

    }

    double invcurvature=1./15000.;
    for (int iter = 0 ; iter <2 ;++iter ){
        double scf = 1;
        if (iter == 0) scf = 20*fabs(invcurvature)*50000;
        if (scf < 1. ) scf = 1.; 
        if (scf > 100. ) scf = 100.;
        if (m_debug) std::cout << " error scaling in Cosmics Curved fit " << scf << std::endl; 
        if (std::abs(std::cos(thetasp))>cos_barrel || std::abs(std::cos(thetaspe))>cos_barrel ) scf = 2*scf;

        std::string st1 = info1.station;
        std::string st2 = info2.station;
        bool barrel = false;
        if (st1[0] == 'B' || st2[0] == 'B' ) barrel = true;

// additional error scaling 
        double scfn = 0.;
        double phiHitScale = 1.;
        if ( info1.phiPatHits == 0 || info2.phiPatHits == 0 ) phiHitScale = 2.; 
    // Barrel - Endcap 
        if ( (st1[0] == 'E' && st2[0] == 'B' ) || (st2[0] == 'E' && st1[0] == 'B' ) )  scfn = phiHitScale*25.;
    // Small - Large
        if ( st1[2] != st2[2] ) scfn = phiHitScale*25.;
    // Extended 
        if ( st1[1] == 'E' || st2[1] == 'E' ) scfn = phiHitScale*25.;

    // Error definitions
    //  double scf = 1; // pt = 100 GeV scf = 1;  pt 5 GeV scf = 20*20
        double ers2 = 0.1*0.1+scf*scf+scfn*scfn; //error squared position 
        double ebs2 = 10000.*10000.; //error squared beam position 
        double ets21 = era1*era1 + 0.002*0.002*(scf*scf+scfn*scfn); // error squared angle 
        double ets22 = era2*era2 + 0.002*0.002*(scf*scf+scfn+scfn); // error squared angle 

        double sign = 1;
        if ( zs < 0 ) sign = -1.;

        if (barrel) {
            imeth = 0;
    // Barrel Track Model Matrix
            Model(0,0) = 1.;
            Model(1,0) = 1.;
            Model(1,1) = rsc;
            Model(1,2) = (rs-radius_cylinder)*(rs-radius_cylinder);
            Model(2,1) = 1.;
            Model(2,2) = 2*(rs-radius_cylinder);
            Model(3,0) = 1.;
            Model(3,1) = rsec;
            Model(3,2) = (rse-radius_cylinder)*(rse-radius_cylinder);
            Model(4,1) = 1.;
            Model(4,2) = 2*(rse-radius_cylinder);
    // Measurements ym
    // correspondig squared errors: zs -> ers2  cot(thetas) ->ets2
            ym(0,0) = 0.; 
            ym(1,0) = zs; 
            ym(2,0)= cos(thetas)/sin(thetas);
            ym(3,0) = zse; 
            ym(4,0)= cos(thetase)/sin(thetase);
        } else {
            imeth = 1;
            if (m_debug) std::cout << " forward fit " << cos(thetas) << std::endl;
    // Forward Track Model Matrix
            Model(0,0) = 1.;
            Model(1,0) = 1.;
            Model(1,1) = zs;
            Model(1,2) = sign*(zs-sign*z_cylinder)*(zs-sign*z_cylinder);
            Model(2,1) = 1;
            Model(2,2) = sign*2*(zs-sign*z_cylinder);
            Model(3,0) = 1.;
            Model(3,1) = zse;
            Model(3,2) = sign*(zse-sign*z_cylinder)*(zse-sign*z_cylinder);
            Model(4,1) = 1;
            Model(4,2) = sign*2*(zse-sign*z_cylinder);

            if (fabs(zs) > z_end+2000) {
                Model(1,2) = sign*(-z_end*z_end + z_cylinder*z_cylinder + 2*zs*sign*(z_end-z_cylinder)
                    +      (zs-sign*z_end)*(zs-sign*z_end)/5.);
                Model(2,2) = sign*(2*(sign*z_end-sign*z_cylinder)
                    +     (zs-sign*z_end)/5.);
            }
            if (fabs(zse) > z_end+2000) {
                Model(3,2) = sign*(-z_end*z_end + z_cylinder*z_cylinder + 2*zse*sign*(z_end-z_cylinder) 
                    +      (zse-sign*z_end)*(zse-sign*z_end)/5.);
                Model(4,2) = sign*(2*(sign*z_end-sign*z_cylinder)
                    +     (zse-sign*z_end)/5.);
            }  
    // Measurements ym
    // correspondig squared errors: rs -> ers2  tan(thetas) ->ets2
            ym(0,0) = 0.; 
            ym(1,0) = rsc; 
            ym(2,0)= tan(thetas);
            ym(3,0) = rsec; 
            ym(4,0)= tan(thetase);
    //   std::cout << " Forward zs " << zs << std::endl; 
        }
        if (m_debug) std::cout << " distance segments " << sqrt((zs-zse)*(zs-zse)+(rs-rse)*(rs-rse))<< std::endl;

        for(int i = 0; i <3 ; ++i )  {
            V(i,0)= Model(0,i)*ym(0,0)/ebs2 + Model(1,i)*ym(1,0)/ers2 + Model(2,i)*ym(2,0)/ets21
                + Model(3,i)*ym(3,0)/ers2 + Model(4,i)*ym(4,0)/ets22;
        }    

        Amg::MatrixX CovTI(3,3); // Covariance Inverse of Track parameters
	CovTI.setZero();
        for(int i = 0; i <3 ; ++i )  {
            for(int j = 0; j <3 ; ++j )  {
                CovTI(i,j) += Model(0,i)*Model(0,j)/ebs2;
                CovTI(i,j) += Model(1,i)*Model(1,j)/ers2;
                CovTI(i,j) += Model(2,i)*Model(2,j)/ets21;
                CovTI(i,j) += Model(3,i)*Model(3,j)/ers2;
                CovTI(i,j) += Model(4,i)*Model(4,j)/ets22;
            }
        }
    // Solution for Track parameters
        CovT = CovTI.inverse(); 
        T = CovT*V;
    //   std::cout << " covariance at track 00 " << CovT(0,0) << " 11 " << CovT(1,1) << " 12 " << CovT(1,2) << " 22 " << CovT(2,2) << std::endl; 

        double theta=0.; 

        if (imeth == 0) {
            theta = atan2(1.,T(1,0));
            invcurvature = T(2,0)*sin(theta);
        } else if (imeth == 1) {
            theta = atan(T(1,0));
            invcurvature = T(2,0)*cos(theta);
        }  

        if (m_debug) std::cout << " Momentum in MeV " << (1./invcurvature)/10. << std::endl;  

    // calculate residuals and chi2
        for(int i = 0; i <5 ; ++i )  {
            Resi(i,0)= Model(i,0)*T(0,0) + Model(i,1)*T(1,0) + Model(i,2)*T(2,0) - ym(i,0);
        }    
        double chi2 = Resi(0,0)*Resi(0,0)/ebs2 + Resi(1,0)*Resi(1,0)/ers2 + Resi(2,0)*Resi(2,0)/ets21
            + Resi(3,0)*Resi(3,0)/ers2 + Resi(4,0)*Resi(4,0)/ets22;
        if (m_debug) std::cout << " resi 00 " << Resi(0,0) << " chi2 " << chi2 << std::endl;   
    //   std:: cout<< " Track parameters Matrix T00 " << T(0,0) << " T10 " << T(1,0) << std::endl;     

    // Reshuffle residuals  Res(0) -> segment position in Model (1,x)
    //                      Res(1) -> segment angle in Model (2,x)

        Res = Amg::VectorX(4);
	Res.setZero();
        for(int i = 1; i <5 ; ++i )  {
            Res[i-1]= Resi(i,0);
        }    

        Pull = Amg::VectorX(4); 
	Pull.setZero();

        bool toobig = false; 
        for(int i = 0; i <4 ; ++i )  {
            if ( i==0 ) Pull[i] = Res[i]/sqrt(ers2);
            if ( i==1 ) Pull[i] = Res[i]/sqrt(ets21);
            if ( i==2 ) Pull[i] = Res[i]/sqrt(ers2);
            if ( i==3 ) Pull[i] = Res[i]/sqrt(ets22);
            if ( fabs( Pull[i] ) > 5 ) toobig = true;
        }

        if (toobig && m_debug) {
            std::cout << " Pull too BIGFIT " << " rad pos1 " << rs << " rad pos2 " << rse << " ang1 " << thetas << " ang2 " << thetase << std::endl;
        }
        if (m_debug) std::cout << " Fit2Segments: Residual 0 " << Res[0] << " pull 0 " << Pull[0] << " Residual 1 " << Res[1] << " pull 1 " << Pull[1] << std::endl;   
        if (m_debug) std::cout << " Fit2Segments: Residual 2 " << Res[2] << " pull 2 " << Pull[2] << " Residual 3 " << Res[3] << " pull 3 " << Pull[3] << std::endl;   
    }
    }

void Muon::MuonCurvedSegmentCombiner::extrapolateSegment( Muon::MuonSegment& seg , Muon::MuonSegment& sege , Amg::VectorX & Res, Amg::VectorX & Pull) {

    // extrapolate segment seg* to segment sege* and determine residuals and pulls

    // Tracking matrices 
    Amg::MatrixX Model(3,3);// H projection matrix PDG 
    Model.setZero();
    Amg::MatrixX CovT(3,3); // Covariance or Track parameters here 3: z beam; r/z segment; tan/cot theta segment;
    CovT.setZero();
    Amg::MatrixX V(3,1);// V = H^T Cov^-1 r   Cov-1 = Covariance of measurements  
    V.setZero();
    Amg::MatrixX T(3,1);// Track parameters   
    T.setZero();
    Amg::MatrixX ym(3,1); // Vector of measurememns
    ym.setZero();
    // Extrapolation Matrices 
    Amg::MatrixX ModelE(3,3);// H projection matrix for second segment  
    ModelE.setZero();
    Amg::MatrixX ymE(3,1); // Vector of measurememts second segment
    ymE.setZero();
    Amg::MatrixX Resi(3,1); // Residuals
    Resi.setZero();

    // Geometry conventions 
    double radius_cylinder = 4000.;
    double z_cylinder = 5000.;
    double z_end = 15000.;
    //  double cos_barrel = std::cos(std::atan2(11430.,14000.));  
    //int imeth;

    // Segment to start
    double thetas = seg.globalDirection().theta();
    double rs = seg.globalPosition().perp();
    double zs = seg.globalPosition().z();
    // Second Segment to extrapolate to
    double thetase = sege.globalDirection().theta();
    double rse = sege.globalPosition().perp();
    double zse = sege.globalPosition().z();
    // Error definitions
    double ebs2 = 50*50; //error squared beam position 
    double ers2 = 0.1*0.1; //error squared position 
    // high pt
    //  double ets2 = 0.0001*0.0001; // error squared angle 
    double ets2 = 0.05*0.05; // error squared angle 
    double sign = 1;
    if ( zs < 0 ) sign = -1.;

    const Muon::MCSCSegmentInfo& info1 = m_segInfoMap[&seg];
    const Muon::MCSCSegmentInfo& info2 = m_segInfoMap[&sege];

    std::string st1 = info1.station;
    std::string st2 = info2.station;
    bool barrel = false;
    if (st1[0] == 'B' || st2[0] == 'B' ) barrel = true;

    if (barrel) {
      //imeth = 0;
    // Barrel Track Model Matrix
        Model(0,0) = 1.;
        Model(1,0) = 1.;
        Model(1,1) = rs;
        Model(1,2) = (rs-radius_cylinder)*(rs-radius_cylinder);
        Model(2,1) = 1.;
        Model(2,2) = 2*(rs-radius_cylinder);
        ModelE(0,0) = 1.;
        ModelE(1,0) = 1.;
        ModelE(1,1) = rse;
        ModelE(1,2) = (rse-radius_cylinder)*(rse-radius_cylinder);
        ModelE(2,1) = 1.;
        ModelE(2,2) = 2*(rse-radius_cylinder);
    // Measurements ym
    // correspondig squared errors: zs -> ers2  cot(thetas) ->ets2
        ym(0,0) = 0.; 
        ym(1,0) = zs; 
        ym(2,0)= cos(thetas)/sin(thetas);
        ymE(0,0) = 0.; 
        ymE(1,0) = zse; 
        ymE(2,0)= cos(thetase)/sin(thetase);
    } else {
      //imeth = 1;
    // Forward Track Model Matrix
        Model(0,0) = 1.;
        Model(1,0) = 1.;
        Model(1,1) = zs;
        Model(1,2) = sign*(zs-sign*z_cylinder)*(zs-sign*z_cylinder);
        Model(2,1) = 1;
        Model(2,2) = sign*2*(zs-sign*z_cylinder);
        ModelE(0,0) = 1.;
        ModelE(1,0) = 1.;
        ModelE(1,1) = zse;
        ModelE(1,2) = sign*(zse-sign*z_cylinder)*(zse-sign*z_cylinder);
        ModelE(2,1) = 1;
        Model(2,2) = sign*2*(zse-sign*z_cylinder);

        if (fabs(zs) > z_end) {
            Model(1,2) = sign*(-z_end*z_end + z_cylinder*z_cylinder + 2*zs*sign*(z_end-z_cylinder));
            Model(2,2) = sign*2*(sign*z_end-sign*z_cylinder);
            ModelE(1,2) = sign*(-z_end*z_end + z_cylinder*z_cylinder + 2*zse*sign*(z_end-z_cylinder));
            ModelE(2,2) = sign*2*(sign*z_end-sign*z_cylinder);
        }  
    // Measurements ym
    // correspondig squared errors: rs -> ers2  tan(thetas) ->ets2
        ym(0,0) = 0.; 
        ym(1,0) = rs; 
        ym(2,0)= tan(thetas);
        ymE(0,0) = 0.; 
        ymE(1,0) = rse; 
        ymE(2,0)= tan(thetase);
    //   std::cout << " Forward zs " << zs << std::endl; 
    }

    for (int iter = 0; iter<1; ++iter) {

        for(int i = 0; i <3 ; ++i )  {
            V(i,0)= Model(0,i)*ym(0,0)/ebs2 + Model(1,i)*ym(1,0)/ers2 + Model(2,i)*ym(2,0)/ets2;
        }    

        Amg::MatrixX CovTI(3,3); // Covariance Inverse of Track parameters
	CovTI.setZero();
        for(int i = 0; i <3 ; ++i )  {
            for(int j = 0; j <3 ; ++j )  {
                CovTI(i,j) += Model(0,i)*Model(0,j)/ebs2;
                CovTI(i,j) += Model(1,i)*Model(1,j)/ers2;
                CovTI(i,j) += Model(2,i)*Model(2,j)/ets2;
            }
        }
    // Solution for Track parameters
        CovT = CovTI.inverse(); 
        T = CovT*V;
    //   std::cout << " covariance at track 00 " << CovT(0,0) << " 11 " << CovT(1,1) << " 12 " << CovT(1,2) << " 22 " << CovT(2,2) << std::endl; 

        // double theta=0.; 
        // if (imeth == 0) {
        //     theta = atan2(1.,T(1,0));
        //     //invcurvature = T(2,0)*sin(theta);
        // } else if (imeth == 1) {
        //     theta = atan(T(1,0));
        //     //invcurvature = T(2,0)*cos(theta);
        // }  
        if (iter == 0) {
    //     ets2 = invcurvature*invcurvature*10000.*50*50;
    //     if (ets2 < 1e-8) ets2 = 1e-8;
    //     if (ets2 > 1.e-2) ets2 = 1e-2;
        } 
        if (iter == 1) {
    //     std::cout << " reiterated with ets2 angular spread" << ets2 << " invcurv " << invcurvature  << " cos(theta) " << cos(theta) << std::endl;
        } 
    }


    // calculate residuals and chi2
    for(int i = 0; i <3 ; ++i )  {
        Resi(i,0)= Model(i,0)*T(0,0) + Model(i,1)*T(1,0) + Model(i,2)*T(2,0) - ym(i,0);
    }    
    //   double chi2 = Resi(0,0)*Resi(0,0)/ebs2 + Resi(1,0)*Resi(1,0)/ers2 + Resi(2,0)*Resi(2,0)/ets2;
    //   std::cout << " residuals matrix method  z " << Res(0,0) << " cot(theta) " << Res(1,0) << " chi2 " << chi2 << std::endl;   
    //   std:: cout<< " Track parameters Matrix T00 " << T(0,0) << " T10 " << T(1,0) << std::endl;     

    // Reshuffle residuals  Res(0) -> segment position in Model (1,x)
    //                      Res(1) -> segment angle in Model (2,x)

    //   Amg::VectorX Res(2,0);
    Res = Amg::VectorX(2); 
    Res.setZero();
    for(int i = 1; i <3 ; ++i )  {
        Res[i-1]= ModelE(i,0)*T(0,0) + ModelE(i,1)*T(1,0) + ModelE(i,2)*T(2,0) - ymE(i,0);
    }    

    Amg::MatrixX ModelET(3,3);// Transposed  
    ModelET.setZero();
    for(int i = 0; i <3 ; ++i )  {
        for(int j = 0; j <3 ; ++j )  {
            ModelET(i,j)=ModelE(j,i);
        }
    }
    Amg::MatrixX CovE(3,3);// Covariance of predicition 
    CovE.setZero();
    CovE = ModelE*CovT*ModelET;

    Pull = Amg::VectorX(2); 
    Pull.setZero();

    for(int i = 0; i <2 ; ++i )  {
      if ( i==0 ) Pull[i] = Res[i]/sqrt(CovE(1,1)+ers2);
      if ( i==1 ) Pull[i] = Res[i]/sqrt(CovE(2,2)+ets2);
    }

    if (m_debug) std::cout << " Extrapolation: Residual 0 " << Res[0] << " pull 0 " << Pull[0] << " Residual 1 " << Res[1] << " pull 1 " << Pull[1] << std::endl;   


}

