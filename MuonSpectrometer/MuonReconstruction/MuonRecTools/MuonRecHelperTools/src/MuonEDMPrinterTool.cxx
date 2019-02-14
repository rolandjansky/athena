/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"

#include "TrkToolInterfaces/ITrackSummaryHelperTool.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/AlignmentEffectsOnTrack.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
#include "TrkTrackSummary/TrackSummary.h"

#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"

#include "MuonPrepRawData/MuonCluster.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/CscStripPrepData.h"

#include "MuonPattern/MuonPattern.h"
#include "MuonPattern/MuonPatternCombination.h"
#include "MuonPattern/MuonPatternChamberIntersect.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"

#include "MuonSegment/MuonSegmentQuality.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentCombination.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "Identifier/Identifier.h"

#include "MuonIdHelpers/MuonStationIndex.h"

#include <algorithm>
#include <iostream>

namespace Muon {


  MuonEDMPrinterTool::MuonEDMPrinterTool(const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa), 
      m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
      m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
      m_summaryHelper("Muon::MuonTrackSummaryHelperTool/MuonTrackSummaryHelperTool"),
      m_pullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator")
  {
    declareInterface<MuonEDMPrinterTool>(this);
    declareProperty( "MuonIdHelperTool",    m_idHelper);
    declareProperty( "MuonEDMHelperTool",   m_helper);
    declareProperty( "MuonTrackSummaryHelperTool", m_summaryHelper);
  }


  MuonEDMPrinterTool::~MuonEDMPrinterTool(){}


  StatusCode MuonEDMPrinterTool::initialize()
  {
    if ( AthAlgTool::initialize().isFailure() ) return StatusCode::FAILURE;
  
    if (m_idHelper.retrieve().isFailure()){
      ATH_MSG_WARNING("Could not get " << m_idHelper); 
      return StatusCode::FAILURE;
    }

    
    if (m_helper.retrieve().isFailure()){
      ATH_MSG_WARNING("Could not get " << m_helper); 
      return StatusCode::FAILURE;
    }

    if (m_summaryHelper.retrieve().isFailure()){
      ATH_MSG_WARNING("Could not get " << m_summaryHelper); 
      return StatusCode::FAILURE;
    }
  
    return StatusCode::SUCCESS;
  }

  StatusCode MuonEDMPrinterTool::finalize()
  {
    if( AthAlgTool::finalize().isFailure() ) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
  }

  std::string MuonEDMPrinterTool::print( const Trk::MuonTrackSummary& summary ) const{

    std::ostringstream sout;
    // create detailed track summary
    typedef Trk::MuonTrackSummary::ChamberHitSummary    ChamberHitSummary;

    sout << "Hits: eta " << summary.netaHits() << "  phi " << summary.nphiHits() 
	 << "  holes "  << summary.nholes() << "  outliers " << summary.noutliers()
	 << "  pseudo " << summary.npseudoMeasurements()
	 << "  scatterers " << summary.nscatterers() << " close Hits " << summary.ncloseHits() << std::endl; 
    

    const std::vector<ChamberHitSummary>& chamberHitSummary = summary.chamberHitSummary();
    sout.setf(std::ios::left);
    std::vector<ChamberHitSummary>::const_iterator chit = chamberHitSummary.begin();
    std::vector<ChamberHitSummary>::const_iterator chit_end = chamberHitSummary.end();
    std::vector<ChamberHitSummary>::const_iterator chit_last = chit_end - 1;
    for( ;chit!=chit_end;++chit ){
      const Identifier& chId = chit->chamberId();
      bool isMdt = m_idHelper->isMdt(chId);
    
      sout << "  " << std::setw(35) << m_idHelper->toStringChamber(chId);
    
      const ChamberHitSummary::Projection& first  = isMdt ? chit->mdtMl1() : chit->etaProjection();
      const ChamberHitSummary::Projection& second = isMdt ? chit->mdtMl2() : chit->phiProjection();
    
    
      std::string firstString = isMdt ? "ml1 " : "eta ";
      std::string secondString = isMdt ? "ml2 " : "phi ";
    
      sout << " Hits:  " << firstString << std::setw(3) << first.nhits
	   << "   " << secondString << std::setw(3) << second.nhits; 
    
      if( first.nholes || second.nholes ) {
	sout << "  Holes:  ";
	if( first.nholes != 0 ) sout << firstString << std::setw(3) << first.nholes;
	if( second.nholes != 0 ) {
	  if( first.nholes != 0 ) sout << "  ";
	  sout << secondString << std::setw(3) << second.nholes; 
	}
      }
      if( first.noutliers || second.noutliers ) {
	sout << "  Outliers:  ";
	if( first.noutliers != 0 ) sout << firstString << std::setw(3) << first.noutliers << "  ";
	if( second.noutliers != 0 ) {
	  if( first.noutliers != 0 ) sout << "  ";
	  sout << secondString << std::setw(3) << second.noutliers; 
	}
      }
    
      if( first.ndeltas || second.ndeltas ) {
	sout << "  Deltas:  ";
	if( first.ndeltas != 0 ) sout << firstString << std::setw(3) << first.ndeltas << "  ";
	if( second.ndeltas != 0 ) {
	  if( first.ndeltas != 0 ) sout << "  ";
	  sout << secondString << std::setw(3) << second.ndeltas; 
	}
      }

      if( first.ncloseHits || second.ncloseHits ) {
	sout << "  Close Hits:  ";
	if( first.ncloseHits != 0 ) sout << firstString << std::setw(3) << first.ncloseHits << "  ";
	if( second.ncloseHits != 0 ) {
	  if( first.ncloseHits != 0 ) sout << "  ";
	  sout << secondString << std::setw(3) << second.ncloseHits; 
	}
      }
    
      if( chit != chit_last ) sout << std::endl;
    }

    return sout.str();
  }


