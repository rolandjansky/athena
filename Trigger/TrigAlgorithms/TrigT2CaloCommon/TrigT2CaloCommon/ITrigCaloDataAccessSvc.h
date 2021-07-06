/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigT2CaloCommon_ITrigCaloDataAccessSvc_h
#define TrigT2CaloCommon_ITrigCaloDataAccessSvc_h

#include "LArRawUtils/LArTT_Selector.h"
#include "TrigT2CaloCommon/LArCellCont.h"
#include "TileEvent/TileCellCollection.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "IRegionSelector/RegSelEnums.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/StatusCode.h"


class ITrigCaloDataAccessSvc: virtual public IService {
 public:
  /** Interface for Virtual Class */
  DeclareInterfaceID(ITrigCaloDataAccessSvc, 1, 0);
  
  /** 
   * @brief downloads the LAr data for an RoI and makes sure the cache collection is filled wiht decoded cells   
   */
  virtual StatusCode loadCollections( const EventContext& context,
                                      const IRoiDescriptor& roi,
                                      const DETID detId, const int sampling,
                                      LArTT_Selector<LArCellCont>& loadedCells ) = 0;
	/* /\**  */
	/* * @brief LoadCollections fetches data via ROBDataProvider */
	/* * and really provides ByteStream Conversion by calling  */
	/* * detector specific (LAr or Tile) ByteStream Conversion */
	/* * Tools within RoI as defined in the above method. */
	/* * @param[out] TileCellCollection::const_iterator outputs */
	/* *	begin, end interator to TileCells. */
	/* * @param[in] sample (deprecated) maintained for compatibility. */
	/* * @param[in] prepare if container should be prepared or */
	/* *	considered as already existing (multilayer HEC or Tile */
	/* *	access). */
	/* *\/ */
	virtual StatusCode loadCollections (const EventContext& context,
					    const IRoiDescriptor& roi,
	 				    TileCellCollection&) = 0;

  virtual StatusCode loadMBTS ( const EventContext& context,
                                TileCellCollection& )=0;


  /**
   * @brief Loads the full collection for the missing et computation
   */
  virtual StatusCode loadFullCollections ( const EventContext& context,
                                           CaloConstCellContainer& cont ) = 0;

  /*
    method to store cells in a container with
	a given key. The hottest number cells are placed in the
    container or all if number=-1, no Key makes the default
    key to be RoICells
  */
  template <class T>
  void  storeCells( const EventContext& context, T Begin, T End, CaloCellContainer*& pContainer,
                    const float threshold = 0., const uint32_t maxsize=1000 );

private :
	// Dummy method just to help compilation
	void comp (const EventContext& context) {
	  LArTT_Selector<LArCellCont>::const_iterator l;
	  TileCellCollection::const_iterator t;
	  CaloCellContainer * cont=NULL;
	  storeCells(context, l,l,cont);
	  storeCells(context, t,t,cont);
	}
protected:
};

#endif
