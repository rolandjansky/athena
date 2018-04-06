/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigCaloDataAccessSvc_ITrigCaloDataAccessSvc_h
#define TrigCaloDataAccessSvc_ITrigCaloDataAccessSvc_h

#include "LArRawUtils/LArTT_Selector.h"
#include "TileByteStream/TileCellCont.h"
#include "TrigT2CaloCommon/LArCellCont.h"
#include "LArRecEvent/LArFebEnergyCollection.h"
#include "TileEvent/TileCellCollection.h"
#include "TileEvent/TileL2Container.h"
#include "IRegionSelector/RegSelEnums.h"
#include "CaloEvent/CaloCellContainer.h"
#include "ZdcEvent/ZdcRawChannelCollection.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/StatusCode.h"

// For callbacks
#include "AthenaKernel/IOVSvcDefs.h"

class ITrigCaloDataAccessSvc: virtual public IService {
 public:
  /** Interface for Virtual Class */
  DeclareInterfaceID(ITrigCaloDataAccessSvc, 1, 0);
  
  /** Enum for StatusCode */
  enum class Status : StatusCode::code_t {
    // Gaudi defaults
    FAILURE          = 0,
    SUCCESS          = 1,
    RECOVERABLE      = 2,
    // Our own error codes
    MissingROB       = 0x10000000,
    RawDataCorrupted = 0x20000000,
    InternalError    = 0x40000000,
    // Masks and bitshifts
    StatusMask       = 0xff000000,  // 1 byte for our own error codes
    LArDecoderMask   = 0x00000ff0,  // 1 byte for LAr decoder error
    LArDecoderShift  = 4,           // error shift (relative to LArRodDecoder::report_error)
    TileDecoderMask  = 0x00ff0000,  // 1 byte for Tile decoder error
    TileDecoderShift = 0            // error shift (relative to TileRodDecoder::report_error)
  };

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
	/* virtual ExtendedStatusCode loadCollections (const EventContext& context, */
	/* 					    TileCellCollection::const_iterator&, */
	/* 					    TileCellCollection::const_iterator&, */
	/* 					    const unsigned int sample = 0, bool prepare=true) = 0; */
        /* /\** MBTS loading *\/ */
	/* virtual ExtendedStatusCode loadMBTS(const EventContext& context, */
	/* 				    TileCellCollection::const_iterator&, */
	/* 				    TileCellCollection::const_iterator&) = 0; */
        /* /\** Load Zdc Collection *\/ */
	/* virtual ExtendedStatusCode loadZdcCollection( const EventContext& context, */
	/* 					      ZdcRawChannelCollection::const_iterator&, */
	/* 					      ZdcRawChannelCollection::const_iterator&)=0; */

	/* /\** */
	/* * @brief LoadCollections fetches data via ROBDataProvider */
	/* * and really provides ByteStream Conversion by calling */
	/* * detector specific (LAr or Tile) ByteStream Conversion */
	/* * Tools within RoI as defined in the above method. */
	/* * @param[out] LArFebEnergyCollection::const_iterator outputs */
	/* *	begin, end interator to LArFebEnergy Collections. */
	/* * @param[in] sample (deprecated) maintained for compatibility. */
	/* * @param[in] prepare if container should be prepared or */
	/* *	considered as already existing (multilayer HEC or Tile */
	/* *	access). */
	/* *\/ */
	/* virtual ExtendedStatusCode loadCollections ( const EventContext& context, */
	/* 					     LArFebEnergyCollection::const_iterator&, */
	/* 					     LArFebEnergyCollection::const_iterator&, */
	/* 					     const unsigned int sample = 0, bool prepare=true) = 0; */
        /**
        * @brief Loads the full collection for the missing et computation
        */

  virtual StatusCode prepareFullCollections( const EventContext& context ) = 0;
  
  virtual StatusCode loadFullCollections ( const EventContext& context,
                                           ConstDataVector<CaloCellContainer>& cont ) = 0;