  std::string MuonEDMPrinterTool::printStations( const Trk::Track& track ) const{

    const Trk::MuonTrackSummary* muonSummary = 0;

    // check if the track already has a MuonTrackSummary, if not calculate it using the helper
    const Trk::TrackSummary* summary = track.trackSummary();
    if ( summary ) muonSummary = summary->muonTrackSummary();
    Trk::TrackSummary tmpSummary;
    if( !muonSummary ) {
      m_summaryHelper->addDetailedTrackSummary(track,tmpSummary);
      muonSummary = tmpSummary.muonTrackSummary();
      if( !muonSummary ) return "failed to create MuonTrackSummary ";
    }

    return print(*muonSummary);
  }


  std::string MuonEDMPrinterTool::printMeasurements( const Trk::Track& track ) const {
    std::ostringstream sout;
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if ( !states ) return "";
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it_end = states->end();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it2 = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it2_end = states->end();
    // Build map of AEOTs and the measurements they affect
    std::multimap<const Trk::MeasurementBase*, const Trk::AlignmentEffectsOnTrack *> measAndTheirAlignmentEffects;
    const Trk::MeasurementBase* m = 0;
    const Trk::AlignmentEffectsOnTrack* aeot = 0;
    for ( ; it != it_end; ++it ) {
        aeot = (*it)->alignmentEffectsOnTrack();
        if (aeot) {
            // Now get the list of identifiers which this AEOT impacts.
            const std::vector< Identifier>& identifiers = aeot->vectorOfAffectedTSOS();
            it2 = states->begin();
            it2_end = states->end();
            for (; it2 != it2_end; ++it2 ) {
                m = (*it2)->measurementOnTrack();
                if (m) {
                    Identifier id = m_helper->getIdentifier(*m);
                    if ( ( id.is_valid() && (std::find(identifiers.begin(), identifiers.end(), id)!=identifiers.end() ) ) 
                       ||  (aeot->effectsLastFromNowOn() && it2>it) ) {
                        // Either this measurement is explicitly listed, OR it is in a TSOS after an AEOT whose effects last from now on. 
                        measAndTheirAlignmentEffects.insert(std::pair<const Trk::MeasurementBase*, const Trk::AlignmentEffectsOnTrack*>(m,aeot) );
                    }
                }
            }
        }
    }
//    std::cout << " measAndTheirAlignmentEffects " << measAndTheirAlignmentEffects.size() << std::endl;

    // Reset
    it = states->begin();
    it_end = states->end();
    // first loop to get width of Id's for nice alignment
    std::vector< std::string > idStrings;
    std::vector< std::string > dataStrings;
    unsigned int idWidth = 0;
    for ( ; it != it_end; ++it ) {
      m = (*it)->measurementOnTrack();
      if (m) {
	// Identifier part
	std::string idStr = printId(*m);
	idStrings.push_back(idStr);
	if ( idStr.length() > idWidth ) idWidth = idStr.length();
	// Data part
        const Trk::TrackParameters* trackParameters = (*it)->trackParameters();
	std::string dataStr = printData(*m);
        if (trackParameters) {
          std::multimap<const Trk::MeasurementBase*, const Trk::AlignmentEffectsOnTrack *>::iterator itMap = measAndTheirAlignmentEffects.begin();
          itMap = measAndTheirAlignmentEffects.find( m );
          if(itMap != measAndTheirAlignmentEffects.end() ) { 
            std::vector <const Trk::AlignmentEffectsOnTrack*> aeotos;
            aeotos.push_back(itMap->second);
            itMap++;
            if(itMap != measAndTheirAlignmentEffects.end()&&itMap->first==m) aeotos.push_back(itMap->second);
            const Trk::ResidualPull* resPull = m_pullCalculator->residualPull(m, trackParameters, Trk::ResidualPull::Unbiased, Trk::TrackState::unidentified, aeotos); 
            if (resPull) dataStr += print(*resPull);
            if (resPull) dataStr += " (AEOT)";   
          } else {
            const Trk::ResidualPull* resPull = m_pullCalculator->residualPull(m, trackParameters, Trk::ResidualPull::Unbiased);
            if (resPull) dataStr += print(*resPull);
          }
        }
	if ( (*it)->type(Trk::TrackStateOnSurface::Outlier) ) {
	  dataStr += " (Outlier)";
	} else if ( (*it)->type(Trk::TrackStateOnSurface::Hole) ) {
	  dataStr += " (Hole)";
	}
	dataStrings.push_back(dataStr);
      }
      aeot = (*it)->alignmentEffectsOnTrack();
      if (aeot) {
	std::string idStr = " AEOT ";
	idStrings.push_back(idStr);
        std::ostringstream souta;
        souta << std::setprecision(3) << " deltaTranslation (mm) " << aeot->deltaTranslation()  << " error " << aeot->sigmaDeltaTranslation() << " deltaAngle (mrad) " << 1000*aeot->deltaAngle() << " error " << 1000*aeot->sigmaDeltaAngle(); 
        dataStrings.push_back(souta.str());
      } 
    }

    // second loop to print out aligned strings
    unsigned int n = idStrings.size();
    for ( unsigned int i = 0; i < n; ++i ) {
      sout << std::left << std::setw(idWidth) << idStrings[i] << std::right << "  " << dataStrings[i];
      if ( i != n-1 ) sout << std::endl;
    }

    return sout.str();
  }


