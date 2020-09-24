/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPrepRawDataProviderTools/MuonLayerHashProviderTool.h"


#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

#include "MuonDetDescrUtils/MuonSectorMapping.h"

namespace Muon {

  MuonLayerHashProviderTool::MuonLayerHashProviderTool(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_ntechnologies(4)
  {
    declareInterface<MuonLayerHashProviderTool>(this);

  }

  StatusCode MuonLayerHashProviderTool::initialize() {

    ATH_CHECK(m_idHelperSvc.retrieve());

    if( !initializeSectorMapping() ){
      ATH_MSG_ERROR("Failed to initialize sector mapping");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }


  void MuonLayerHashProviderTool::insertHash( const IdentifierHash& hash, const Identifier& id ) {
    insertHash(m_idHelperSvc->sector(id),hash,id);
  }

  void MuonLayerHashProviderTool::insertHash( int sector, const IdentifierHash& hash, const Identifier& id ) {
    MuonStationIndex::TechnologyIndex techIndex = m_idHelperSvc->technologyIndex(id);
    int sectorLayerHash = MuonStationIndex::sectorLayerHash(m_idHelperSvc->regionIndex(id),m_idHelperSvc->layerIndex(id));
    m_regionHashesPerSector[sector-1].technologyRegionHashVecs[techIndex][sectorLayerHash].push_back(hash);
  }

  void MuonLayerHashProviderTool::insertTechnology( const MuonIdHelper& idHelper ){

    // loop over all available STGC collection identifiers and order them per sector
    MuonIdHelper::const_id_iterator it = idHelper.module_begin();
    MuonIdHelper::const_id_iterator it_end = idHelper.module_end();
    for( ;it!=it_end; ++it ){
      IdentifierHash hash;
      idHelper.get_module_hash(*it,hash);
      insertHash(hash,*it);
    }

  }

  bool MuonLayerHashProviderTool::insertTgcs(){
    
    // the tgc's can be in multiple sectors so we need to do something special here
    const MuonGM::MuonDetectorManager* detMgr;
    if( detStore()->retrieve( detMgr ).isFailure() || !detMgr ){
      ATH_MSG_ERROR("Failed to initialize detector manager" );
      return false;
    }

    MuonSectorMapping sectorMapping;

    // loop over all available TGC collection identifiers and order them per sector
    MuonIdHelper::const_id_iterator it = m_idHelperSvc->tgcIdHelper().module_begin();
    MuonIdHelper::const_id_iterator it_end = m_idHelperSvc->tgcIdHelper().module_end();
    for( ;it!=it_end; ++it ){
      const MuonGM::TgcReadoutElement* detEl = detMgr->getTgcReadoutElement(*it);
      if( !detEl ) {
        ATH_MSG_DEBUG(" No detector element found for " << m_idHelperSvc->toString(*it) );
        continue;
      }
      IdentifierHash hash;
      m_idHelperSvc->tgcIdHelper().get_module_hash(*it,hash);
      int nstrips = detEl->getNStrips(1);
      Amg::Vector3D p1 = detEl->channelPos(1,1,1);
      Amg::Vector3D p2 = detEl->channelPos(1,1,nstrips);
      std::vector<int> sectors1;
      sectorMapping.getSectors(p1.phi(),sectors1);
      std::set<int> added;
      std::vector<int>::iterator sit = sectors1.begin();
      std::vector<int>::iterator sit_end = sectors1.end();
      for( ;sit!=sit_end; ++sit ){
        insertHash(*sit,hash,*it);
        added.insert(*sit);
      }

      std::vector<int> sectors2;
      sectorMapping.getSectors(p2.phi(),sectors2);
      sit = sectors2.begin();
      sit_end = sectors2.end();
      for( ;sit!=sit_end; ++sit ){
        if( added.count(*sit) ) continue;
        added.insert(*sit);
        insertHash(*sit,hash,*it);
      }
    }
    
    return true;
  }

  // all chambers are mapped onto a layer and sector map
  bool MuonLayerHashProviderTool::initializeSectorMapping() {

    m_ntechnologies = m_idHelperSvc->mdtIdHelper().technologyNameIndexMax()+1;
    m_regionHashesPerSector.resize(MuonStationIndex::numberOfSectors());
    // set sector numbers
    unsigned int nsectorHashMax = MuonStationIndex::sectorLayerHashMax();
    for( unsigned int i=0;i<m_regionHashesPerSector.size();++i ) {
      m_regionHashesPerSector[i].sector=i+1;
      m_regionHashesPerSector[i].technologyRegionHashVecs.resize(MuonStationIndex::TechnologyIndexMax);
      for( auto it = m_regionHashesPerSector[i].technologyRegionHashVecs.begin();it!=m_regionHashesPerSector[i].technologyRegionHashVecs.end(); ++it ) {
        it->resize(nsectorHashMax);
      }
    }
    ATH_MSG_DEBUG("Initializing hashes: number of sectors " << MuonStationIndex::numberOfSectors() 
                  << " technologies " << m_ntechnologies << " sectorLayers " << MuonStationIndex::sectorLayerHashMax() );

    // add technologies
    if (m_idHelperSvc->hasMDT()) insertTechnology(m_idHelperSvc->mdtIdHelper());
    if (m_idHelperSvc->hasRPC()) insertTechnology(m_idHelperSvc->rpcIdHelper());
    if (m_idHelperSvc->hasCSC()) insertTechnology(m_idHelperSvc->cscIdHelper());
    if (m_idHelperSvc->hasMM()) insertTechnology(m_idHelperSvc->mmIdHelper());
    if (m_idHelperSvc->hasSTgc()) insertTechnology(m_idHelperSvc->stgcIdHelper());

    if( !insertTgcs() ) return false;

    if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << " Printing collections per sector, number of technologies " << m_ntechnologies;
    for( int sector = 1; sector<=16; ++sector ){
      MuonStationIndex::DetectorRegionIndex currentRegion = MuonStationIndex::DetectorRegionUnknown;
      if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << " sector " << sector;
      TechnologyRegionHashVec& vec = m_regionHashesPerSector[sector-1].technologyRegionHashVecs;
      for( unsigned int hash = 0; hash < nsectorHashMax; ++hash ){
        std::pair<MuonStationIndex::DetectorRegionIndex,MuonStationIndex::LayerIndex> regionLayer = MuonStationIndex::decomposeSectorLayerHash(hash);
        if( msgLvl(MSG::DEBUG) ) if( regionLayer.first != currentRegion ) msg(MSG::DEBUG) << std::endl << "  " << MuonStationIndex::regionName(regionLayer.first);
        bool first = true;
        currentRegion = regionLayer.first;
        for( unsigned int tech=0; tech<m_ntechnologies;++tech ){
          std::stable_sort(vec[tech][hash].begin(),vec[tech][hash].end());
          if( !vec[tech][hash].empty() ) {
            if( msgLvl(MSG::DEBUG) ) {
              if( first ) {
                msg(MSG::DEBUG) << "  " << std::setw(7) << MuonStationIndex::layerName(regionLayer.second);
                first = false;
              }
              msg(MSG::DEBUG) << " " << std::setw(4) << MuonStationIndex::technologyName(static_cast<MuonStationIndex::TechnologyIndex>(tech)) 
                              << " " << std::setw(4) << vec[tech][hash].size(); 
            }
          }
        }
      }
      if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << std::endl;
    }
    if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << endmsg;

    return true;
  }

}
