/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : ResultBuilder
 *
 * @brief : tool to create the HLTResult at the end of every HLT event processing
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - UC Irvine
 *
 * File and Version Information:
 * $Id: ResultBuilder.h,v 1.20 2009-01-29 16:37:35 tbold Exp $
 **********************************************************************************/

#ifndef TRIGSTEERING_RESULTBUILDER_H
#define TRIGSTEERING_RESULTBUILDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include <inttypes.h>
#include "TrigSteeringEvent/Enums.h"
#include "EventInfo/TriggerInfo.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigSteering/IStreamingStrategy.h"
#include "TrigSteering/IExecutionOrderStrategy.h"

// forward declarations
class MsgStream;


namespace HLT {

  class SteeringChain;
  class HLTResult;
  class AlgoConfig;

  static const InterfaceID IID_ResultBuilder("HLT::ResultBuilder", 1, 0); //!< Gaudi interface


 /**
     @class ResultBuilder
     tool to create the HLTResult at the end of every HLT event processing

     @author Till Eifert     <Till.Eifert@cern.ch>
     @author Nicolas Berger  <Nicolas.Berger@cern.ch>
     @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - UC Irvine
*/

  class ResultBuilder: public AthAlgTool
  {
  public:
    ResultBuilder(const std::string& name, const std::string& type,
		  const IInterface* parent); //!< std Gaudi constructor

    virtual ~ResultBuilder(); //!< destructor -> do nothing

    StatusCode initialize(); //!< Gaudi initialize
    StatusCode finalize();   //!< Gaudi finalize

    /**
     * @brief method filling EventInfo with updated TriggerInfo
     **/
    ErrorCode fillTriggerInfo(const std::vector<SteeringChain*>& activeChainsm );         

    /** @brief do the actual work:
	at the end of each event, this method is called by the TrigSteer algorithm.
	Inside here, the HLTResul object is created, all the trigger information from
	this trigger level is put into the HLTResult (in compressed form).

	@param activeChains all Chains that were used in this trigger level, i.e. that
	                    were activated in the beginning by the level converter tool.
	@warning always need to call fillTriggerInfo before this
    */
    ErrorCode hltExecute(const std::vector<HLT::SteeringChain*>& activeChains );

    void setConfig(AlgoConfig* config) { m_config = config; } //!< set global config object pointer
    virtual ErrorCode setConfiguredChains(const std::vector<HLT::SteeringChain*>& configuredChains); //!< Setup a search map of all configured chains
    virtual ErrorCode setConfigurationKeys(uint32_t supermaster, uint32_t prescales);
    virtual std::vector<TriggerInfo::StreamTag> getErrorStreamTags() const;

    static const InterfaceID& interfaceID() { return  IID_ResultBuilder; } //!< Gaudi interface


 private:

    AlgoConfig* m_config;  //!< global config object

    ToolHandle<IStreamingStrategy> m_streamingStrategy; //!< tool which implements streaming policies

    //    bool m_doStreamTags;    //!< jobProperty: put HLT StreamTags into HLTResult?
    //    bool m_doTriggerTypes;  //!< jobProperty: put HLT TriggerTypes into HLTResult?
    
    void setTriggerTypeBit(unsigned int bit, std::vector<uint32_t>& bitarray ); //!< utility method to set bits in the array of integers
    unsigned int m_highestTriggerTypeBit; //!< highest TriggerType bit -> therefore size of the bitarray

    std::map<HLT::ErrorCode, TriggerInfo::StreamTag> m_errorStreamTags; //!< map of errors to StreamTags, this is filled by internally!
    std::vector<std::string> m_errorStreamTagsProperty; //!< jobProperty: mapping of errors to StreamTags
    std::string m_defaultStreamTagForErrors;

    std::vector<TriggerInfo::StreamTag> m_uniqueStreams; //!< unique list of streams in this event

    StatusCode decodeErrorStreamTagsProperty(); //!< method to evaluate the mapping of errors to StreamTags

    unsigned int m_infoSize;

    uint32_t m_superMasterKey{0}; 
    uint32_t m_prescalesKey{0}; 

    bool m_buildAlways; //!< forces event building on every event (also the rejected)
    std::map<unsigned int, std::set<std::pair<CLID, std::string> > > m_modId_clid_name; // map from merged ScoutingInfo

  };
} // end namespace
#endif
