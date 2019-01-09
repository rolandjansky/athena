/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef IDPERFMON_SERVICES_H
#define IDPERFMON_SERVICES_H

#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Algorithm.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"

class PerfMonServices
{
 public:
  // Constructors and Destructors.
  PerfMonServices();
  ~PerfMonServices();

  // One off initialise function.
  static StatusCode     InitialiseServices( ISvcLocator* pxSvcLocator );

  // Accessor functions for the various Atlas services.
  static StoreGateSvc*  getStoreGateSvc()        { return s_pxStoreGateSvc; }
  static IMessageSvc*   getMessagingService()    { return s_pxMessageSvc;   }
  static INTupleSvc*    getRootTupleService()    { return s_pxNtupleSvc;    }
  static ITHistSvc*     getHistogramService()    { return s_pxTHistSvc;     }
  //static IAtRndmGenSvc* getRandomNumberService() { return s_pxAtRndmGenSvc; }

  enum CONTAINERS
  {
    MUON_START = 0,
    MUID_COLLECTION   = MUON_START,
    STACO_COLLECTION,
    MUON_COLLECTION,
		MUON_END,
    
    ELEC_START = MUON_END,
    ELECTRON_COLLECTION = ELEC_START,
    ELEC_END,
    
    PHOT_START = ELEC_END,
    PHOTON_COLLECTION = PHOT_START,
    PHOT_END,

    MET_START = PHOT_END,
    MET_COLLECTION = MET_START,
    MET_END,
    
    TRK_START = MET_END,
    TRK_COLLECTION = TRK_START,
    TRK_END,
   
    VTX_START = TRK_END,
    VTX_COLLECTION = VTX_START,
    VTX_END,
    
    NUM_CONTAINERS = VTX_END
  };

  static const std::string&  getContainerName( CONTAINERS eContainer )         { return s_sContainerNames[eContainer]; }
  static const std::string&  getAtlfastContainerName( CONTAINERS eContainer )  { return s_sAtlfastContainerNames[eContainer]; }

  template <class T>  static const T* getContainer( CONTAINERS eContainer )
  {
    const T* pxContainer = NULL;
    const std::string sContainerName = PerfMonServices::getContainerName( eContainer );

    if ( s_pxStoreGateSvc )
    {
      StatusCode xStatus = s_pxStoreGateSvc->retrieve( pxContainer , sContainerName );
      if ( CheckServiceInit( xStatus, !pxContainer /* , sContainerName */ ) )
      {
    	return pxContainer;
      }
      else
      { // if that failed then try the Atlfast types.
	//pxContainer = NULL;
	//const std::string sAtlfastContainerName = Services::getAtlfastContainerName( eContainer );

	//if ( CheckServiceInit( xStatus, !pxContainer, sAtlfastContainerName ) )
	//{
	//  return pxContainer;
	//}
      }
    }
    return NULL;
  }

 protected:

 private:
  // Static variables.
  // The service locate. Pointer to all the other services.
  static ISvcLocator*    s_pxSvcLocator;

  // Pointers to various services in the Atlas software.
  static IMessageSvc*    s_pxMessageSvc;
  static StoreGateSvc*   s_pxStoreGateSvc;
  static INTupleSvc*     s_pxNtupleSvc;
  static ITHistSvc*      s_pxTHistSvc;
  static IAtRndmGenSvc*  s_pxAtRndmGenSvc;

  // Internal initialisers used by the main InitialiseServices function.
  static bool InitialiseMessaging();
  static bool InitialiseStoreGate();
  static bool InitialiseRootTuple();
  static bool InitialiseHistograms();
  static bool InitialiseRandom();

  static bool CheckServiceInit( const StatusCode& xSC, bool bNullPointer /*, const std::string& sType */ );

  // Names of the various object containers.
  static const std::string s_sContainerNames[NUM_CONTAINERS];
  static const std::string s_sAtlfastContainerNames[NUM_CONTAINERS];
};

#endif