  std::string MuonEDMPrinterTool::printPatRec( const Trk::Track& track ) const {
    if(track.info().patternRecoInfo(Trk::TrackInfo::STACO))   		return "STACO";
    if(track.info().patternRecoInfo(Trk::TrackInfo::StacoLowPt))        return "StacoLowPt";
    if(track.info().patternRecoInfo(Trk::TrackInfo::Muonboy))           return "Muonboy";
    if(track.info().patternRecoInfo(Trk::TrackInfo::MuTag))		return "MuTag";
    if(track.info().patternRecoInfo(Trk::TrackInfo::MuidComb) ||
       track.info().patternRecoInfo(Trk::TrackInfo::MuidCombined))	return "MuidCombined";
    if(track.info().patternRecoInfo(Trk::TrackInfo::MuidStandAlone))	return "MuidStandAlone";
    if(track.info().patternRecoInfo(Trk::TrackInfo::Moore))		return "Moore";		   
    if(track.info().patternRecoInfo(Trk::TrackInfo::MuGirl))		return "MuGirl";
    if(track.info().patternRecoInfo(Trk::TrackInfo::MuGirlUnrefitted))  return "MuGirlUnrefitted";
    if(track.info().patternRecoInfo(Trk::TrackInfo::MuidVertexAssociator)) return "MuidVertexAssociator";
    if(track.info().patternRecoInfo(Trk::TrackInfo::MuGirlLowBeta))	return "MuGirlLowBeta";
    return "Unknown";       
  }

  std::string MuonEDMPrinterTool::print( const Trk::Track& track ) const {

    std::ostringstream sout;

    sout << printPatRec(track);
    
    const Trk::FitQuality* fq = track.fitQuality();
    if( fq ) {
      sout << std::setprecision(4) << " : chi2 " << fq->chiSquared() << " ndof " << fq->numberDoF();
    }else{
      sout << " no fit Quality ";
    }
    const Trk::Perigee* pp = track.perigeeParameters();
    if( pp ){
      sout << "  " << print(*pp);
    }else{
      sout << " no perigee ";
    }
    
    return sout.str();
  }

  
  std::string MuonEDMPrinterTool::print( const MuonSegment& segment ) const {

    std::ostringstream sout;
    
    // get first none-trigger id 
    Identifier chid = m_helper->chamberId(segment);
    int nphi = 0;
    int ntrigEta = 0;
    int neta    = 0;

    const MuonGM::MdtReadoutElement* mdtDetEl = 0;
    Identifier shortestTubeId;
    double shortestTubeLen = 1e9;

    std::vector< const Trk::MeasurementBase* >::const_iterator hit = segment.containedMeasurements().begin();
    std::vector< const Trk::MeasurementBase* >::const_iterator hit_end = segment.containedMeasurements().end();
    for( ;hit!=hit_end;++hit ){
      Identifier id;
      const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(*hit);
      if( rot ) id = rot->identify();
      else{
	const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(*hit);
	if( crot ) id = crot->containedROTs().front()->identify();
      }
      if( !id.is_valid() ) continue;
      bool measuresPhi = m_idHelper->measuresPhi(id);
      bool isTrigger   = m_idHelper->isTrigger(id);
      if( measuresPhi )    ++nphi;
      else if( !isTrigger) ++neta;

      if( !isTrigger ){
	const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(rot);
	if( mdt && mdt->prepRawData() ) {
	  double tubelen = 0.5*mdt->prepRawData()->detectorElement()->tubeLength(id); 
	  if( tubelen < shortestTubeLen ) {
	    shortestTubeId = id;
	    shortestTubeLen = tubelen;
	    mdtDetEl = mdt->prepRawData()->detectorElement();
	  }
	}
      }else{
	if( !measuresPhi ) ++ntrigEta;
      }
    }

    sout << m_idHelper->toStringChamber(chid);

    const Trk::FitQuality* fq = segment.fitQuality();
    if( fq ) {
      sout << std::setprecision(2) << "   chi2 "  << std::setw(7) << fq->chiSquared();
    }else{
      sout << " no fit Quality ";
    }

    sout << " neta " << std::setw(2) << neta << " nphi " << std::setw(2) << nphi << " nTrigEta " << std::setw(2) << ntrigEta;
    
    const MuonSegmentQuality* q = dynamic_cast<const MuonSegmentQuality*>(fq);
    if( q ) sout << std::setw(2) << " nholes " << q->numberOfHoles();

    //sout << " r "     << std::fixed << std::setprecision(0) << std::setw(5) << segment.globalPosition().perp()
         // << " phi "   << std::fixed << std::setprecision(3) << std::setw(6) << segment.globalPosition().phi()
	 //<< " z "     << std::fixed << std::setprecision(0) << std::setw(6) << segment.globalPosition().z()
    sout << " theta " << std::fixed << std::setprecision(5) << std::setw(7) << segment.globalDirection().theta()
	 << " phi "   << std::fixed << std::setprecision(3) << std::setw(6) << segment.globalDirection().phi();

    if( segment.hasFittedT0() ) sout << " T0 " << std::fixed << std::setprecision(2) << std::setw(5) << segment.time()
				     << " err " << std::setw(5) << segment.errorTime();
    
    if( mdtDetEl ){
      double posAlongTube = fabs(mdtDetEl->globalToLocalCoords( segment.globalPosition(), shortestTubeId ).z());
      double distFromEdge = posAlongTube - shortestTubeLen;
      if( distFromEdge < -100. )        sout << " inside chamber";
      else if( distFromEdge < -1. )     sout << " close to edge";
      else if( distFromEdge < 0.0001 )  sout << " on edge";
      else                              sout << " outside chamber";
    }
    return sout.str();
  }

