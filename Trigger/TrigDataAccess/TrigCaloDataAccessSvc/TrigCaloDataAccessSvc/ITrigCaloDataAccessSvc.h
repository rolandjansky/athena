/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

// For callbacks
#include "AthenaKernel/IOVSvcDefs.h"

class ITrigCaloDataAccessSvc: virtual public IService {
 public:
  /** Interface for Virtual Class */
  DeclareInterfaceID(ITrigCaloDataAccessSvc, 1, 0);
  
  class Status {
  public:
    Status() {}
    Status( uint32_t x) : m_mask( x ) {}

    //! possible, predefined errors
    enum Mask { MissingROB = 0x10000000, RawDataCorrupted = 0x20000000, InternalError = 0x40000000 };
    
    inline void addError( uint32_t m ) { m_mask |= m; }
    inline bool hasError( uint32_t m ) const { return m & m_mask; }
    inline bool success() const { return mask() == 0; }

    //!< to be usable with the ATH_CHECK
    inline operator StatusCode() const { 
      return success() ? StatusCode::SUCCESS : StatusCode::FAILURE;  
    }
    void operator += ( const Status& rhs ) { m_mask &= rhs.m_mask; }
    //!< raw mask
    inline uint32_t mask() const { return m_mask; }
  private:
    uint32_t m_mask = 0;
  };

  /**
   * Will we ever need layer here
   */
  virtual Status prepareCollections( const EventContext& context,
				     const IRoiDescriptor& roi, 
				     DETID detector ) = 0;


  
  /** 
   * @brief downloads the LAr data for an RoI and makes sure the cache collection is filled wiht decoded cells   
   */
  virtual Status loadCollections( const EventContext& context,
				  const IRoiDescriptor& roi,
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

  virtual Status prepareFullCollections( const EventContext& context, DETID detid ) = 0;
  
  virtual Status loadFullCollections ( const EventContext& context,
				       DETID detid,
				       LArTT_Selector<LArCellCont>::const_iterator& begin,
				       LArTT_Selector<LArCellCont>::const_iterator& end ) = 0;

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
        /*
        template <class T>
        void  storeCells( const EventContext& context, T Begin, T End, CaloCellContainer*& pContainer,
        const float threshold = 0., const uint32_t maxsize=1000 );
        */	
	/*
	Expose list of ROBs
	 // TB unclear if this is meant to be the list of ROBs already downloaded or all robs covering RoI,
	 // if the former then it makes no sense in MT, if the later then context is not needed
	 */
	//	virtual void ROBList( const IRoiDescriptor& roi, std::vector<uint32_t>& vec) const = 0;


private :
	// Dummy method just to help compilation
  /*
	void comp (const EventContext& context) {
	  LArTT_Selector<LArCellCont>::const_iterator l;
	  TileCellCollection::const_iterator t;
	  CaloCellContainer * cont=NULL;
	  l=l;
	  t=t;
	  storeCells(context, l,l,cont);
	  storeCells(context, t,t,cont);
	}
  */
protected:
};

#endif