        /* /\** */
        /* * @brief Loads the full collection for the missing et computation */
        /* *\/ */
	/* virtual ExtendedStatusCode loadFullCollections ( const EventContext& context, */
	/* 						 TileCellCollection::const_iterator&, */
	/* 						 TileCellCollection::const_iterator&, */
	/* 						 const unsigned int sample = 0, bool prepare=true ) = 0; */
        /* /\** */
        /* * @brief Loads the full collection for the missing et computation */
        /* *\/ */
	/* virtual ExtendedStatusCode loadFullCollections ( const EventContext& context, */
	/* 						 LArFebEnergyCollection::const_iterator&, */
	/* 						 LArFebEnergyCollection::const_iterator&, */
	/* 						 DETID detid, bool prepare=true ) = 0; */
        /* /\** */
        /* * @brief Loads the full collection for the missing et computation */
        /* *\/ */
	/* virtual ExtendedStatusCode loadFullCollections ( const  EventContext& context, */
	/* 						 TileL2Container::const_iterator& Begin, */
	/* 						 TileL2Container::const_iterator& End ) = 0; */
        /* /\** */
        /* * @brief Loads the full collection fast */
        /* *\/ */
	/* virtual ExtendedStatusCode loadFullCollections ( const EventContext& context, */
	/* 						 CaloCellContainer::const_iterator&, */
	/* 						 CaloCellContainer::const_iterator& ) = 0; */

	
	/* /\** */
	/*  * //TB numberof Tile identifiers */
	/*  *  Number of Tile Calorimeter modules found in */
	/*  * the Region of Interest *\/ */
	/* virtual unsigned int TileContSize ( void ) const = 0; */

	/* /\** */
	/*  * //TB afain this is not depending on RoIs */
	/*  * Number of Tile Calorimeter modules found in */
	/*  * the Region of Interest MET case -> All Calo*\/ */
	/* virtual unsigned int TileFullContSize ( void ) const = 0; */

	/**
	 * //TB errors cound should be accosiated with each conversion step
	 * this is now returned from each loadCollection method
	 * Error reporting from BSCnv 
	 */
	
	//virtual uint32_t report_error( void ) = 0;

	
	/* //TB this methods should be updated/replaced and use new conditions infrastructure */
        /* /\** begin run handle for conditions *\/ */
        /* virtual StatusCode beginRunHandle(IOVSVC_CALLBACK_ARGS) = 0; */
        /* /\** begin run handle for conditions and RegSelSvc *\/ */
        /* virtual StatusCode beginRunHandle_RegSelSvc(IOVSVC_CALLBACK_ARGS) = 0; */

	/*
        method to store cells in a container with
	a given key. The hottest number cells are placed in the
	  container or all if number=-1, no Key makes the default
	  key to be RoICells 
	*/
	
	template <class T>
	  void  storeCells( const EventContext& context, T Begin, T End, CaloCellContainer*& pContainer,
		const float threshold = 0., const uint32_t maxsize=1000 );
	
	/*
	Expose list of ROBs
	 // TB unclear if this is meant to be the list of ROBs already downloaded or all robs covering RoI,
	 // if the former then it makes no sense in MT, if the later then context is not needed
	 */
	//	virtual void ROBList( const IRoiDescriptor& roi, std::vector<uint32_t>& vec) const = 0;


private :
	// Dummy method just to help compilation
	void comp (const EventContext& context) {
	  LArTT_Selector<LArCellCont>::const_iterator l;
	  TileCellCollection::const_iterator t;
	  CaloCellContainer * cont=NULL;
	  l=l;
	  t=t;
	  storeCells(context, l,l,cont);
	  storeCells(context, t,t,cont);
	}
protected:
};

// Register enum as StatusCode
STATUSCODE_ENUM_DECL(ITrigCaloDataAccessSvc::Status)

#endif