  std::string MuonEDMPrinterTool::print( const Trk::PrepRawData& prd ) const {

    std::ostringstream sout;

    Identifier id = prd.identify();
    sout << m_idHelper->toString( id ) << "  ";

    const Amg::Vector3D* pos = 0;
    const MuonCluster* cl = dynamic_cast<const MuonCluster*>(&prd);
    if( cl ) pos = &cl->globalPosition();
    else{
      const MdtPrepData* mdt = dynamic_cast<const MdtPrepData*>(&prd);
      if( mdt ) pos = &mdt->detectorElement()->surface(id).center();
      else{
	const CscStripPrepData* strip = dynamic_cast<const CscStripPrepData*>(&prd);
	if( strip ) pos = &strip->globalPosition();
      }
    }
    if(!pos) sout << " unknown type of muon prepdata " << std::endl;
    else{
      double h_r = pos->perp();
      double h_z = pos->z();
      double h_phi = pos->phi();
      double h_theta = pos->theta();
      
      // add time for RPC
      double rpcTime = 0.0;
      const RpcPrepData* rpc = dynamic_cast<const RpcPrepData*>(&prd);
      if (rpc) rpcTime = rpc->time();
      
      sout << "r "     << std::fixed << std::setprecision(0) << std::setw(5) << h_r
	   << " z "     << std::fixed << std::setprecision(0) << std::setw(5) << h_z 
	   << " theta " << std::fixed << std::setprecision(3) << std::setw(4) << h_theta
	   << " phi "   << std::fixed << std::setprecision(3) << std::setw(4) << h_phi;
      if (rpc) sout << "  time " << std::fixed << std::setprecision(2) << std::setw(5) << rpcTime;

    }
    return sout.str();
  }

  std::string MuonEDMPrinterTool::print( const Trk::MeasurementBase& measurement ) const {
    return printId( measurement ) + "  " + printData( measurement );
  }


  std::string MuonEDMPrinterTool::print( const std::vector< const Trk::MeasurementBase* >& measurements ) const {

    std::ostringstream sout;

    // first loop to get width of Id's for nice alignment
    std::vector< std::string > idStrings;
    std::vector< std::string > dataStrings;
    unsigned int idWidth = 0;

    std::vector< const Trk::MeasurementBase* >::const_iterator hit = measurements.begin();
    std::vector< const Trk::MeasurementBase* >::const_iterator hit_end = measurements.end();
    //std::vector< const Trk::MeasurementBase* >::const_iterator hit_last = hit_end;
    for( ;hit!=hit_end;++hit ){
      if(!*hit) {
	sout << " WARNING, zero pointer detected in MeasurementBase vector!!! " << std::endl;
	continue;
      }
      // Identifier part
      std::string idStr = printId(**hit);
      idStrings.push_back(idStr);
      if ( idStr.length() > idWidth ) idWidth = idStr.length();
      // Data part
      std::string dataStr = printData(**hit);
      dataStrings.push_back(dataStr);
    }


    // second loop to print out aligned strings
    unsigned int n = idStrings.size();
    for ( unsigned int i = 0; i < n; ++i ) {
      sout << std::left << std::setw(idWidth) << idStrings[i] << std::right << "  " << dataStrings[i];
      if ( i != n-1 ) sout << std::endl;
    }
    
    return sout.str();
  }

  std::string MuonEDMPrinterTool::print( const std::vector< const MuonSegment* >& segs ) const {

    std::ostringstream sout;

    std::vector< const MuonSegment* >::const_iterator it = segs.begin();
    std::vector< const MuonSegment* >::const_iterator it_end = segs.end();
    std::vector< const MuonSegment* >::const_iterator it_last = it_end; --it_last;
    for( ;it!=it_end;++it ){
      if(!*it) {
	sout << " WARNING, zero pointer detected in MuonSegment vector!!! " << std::endl;
	continue;
      }
      sout << print(**it);
      if( it != it_last ) sout << std::endl;
    }
    
    return sout.str();
  }

