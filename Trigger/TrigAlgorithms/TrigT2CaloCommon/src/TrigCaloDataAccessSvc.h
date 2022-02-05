/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT2CaloCommon_TrigCaloDataAccessSvc_h
#define TrigT2CaloCommon_TrigCaloDataAccessSvc_h

#include <mutex>
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "LArByteStream/LArRodDecoder.h"
#include "TileByteStream/TileROD_Decoder.h"
#include "LArRawUtils/LArTT_Selector.h"
#include "TileByteStream/TileCellCont.h"
#include "TrigT2CaloCommon/LArCellCont.h"
#include "LArRecEvent/LArFebEnergyCollection.h"
#include "TileEvent/TileCellCollection.h"
#include "TileEvent/TileL2Container.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "IRegionSelector/IRegSelTool.h"
#include "TrigT2CaloCommon/ITrigCaloDataAccessSvc.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "CaloEvent/CaloBCIDAverage.h"
#include "LArRawConditions/LArMCSym.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArFebRodMapping.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArRecConditions/LArRoIMap.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

class TrigCaloDataAccessSvc : public extends<AthService, ITrigCaloDataAccessSvc> {
 public:
  TrigCaloDataAccessSvc(const std::string& name, ISvcLocator* pSvcLocator);

  using ITrigCaloDataAccessSvc::Status;


  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  
  virtual StatusCode loadCollections ( const EventContext& context,
                                       const IRoiDescriptor& roi,
                                       const DETID detID,
                                       const int sampling,
                                       LArTT_Selector<LArCellCont>& loadedCells ) override;
  
  virtual StatusCode loadCollections ( const EventContext& context,
                                       const IRoiDescriptor& roi,
                                       TileCellCollection& loadedCells ) override;
  
  virtual StatusCode loadMBTS ( const EventContext& context,
                                                    TileCellCollection& loadedCells ) override;


  
  virtual StatusCode loadFullCollections ( const EventContext& context,
                                           CaloConstCellContainer& cont ) override;
  
 private:
  
  PublicToolHandle<LArRodDecoder> m_larDecoder { this, "LArDecoderTool", "LArRodDecoder/LArRodDecoder", "Tool to decode LAr raw data" };
  PublicToolHandle<TileROD_Decoder> m_tileDecoder { this, "TileDecoderTool", "TileROD_Decoder/TileROD_Decoder", "Tool to decode Tile raw data" };

  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Tool to monitor performance of the service" };

  ServiceHandle<IROBDataProviderSvc>  m_robDataProvider{ this, "ROBDataProvider", "ROBDataProviderSvc/ROBDataProviderSvc", ""};
  ToolHandle<IRegSelTool>           m_regionSelector_TTEM  { this, "RegSelToolEM",  "RegSelTool/RegSelTool_TTEM" };
  ToolHandle<IRegSelTool>           m_regionSelector_TTHEC  { this, "RegSelToolHEC",  "RegSelTool/RegSelTool_TTHEC" };
  ToolHandle<IRegSelTool>           m_regionSelector_FCALEM  { this, "RegSelToolFCALEM",  "RegSelTool/RegSelTool_FCALEM" };
  ToolHandle<IRegSelTool>           m_regionSelector_FCALHAD  { this, "RegSelToolFCALHAD",  "RegSelTool/RegSelTool_FCALHAD" };
  ToolHandle<IRegSelTool>           m_regionSelector_TILE  { this, "RegSelToolTILE",  "RegSelTool/RegSelTool_TILE" };
  
  Gaudi::Property<bool> m_applyOffsetCorrection { this, "ApplyOffsetCorrection", true, "Enable offset correction" };

  SG::ReadHandleKey<CaloBCIDAverage> m_bcidAvgKey ;
  SG::ReadCondHandleKey<LArMCSym> m_mcsymKey 
   {this, "MCSymKey", "LArMCSym", "SG Key of LArMCSym object"} ;
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_onOffIdMappingKey
   {this, "CablingKey", "LArOnOffIdMap", "SG Key for LArOnOffIdMapping"} ;
  SG::ReadCondHandleKey<LArFebRodMapping> m_febRodMappingKey
   {this, "RodFebKey", "LArFebRodMap", "SG Key for LArFebRodMapping"} ;
  SG::ReadCondHandleKey<LArBadChannelCont> m_bcContKey
   {this, "LArBadChannelKey", "LArBadChannel", "Key of the LArBadChannelCont CDO" };
  SG::ReadCondHandleKey<LArRoIMap> m_larRoIMapKey
   {this, "LArRoIMapKey", "LArRoIMap", "Key of the LArRoIMap CDO" };
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey
   {this, "CaloDetDescrManager", "CaloDetDescrManager", "SG Key for CaloDetDescrManager in the Condition Store" };

