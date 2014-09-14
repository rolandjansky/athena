/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : LoopbackConverterFromPersistency
 *
 * @brief this tool initializes the HLT from a previous run (HLTResult)
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - UC Irvine
 *
 * File and Version Information:
 * $Id: LoopbackConverterFromPersistency.h,v 1.6 2009-02-16 15:47:04 tbold Exp $
 **********************************************************************************/

#ifndef TRIGSTEERING_LOOPBACKCONVERTERFROMPERSISTENCY_H
#define TRIGSTEERING_LOOPBACKCONVERTERFROMPERSISTENCY_H


#include "TrigSteeringEvent/Enums.h"
#include "TrigSteering/LvlConverter.h"

namespace HLT {

  class Chain;

  /**
     @class LoopbackConverterFromPersistency
     this is a trigger level converter (inherits from LvlConverter), which is used when re-running
     the HLT w/o FEX.
     What happens: it reads in the HLTResult from the previous run, initializes everything from this,
     and then deletes the old HLTResult so that the new one can be saved with the same SG name

     @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
     @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
     @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - UC Irvine
  */
  class LoopbackConverterFromPersistency : public LvlConverter
  {
  public:

    LoopbackConverterFromPersistency(const std::string& name, const std::string& type,
				     const IInterface* parent); //!< std Gaudi constructor

    virtual ErrorCode hltInitialize(); //!< HLT initialize -> do nothing here
    virtual ErrorCode hltFinalize();   //!< HLT finalize   -> do nothing here
    virtual ErrorCode hltExecute(std::vector<HLT::SteeringChain*>& chains); //!< do the work as described in the class desription

    virtual ErrorCode setConfiguredChains(const std::vector<HLT::SteeringChain*>& configuredChains); //!< Setup a search map of all configured chains

    virtual ErrorCode activateChains(std::vector<HLT::SteeringChain*>& chains,
				     const std::vector<HLT::Chain>& previousChains,
				     bool stickyPassThrough=true ); //!< activate chains which were active in the previous run

  private:
    std::map<unsigned int, HLT::SteeringChain*>  m_counterToChain; //!< Map for quick lookup
    bool m_activateAllChains; //!< flag to say if all chains should be activated irrespectively of the fact that they were rejected in previous pass
    bool m_removeOldResult;   //!< flag deciding if old result should be removed
    bool m_unpackNavigation;

    std::string m_resultName;
  };
} // end namespace


#endif