  std::string MuonEDMPrinterTool::print( std::vector< std::unique_ptr< MuonSegment> >& segs ) const {

    std::ostringstream sout;

    std::vector< std::unique_ptr<MuonSegment> >::iterator it = segs.begin();
    std::vector< std::unique_ptr<MuonSegment> >::iterator it_end = segs.end();
    std::vector< std::unique_ptr<MuonSegment> >::iterator it_last = it_end; --it_last;
    for( ;it!=it_end;++it ){
      if(!*it) {
	sout << " WARNING, zero pointer detected in MuonSegment vector!!! " << std::endl;
        continue;
      }
      sout << print(**it);
      if( it != it_last ) sout << std::endl;
    }

    return sout.str();
  }

  std::string MuonEDMPrinterTool::print( const MuonSegmentCombinationCollection& combiCol ) const {
    std::ostringstream sout;

    sout << "MuonSegmentCombinationCollection with combis " << combiCol.size() << std::endl;
    MuonSegmentCombinationCollection::const_iterator it = combiCol.begin();
    MuonSegmentCombinationCollection::const_iterator it_end = combiCol.end();
    MuonSegmentCombinationCollection::const_iterator it_last = it_end; --it_last;
    for( ;it!=it_end;++it ){
      if( !*it ) {
	sout << " WARNING, zero pointer detected in MuonSegmentCombinationCollection!!! " << std::endl;
	continue;
      }
      sout << print( **it );
      if( it != it_last ) sout << std::endl;
    }
    return sout.str();
  }

  std::string MuonEDMPrinterTool::print( const MuonSegmentCombination& combi ) const {
    std::ostringstream sout;

    unsigned int nstations = combi.numberOfStations();
    sout << "SegmentCombination with stations " << nstations << std::endl;

    // loop over chambers in combi 
    for(unsigned int i=0; i<nstations; ++i){

      // loop over segments in station
      MuonSegmentCombination::SegmentVec* stationSegs = combi.stationSegments( i ) ;
         
      // check if not empty
      if( !stationSegs || stationSegs->empty() ) continue;
  
      // get chamber identifier, chamber index and station index
      //Identifier chid = m_helper->chamberId( *stationSegs->front() );
      sout << print(*stationSegs);
      if( i != nstations-1 ) sout << std::endl;
    }

    return sout.str();
  }

  std::string MuonEDMPrinterTool::print( const MuonPattern& pattern ) const {
    std::ostringstream sout;

    unsigned int nhits = pattern.numberOfContainedPrds();
    sout << "MuonPattern with " << nhits << " hits"
	 << std::setprecision(5) << " theta " << pattern.globalDirection().theta() << " phi " << pattern.globalDirection().phi() 
	 << std::setprecision(1) << " r " << std::setw(5) << (int)pattern.globalPosition().perp() 
	 << "  z " << std::setw(5) << (int)pattern.globalPosition().z() << std::endl;
    for( unsigned int i=0;i<nhits;++i ){
      const Trk::PrepRawData* prd = pattern.prd(i);
      if( !prd ){
	sout << " ERROR found zero pointer on MuonPattern " << std::endl;
	continue;
      }
      sout << "  " << print( *prd );
      if( i < nhits-1 ) sout << std::endl;
    }
    return sout.str();
  }

  std::string MuonEDMPrinterTool::print( const MuonPatternCollection& patCol ) const {
    std::ostringstream sout;

    sout << "MuonPatternCollection with " << patCol.size() << " patterns " << std::endl;
    MuonPatternCollection::const_iterator it = patCol.begin();
    MuonPatternCollection::const_iterator it_end = patCol.end();
    MuonPatternCollection::const_iterator it_last = it_end; --it_last;
    for( ;it!=it_end;++it ){
      if( !*it ) {
	sout << " WARNING, zero pointer detected in MuonPatternCollection!!! " << std::endl;
	continue;
      }
      sout << " " << print( **it );
      if( it != it_last ) sout << std::endl;
    }
    return sout.str();

  }

  std::string MuonEDMPrinterTool::print( const MuonPrdPatternCollection& patCol ) const {
    std::ostringstream sout;

    sout << "MuonPrdPatternCollection with " << patCol.size() << " patterns " << std::endl;
    MuonPrdPatternCollection::const_iterator it = patCol.begin();
    MuonPrdPatternCollection::const_iterator it_end = patCol.end();
    MuonPrdPatternCollection::const_iterator it_last = it_end; --it_last;
    for( ;it!=it_end;++it ){
      if( !*it ) {
	sout << " WARNING, zero pointer detected in MuonPatternCollection!!! " << std::endl;
	continue;
      }
      sout << " " << print( **it );
      if( it != it_last ) sout << std::endl;
    }
    return sout.str();

  }

  std::string MuonEDMPrinterTool::print( const MuonPatternCombinationCollection& combiCol ) const {
    std::ostringstream sout;

    sout << "MuonPatternCombinationCollection with combis " << combiCol.size() << std::endl;
    MuonPatternCombinationCollection::const_iterator it = combiCol.begin();
    MuonPatternCombinationCollection::const_iterator it_end = combiCol.end();
    MuonPatternCombinationCollection::const_iterator it_last = it_end; --it_last;
    for( ;it!=it_end;++it ){
      if( !*it ) {
	sout << " WARNING, zero pointer detected in MuonPatternCombinationCollection!!! " << std::endl;
	continue;
      }
      sout << print( **it );
      if( it != it_last ) sout << std::endl;
    }
    return sout.str();

  }

