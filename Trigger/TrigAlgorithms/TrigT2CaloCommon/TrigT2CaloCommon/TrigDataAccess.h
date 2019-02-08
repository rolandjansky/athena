// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     TrigDataAccess.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
 
 AUTHOR:   Denis Oliveira Damazio

 PURPOSE:  Tool designed to perform data access to
           Calorimeter cells. It drives either fast
           ByteStream Conversion (*CellCont) or slow
           ByteStream Conversion (*CellIDC). The Tools
           is driven by Region of Interest 
           definition.
 *******************************************************************/

#ifndef TRIGT2CALOCOMMON_TRIGDATAACCESS_H 
#define TRIGT2CALOCOMMON_TRIGDATAACCESS_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/DataHandle.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigT2CaloCommon/ITrigDataAccess.h"
#include "Identifier/IdentifierHash.h"

#include "LArRecEvent/LArCellCollection.h"
#include "LArRecEvent/LArFebEnergyCollection.h"
#include "TileEvent/TileL2Container.h"

#include "TileEvent/TileCell.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "LArByteStream/LArRodDecoder.h"
#include "TileByteStream/TileROD_Decoder.h"
#include "ZdcByteStream/ZdcByteStreamTool.h"
#include "ZdcByteStream/ZdcByteStreamReadV1V2Tool.h"
#include "ZdcRec/ZdcRecChannelTool.h"
//#include "LumiBlockComps/ILuminosityTool.h"

#include "TrigT2CaloCommon/T2GeometryTool.h"
#include "TrigT2CaloCommon/T2Calibration.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "eformat/old/util.h"

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Incident.h"
#include "ZdcEvent/ZdcDigitsCollection.h"

// Roi information
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigT2CaloCommon/phiutils.h"

class IRegSelSvc;
// class ITrigRegionSelector;
class LArRoI_Map;
class IROBDataProviderSvc;
class StoreGateSvc;
class T2CaloConfig;
class ICaloLumiBCIDTool;

/** Class that provides access to data for
    Calorimeter LVL2 Algorithms */
