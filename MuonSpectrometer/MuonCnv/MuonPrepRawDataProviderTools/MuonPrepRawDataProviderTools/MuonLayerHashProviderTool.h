/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONLAYERHASHPROVIDERTOOL_H
#define MUON_MUONLAYERHASHPROVIDERTOOL_H

#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonStationIndex.h"

static const InterfaceID IID_MuonLayerHashProviderTool("Muon::MuonLayerHashProviderTool",1,0);

class MuonIdHelper;

namespace Muon {

  class MuonIdHelperTool;

  class MuonLayerHashProviderTool :  public AthAlgTool {
  public:
    /** define data structure 
        - sector 
          - technology
            - hashes
    */
    typedef std::vector<IdentifierHash>   HashVec;
    typedef std::vector< HashVec >        RegionHashVec;
    typedef std::vector< RegionHashVec >  TechnologyRegionHashVec;
    typedef RegionHashVec::const_iterator RegionHashVecCit;

    struct RegionHashesPerSector {
      int sector;
      TechnologyRegionHashVec technologyRegionHashVecs;
    };
    typedef std::vector<RegionHashesPerSector>       RegionHashesPerSectorVec;
    typedef RegionHashesPerSectorVec::const_iterator RegionHashesPerSectorCit;

    /** access hashes for a given sector, technology and layer hash */
    const HashVec& getHashes( int sector, MuonStationIndex::TechnologyIndex technologyIndex, unsigned int sectorLayerHash ) const;

    /** access hashes in a given sector and layer using an identifier */
    const HashVec& getHashes( const Identifier& id ) const;

    /** check whether a collection hash is part of given sector, technology and sector layer hash */
    bool hashInSector( IdentifierHash hash, int sector, MuonStationIndex::TechnologyIndex technologyIndex, unsigned int sectorLayerHash ) const;

    /** Default AlgTool functions */
    MuonLayerHashProviderTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonLayerHashProviderTool();
    StatusCode initialize();
    StatusCode finalize();

    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuonLayerHashProviderTool; }


  private:
    
    /** insert hashes of a given technology */
    void insertTechnology( const MuonIdHelper& idHelper );

    /** insert hashes for the tgcs */
    bool insertTgcs();

    /** insert a single hash for a given identifier */
    void insertHash( const IdentifierHash& hash, const Identifier& id );

    /** insert a single hash for a given identifier and sector */
    void insertHash( int sector, const IdentifierHash& hash, const Identifier& id );

    /** initialize the mapping structure */
    bool initializeSectorMapping();

    /** number of technologies */
    unsigned int m_ntechnologies;

    /** cachaed hash data structure */
    RegionHashesPerSectorVec m_regionHashesPerSector;

    /** tool handles */
    ToolHandle<MuonIdHelperTool> m_idHelper; 
    
  };

  inline bool MuonLayerHashProviderTool::hashInSector( IdentifierHash hash, int sector, 
                                                       MuonStationIndex::TechnologyIndex technologyIndex, unsigned int sectorLayerHash ) const {
    const std::vector<IdentifierHash>& hashes = getHashes(sector,technologyIndex,sectorLayerHash);
    return std::binary_search(hashes.begin(),hashes.end(),hash);
  }

  inline const std::vector<IdentifierHash>& MuonLayerHashProviderTool::getHashes( const Identifier& id ) const {
    MuonStationIndex::DetectorRegionIndex regionIndex = m_idHelper->regionIndex(id);
    MuonStationIndex::LayerIndex layerIndex = m_idHelper->layerIndex(id);
    unsigned int sectorLayerHash = MuonStationIndex::sectorLayerHash(regionIndex,layerIndex);
    return getHashes(m_idHelper->sector(id),m_idHelper->technologyIndex(id),sectorLayerHash);
  }


  inline const std::vector<IdentifierHash>& MuonLayerHashProviderTool::getHashes( int sector, MuonStationIndex::TechnologyIndex technologyIndex, 
                                                                                  unsigned int sectorLayerHash ) const {
    return m_regionHashesPerSector[sector-1].technologyRegionHashVecs[technologyIndex][sectorLayerHash];
  }
}



#endif