  std::string MuonEDMPrinterTool::print( const MuonPatternCombination& combi ) const {
    std::ostringstream sout;

    unsigned int nchambers = combi.chamberData().size();
    sout << "MuonPatternCombination with " << nchambers << " chambers ";
    const Trk::TrackParameters* pars = combi.trackParameter();
    if( pars ) {
      sout << " " << print(*pars);
    }else{
      sout << "    -> combi has no parameters ";
    }
    if( !combi.chamberData().empty() ) sout << std::endl;
    std::vector< MuonPatternChamberIntersect >::const_iterator chit = combi.chamberData().begin();
    std::vector< MuonPatternChamberIntersect >::const_iterator chit_end = combi.chamberData().end();
    std::vector< MuonPatternChamberIntersect >::const_iterator chit_last = chit_end; --chit_last;
    for( ;chit!=chit_end;++chit ){
      const MuonPatternChamberIntersect& chamberIntersect = *chit;
      sout << print(chamberIntersect);
      if( chit != chit_last ) sout << std::endl;
    }
    return sout.str();
  }

  std::string MuonEDMPrinterTool::print( const MuonPatternChamberIntersect& intersect ) const {
    std::ostringstream sout;
    
    std::string chIdString("Unknown id");
    double chTheta(-99.);
    double chPhi(-99.);
    // if the intersect is not empty use the Identifier of first hit as chamber ID
    const Trk::PrepRawData* firstPrd = 0;
    if( !intersect.prepRawDataVec().empty() && intersect.prepRawDataVec().front() ){
      firstPrd = intersect.prepRawDataVec().front();
      chIdString = m_idHelper->toStringChamber(firstPrd->identify());
      chTheta = firstPrd->detectorElement()->center().theta();
      chPhi = firstPrd->detectorElement()->center().phi();
    }else{
      return chIdString;
    }
    Identifier chId = m_idHelper->chamberId(firstPrd->identify());
    int neta = 0;
    int nphi = 0;
    bool isMdt = false;

    unsigned int nchannelsEta = 0;
    unsigned int nchannelsPhi = 0;
    std::set<const Trk::TrkDetElementBase*> detEls;
    std::vector< const Trk::PrepRawData* >::const_iterator hit = intersect.prepRawDataVec().begin();
    std::vector< const Trk::PrepRawData* >::const_iterator hit_end = intersect.prepRawDataVec().end();
    for(;hit!=hit_end;++hit ){
      const Trk::PrepRawData* prd = *hit;
      if( !prd ){
	sout << " ERROR found zero pointer on MuonPatternChamberIntersect " << std::endl;
	continue;
      }
      const Identifier& id = prd->identify();
      if( m_idHelper->measuresPhi(id) ) ++nphi;
      else                              ++neta;
      if( !isMdt && m_idHelper->isMdt(id) ) isMdt = true;

      if( !detEls.count(prd->detectorElement()) ) {
	detEls.insert(prd->detectorElement());

	if( isMdt && detEls.empty() ){

          const MuonGM::MuonDetectorManager* detMgr = 0;
          if( !detStore()->retrieve( detMgr ) || !detMgr ){
            ATH_MSG_DEBUG("Cannot retrieve DetectorManager ");
          }else{
            Identifier idml1 = m_idHelper->mdtIdHelper().channelID(id,1,1,1);
            Identifier idml2 = m_idHelper->mdtIdHelper().channelID(id,2,1,1);
            const MuonGM::MdtReadoutElement* detEl1 = detMgr->getMdtReadoutElement( idml1 );
            const MuonGM::MdtReadoutElement* detEl2 = 0;
            if (m_idHelper->mdtIdHelper().numberOfMultilayers(id) == 2){
              detEl2 = detMgr->getMdtReadoutElement( idml2 );
            }else{
              ATH_MSG_DEBUG("A single multilayer for this station " << m_idHelper->toString(id));
            }	
            if( detEl1 /** && !detEls.count(detEl)**/ ){
              detEls.insert(detEl1);
              nchannelsEta += detEl1->getNLayers()*detEl1->getNtubesperlayer();
            }else{
              ATH_MSG_DEBUG("Couldn't find first detector element ");
            }
            if( detEl2 /**&& !detEls.count(detEl2)**/ ){
              detEls.insert(detEl2);
              nchannelsEta += detEl2->getNLayers()*detEl2->getNtubesperlayer();
            }
          }
	}else if( m_idHelper->isTgc(id) ){
	  const MuonGM::TgcReadoutElement* detEl = dynamic_cast<const MuonGM::TgcReadoutElement*>(prd->detectorElement());
          if(detEl) {
	    for( int i=1;i<=detEl->Ngasgaps();++i ) {
	      nchannelsEta += detEl->getNGangs(i);
	      nchannelsPhi += detEl->nStrips(i);
	    }
          }
	}else if( m_idHelper->isRpc(id) ){
	  const MuonGM::RpcReadoutElement* detEl = dynamic_cast<const MuonGM::RpcReadoutElement*>(prd->detectorElement());
          if(detEl) {
	    nchannelsPhi += detEl->Nphigasgaps()*detEl->NphiStripPanels()*detEl->NphiStrips();
	    nchannelsEta += detEl->Nphigasgaps()*detEl->NetaStripPanels()*detEl->NetaStrips();
	  }
	}
      }
    }

    unsigned int nchHitsEta = 0;
    unsigned int nchHitsPhi = 0;


    StoreGateSvc* storeGate = 0;
    if (service("StoreGateSvc", storeGate).isFailure() || !storeGate ){
      ATH_MSG_DEBUG("Cannot retrieve StoreGateSvc ");
    }else{
      if( isMdt ){
	const MdtPrepDataContainer* mdtPrdContainer = 0;
	std::string key = "MDT_DriftCircles";
	if(storeGate->retrieve(mdtPrdContainer,key).isFailure()) {
	  ATH_MSG_DEBUG("Cannot retrieve " << key);
	}else{
	  IdentifierHash hash_id;
	  m_idHelper->mdtIdHelper().get_module_hash(chId,hash_id );
	  MdtPrepDataContainer::const_iterator colIt = mdtPrdContainer->indexFind(hash_id);
	  if( colIt != mdtPrdContainer->end() ) nchHitsEta = (*colIt)->size();
	  else 	  ATH_MSG_DEBUG("Collection not found: hash " << hash_id);
	}
      }else if( m_idHelper->isRpc(chId) ){
	const RpcPrepDataContainer* rpcPrdContainer = 0;
	std::string key = "RPC_Measurements";
	if(storeGate->retrieve(rpcPrdContainer,key).isFailure()) {
	  ATH_MSG_DEBUG("Cannot retrieve " << key);
	}else{
	  IdentifierHash hash_id;
	  m_idHelper->rpcIdHelper().get_module_hash(chId,hash_id );
	  RpcPrepDataContainer::const_iterator colIt = rpcPrdContainer->indexFind(hash_id);
	  if( colIt != rpcPrdContainer->end() ) {
	    RpcPrepDataCollection::const_iterator rpcIt = (*colIt)->begin();
	    RpcPrepDataCollection::const_iterator rpcIt_end = (*colIt)->end();
	    for( ;rpcIt!=rpcIt_end;++rpcIt ){
	      if( m_idHelper->measuresPhi((*rpcIt)->identify()) ) ++nchHitsPhi;
	      else                                                ++nchHitsEta;
	    }
	  }else ATH_MSG_DEBUG("Collection not found: hash " << hash_id);
	}
      }else if( m_idHelper->isTgc(chId) ){
	const TgcPrepDataContainer* tgcPrdContainer = 0;
	std::string key = "TGC_Measurements";
	if(storeGate->retrieve(tgcPrdContainer,key).isFailure()) {
	  ATH_MSG_DEBUG("Cannot retrieve " << key);
	}else{
	  IdentifierHash hash_id;
	  m_idHelper->tgcIdHelper().get_module_hash(chId,hash_id );
	  TgcPrepDataContainer::const_iterator colIt = tgcPrdContainer->indexFind(hash_id);
	  if( colIt != tgcPrdContainer->end() ) {
	    TgcPrepDataCollection::const_iterator tgcIt = (*colIt)->begin();
	    TgcPrepDataCollection::const_iterator tgcIt_end = (*colIt)->end();
	    for( ;tgcIt!=tgcIt_end;++tgcIt ){
	      if( m_idHelper->measuresPhi((*tgcIt)->identify()) ) ++nchHitsPhi;
	      else                                                ++nchHitsEta;
	    }
	  }else ATH_MSG_DEBUG("Collection not found: hash " << hash_id);
	}
      }
    }

    sout.setf( std::ios::left );
    sout << std::setw(33) << chIdString << " neta " << std::setw(3) << neta;
    if( !isMdt ) sout << " nphi " << std::setw(3) << nphi;
    else         sout << "         ";

    double scaleEta = nchannelsEta != 0 ? 1./nchannelsEta : 0.;
    double scalePhi = nchannelsPhi != 0 ? 1./nchannelsPhi : 0.;
    sout << " occupancy eta " << std::fixed << std::setprecision(2) << std::setw(5) << scaleEta*nchHitsEta;
    if( !isMdt )  sout << " phi " << std::fixed << std::setw(5) << scalePhi*nchHitsPhi;
    else          sout << "          ";

    sout << std::setprecision(2) << "  pat theta " << std::setw(5) << intersect.intersectDirection().theta() 
	 << " ch theta " << std::setw(5) << chTheta
	 << " ch phi " << std::setw(6) << chPhi
	 << std::setprecision(1) << " r " << std::setw(5) << (int)intersect.intersectPosition().perp() 
	 << "  z " << std::setw(5) << (int)intersect.intersectPosition().z();
    
    return sout.str();
  }

