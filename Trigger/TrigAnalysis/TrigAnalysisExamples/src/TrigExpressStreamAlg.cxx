/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <sys/time.h>

// Framework
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/MsgStream.h"

// Reconstruction
#include "xAODEventInfo/EventInfo.h"

// Local
#include "TrigExpressStreamAlg.h"

#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTStreamTag.h"

//---------------------------------------------------------------------------------------
TrigExpressStreamAlg::TrigExpressStreamAlg(const std::string& name,
					   ISvcLocator* pSvcLocator)
  :AthAlgorithm(name, pSvcLocator),
    m_evtNr(0),
    m_streamPrescale(0.),
    m_prescale(0.),
    m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" )
{
}

//---------------------------------------------------------------------------------------
TrigExpressStreamAlg::~TrigExpressStreamAlg()
{
}

//---------------------------------------------------------------------------------------
StatusCode TrigExpressStreamAlg::initialize()
{    
  
   CHECK( m_trigDec.retrieve() );
   m_trigDec->ExperimentalAndExpertMethods()->enable();
   m_numHLTPassedEvents.clear(); // events firing RAW trigger 
   m_numHLTFailedEvents.clear(); // events failing
   m_numL1PrescaledEvents.clear(); // events prescaled at L1
   m_numHLTPrescaledEvents.clear(); // events prescaled at HLT
    return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode TrigExpressStreamAlg::execute()
{
    ++m_evtNr;
    //
    // Process current event
    //
    const xAOD::EventInfo* evtInfo = 0;
    ATH_CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );

    //
    // Print EventInfo and stream tags
    //

    const std::vector< xAOD::EventInfo::StreamTag > streamTags =evtInfo->streamTags(); 
    bool found_express_stream = false;
    for(const xAOD::EventInfo::StreamTag& stag:streamTags){
        if(stag.type() == "express" && stag.name() == "express") {
            found_express_stream = true;
            break;
        }
    }

    if(!found_express_stream) {
        ATH_MSG_INFO( "Failed to find express stream tag" );
        return StatusCode::SUCCESS;
    }

    const Trig::ExpertMethods *em = m_trigDec->ExperimentalAndExpertMethods();

    // Do we need to check this every event?
    if(m_evtNr==1){
        std::vector<std::string> configuredHLTChains = m_trigDec->getListOfTriggers( "HLT_.*" );
        // Get list of triggers in the Express Stream
        for(const auto& chain:configuredHLTChains){
            const HLT::Chain *aChain=em->getChainDetails(chain);
            const auto trig_conf = m_trigDec->ExperimentalAndExpertMethods()->getChainConfigurationDetails(chain);
            const std::vector<TrigConf::HLTStreamTag*> chainStreams=trig_conf->streams();
            const unsigned int chid=aChain->getChainCounter();
            const float prescale=trig_conf->prescale();
            if(prescale!=m_prescale) m_prescale=prescale; // Update the prescale for this chain
            for(const auto chainStream:chainStreams){
                ATH_MSG_INFO("Found chain " << chain << " chid " << chid << chainStream->stream());
                if(chainStream->stream()=="express"){
                    m_streamTriggers.push_back(chain);
                    // Initialize counter map on first event
                    m_numHLTPassedEvents[chain]=0; 
                    m_numHLTFailedEvents[chain]=0; 
                    m_numL1PrescaledEvents[chain]=0; 
                    m_numHLTPrescaledEvents[chain]=0; 
                }
            }
        }
    }

    // Analyse trigger decision for ES triggers
    // Trigger bits only stored for event not per stream 
    // No way to determine whether a trigger was disabled due to
    // physics prescale or ES prescale
    for(const auto& chain:m_streamTriggers){
        const unsigned int bits = m_trigDec->isPassedBits(chain);
        bool isPassed = m_trigDec->isPassed(chain);

        bool efprescale=bits & TrigDefs::EF_prescaled;
        bool rerun=bits&TrigDefs::EF_resurrected;

        // What about the L1?
        bool tbp = bits&TrigDefs::L1_isPassedBeforePrescale;
        bool tap = bits&TrigDefs::L1_isPassedAfterPrescale;
        // bool tav = bits&TrigDefs::L1_isPassedAfterVeto;

        // What is the combined prescale result
        bool l1prescale=tbp && !tap;
        // bool isPrescale=efprescale || l1prescale;

        const auto trig_conf = m_trigDec->ExperimentalAndExpertMethods()->getChainConfigurationDetails(chain);
        const std::vector<TrigConf::HLTStreamTag*> chainStreams=trig_conf->streams();
        const float prescale=trig_conf->prescale();
        if(prescale!=m_prescale) m_prescale=prescale; // Update the prescale for this chain
        for(const auto chainStream:chainStreams){
            if(chainStream->stream()=="express"){
                if(chainStream->prescale()!=m_streamPrescale){
                    m_streamPrescale=chainStream->prescale();
                    ATH_MSG_INFO("Express Stream chain " << chain 
                            << " Stream prescale " << m_streamPrescale << " prescale " << m_prescale);
                }
            }
        }

        // Raw trigger fires
        if(isPassed && !rerun)
            m_numHLTPassedEvents[chain]+=1; 
        else if (l1prescale)
            m_numL1PrescaledEvents[chain]+=1; 
        else if (efprescale)
            m_numHLTPrescaledEvents[chain]+=1; 
        else
            m_numHLTFailedEvents[chain]+=1;
    }

    // Need to retrieve TrigComposite object to determine whether chain was active in the Express Stream
    // Not enabled yet
    //
    /*ATH_MSG_INFO( ">>>>>>>>>>>>>>>>"
      << " run #" << evtInfo->run_number()
      << " lumi #" << evtInfo->lumi_block()
      << " event #" << evtInfo->event_number() 
      << " has express stream tag" );*/

    return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode TrigExpressStreamAlg::finalize()
{
   for(const std::string& chain:m_streamTriggers){
       ATH_MSG_INFO( " Chain " << chain);
       ATH_MSG_INFO( "Number of events with a HLT passed probe muon: " << m_numHLTPassedEvents[chain] );
       ATH_MSG_INFO( "Number of events with a HLT passed probe muon: " << m_numL1PrescaledEvents[chain] );
       ATH_MSG_INFO( "Number of events with a HLT passed probe muon: " << m_numHLTPrescaledEvents[chain] );
       ATH_MSG_INFO( "Number of events with a HLT passed probe muon: " << m_numHLTFailedEvents[chain] );
   }
    return StatusCode::SUCCESS;
}
