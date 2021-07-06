/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PrimaryDPDMaker/MBTSTimeFilterTool.h"
#include "TileEvent/TileContainer.h"
#include <float.h> 
namespace {
    constexpr float tile_eta_start = 1.5;
}


MBTSTimeFilterTool::MBTSTimeFilterTool( const std::string& tool_name) : 
    asg::AsgTool( tool_name) {}

StatusCode MBTSTimeFilterTool::initialize(){
  ATH_MSG_DEBUG ("initialize()");
  ATH_CHECK(detStore()->retrieve(m_tileTBID));
  return StatusCode::SUCCESS;
}


StatusCode MBTSTimeFilterTool::getTimeDifference(TimingFilterInformation& time_info) {
   const TileCellContainer* tileCellCnt = nullptr;
   ATH_CHECK(evtStore()->retrieve(tileCellCnt,m_mbtsContainerName));
   fillTimeDifference(time_info,tileCellCnt);
   return StatusCode::SUCCESS;
}
StatusCode MBTSTimeFilterTool::getTimeDifference(TimingFilterInformation& time_info,
                                        const SG::ReadHandleKey<TileCellContainer>& key,
                                        const EventContext& ctx) const{ 
  ATH_MSG_DEBUG ("execute()");
   SG::ReadHandle<TileCellContainer> readHandle{key,ctx};
   if (!readHandle.isValid()){
      ATH_MSG_FATAL (key <<" requested but not found.");
      return StatusCode::FAILURE;
   }
    fillTimeDifference(time_info, readHandle.cptr());
    return StatusCode::SUCCESS;
}
void MBTSTimeFilterTool::fillTimeDifference(TimingFilterInformation& time_info, const TileCellContainer* tileCellCnt) const{
 
  for(const TileCell* tile : *tileCellCnt) {

    // check charge passes threshold
    const float charge = tile->energy();
    ATH_MSG_DEBUG ("Energy =" << charge << "pC");
    if(charge < m_chargethreshold) continue;

    // only endcap 
    const  float eta = tile->eta();
    ATH_MSG_DEBUG ("Eta =" << eta);
    if ( std::abs(eta) < tile_eta_start) continue;

    const Identifier id= tile->ID();
    // cache type, module and channel
    // MBTS Id type is  "side"  +/- 1
    const int type_id = m_tileTBID->type(id);
    // MBTS Id module is "phi"  0-7
    const int module_id = m_tileTBID->module(id);
    // MBTS Id channel is "eta"  0-1   zero is closer to beam pipe
    const  int channel_id = m_tileTBID->channel(id);

    // Catch errors
    if( std::abs(type_id) != 1 ){
      ATH_MSG_WARNING ("MBTS identifier type is out of range.");
      continue;
    }
    if( channel_id < 0 || channel_id > 1 ){
      ATH_MSG_WARNING ("MBTS identifier channel is out of range.");
      continue;
    }
    if( module_id < 0 || module_id > 7 ){
      ATH_MSG_WARNING ("MBTS identifier module is out of range.");
      continue;
    }

    if (type_id > 0)  {
      time_info.timeA +=  tile->time();
      ++time_info.ncellA ; 
    } else {     
      time_info.timeC +=  tile->time();
      ++time_info.ncellC;
    }
  }

  if(time_info.ncellA + time_info.ncellC > 32)
    ATH_MSG_WARNING ("There shoule be <=32 MBTS TileCells  " << (time_info.ncellA + time_info.ncellC) << " were found.");

  if ( time_info.ncellA > 0 ) time_info.timeA /= time_info.ncellA;
  if ( time_info.ncellC > 0 ) time_info.timeC /= time_info.ncellC;

  time_info.timeDiff = FLT_MAX; // Or whatever default you want
  if ( time_info.ncellA >= m_minhitsperside && time_info.ncellC >= m_minhitsperside ) { time_info.timeDiff = (time_info.timeA - time_info.timeC); }
  time_info.passCut = ( std::abs(time_info.timeDiff) < m_maxtimediff);

}
