/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     TrigDataAccessATLFAST.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
 
 AUTHOR:   Denis Oliveira Damazio

 PURPOSE:  Tool designed to perform data access to
           Calorimeter cells. It does not perform
           ByteStream conversion. Just uses the ATLFast
	   CaloCellContainer AllCalo. It is, however,
           driven by Region of Interest (in eta/phi)
           definition.
 *******************************************************************/

#ifndef TRIGT2CALOCOMMONATLFAST_TRIGDATAACCESS_H 
#define TRIGT2CALOCOMMONATLFAST_TRIGDATAACCESS_H

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

#include "TileEvent/TileCell.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "LArByteStream/LArRodDecoder.h"
#include "TileByteStream/TileROD_Decoder.h"

#include "TrigT2CaloCommon/T2GeometryTool.h"
#include "TrigT2CaloCommon/T2Calibration.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "eformat/old/util.h"

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Incident.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigT2CaloCommon/phiutils.h"

class IRegSelSvc;
// class ITrigRegionSelector;
class LArRoI_Map;
class IROBDataProviderSvc;
class StoreGateSvc;
class T2CaloConfig;

/** Class that provides access to data for
    Calorimeter LVL2 Algorithms */
class TrigDataAccessATLFAST: virtual public ITrigDataAccess, public AthAlgTool,
	virtual public IIncidentListener {
  public:
    /** Real Constructor */
    TrigDataAccessATLFAST(const std::string & type, const std::string & name,
                 const IInterface* parent) : AthAlgTool(type,name,parent),
		 m_pRegionSelector("RegSelSvc/RegSelSvc","TrigDataAccess"){
	declareInterface<ITrigDataAccess>(this); 
	declareProperty("RegSelSvc",m_pRegionSelector,"Region Selector Service access");
	// New property for MET slice
        declareProperty("loadFullCollections",  m_usefullcoll=false);
    }
    /** Destructor */
    virtual ~TrigDataAccessATLFAST() { }


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

    void ROBList( const IRoiDescriptor& /* roi */,
		  std::vector<uint32_t>& /*vec*/){ }

    /// obsolete
    void ROBList( const double /* etamin */, const double /*etamax*/,
		  const double /*phimin*/, const double /*phimax*/,
		  std::vector<uint32_t>& /*vec*/){ }


  protected:
    // methods for tools
    // The two methods below could be united in a single one
    // They load this class with the list of ROBs to be looked after
    // By default TTEM is requested but TTHEC and TILE may also be used.

    void RegionSelectorListID (const int sampling, const IRoiDescriptor& roi,
			       const DETID detid=TTEM);    
   
    void RegionSelectorRobID (const int sampling,  const IRoiDescriptor& roi,
			      const DETID detid=TTEM, bool fetchROBs = true);
    
    /** 
        * @brief Region Selector Input. Prepare internal tables
        * with online/offline Identifiers used in ByteStream
        * Conversion and data iterators (to Calorimeter Cells).
        * @param[in] sampling layer (not used for TileCal).
        * @param[in] IRoiDescriptor for Region of Interest definition.
        * @param[in] DETID which can be TTEM (default), TTHEC, TILE.
         */
    virtual inline void RegionSelector(const int sampling, const IRoiDescriptor& roi, const DETID detid=TTEM){
      RegionSelectorListID(sampling, roi, detid);
      RegionSelectorRobID (sampling, roi, detid);
    };

    
    /// obsolete

    /** 
        * @brief Region Selector Input. Prepare internal tables
        * with online/offline Identifiers used in ByteStream
        * Conversion and data iterators (to Calorimeter Cells).
        * @param[in] sampling layer (not used for TileCal).
        * @param[in] etamin/etamax for Region of Interest definition.
        * @param[in] phimin/phimax for Region of Interest definition.
        * @param[in] DETID which can be TTEM (default), TTHEC, TILE.
         */
    void RegionSelectorListID (const int sampling, 
			       const double etamin, const double etamax, 
			       const double phimin, const double phimax,
			       const DETID detid=TTEM) { 
      TrigRoiDescriptor roi( 0.5*(etamin+etamax), etamin, etamax, HLT::phimean(phimin,phimax), phimin, phimax);
      RegionSelectorListID(sampling, roi, detid);
    }
    
    void RegionSelectorRobID (const int sampling, const double etamin,
			      const double etamax, const double phimin, const double phimax,
			      const DETID detid=TTEM, bool /* fetchROBs*/ = true) { 
      TrigRoiDescriptor roi( 0.5*(etamin+etamax), etamin, etamax, HLT::phimean(phimin,phimax), phimin, phimax);
      RegionSelectorRobID(sampling, roi, detid);
    }

    /// obsolete methods ///
    virtual inline void RegionSelector(const int sampling, 
				       const double etamin, const double etamax, 
				       const double phimin, const double phimax,
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
    StatusCode LoadCollections (
	LArTT_Selector<LArCellCont>::const_iterator&,
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
        /** MBTS collection */
    StatusCode LoadMBTS(TileCellCollection::const_iterator&,
                TileCellCollection::const_iterator&){
	return StatusCode::SUCCESS; }
        /** Load Zdc Collection */
    StatusCode LoadZdcCollection(ZdcRawChannelCollection::const_iterator&,
                ZdcRawChannelCollection::const_iterator&){
	return StatusCode::SUCCESS; }

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
        * @brief Loads the full collection fast
        */
    StatusCode LoadFullCollections (
                CaloCellContainer::const_iterator&,
                CaloCellContainer::const_iterator&){
                return StatusCode::SUCCESS; }
	/** Number of Tile Calorimeter modules found in
        * the Region of Interest */
    unsigned int TileContSize ( void ) { return m_rIds.size(); } 
	/** Number of Tile Calorimeter modules found in
        * the Region of Interest MET case -> All Calo*/
    unsigned int TileFullContSize ( void ) { return m_rIdstile.size(); }
	/** Error reporting */
	uint32_t report_error ( void ) { return 0x0; }
    
    /** RegSelSvc handle */
    ServiceHandle<IRegSelSvc>         m_pRegionSelector;
    /** LArRoI_Map used by LArTT_Selector */
    const LArRoI_Map*            m_roiMap;           

	/** iterators to LArCells type depending on
	access way (IDC or Cont) */
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
	/** Input DataBlock to pre-allocate memory space for
	ROBFragment to be decoded */
	//std::vector<uint32_t> m_datablock;
//        LArFebEnergyCollection* m_febcoll;


    // Objects that the Tools should not worry about
  private:
        /** method to fill collection with container information */
        template<class T>
        void fillColl(const DataHandle<CaloCellContainer>& input, T& output);
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
        std::vector<IdentifierHash> m_rIdsfcalhad0;
        std::vector<IdentifierHash> m_rIdsfcalhad1;
        std::vector<IdentifierHash> m_rIdsfcalem0;
        std::vector<IdentifierHash> m_rIdstile;
	/** List of RodIds to be fetched for a given RoI. */
	std::vector<uint32_t> m_vrodid32;
        std::vector<uint32_t> m_vrodid32lar;
        std::vector<uint32_t> m_vrodid32em;
        std::vector<uint32_t> m_vrodid32hec;
        std::vector<uint32_t> m_vrodid32hec0;
        std::vector<uint32_t> m_vrodid32hec1;
        std::vector<uint32_t> m_vrodid32hec2;
        std::vector<uint32_t> m_vrodid32fcalem;
        std::vector<uint32_t> m_vrodid32fcalhad;
        std::vector<uint32_t> m_vrodid32tile;
	/** List of Detector ID (needed for London Schema - TileCal)
	    elements for offline like data organization (module
	    ID for TileCal, TTsample ID for LAr).
	*/
	std::vector<int> m_listID;
	/** space for Tile Calorimeter Identifier */
	std::vector<unsigned int> m_tile;
	/** check whether iov method was called */
	bool m_iov_called;
};

#endif
