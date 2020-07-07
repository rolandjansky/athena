/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     ITrigDataAccess.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
 
 AUTHOR:   Denis Oliveira Damazio
 
 PURPOSE:  Tool designed to perform data access to
	   Calorimeter cells. It drives either fast
	   ByteStream Conversion (*CellCont). The Tools
	   is driven by Region of Interest
	   definition.
 *******************************************************************/

#ifndef TRIGT2CALOCOMMON_ITRIGDATAACCESS_H 
#define TRIGT2CALOCOMMON_ITRIGDATAACCESS_H

#include "LArRawUtils/LArTT_Selector.h"
#include "TileByteStream/TileCellCont.h"
#include "TrigT2CaloCommon/LArCellCont.h"
#include "LArRecEvent/LArFebEnergyCollection.h"
#include "TileEvent/TileCellCollection.h"
#include "TileEvent/TileL2Container.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "IRegionSelector/RegSelEnums.h"
#include "CaloEvent/CaloCellContainer.h"
#include "ZdcEvent/ZdcRawChannelCollection.h"

// For callbacks
#include "AthenaKernel/IOVSvcDefs.h"

static const InterfaceID IID_ITrigDataAccess("ITrigDataAccess",1,0);

/** Interface Class that provides access to data for
    Calorimeter LVL2 Algorithms */
class ITrigDataAccess: virtual public IAlgTool{
  public:
	/** Interface for Virtual Class */
	static const InterfaceID& interfaceID();
	/** 
	* @brief Region Selector Input. Prepare internal tables
	* with online/offline Identifiers used in ByteStream
	* Conversion and data iterators (to Calorimeter Cells).
	* @param[in] sampling layer (not used for TileCal).
	* @param[in] roi Region of Interest 
	* @param[in] DETID which can be TTEM (default), TTHEC, TILE.
	 */
	virtual void RegionSelector(const int sampling, const IRoiDescriptor& roi, const DETID detid=TTEM) = 0;

	/// obsolete
	virtual void RegionSelector(const int /* sampling */, 
				    const double /* etamin */, const double /* etamax */, 
				    const double /* phimin */, const double /* phimax */,
				    const DETID /* detid=TTEM */) { };
	/** 
	* @brief LoadCollections fetches data via ROBDataProvider
	* and really provides ByteStream Conversion by calling 
	* detector specific (LAr or Tile) ByteStream Conversion
	* Tools within RoI as defined in the above method.
	* @param[out] LArTT_Selector<LArCellCont> outputs
	*	begin, end interator to LArCells.
	* @param[in] sample (deprecated) maintained for compatibility.
	* @param[in] prepare if container should be prepared or
	*	considered as already existing (multilayer HEC or Tile
	*	access).
	*/
    virtual StatusCode LoadCollections (
        LArTT_Selector<LArCellCont>::const_iterator&,
        LArTT_Selector<LArCellCont>::const_iterator&,
        const unsigned int sample = 0, bool prepare=true ) = 0;
	/** 
	* @brief LoadCollections fetches data via ROBDataProvider
	* and really provides ByteStream Conversion by calling 
	* detector specific (LAr or Tile) ByteStream Conversion
	* Tools within RoI as defined in the above method.
	* @param[out] TileCellCollection::const_iterator outputs
	*	begin, end interator to TileCells.
	* @param[in] sample (deprecated) maintained for compatibility.
	* @param[in] prepare if container should be prepared or
	*	considered as already existing (multilayer HEC or Tile
	*	access).
	*/
    virtual StatusCode LoadCollections (
        TileCellCollection::const_iterator&,
        TileCellCollection::const_iterator&,
        const unsigned int sample = 0, bool prepare=true) = 0;
        /** MBTS loading */
    virtual StatusCode LoadMBTS(TileCellCollection::const_iterator&,
                TileCellCollection::const_iterator&) = 0;
        /** Load Zdc Collection */
    virtual StatusCode LoadZdcCollection(ZdcRawChannelCollection::const_iterator&,
                ZdcRawChannelCollection::const_iterator&)=0;