  std::string MuonEDMPrinterTool::print( const Trk::TrackParameters& pars ) const {
    std::ostringstream sout;
    sout << "r " << std::fixed << std::setprecision(0) << std::setw(5) << pars.position().perp()
      //<< " phi " << std::fixed << std::setprecision(3) << std::setw(6) << pars.position().phi()
	 << " z " << std::fixed << std::setprecision(0) << std::setw(6) << pars.position().z()
	 << " theta " << std::fixed << std::setprecision(5) << std::setw(7) << pars.momentum().theta()
	 << " phi " << std::fixed << std::setprecision(3) << std::setw(6) << pars.momentum().phi()
	 << " q*p(GeV) "   << std::scientific << std::setprecision(3) << std::setw(10) << pars.momentum().mag()*pars.charge()*1e-3
         << " pt(Gev) "    << std::scientific << std::setprecision(3) << std::setw(9) << pars.momentum().perp()*1e-3;

    return sout.str();
  }

  std::string MuonEDMPrinterTool::print( const Trk::ResidualPull& resPull ) const {
    std::ostringstream sout;
    
    const std::vector<double>& residual = resPull.residual(); 
    const std::vector<double>& pull = resPull.pull(); 
    for( unsigned int i=0;i<residual.size();++i ) {
      if( residual[i] != 999. && residual[i] != -999. ) sout << " residual " << std::setprecision(3) << std::setw(8) << residual[i] << "  ";
    }
    sout << " pull ";
    for( unsigned int i=0;i<pull.size();++i ) sout << std::setprecision(3) << std::setw(8) << pull[i] << "  ";
    return sout.str();
  }


