// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         TrigConfDataIOVChanger.h
 **
 **   Description:  - Algorithm-derived class to set the IOV of the trigger
 **                   configuration meta data according to the run number
 **
 * @author Joerg Stelzer   <Joerg.Stelzer@cern.ch>  - CERN
 **
 **   CVS ID:       $Id: TrigConfDataIOVChanger.h,v 1.2 2008-03-03 16:51:43 stelzer Exp $
 **   Created:      Dec 05 2007
 **
 **************************************************************************/
#ifndef TrigConfigSvc_TrigConfDataIOVChanger_h
#define TrigConfigSvc_TrigConfDataIOVChanger_h

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// core stuff
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

// containers
#include <vector>
#include <string>
#include <map>

class CondAttrListCollection;

namespace TrigConf {

  /**
   * @class TrigConfDataIOVChanger - Algorithm-derived class to be
   * used when producing ESD/AOD using Trigger Menu configuration from
   * xml. In this case the menu.xml is converted into COOL/SQlite.db
   * before the start of the run and read from the SQlite file using
   * the IOVDbSvc and the metadata detector store. Since the run
   * number is unknown at the beginning before the start of the run,
   * the IOV associated with the configuration data is set to
   * [ValidityKeyMin,ValidityKeyMax). This algorithm reads the run
   * number from the first event and modifies the IOV of the
   * configuration metadata in the output metadata store, before it is
   * written to AOD.
   * 
   * @author Joerg Stelzer   <Joerg.Stelzer@cern.ch>  - CERN
   */

  class TrigConfDataIOVChanger : public AthAlgorithm {
  public:
    
    TrigConfDataIOVChanger(const std::string &name, ISvcLocator *pSvcLocator); //!< std Gaudi Algorithm constructor
    ~TrigConfDataIOVChanger();                                                 //!< std  deconstructor

    // IAlgorithm virtual methods to implement
    StatusCode initialize(); //!< std Gaudi initialize method -> read-in trigger configuration
    StatusCode execute();     //!< std Gaudi execute method  -> fill event-wise TrigDecision object, save in SG
    StatusCode finalize();    //!< std Gaudi finalize method -> print out statistics

  private:

    bool CheckIOV(const CondAttrListCollection* coll) const;
    void ChangeIOV(const CondAttrListCollection* coll, unsigned int runNumber) const;
    CondAttrListCollection* CreatePayload(const CondAttrListCollection* coll, unsigned int runNumber) const;
    void printOut(const CondAttrListCollection* coll) const;

    bool m_AdjustIOV; //!< Set to true if the IOV needs adjustment (MC production), false for ESD/AOD -> AOD jobs

    unsigned int m_LastRun; //!< the run number of the event last executed

    ServiceHandle<StoreGateSvc> m_storeGate; //!< handle to SG service
    ServiceHandle<StoreGateSvc> m_metaDataStore; //!< handle to output metadata store
    ServiceHandle<StoreGateSvc> m_detStore; //!< handle to the detector store
    
  };
}

#endif