class TrigDataAccess: virtual public ITrigDataAccess, public AthAlgTool,
		      virtual public IIncidentListener {
public:
  /** Real Constructor */
  TrigDataAccess(const std::string & type, const std::string & name,
                 const IInterface* parent) 
  : AthAlgTool(type,name,parent),
    m_pRegionSelector("RegSelSvc/RegSelSvc","TrigDataAccess"),
    m_robDataProvider("ROBDataProviderSvc/ROBDataProviderSvc","TrigDataAccess"),
    m_lardecoder("LArRodDecoder/LArRodDecoder"),
    m_tiledecoder("TileROD_Decoder/TileROD_Decoder"),
    m_zdcdecoder("ZdcByteStreamReadV1V2Tool/ZdcByteStreamTool"),
    m_zdcrectool("ZdcRecChannelTool/ZdcByteChannelTool"), 
    m_applyOffsetCorrection(true),
    m_caloLumiBCIDTool("ICaloLumiBCIDTool/CaloLumiBCIDToolDefault")
    //m_lumiTool("LuminosityTool")
    //		 m_present_etamin(-10.0),
    //		 m_present_etamax(10.0),
    //		 m_present_phimin(-10.0),
    //		 m_present_phimax(10.0)
  {
    declareInterface<ITrigDataAccess>(this); 
    declareProperty("RegSelSvc",m_pRegionSelector,"Region Selector Service access");
    declareProperty("ROBDataProviderSvc",m_robDataProvider
		    ,"RobDataProvider for data fetching based on ROD ID");
    declareProperty("LArRodDecoder",m_lardecoder,"LArRodDecoder Tool for ByteStream Conversion");
    declareProperty("TileROD_Decoder",m_tiledecoder,"TileROD_Decoder Tool for ByteStream Conversion");
    declareProperty("ZdcByteStreamReadV1V2Tool",m_zdcdecoder,"ZdcByteStreamReadV1V2Tool for ByteStream Conversion");
    declareProperty("ZdcRecTool",m_zdcrectool,"ZdcRecTool for RawChannel rec");
    declareProperty("ApplyOffsetCorrection",m_applyOffsetCorrection,"Apply offset correction or not (false by default)"); 
    declareProperty("CaloLumiBCIDTool",m_caloLumiBCIDTool,"Tool for BCID pileup offset average correction"); 
    //declareProperty("LuminosityTool",m_lumiTool,"Luminosity Tool"); 
    // New property for MET slice
    declareProperty("loadFullCollections",  m_usefullcoll=false);
    // Load all samplings in robDataProviderSvc
    declareProperty("loadAllSamplings",  m_loadAllSamplings=false);
    /*
      declareProperty("NumberOfLArROSes", m_numberOfLarRoses=64);
      declareProperty("NumberOfTileROSes", m_numberOfTileRoses=4);
    */
    m_numberOfLarRoses=64;
    m_numberOfTileRoses=4;
  }
  /** Destructor */
  virtual ~TrigDataAccess() { }
  
  
  /** The same function should initialize all internal Tools
   */
  StatusCode initialize();
  /** Finalize */
  StatusCode finalize();
  /** handle to init a new event */
  void handle (const Incident& );
  /** begin run handle for conditions */
  StatusCode beginRunHandle(IOVSVC_CALLBACK_ARGS);
  /** begin run handle for conditions and RegSelSvc */
  StatusCode beginRunHandle_RegSelSvc(IOVSVC_CALLBACK_ARGS);
  

  /** Expose list of ROBs */
  void ROBList( const IRoiDescriptor& roi, std::vector<uint32_t>& vec);

  /// obsolete method
  void ROBList( const double etamin, const double etamax, const double phimin, const double phimax,
		std::vector<uint32_t>& vec) { 
    TrigRoiDescriptor roi( 0.5*(etamin+etamax), etamin, etamax, HLT::phimean(phimin,phimax), phimin, phimax);
    return ROBList( roi, vec );
  }


protected:
  // methods for tools
  // The two methods below could be united in a single one
  // They load this class with the list of ROBs to be looked after
  // By default TTEM is requested but TTHEC and TILE may also be used.
  void RegionSelectorListID (const int sampling, const IRoiDescriptor& roi, const DETID detid=TTEM);

  /// obsolete method
  void RegionSelectorListID (const int sampling, const double etamin,
			     const double etamax, const double phimin, const double phimax,
			     const DETID detid=TTEM){ 
    TrigRoiDescriptor roi( 0.5*(etamin+etamax), etamin, etamax, HLT::phimean(phimin,phimax), phimin, phimax);
    return RegionSelectorListID( sampling, roi, detid );
  }


  
  void RegionSelectorRobID (const int sampling, const IRoiDescriptor& roi,
			    const DETID detid=TTEM, bool fetchROBs = true);

  /// obsolete method
  void RegionSelectorRobID (const int sampling, const double etamin,
			    const double etamax, const double phimin, const double phimax,
			    const DETID detid=TTEM, bool fetchROBs = true) { 
    TrigRoiDescriptor roi( 0.5*(etamin+etamax), etamin, etamax, HLT::phimean(phimin,phimax), phimin, phimax);
    return RegionSelectorRobID( sampling, roi, detid, fetchROBs );
  }

  /** 
   * @brief Region Selector Input. Prepare internal tables
   * with online/offline Identifiers used in ByteStream
   * Conversion and data iterators (to Calorimeter Cells).
   * @param[in] sampling layer (not used for TileCal).
   * @param[in] etamin/etamax for Region of Interest definition.
   * @param[in] phimin/phimax for Region of Interest definition.
   * @param[in] DETID which can be TTEM (default), TTHEC, TILE.
   */
  virtual inline void RegionSelector(const int sampling, const IRoiDescriptor& roi,
				     const DETID detid=TTEM){
    RegionSelectorListID(sampling, roi, detid);
    RegionSelectorRobID(sampling, roi, detid);
  };

  /// obsolete method
  virtual inline void RegionSelector(const int sampling, const double etamin,
				     const double etamax, const double phimin, const double phimax,
				     const DETID detid=TTEM){
    TrigRoiDescriptor roi( 0.5*(etamin+etamax), etamin, etamax, HLT::phimean(phimin,phimax), phimin, phimax);
    RegionSelector(sampling, roi, detid);
  };

  /** 
   * @brief LoadCollections fetches data via ROBDataProvider
   * and really provides ByteStream Conversion by calling 
   * detector specific (LAr or Tile) ByteStream Conversion
   * Tools within RoI as defined in the above method.
   * @param[out] LArTT_Selector<LArCellCont> outputs
   *       begin, end interator to LArCells.
   * @param[in] sample (deprecated) maintained for compatibility.
   * @param[in] prepare if container should be prepared or
   *       considered as already existing (multilayer HEC or Tile
   *       access).
   */
  StatusCode LoadCollections (LArTT_Selector<LArCellCont>::const_iterator&,
			      LArTT_Selector<LArCellCont>::const_iterator&,
			      const unsigned int sample = 0, bool prepare=true );
  /** 
   * @brief LoadCollections fetches data via ROBDataProvider
   * and really provides ByteStream Conversion by calling 
   * detector specific (LAr or Tile) ByteStream Conversion
   * Tools within RoI as defined in the above method.
   * @param[out] TileCellCollection::const_iterator outputs
   *       begin, end interator to TileCells.
   * @param[in] sample (deprecated) maintained for compatibility.
   * @param[in] prepare if container should be prepared or
   *       considered as already existing (multilayer HEC or Tile
   *       access).
   */
  StatusCode LoadCollections (
	TileCellCollection::const_iterator&,
	TileCellCollection::const_iterator&,
	const unsigned int sample = 0, bool prepare=true);
	/** Load MBTS collection */
    StatusCode LoadMBTS(
                TileCellCollection::const_iterator&,
                TileCellCollection::const_iterator&);
        /** Load Zdc Collection */
    StatusCode LoadZdcCollection(
                ZdcRawChannelCollection::const_iterator&,
		ZdcRawChannelCollection::const_iterator&);
	/** 
        * @brief LoadCollections fetches data via ROBDataProvider
        * and really provides ByteStream Conversion by calling 
        * detector specific (LAr or Tile) ByteStream Conversion
        * Tools within RoI as defined in the above method.
        * @param[out] LArFebEnergyCollection::const_iterator outputs
        *       begin, end interator to LArFebEnergy Collections.
        * @param[in] sample (deprecated) maintained for compatibility.
        * @param[in] prepare if container should be prepared or
        *       considered as already existing (multilayer HEC or Tile
        *       access).
        */
    StatusCode LoadCollections (
	LArFebEnergyCollection::const_iterator&,
	LArFebEnergyCollection::const_iterator&,
	const unsigned int sample = 0, bool prepare=true);
        /** 
        * @brief Loads the full collection for the missing et computation
        */
    StatusCode LoadFullCollections (
        LArTT_Selector<LArCellCont>::const_iterator&,
        LArTT_Selector<LArCellCont>::const_iterator&,
        DETID detid, bool prepare=true );
        /** 
        * @brief Loads the full collection for the missing et computation
        */
    StatusCode LoadFullCollections (
        TileCellCollection::const_iterator&,
        TileCellCollection::const_iterator&,
        const unsigned int sample = 0, bool prepare=true );
        /** 
        * @brief Loads the full collection for the missing et computation
        */

    StatusCode LoadFullCollections (
        LArFebEnergyCollection::const_iterator&,
        LArFebEnergyCollection::const_iterator&,
        DETID detid, bool prepare=true );
        /** 
        * @brief Loads the full collection for the missing et computation
        */

    StatusCode LoadFullCollections (
                TileL2Container::const_iterator& Begin,
                TileL2Container::const_iterator& End);
	/**
        * @brief Loads the full fast collection
        */
    StatusCode LoadFullCollections (
        CaloCellContainer::const_iterator&,
        CaloCellContainer::const_iterator& );
	/** Number of Tile Calorimeter modules found in
        * the Region of Interest */
    unsigned int TileContSize ( void ) { return m_rIds.size(); } 
	/** Number of Tile Calorimeter modules found in
        * the Region of Interest MET case -> All Calo*/
    unsigned int TileFullContSize ( void ) { return m_rIdstile.size(); }
    
    /** RegSelSvc handle */
    ServiceHandle<IRegSelSvc>         m_pRegionSelector;
    /** LArRoI_Map used by LArTT_Selector */
    const LArRoI_Map*            m_roiMap;           
    /** IROBDataProviderSvc handle to allow data fetching
	by the algorithm */
    ServiceHandle<IROBDataProviderSvc>         m_robDataProvider;

  /** iterators to LArCells type depending on
      access wa§y (IDC or Cont) */
  LArTT_Selector<LArCellCont>* m_sel;
  LArTT_Selector<LArCellCont>* m_sellarfull;
  LArTT_Selector<LArCellCont>* m_selem;
  LArTT_Selector<LArCellCont>* m_selhec;
  LArTT_Selector<LArCellCont>* m_selfcalhad;
  LArTT_Selector<LArCellCont>* m_selfcalem;
  /** LArCellCont pointer - has collections of LArCells */
  LArCellCont* m_larcell;
  /** TileCellCont pointer - has collections of TileCells */
  TileCellCont* m_tilecell;
  /** Full Container */
  CaloCellContainer* m_fullCellContainer;
  /** Input DataBlock to pre-allocate memory space for
      ROBFragment to be decoded */
  //std::vector<uint32_t> m_datablock;
  //        LArFebEnergyCollection* m_febcoll;
  /** report error */
  uint32_t report_error ( void ) { return m_error; }
  

  // Objects that the Tools should not worry about
private:
  /** fill vector with all RodIds and Hash Id's in initialize */
  bool m_usefullcoll;
  /** List of Hash Id's to be fetched for a given RoI. */
  std::vector<IdentifierHash> m_rIds;
  std::vector<IdentifierHash> m_rIdsem0;
        std::vector<IdentifierHash> m_rIdsem1;
  std::vector<IdentifierHash> m_rIdsem2;
  std::vector<IdentifierHash> m_rIdsem3;
  std::vector<IdentifierHash> m_rIdshec;
  std::vector<IdentifierHash> m_rIdshec0;
  std::vector<IdentifierHash> m_rIdshec1;
  std::vector<IdentifierHash> m_rIdshec2;
  std::vector<IdentifierHash> m_rIdshec3;
  std::vector<IdentifierHash> m_rIdsfcalhad0;
  std::vector<IdentifierHash> m_rIdsfcalhad1;
  std::vector<IdentifierHash> m_rIdsfcalhad2;
  std::vector<IdentifierHash> m_rIdsfcalhad3;
  std::vector<IdentifierHash> m_rIdsfcalem0;
  std::vector<IdentifierHash> m_rIdstile;
  /** List of RodIds to be fetched for a given RoI. */
  std::vector<uint32_t> m_vrodid32;
  std::vector<uint32_t> m_partial_vrodid32;
  std::vector<uint32_t> m_full_vrodid32;
  std::vector<uint32_t> m_vrodid32lar;
  std::vector<uint32_t> m_vrodid32em;
  std::vector<uint32_t> m_vrodid32hec;
  std::vector<uint32_t> m_vrodid32hec0;
  std::vector<uint32_t> m_vrodid32hec1;
  std::vector<uint32_t> m_vrodid32hec2;
  std::vector<uint32_t> m_vrodid32hec3;
  std::vector<uint32_t> m_vrodid32hec0SideA;
  std::vector<uint32_t> m_vrodid32hec0SideC;
  std::vector<uint32_t> m_vrodid32fcalem;
  std::vector<uint32_t> m_vrodid32fcalhad;
  std::vector<uint32_t> m_vrodid32tile;
  std::vector<uint32_t> m_vrodid32ros; //  for virtual  lar ros in ROBs
  std::vector<uint32_t> m_vrodid32tros; // for virtual tile ros in ROBs
  std::vector<uint32_t> m_vrodid32fullDet;
  std::vector<unsigned int> m_mbts_add_rods;
  const std::vector<unsigned int>* m_mbts_rods;
  // Copy to be used in case rob_misses
  std::vector<uint32_t> m_vrodid32_found;
  // List of missed robs
  std::vector<uint32_t> m_vrodid32_missed;
  /** List of Detector ID (needed for London Schema - TileCal)
      elements for offline like data organization (module
      ID for TileCal, TTsample ID for LAr).
  */
  std::vector<int> m_listID;
  /** Pointer to the LAr ROD Decoder Tool. This will perform
      the real ByteStream Conversion for LAr data. */
  ToolHandle<LArRodDecoder> m_lardecoder;
  /** Pointer to the Tile ROD Decoder Tool. This will perform
      the real ByteStream Conversion for Tile data. */
  ToolHandle<TileROD_Decoder> m_tiledecoder;
  /** Pointer to the Zdc ROD Decoder Tool. This will perform
      the real ByteStream Conversion for Zdc data. */
  ToolHandle<ZdcByteStreamReadV1V2Tool> m_zdcdecoder;
  /** Pointer to the Zdc Rec Tool. This will perform
      the real ByteStream Conversion for Zdc data. */
  ToolHandle<ZdcRecChannelTool> m_zdcrectool;
  /** CaloLumiCorrection */
  bool m_applyOffsetCorrection;
  ToolHandle<ICaloLumiBCIDTool> m_caloLumiBCIDTool;
  //ToolHandle<ILuminosityTool> m_lumiTool;
  /** robFragments pointers */
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> m_robFrags;
  /** space for Tile Calorimeter Identifier */
  std::vector<unsigned int> m_tile;
  std::vector<unsigned int> m_alltile;
  /** check whether iov method was called */
  bool m_iov_called;
  /** holds all the LArFEB collections */
  LArFebEnergyCollection* m_febcoll;
  /** holds all the TileL2 collections */
  TileL2Container* m_drawcoll;
  /** Either use one calo layer at a time, or do all of it */
  bool m_loadAllSamplings;
  /** new Tile ROD schema (2017) */
  /** error reporting */
  uint32_t m_error;
  /** mbts cached */
  bool m_mbts_done;
  /** reset collection ( put all cell energies to 0 ) */
  void reset_LArCol ( LArCellCollection* coll ){
    for(LArCellCollection::iterator ii=coll->begin();ii!=coll->end();++ii)
      (*ii)->setEnergyFast(0.0);
  }
  /** reset collections (put all cell energies to 0 ) */
  void reset_TileCol(TileCellCollection* col){
     for(TileCellCollection::iterator tr=col->begin();tr!=col->end(); ++tr) { 
	(*tr)->setEnergy_nonvirt(0.0F, 0.0F, 0, CaloGain::INVALIDGAIN);
	(*tr)->setTime_nonvirt(-100.0F);
	(*tr)->setQuality_nonvirt(static_cast<unsigned char>(255), 0, 0);
	(*tr)->setQuality_nonvirt(static_cast<unsigned char>(255), 0, 1);
     } // end of for all channels
  }
  ZdcDigitsCollection m_zdcDigitCollection;
  ZdcRawChannelCollection m_zdcCol;
  /** caching eta/phi for RoI based ROB request fetching */
  //  double m_present_etamin;
  //  double m_present_etamax;
  //  double m_present_phimin;
  //  double m_present_phimax;
  const IRoiDescriptor* m_present_roi;
  std::vector<unsigned int> m_zdc_rods;
  uint32_t m_numberOfLarRoses;
  uint32_t m_numberOfTileRoses;

  TileROD_Decoder::D0CellsHLT m_d0cells;
};

#endif