  std::string MuonEDMPrinterTool::printData( const Trk::MeasurementBase& measurement ) const {
    std::ostringstream sout;

    // print position of hit
    double h_r = measurement.globalPosition().perp();
    double h_z = measurement.globalPosition().z();
    double h_phi = measurement.globalPosition().phi();
    double h_theta = measurement.globalPosition().theta();

    sout << "r "      << std::fixed << std::setprecision(0) << std::setw(5) << h_r
	 << " z "     << std::fixed << std::setprecision(0) << std::setw(5) << h_z 
	 << " phi "   << std::fixed << std::setprecision(3) << std::setw(4) << h_phi 
	 << " theta " << std::fixed << std::setprecision(3) << std::setw(4) << h_theta;

    // print measurement data
    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(&measurement);
    if ( rot ) {
      // add drift time for MDT
      const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(rot);
      if (mdt) {
        double error = std::sqrt(measurement.localCovariance()(0,0));
	sout << "  r_drift " << std::fixed << std::setprecision(2) << std::setw(5) << mdt->driftRadius() << " error " <<  std::fixed << std::setprecision(2) << std::setw(5) << error ;
      } else {
	// add time for RPC
	const RpcClusterOnTrack* rpc = dynamic_cast<const RpcClusterOnTrack*>(rot);
	if ( rpc ) {
	  const RpcPrepData* rpcPRD = rpc->prepRawData();
	  if( rpcPRD ) {
	    sout << "  time " << std::fixed << std::setprecision(2) << std::setw(5) << rpcPRD->time();
	  }
	}
      }
    } else { // !rot 
      // if we get here: not a ROT, maybe a CROT
      const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(&measurement);
      if( crot ){
	unsigned int nlayers = 0;
	unsigned int nhits = 0;
	std::set<Identifier> layers;
	std::vector<double> rpcTimes;
	const std::vector<const MuonClusterOnTrack*>& rots = crot->containedROTs();
	nhits = rots.size();
	rpcTimes.reserve(nhits);
	std::vector<const MuonClusterOnTrack*>::const_iterator itR = rots.begin(), itR_end = rots.end();
	for ( ; itR != itR_end; ++itR ) {
	  Identifier layerId = m_idHelper->layerId((*itR)->identify());
	  layers.insert(layerId);
	  const RpcClusterOnTrack* rpc = dynamic_cast<const RpcClusterOnTrack*>(*itR);
	  const RpcPrepData* rpcPRD = rpc ? rpc->prepRawData() : 0;
	  if( rpcPRD ) rpcTimes.push_back(rpcPRD->time());
	}
	nlayers = layers.size();
	sout << "  CompRot: hits " << nhits << " layers " << nlayers;
	// add time for RPC
	if (rpcTimes.size()) {
	  sout << (rpcTimes.size() == 1 ? "  time" : "  times") << std::fixed << std::setprecision(2);
	  std::vector<double>::iterator itD = rpcTimes.begin(), itD_end = rpcTimes.end();
	  for ( ; itD != itD_end; ++itD ) sout << " " << std::setw(5) << *itD;
	}
      } // if crot
    } // else !rot    

    return sout.str();
  } // printData( Trk::MeasurementBase )


  std::string MuonEDMPrinterTool::printId( const Trk::MeasurementBase& measurement ) const {
    std::string idStr;
    Identifier id = m_helper->getIdentifier( measurement );
    if( !id.is_valid() ) {
      const Trk::PseudoMeasurementOnTrack* pseudo = dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(&measurement);
      if( pseudo ) idStr = "pseudo measurement";
      else idStr = "no Identifier";
    }else if( !m_idHelper->mdtIdHelper().is_muon(id) ) {
      idStr = "Id hit"; 
    }else{
      idStr = m_idHelper->toString( id );
    }

    return idStr;
  }
} // namespace Muon
