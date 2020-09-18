/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONLAYERHASHPROVIDERTOOL_H
#define MUON_MUONLAYERHASHPROVIDERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <string>
#include <vector>

static const InterfaceID IID_MuonLayerHashProviderTool("Muon::MuonLayerHashProviderTool",1,0);

namespace Muon {

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
    virtual ~MuonLayerHashProviderTool()=default;
    StatusCode initialize();

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

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    
  };

  inline bool MuonLayerHashProviderTool::hashInSector( IdentifierHash hash, int sector, 
                                                       MuonStationIndex::TechnologyIndex technologyIndex, unsigned int sectorLayerHash ) const {
    const std::vector<IdentifierHash>& hashes = getHashes(sector,technologyIndex,sectorLayerHash);
    return std::binary_search(hashes.begin(),hashes.end(),hash);
  }

  inline const std::vector<IdentifierHash>& MuonLayerHashProviderTool::getHashes( const Identifier& id ) const {
    MuonStationIndex::DetectorRegionIndex regionIndex = m_idHelperSvc->regionIndex(id);
    MuonStationIndex::LayerIndex layerIndex = m_idHelperSvc->layerIndex(id);
    unsigned int sectorLayerHash = MuonStationIndex::sectorLayerHash(regionIndex,layerIndex);
    return getHashes(m_idHelperSvc->sector(id),m_idHelperSvc->technologyIndex(id),sectorLayerHash);
  }


  inline const std::vector<IdentifierHash>& MuonLayerHashProviderTool::getHashes( int sector, MuonStationIndex::TechnologyIndex technologyIndex, 
                                                                                  unsigned int sectorLayerHash ) const {
    return m_regionHashesPerSector[sector-1].technologyRegionHashVecs[technologyIndex][sectorLayerHash];
  }
}



#endif
