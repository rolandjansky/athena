/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// This class's header
#include "PrimaryDPDMaker/CaloTimeFilterTool.h"
#include <float.h>

CaloTimeFilterTool::CaloTimeFilterTool( const std::string& tool_type) : 
       asg::AsgTool( tool_type ){
}

StatusCode CaloTimeFilterTool::getTimeDifference(TimingFilterInformation& time_info, const SG::ReadHandleKey<LArCollisionTime>& read_key,
                                                 const EventContext& ctx) const {
    ATH_MSG_DEBUG("CaloTimeFilterTool::getTimeDifference()");

    SG::ReadHandle<LArCollisionTime> readHandle{read_key,ctx};
    if (!readHandle.isValid()){
        ATH_MSG_FATAL("Failed to retrieve "<<read_key);
        return StatusCode::FAILURE;
    }

   fillTimeDifference(time_info, readHandle.cptr());
   return StatusCode::SUCCESS;
}  

StatusCode CaloTimeFilterTool::getTimeDifference(TimingFilterInformation& time_info){
    const LArCollisionTime* larCollisionTime = nullptr;
    ATH_CHECK(evtStore()->retrieve(larCollisionTime, m_containerName));
    fillTimeDifference(time_info, larCollisionTime);
    return StatusCode::SUCCESS;
} 
void CaloTimeFilterTool::fillTimeDifference(TimingFilterInformation& time_info, const LArCollisionTime* larCollisionTime ) const{   

   time_info.ncellA    = larCollisionTime->ncellA();
   time_info.ncellC    = larCollisionTime->ncellC();
   time_info.timeA     = larCollisionTime->timeA();
   time_info.timeC     = larCollisionTime->timeC();


   time_info.timeDiff = FLT_MAX;
   if ( time_info.ncellA > m_mincellsperside && time_info.ncellC > m_mincellsperside ) { time_info.timeDiff = (time_info.timeA-time_info.timeC); }
   
   time_info.passCut = ( std::abs(time_info.timeDiff) < m_timeCut );
}