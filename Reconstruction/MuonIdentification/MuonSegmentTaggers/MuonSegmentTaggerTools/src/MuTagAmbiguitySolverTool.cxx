/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuTagAmbiguitySolverTool.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentQuality.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "MuonSegmentMakerUtils/MuonSegmentKey.h"
#include "MuonSegmentMakerUtils/CompareMuonSegmentKeys.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentMatchingTool.h"
#include "TrkTrack/Track.h"
#include "MuonCombinedEvent/MuonSegmentInfo.h"

MuTagAmbiguitySolverTool::MuTagAmbiguitySolverTool(const std::string& t, 
				     const std::string& n,
				     const IInterface*  p ):
  AthAlgTool(t,n,p),
  p_muonPrinter("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
  p_segmentMatchingTool("Muon::MuonSegmentMatchingTool/MuonSegmentMatchingTool")
{
  declareInterface<IMuTagAmbiguitySolverTool>(this);
  declareProperty("MuonSegmentMatchingTool" , p_segmentMatchingTool ) ;
  declareProperty("DoHitOverlapMatching", m_hitOverlapMatching = true );
  declareProperty("ResolveSLOverlaps",   m_slOverlapMatching = false );
  declareProperty("RejectOuterEndcap",   m_rejectOuterEndcap = true );
  declareProperty("RejectMatchPhi",   m_rejectMatchPhi = true );
}

StatusCode MuTagAmbiguitySolverTool::initialize()
{
  ATH_CHECK( AthAlgTool::initialize() ); 
  
  ATH_MSG_INFO( "================================" );
  ATH_MSG_INFO( "=Proprieties are " );
  ATH_MSG_INFO( "================================" );

  ATH_CHECK( m_edmHelperSvc.retrieve() );
  ATH_CHECK( p_muonPrinter.retrieve() );
  ATH_CHECK( m_idHelperSvc.retrieve() );
  ATH_CHECK( p_segmentMatchingTool.retrieve() );
  
  return StatusCode::SUCCESS;

}

std::vector<  MuonCombined::MuonSegmentInfo >  MuTagAmbiguitySolverTool::solveAmbiguities( std::vector<  MuonCombined::MuonSegmentInfo > mtos ) const {
	ATH_MSG_DEBUG(  "mtos size before any cuts " << mtos.size() );
  // Store the number of segments associated to one track (pointer) 
    for( unsigned int ns1 = 0; ns1 < mtos.size(); ns1++){
      ATH_MSG_DEBUG(  " index " << ns1 << " nsegments " << mtos[ns1].nsegments << " track pointer " << mtos[ns1].track << " stationLayer " <<  mtos[ns1].stationLayer << " selected " << mtos[ns1].selected );
      int nsegments = 1;
      if(mtos[ns1].selected==2) continue;
      for( unsigned int ns2 = ns1+1; ns2 < mtos.size(); ns2++){
        if(mtos[ns1].track!=mtos[ns2].track) break;
        nsegments++;
      }
      ATH_MSG_DEBUG(  " ns1 " << ns1 << " nsegments " << nsegments );
 
      for( unsigned int ns2 = ns1;  ns2 < mtos.size(); ns2++){
          if(mtos[ns1].track!=mtos[ns2].track) break;
	  mtos[ns2].nsegments = nsegments;
          mtos[ns2].selected=2;
      }
      ATH_MSG_DEBUG(  " Updated index " << ns1 << " nsegments " << mtos[ns1].nsegments << " track pointer " << mtos[ns1].track << " stationLayer " <<  mtos[ns1].stationLayer << " selected " << mtos[ns1].selected );
    }
    
    // drop CSC single segment 
   for( unsigned int ns1 = 0;  ns1 < mtos.size(); ns1++){
      if(mtos[ns1].selected==2) mtos[ns1].selected=1;
      if(mtos[ns1].nsegments==1 && mtos[ns1].stationLayer==21){
	mtos[ns1].selected=0; 
        mtos[ns1].nsegments=0;
      }
    }

   // solve ambiguous segments
    for( unsigned int ns1 = 0;  ns1 < mtos.size(); ns1++){
       ATH_MSG_DEBUG(  " First pass  index " << ns1 << " nsegments " << mtos[ns1].nsegments << " track pointer " << mtos[ns1].track << " stationLayer " <<  mtos[ns1].stationLayer << " selected " << mtos[ns1].selected );
     
 
      for( unsigned int ns2 = ns1+1;  ns2 < mtos.size(); ns2++){
        if(mtos[ns1].segment==mtos[ns2].segment || ambiguousSegment(*mtos[ns1].segment,*mtos[ns2].segment)) {
	  double R1 = fabs(mtos[ns1].pullCY*Rseg(mtos[ns1].nsegments));
	  double R2 = fabs(mtos[ns2].pullCY*Rseg(mtos[ns2].nsegments));
          ATH_MSG_DEBUG(" Ambiguous segment at index " << ns1 << " and " << ns2 ); 
	  if(R1>R2) {
	    mtos[ns1].selected=0;
	    mtos[ns1].nsegments=0;
	  }
	  else {
	    mtos[ns2].selected=0;
	    mtos[ns2].nsegments=0;
	  }  
	}
      }
    }

    //update nsegments
    for( unsigned int ns1 = 0;  ns1 < mtos.size(); ns1++){
      int nsegments = 1;
      if(mtos[ns1].selected==0) nsegments = 0;
      if(mtos[ns1].selected==2) continue;
      for( unsigned int ns2 = ns1+1;  ns2 < mtos.size(); ns2++){
        if(mtos[ns1].track!=mtos[ns2].track) break;
        if( mtos[ns2].selected!=0) nsegments++;
      }
      for( unsigned int ns2 = ns1;  ns2 < mtos.size(); ns2++){
          if(mtos[ns1].track!=mtos[ns2].track) break;
	  mtos[ns2].nsegments = nsegments;
          mtos[ns2].selected=2;
      }	
      ATH_MSG_DEBUG(  " Second pass index " << ns1 << " nsegments " << mtos[ns1].nsegments << " track pointer " << mtos[ns1].track << " stationLayer " <<  mtos[ns1].stationLayer << " selected " << mtos[ns1].selected );
      ATH_MSG_DEBUG(  " ns1 " << ns1 << " nsegments " << nsegments );
    }

    // update drop CSC single segment 
   for( unsigned int ns1 = 0;  ns1 < mtos.size(); ns1++){
      if(mtos[ns1].selected==2) mtos[ns1].selected=1;
      if(mtos[ns1].nsegments==1 && mtos[ns1].stationLayer==21){
	mtos[ns1].selected=0; 
        mtos[ns1].nsegments=0;
        ATH_MSG_DEBUG(  " DROP CSC index " << ns1 << " nsegments " << mtos[ns1].nsegments << " track pointer " << mtos[ns1].track << " stationLayer" <<  mtos[ns1].stationLayer << " selected " << mtos[ns1].selected );
      }
    }

   // drop single tag with too many holes
    for( unsigned int ns1 = 0;  ns1 < mtos.size(); ns1++){
      if(mtos[ns1].nsegments==1 && mtos[ns1].nholes>2){
	mtos[ns1].selected=0; 
        mtos[ns1].nsegments=0;
        ATH_MSG_DEBUG(  " DROP HOLES index " << ns1 << " nsegments " << mtos[ns1].nsegments << " track pointer " << mtos[ns1].track << " stationLayer" <<  mtos[ns1].stationLayer << " selected " << mtos[ns1].selected );
      }
    }  

    // drop single tag without phi match
    if( m_rejectMatchPhi) {
      for( unsigned int ns1 = 0;  ns1 < mtos.size(); ns1++){
        if(mtos[ns1].nsegments==1 && mtos[ns1].minimumPullPhi>3 && mtos[ns1].hasPhi>0){
  	  mtos[ns1].selected=0; 
          mtos[ns1].nsegments=0;
        ATH_MSG_DEBUG(  " DROP MatchPhi index " << ns1 << " nsegments " << mtos[ns1].nsegments << " track pointer " << mtos[ns1].track << " stationLayer" <<  mtos[ns1].stationLayer << " selected " << mtos[ns1].selected );
        }
      }
    }

    int multiplicity = mtos.size();

    // drop segments as function of multiplicity and holes
    if(multiplicity>50) {
      for( unsigned int ns1 = 0;  ns1 < mtos.size(); ns1++){
        if(mtos[ns1].nholes>1){ 
  	  mtos[ns1].selected=0; 
          mtos[ns1].nsegments=0;
          ATH_MSG_DEBUG(  " DROP multiplicity 50 index " << ns1 << " nsegments " << mtos[ns1].nsegments << " track pointer " << mtos[ns1].track << " stationLayer" <<  mtos[ns1].stationLayer << " selected " << mtos[ns1].selected );
        }
      }  
    } else if(multiplicity>30) {
     for( unsigned int ns1 = 0;  ns1 < mtos.size(); ns1++){
        if(mtos[ns1].nholes>2){ 
  	  mtos[ns1].selected=0; 
          mtos[ns1].nsegments=0;
          ATH_MSG_DEBUG(  " DROP multiplicity 30 index " << ns1 << " nsegments " << mtos[ns1].nsegments << " track pointer " << mtos[ns1].track << " stationLayer" <<  mtos[ns1].stationLayer << " selected " << mtos[ns1].selected );
        }
      }  
    }

    // drop single multilayers for single tag
   for( unsigned int ns1 = 0;  ns1 < mtos.size(); ns1++){
      if(mtos[ns1].nsegments==1 && mtos[ns1].nholes>0 && mtos[ns1].singleML==1){
	mtos[ns1].selected=0; 
        mtos[ns1].nsegments=0;
        ATH_MSG_DEBUG(  " DROP single multilayers single tag " << ns1 << " nsegments " << mtos[ns1].nsegments << " track pointer " << mtos[ns1].track << " stationLayer" <<  mtos[ns1].stationLayer << " selected " << mtos[ns1].selected );
      }
    }


   // drop single multilayer segment for below pT cut
   double pTmin(2000.);
   double pTmax(10000.);
   double pTcut = pTmin;
   if( multiplicity > 100 )      pTcut = pTmax ;
   else if( multiplicity < 10 )  pTcut = pTmin ; 
   else                          pTcut = 2*( ( pTmax - pTmin ) / 90. ) * multiplicity ;
 

   for( unsigned int ns1 = 0;  ns1 < mtos.size(); ns1++){
     const Trk::Perigee* perigee =  (mtos[ns1].track)->perigeeParameters();
     double sintheta = sin( perigee->parameters()[Trk::theta] );
     double OneOverP = fabs(perigee->parameters()[Trk::qOverP]);
     double pT = 0.0;
     if(OneOverP>0.0)
       pT = sintheta/OneOverP;
     else
       pT = sqrt(perigee->momentum()[Amg::px]*perigee->momentum()[Amg::px] +
		 perigee->momentum()[Amg::py]*perigee->momentum()[Amg::py]);
     if(pT<pTcut) {
       if(mtos[ns1].nsegments==1 && mtos[ns1].singleML==1){
	 mtos[ns1].selected=0; 
	 mtos[ns1].nsegments=0;
         ATH_MSG_DEBUG(  " DROP single multilayers low pT " << ns1 << " nsegments " << mtos[ns1].nsegments << " track pointer " << mtos[ns1].track << " stationLayer" <<  mtos[ns1].stationLayer << " selected " << mtos[ns1].selected );
       }
     }
   }


   // drop if single tag outer endcap
   if( m_rejectOuterEndcap) {
   for( unsigned int ns1 = 0;  ns1 < mtos.size(); ns1++){
      if(mtos[ns1].nsegments==1 && mtos[ns1].stationLayer==13){
	mtos[ns1].selected=0; 
        mtos[ns1].nsegments=0;
        ATH_MSG_DEBUG(  " DROP single tag OuterEndcap " << ns1 << " nsegments " << mtos[ns1].nsegments << " track pointer " << mtos[ns1].track << " stationLayer" <<  mtos[ns1].stationLayer << " selected " << mtos[ns1].selected );
      }
    }     
   }

   std::vector<  MuonCombined::MuonSegmentInfo > mtosOutput;
   mtosOutput.reserve(mtos.size());

   for( unsigned int ns1 = 0;  ns1 < mtos.size(); ns1++){
     if(mtos[ns1].nsegments>0){
       mtosOutput.push_back(mtos[ns1]);
     }
   }

   ATH_MSG_DEBUG(  "mtos size after all cuts " << mtos.size() );

  return mtosOutput;
}

int MuTagAmbiguitySolverTool::ambiguousSegment( const Muon::MuonSegment& seg1, const Muon::MuonSegment& seg2 ) const {

  // first check pointer
  if( &seg1 == &seg2 ) return 1;

  // check whether the segments are in the same station layer
  Identifier ch1 = m_edmHelperSvc->chamberId(seg1);
  Identifier ch2 = m_edmHelperSvc->chamberId(seg2);
  Muon::MuonStationIndex::StIndex st1 = m_idHelperSvc->stationIndex(ch1);
  Muon::MuonStationIndex::StIndex st2 = m_idHelperSvc->stationIndex(ch2);
  if( st1 != st2 ) return 0;

  // check whether the segments are in the same chamber layer (small/large)
  Muon::MuonStationIndex::ChIndex chI1 = m_idHelperSvc->chamberIndex(ch1);
  Muon::MuonStationIndex::ChIndex chI2 = m_idHelperSvc->chamberIndex(ch2);
  if( chI1 != chI2 ){

    // only match if segments both MDT or both CSC
    if( m_idHelperSvc->isMdt(ch1) ==  m_idHelperSvc->isMdt(ch2) ) {
      
      // make sure segments are in same sector or neighbouring one
      int sector1 = m_idHelperSvc->sector(ch1);
      int sector2 = m_idHelperSvc->sector(ch2);
      bool sectorOk = false;
      if( sector1 == sector2 ) sectorOk = true;
      if( abs(sector1-sector2) == 1 ) sectorOk = true;
      if( (sector1 == 16 && sector2 == 1) || (sector1 == 1 && sector2 == 16) ) sectorOk = true;
      
      if( sectorOk ){
	// check whether the two segments actually belong to the same particle    
	bool match = p_segmentMatchingTool->match(seg1,seg2);
	if( match ){
	  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " found matching segment pair: " << std::endl
						      << p_muonPrinter->print(seg1) << std::endl
						      << p_muonPrinter->print(seg2) << endmsg;
	  
	  // if overlap matching enabled flag as ambiguous
	  if( m_slOverlapMatching ) return 2;
	}
      }
    }
  }else{

    // the segments are in the same chamber, calculate hit overlap
    Muon::MuonSegmentKey key1;
    key1.calculateKeys(seg1.containedMeasurements(),2); // the '2' means that the code will ignore the sign of the drift radii
    Muon::MuonSegmentKey key2;
    key2.calculateKeys(seg2.containedMeasurements(),2);
    
    Muon::CompareMuonSegmentKeys compareSegments;
    Muon::CompareMuonSegmentKeys::OverlapResult result = compareSegments(key1,key2);
    
    if( compareSegments.intersectionSize > 0 ){
      
      // if hit overlap matching enables flag as ambiguous
      if( m_hitOverlapMatching ){
	if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " found overlapping segment pair: " << compareSegments.print(result) << std::endl
						    << p_muonPrinter->print(seg1) << std::endl
						    << p_muonPrinter->print(seg2) << endmsg;
	return 3;
      }
    }
  }


  return 0;
}


std::vector<  MuonCombined::MuonSegmentInfo > MuTagAmbiguitySolverTool::selectBestMuTaggedSegments( std::vector<  MuonCombined::MuonSegmentInfo > mtss ) const {
  ATH_MSG_DEBUG( "cleaning set of MTSs" );
  
  std::vector<  MuonCombined::MuonSegmentInfo > outputMTSs;
  std::vector< bool > accept( mtss.size(), true );
  for( unsigned int mts1=0; mts1 < mtss.size() ; ++mts1 ){
    const Muon::MuonSegment* museg1 = mtss[mts1].segment;
    if( !museg1 ){
      ATH_MSG_DEBUG( "MTO doesn't hold a Muon::MuonSegment" );
      continue;
    } 

    Identifier ch1 = m_edmHelperSvc->chamberId(*museg1);
    Muon::MuonStationIndex::StIndex st1 = m_idHelperSvc->stationIndex(ch1);
    int eta1 = m_idHelperSvc->stationEta(ch1);

    
    for( unsigned int mts2=mts1+1; mts2 < mtss.size() ; ++mts2 ){
      const Muon::MuonSegment* museg2 = mtss[mts2].segment;
      if( !museg2 ){
	ATH_MSG_DEBUG(  "MTO doesn't hold a Muon::MuonSegment" );
	continue;
      } 

      Identifier ch2 = m_edmHelperSvc->chamberId(*museg2);
      Muon::MuonStationIndex::StIndex st2 = m_idHelperSvc->stationIndex(ch2);    
      
      if( st1 != st2 ) continue;
      int eta2 = m_idHelperSvc->stationEta(ch2);
      if( eta1 != eta2 ) continue;

      if(ambiguousSegment(*museg1,*museg2)) {
        if(museg1->numberOfContainedROTs()>museg2->numberOfContainedROTs()-1) {
          ATH_MSG_DEBUG( "segments in the same station with rots keep First " << museg1->numberOfContainedROTs() << " and " << museg2->numberOfContainedROTs());
          accept[mts2] = false;
        } else if (museg1->numberOfContainedROTs()<museg2->numberOfContainedROTs()-1) {
          ATH_MSG_DEBUG( "segments in the same station with rots keep Second " << museg1->numberOfContainedROTs() << " and " << museg2->numberOfContainedROTs());
          accept[mts1] = false;
        } else { 
          double chi2mts1 = fabs(mtss[mts1].pullCY);
          double  chi2mts2 = fabs(mtss[mts2].pullCY);
          ATH_MSG_DEBUG( "segments in the same station with MatchLocY " << chi2mts1 << " and " <<chi2mts2 );
          if( fabs(chi2mts1) < fabs(chi2mts2) ){
            ATH_MSG_DEBUG( "dropping mts " << mts2 << " since he has a larger MatchLocY: " << chi2mts2 << " vs " << chi2mts1 );
            accept[mts2] = false;
          }
          if( fabs(chi2mts1) > fabs(chi2mts2) ){
            ATH_MSG_DEBUG( "dropping mts " << mts1 << " since he has a larger MatchLocY: " << chi2mts1 << " vs " << chi2mts2 );
            accept[mts1] = false;
          }
        }
      }
    }
    if( accept[mts1] ) outputMTSs.push_back( mtss[mts1] ) ;
  }
  return outputMTSs;
}

double  MuTagAmbiguitySolverTool::Rseg( unsigned int nseg ) const {
  const float a_seg(3.61883);
  const float b_seg(20.4547);
  const float c_seg(1./0.132675);
  const float d_seg(0.102262);
  return  static_cast<double>(a_seg / ( 1. + exp( b_seg - nseg*c_seg ) ) + d_seg);
}
