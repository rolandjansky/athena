/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONPREPRAWDATACOLLECTIONPROVIDERTOOL_H
#define MUON_MUONPREPRAWDATACOLLECTIONPROVIDERTOOL_H

#include <vector>
#include <typeinfo>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "MuonLayerHashProviderTool.h"

#include "MuonLayerEvent/MuonLayerPrepRawData.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"


static const InterfaceID IID_MuonPrepRawDataCollectionProviderTool("Muon::MuonPrepRawDataCollectionProviderTool",1,0);

namespace Muon {

  class MuonIdHelperTool;
  class MuonLayerHashProviderTool;

  class MuonPrepRawDataCollectionProviderTool :  public AthAlgTool {
  public:

    /** Default AlgTool functions */
    MuonPrepRawDataCollectionProviderTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonPrepRawDataCollectionProviderTool();
    StatusCode initialize();
    StatusCode finalize();

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
      ContainerType* container = 0;
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
        typename ContainerType::const_iterator colIt = container->indexFind(*it);
        if( colIt == container->end() ) {
          //ATH_MSG_WARNING("Cannot find hash " << *it << " in container at " << location);
          continue;
        }
        ATH_MSG_VERBOSE("  adding " << m_idHelper->toStringChamber((*colIt)->identify()) << " size " << (*colIt)->size());
        // else add
        output.push_back(*colIt);
      }
      return true;
    }

  private:
    
    /** tool handles */
    ToolHandle<MuonIdHelperTool> m_idHelper; 
    ToolHandle<MuonLayerHashProviderTool> m_layerHashProvider; 
    
    /** storegate keys of PRDs*/
    std::vector< std::string > m_locations;
  };
}



#endif
 