  void reset_LArCol ( LArCellCollection* coll ){
    for(LArCellCollection::iterator ii=coll->begin();ii!=coll->end();++ii)
      (*ii)->setEnergyFast(0.0);
  }
  void reset_TileCol(TileCellCollection* col){
     for ( TileCell* tr: *col ) {
        (tr)->setEnergy_nonvirt(0.0F, 0.0F, 0, CaloGain::INVALIDGAIN);
        (tr)->setTime_nonvirt(-100.0F);
        (tr)->setQuality_nonvirt(static_cast<unsigned char>(255), 0, 0);
        (tr)->setQuality_nonvirt(static_cast<unsigned char>(255), 0, 1);
      } // end of for all channels
   }

  /**
   * @brief Convenience structure to keep together all ROBs and IdentifierHashes 
   * for whole detectors.
   **/
  struct FullDetIDs {
    std::vector<uint32_t> robs;
    std::vector<IdentifierHash> ids;
    DETID detid;
    void merge( const std::initializer_list<FullDetIDs>& list ) {
      for ( auto& el: list ) {
	std::copy( el.robs.begin(), el.robs.end(), std::back_inserter(robs) );
	std::copy( el.ids.begin(), el.ids.end(), std::back_inserter(ids) );
      }
    }

  };
  
  /**
   * @brief convience structure to keep together a collection and auxiliar full collection selectors
   */
  struct  HLTCaloEventCache {
    std::mutex mutex;    
    LArCellCont* larContainer;
    LArRodBlockStructure* larRodBlockStructure_per_slot; // LAr Rod Block to ease decoding
    uint16_t rodMinorVersion;
    uint32_t robBlockType;
    TileCellCont* tileContainer;
    CaloCellContainer* fullcont;
    TileROD_Decoder::D0CellsHLT* d0cells;
    unsigned int lastFSEvent;
  };

  
  SG::SlotSpecificObj< HLTCaloEventCache > m_hLTCaloSlot;

  std::mutex m_initMutex; // this will be gone once we move to new conditions
  std::mutex m_dataPrepMutex; // this will be gone when reg sel & Rob DP will become thread safe
  std::mutex m_getCollMutex; // this will be gone
  std::mutex m_lardecoderProtect;  // protection for the larRodDecoder
  std::mutex m_tiledecoderProtect;  // protection for the tileRodDecoder

  unsigned int lateInit( const EventContext& context );
  bool m_lateInitDone = false;

  unsigned int convertROBs(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& robFrags, LArCellCont* larcell, LArRodBlockStructure*& larRodBlockStructure, uint16_t rodMinorVersion, uint32_t robBlockType );
  unsigned int convertROBs( const EventContext& context, const std::vector<IdentifierHash>& rIds, TileCellCont* tilecell, TileROD_Decoder::D0CellsHLT* d0cells );


  /**
   * @brief fill the set of missing robs given the request and response from RoBDatProvider
   **/
  void missingROBs( const std::vector<uint32_t>& request,
		    const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& response,
		    std::set<uint32_t>& missing ) const;

  /**
   * @brief clear fragments of the collection for which ROBs were not available
   **/
  void clearMissing( const std::vector<uint32_t>& request,
		     const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& response, 
		     LArCellCont* larcell );

  /**
   * @brief LAr TT collections preparation code
   **/
  unsigned int prepareLArCollections( const EventContext& context,
				const IRoiDescriptor& roi, 
				const int sampling,
				DETID detector );

  unsigned int prepareTileCollections( const EventContext& context,
				const IRoiDescriptor& roi );

  unsigned int prepareMBTSCollections( const EventContext& context);

  unsigned int prepareFullCollections( const EventContext& context );

  unsigned int prepareLArFullCollections( const EventContext& context );
  unsigned int prepareTileFullCollections( const EventContext& context );

  std::vector<uint32_t> m_vrodid32fullDet;
  std::vector<uint32_t> m_vrodid32tile;
  std::vector<unsigned int> m_mbts_add_rods;
  const std::vector<unsigned int>* m_mbts_rods;
  std::vector<IdentifierHash> m_rIdstile;
  std::vector<std::vector<uint32_t> > m_vrodid32fullDetHG;
  size_t m_nSlots;
};


#endif