	/** 
	* @brief LoadCollections fetches data via ROBDataProvider
	* and really provides ByteStream Conversion by calling 
	* detector specific (LAr or Tile) ByteStream Conversion
	* Tools within RoI as defined in the above method.
	* @param[out] LArFebEnergyCollection::const_iterator outputs
	*	begin, end interator to LArFebEnergy Collections.
	* @param[in] sample (deprecated) maintained for compatibility.
	* @param[in] prepare if container should be prepared or
	*	considered as already existing (multilayer HEC or Tile
	*	access).
	*/
    virtual StatusCode LoadCollections (
        LArFebEnergyCollection::const_iterator&,
        LArFebEnergyCollection::const_iterator&,
       const unsigned int sample = 0, bool prepare=true) = 0;
        /** 
        * @brief Loads the full collection for the missing et computation
        */
    virtual StatusCode LoadFullCollections (
        LArTT_Selector<LArCellCont>::const_iterator&,
        LArTT_Selector<LArCellCont>::const_iterator&,
        DETID detid, bool prepare=true ) = 0;
        /** 
        * @brief Loads the full collection for the missing et computation
        */
    virtual StatusCode LoadFullCollections (
        TileCellCollection::const_iterator&,
        TileCellCollection::const_iterator&,
        const unsigned int sample = 0, bool prepare=true ) = 0;
        /** 
        * @brief Loads the full collection for the missing et computation
        */
    virtual StatusCode LoadFullCollections (
        LArFebEnergyCollection::const_iterator&,
        LArFebEnergyCollection::const_iterator&,
        DETID detid, bool prepare=true ) = 0;
        /**
        * @brief Loads the full collection for the missing et computation
        */
    virtual StatusCode LoadFullCollections (
                TileL2Container::const_iterator& Begin,
                TileL2Container::const_iterator& End) = 0;
        /**
        * @brief Loads the full collection fast
        */
    virtual StatusCode LoadFullCollections (
                CaloCellContainer::const_iterator&,
                CaloCellContainer::const_iterator&) = 0;
	/** Number of Tile Calorimeter modules found in
	* the Region of Interest */
	virtual unsigned int TileContSize ( void ) = 0;
	/** Number of Tile Calorimeter modules found in
	* the Region of Interest MET case -> All Calo*/
	virtual unsigned int TileFullContSize ( void ) = 0;
        /** begin run handle for conditions */
        virtual StatusCode beginRunHandle(IOVSVC_CALLBACK_ARGS) = 0;
        /** begin run handle for conditions and RegSelSvc */
        virtual StatusCode beginRunHandle_RegSelSvc(IOVSVC_CALLBACK_ARGS) = 0;
	/** Error reporting from BSCnv */
	virtual uint32_t report_error( void ) = 0;
        /** method to store cells in a container with
        a given key. The hottest number cells are placed in the
        container or all if number=-1, no Key makes the default
        key to be RoICells */
	template <class T>
        void
        storeCells(T Begin, T End, CaloCellContainer*& pContainer,
		const float threshold = 0., const uint32_t maxsize=1000 );

	/** Expose list of ROBs */
        virtual void ROBList( const IRoiDescriptor& roi, std::vector<uint32_t>& vec) = 0;

	/// obsolete
        virtual void ROBList( const double /* etamin */, const double /* etamax */, 
			      const double /* phimin */, const double /* phimax */,
			      std::vector<uint32_t>& /* vec */) { } ;

private :
	// Dummy method just to help compilation
	void comp () {
	  LArTT_Selector<LArCellCont>::const_iterator l;
	  TileCellCollection::const_iterator t;
	  CaloCellContainer * cont=NULL;
	  storeCells(l,l,cont);
	  storeCells(t,t,cont);
	}
protected:
};

inline const InterfaceID& ITrigDataAccess::interfaceID()
{
	return IID_ITrigDataAccess;
}

template <class T>
void
ITrigDataAccess::storeCells(T Begin, T End, CaloCellContainer*& pContainer,
       const float threshold, const uint32_t maxsize ){

        if ( pContainer == 0 ){ // If container does not exist, create it
             pContainer = new CaloCellContainer();
        } // End of if contains
        T it;
        uint32_t size = pContainer->size();
        for ( it = Begin; ((it != End) && (size < maxsize)) ; ++it) {
		if ( (*it)->energy() > threshold ) {
                  CaloCell* cell = (CaloCell*)(*it)->clone();
                  pContainer->push_back(cell);
		  size++;
		}
        }
}

#endif
