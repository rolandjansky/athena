/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPrepRawDataProviderTools/MuonPrepRawDataCollectionProviderTool.h"
#include "MuonIdHelpers/MuonStationIndexHelpers.h"

namespace Muon {

  MuonPrepRawDataCollectionProviderTool::MuonPrepRawDataCollectionProviderTool(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type,name,parent) {
    declareInterface<MuonPrepRawDataCollectionProviderTool>(this);
    m_locations.resize(MuonStationIndex::TechnologyIndexMax);
    m_locations[MuonStationIndex::MDT] = "MDT_DriftCircles";
    m_locations[MuonStationIndex::RPC] = "RPC_Measurements";
    m_locations[MuonStationIndex::TGC] = "TGC_Measurements";
    m_locations[MuonStationIndex::CSCI] = "CSC_Clusters";
    m_locations[MuonStationIndex::STGC] = "sTGC_Clusters";
    m_locations[MuonStationIndex::MM] = "MM_Clusters";
    for( const auto& entry : m_locations ){
      ATH_MSG_DEBUG(" location " << entry);
    }
  }

  StatusCode MuonPrepRawDataCollectionProviderTool::initialize() {

    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_layerHashProvider.retrieve());

    return StatusCode::SUCCESS;
  }

  bool MuonPrepRawDataCollectionProviderTool::getLayerData( int sector, MuonStationIndex::DetectorRegionIndex regionIndex, 
                                                            MuonStationIndex::LayerIndex layerIndex, MuonLayerPrepRawData& layerPrepRawData ) const {
    
    // get technologies in the given layer
    MuonStationIndex::StIndex stIndex = MuonStationIndex::toStationIndex( regionIndex, layerIndex );
    std::vector<MuonStationIndex::TechnologyIndex> technologiesInStation = MuonStationIndexHelpers::technologiesInStation( stIndex );
    std::string techString;
    for( auto tech : technologiesInStation ) techString += " " + MuonStationIndex::technologyName(tech);
    ATH_MSG_DEBUG("getLayerData: sector " << sector << " " << MuonStationIndex::regionName(regionIndex) << " " << MuonStationIndex::layerName(layerIndex)
                  << " technologies " << techString);

    bool success = true;
    // loop over technologies and get data 
    for( std::vector<MuonStationIndex::TechnologyIndex>::iterator it=technologiesInStation.begin();it!=technologiesInStation.end();++it ){

      // get collections, keep track of failures
      if( *it == MuonStationIndex::MDT )       success = !getLayerData<MdtPrepDataCollection>(sector,*it,regionIndex,layerIndex,layerPrepRawData.mdts)   ? false : success;
      else if( *it == MuonStationIndex::RPC )  success = !getLayerData<RpcPrepDataCollection>(sector,*it,regionIndex,layerIndex,layerPrepRawData.rpcs)   ? false : success;
      else if( *it == MuonStationIndex::TGC )  success = !getLayerData<TgcPrepDataCollection>(sector,*it,regionIndex,layerIndex,layerPrepRawData.tgcs)   ? false : success;
      else if( *it == MuonStationIndex::CSCI ) success = !getLayerData<CscPrepDataCollection>(sector,*it,regionIndex,layerIndex,layerPrepRawData.cscs)   ? false : success;
      else if( *it == MuonStationIndex::STGC ) success = !getLayerData<sTgcPrepDataCollection>(sector,*it,regionIndex,layerIndex,layerPrepRawData.stgcs) ? false : success;
      else if( *it == MuonStationIndex::MM )   success = !getLayerData<MMPrepDataCollection>(sector,*it,regionIndex,layerIndex,layerPrepRawData.mms)     ? false : success;
    }

    if( msgLvl(MSG::DEBUG) ){
      msg(MSG::DEBUG) << " Got data: sector " << sector << " " << MuonStationIndex::regionName(regionIndex) 
                      << " " << MuonStationIndex::layerName(layerIndex);
      if( !layerPrepRawData.mdts.empty() )  msg(MSG::DEBUG) << " MDTs " << layerPrepRawData.mdts.size();
      if( !layerPrepRawData.rpcs.empty() )  msg(MSG::DEBUG) << " RPCs "  << layerPrepRawData.rpcs.size();
      if( !layerPrepRawData.tgcs.empty() )  msg(MSG::DEBUG) << " TGCs "  << layerPrepRawData.tgcs.size();
      if( !layerPrepRawData.cscs.empty() )  msg(MSG::DEBUG) << " CSCs "  << layerPrepRawData.cscs.size();
      if( !layerPrepRawData.stgcs.empty() ) msg(MSG::DEBUG) << " STGCs "  << layerPrepRawData.stgcs.size();
      if( !layerPrepRawData.mms.empty() )   msg(MSG::DEBUG) << " MMs "  << layerPrepRawData.mms.size();
      msg(MSG::DEBUG) << endmsg;
    }
    return success;
  }


}
 
