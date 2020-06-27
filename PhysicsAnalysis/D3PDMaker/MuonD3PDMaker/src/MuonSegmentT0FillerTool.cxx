/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentT0FillerTool.h"
#include "TrkSegment/Segment.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {


MuonSegmentT0FillerTool::MuonSegmentT0FillerTool (const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent)
  : BlockFillerTool<Trk::Segment> (type, name, parent) {
  declareProperty("doMuonBoyCSCTiming", m_doMuonBoyCSCTiming=false);

  book().ignore(); // Avoid coverity warnings.
}

StatusCode MuonSegmentT0FillerTool::initialize()
{
  CHECK( BlockFillerTool<Trk::Segment>::initialize() );
  CHECK( m_edmHelperSvc.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode MuonSegmentT0FillerTool::book()
{
  CHECK( addVariable ("t0",             m_t0)  );
  CHECK( addVariable ("t0err",             m_t0err)  );
  return StatusCode::SUCCESS;
}


StatusCode MuonSegmentT0FillerTool::fill (const Trk::Segment& ts) {
  const Muon::MuonSegment& mSeg = dynamic_cast<const Muon::MuonSegment&> (ts);
  float t0=0, t0err=0;
  t0 = mSeg.time();
  t0err = mSeg.errorTime();
  if(m_doMuonBoyCSCTiming) MuonboyT0CSCSegment(&mSeg, t0, t0err);

  *m_t0 = t0;
  *m_t0err = t0err;

  return StatusCode::SUCCESS;
}

// CSC timing for muonboy
// Code from: MboyEDMTool-07-83-41/src//MuonboyToSegmentTool.cxx

void MuonSegmentT0FillerTool::MuonboyT0CSCSegment(const Muon::MuonSegment* pMuonSegment, float& t0, float& t0Error) {
   t0      = pMuonSegment->time()      ;
   t0Error = pMuonSegment->errorTime() ;
   
   int   DoAlternative = 0 ;
   int   Alternative_ThereIsaCscTimeEarly  = 0 ;
   int   Alternative_ThereIsaCscTimeLate   = 0 ;
   float Alternative_LatestEarlyTime  = 0. ;
   float Alternative_EarliestLateTime = 0. ;
   float Alternative_counter = 0. ;
   float Alternative_t0      = 0. ;
   float Alternative_t0sqr   = 0. ;
   float Alternative_t0Error = 0. ;
   for(unsigned int irot=0;irot<pMuonSegment->numberOfContainedROTs();irot++){
     const Trk::RIO_OnTrack* pRIO_OnTrack=pMuonSegment->rioOnTrack(irot);
     if ( (pRIO_OnTrack) ){
       const Trk::PrepRawData* pPrepRawData =(pRIO_OnTrack)->prepRawData () ;
       if ( pPrepRawData ){
         const Muon::CscPrepData* pPrepData = dynamic_cast<const Muon::CscPrepData*>(pPrepRawData);
         if ( pPrepData ){
           DoAlternative = 1 ;
           double Time = pPrepData->time() ;
           if (pPrepData->timeStatus() == Muon::CscTimeSuccess){
             Alternative_counter = Alternative_counter + 1. ;
             Alternative_t0    = Alternative_t0    + Time      ;
             Alternative_t0sqr = Alternative_t0sqr + Time*Time ;
           }
           if (pPrepData->timeStatus() == Muon::CscTimeEarly) {
             if ( Alternative_ThereIsaCscTimeEarly == 0 ) Alternative_LatestEarlyTime = Time ;
             if ( Time >= Alternative_LatestEarlyTime   ) Alternative_LatestEarlyTime = Time ; 
             Alternative_ThereIsaCscTimeEarly = 1 ;
           }
           if (pPrepData->timeStatus() == Muon::CscTimeLate ) {
             if ( Alternative_ThereIsaCscTimeLate == 0 ) Alternative_EarliestLateTime = Time ;
             if ( Time <= Alternative_EarliestLateTime ) Alternative_EarliestLateTime = Time ; 
             Alternative_ThereIsaCscTimeLate  = 1 ;
           }
         }
       }
     }
   }
   
   if (DoAlternative==1){ 
     if (Alternative_counter){
       const float inv_Alternative_counter = 1. / Alternative_counter;
       Alternative_t0    =  Alternative_t0   * inv_Alternative_counter;
       Alternative_t0sqr =  Alternative_t0sqr * inv_Alternative_counter;
       Alternative_t0Error =  Alternative_t0sqr - Alternative_t0*Alternative_t0 ;
       if (Alternative_t0Error < 0.){
         Alternative_t0Error = 0. ;
       }else{
         Alternative_t0Error = sqrt(Alternative_t0Error) ;
       }
     }else if (Alternative_ThereIsaCscTimeEarly == 1 && Alternative_ThereIsaCscTimeLate == 0){
       Alternative_t0      = Alternative_LatestEarlyTime       ;
       Alternative_t0Error = fabs(Alternative_LatestEarlyTime) ;
     }else if (Alternative_ThereIsaCscTimeEarly == 0 && Alternative_ThereIsaCscTimeLate == 1){
       Alternative_t0      = Alternative_EarliestLateTime ;
       Alternative_t0Error = Alternative_EarliestLateTime ;
     }else{
       Alternative_t0      = 99999. ;
       Alternative_t0Error = 99999. ;
     }
     t0      = Alternative_t0      ;
     t0Error = Alternative_t0Error ;
   }
}


} // namespace D3PD
