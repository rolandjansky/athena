/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONPREPRAWDATACOLLECTIONPROVIDERTOOL_H
#define MUON_MUONPREPRAWDATACOLLECTIONPROVIDERTOOL_H

#include <vector>
#include <typeinfo>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonLayerHashProviderTool.h"

#include "MuonLayerEvent/MuonLayerPrepRawData.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

static const InterfaceID IID_MuonPrepRawDataCollectionProviderTool("Muon::MuonPrepRawDataCollectionProviderTool",1,0);

namespace Muon {

  class MuonLayerHashProviderTool;

  class MuonPrepRawDataCollectionProviderTool :  public AthAlgTool {
  public:

    /** Default AlgTool functions */
    MuonPrepRawDataCollectionProviderTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonPrepRawDataCollectionProviderTool() {};
    StatusCode initialize();

    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuonPrepRawDataCollectionProviderTool; }

    /** access data in layer */
    bool getLayerData( int sector, MuonStationIndex::DetectorRegionIndex regionIndex, MuonStationIndex::LayerIndex layerIndex, MuonLayerPrepRawData& layerPrepRawData ) const;

    /** access data in layer for a given technology */
    template<class COL>
    bool getLayerData( int sector, MuonStationIndex::TechnologyIndex technology, MuonStationIndex::DetectorRegionIndex regionIndex, 
                       MuonStationIndex::LayerIndex layerIndex, std::vector<const COL*>& output ) const {

      // get technologies in the given layer
      unsigned int sectorLayerHash = MuonStationIndex::sectorLayerHash( regionIndex, layerIndex );

      // get hashes 
      const MuonLayerHashProviderTool::HashVec& hashes = m_layerHashProvider->getHashes( sector, technology, sectorLayerHash );

      // skip empty inputs
      if( hashes.empty() ) return true;

      // access PRD container
      typedef MuonPrepDataContainer< COL > ContainerType;
      const ContainerType* container = 0;
      std::string location = m_locations[technology];
      if( !evtStore()->contains<ContainerType>(location) ) {
        ATH_MSG_DEBUG("Cannot find container " << typeid(container).name() << " at " << location << " technology index " << technology);
        return true;
      }
      if (evtStore()->retrieve(container,location).isFailure()) {
        ATH_MSG_WARNING("Cannot retrieve container " << typeid(container).name() << " at " << location);
        return false;
      }      

      // loop over hashes 
      for( MuonLayerHashProviderTool::HashVec::const_iterator it=hashes.begin();it!=hashes.end();++it ){
        
        // skip if not found
        auto col = container->indexFindPtr(*it);
        if( col == nullptr ) {
          continue;
        }
        ATH_MSG_VERBOSE("  adding " << m_idHelperSvc->toStringChamber(col->identify()) << " size " << col->size());
        output.push_back(col);
      }
      return true;
    }

  private:
    
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ToolHandle<MuonLayerHashProviderTool> m_layerHashProvider; 
    
    /** storegate keys of PRDs*/
    std::vector< std::string > m_locations;
  };
}



#endif
 
