// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCostD3PDMakerTool.h 603620 2014-06-25 17:55:37Z tamartin $
#ifndef TRIGGERD3PDMAKER_TRIGCOSTD3PDMAKERTOOL_H
#define TRIGGERD3PDMAKER_TRIGCOSTD3PDMAKERTOOL_H

// STL include(s):
#include <map>
#include <string>
#include <set>

// Gaudi/Athena include(s):
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

// D3PDMaker include(s):
#include "D3PDMakerInterfaces/IMetadataTool.h"
#include "D3PDMakerInterfaces/ID3PDSvc.h"

namespace D3PD {

    // Forward declaration(s):
    class ID3PD;

    /**
    * @short D3PDMaker package to write out TrigMonEvent objects to flat ntuple.
    * As these are written out to the bytestream en-bulk, have to use tricks from the 
    * TrigConfMetadata writer and manually write out the TTree with the D3PD metadata tools
    * rather than using a standard D3PDMaker alg.
    *
    * Hence we do not get one entry per event, we get one entry for every TrigMonEvent object 
    * (represnets an event in the HLT).
    */
    class TrigCostD3PDMakerTool : public AthAlgTool,
                                  public IMetadataTool,
                                  public IIncidentListener {

      public:
        /// Regular AlgTool constructor
        TrigCostD3PDMakerTool( const std::string& type, const std::string& name,
        const IInterface* parent );

        /// AlgTool initialization function
        virtual StatusCode initialize();
        /// Gaudi function describing which interfaces the tool implements
        virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );

        /// Function writing the configuration information to the D3PD
        virtual StatusCode writeMetadata( ID3PD* d3pd );

        /// Function called when a relevant incident happened
        virtual void handle( const Incident& inc );

      private:
        /// Function handling new trigger configurations
        void handleNewEvent( const Incident& inc );

        void clear();

        /// Directory in which to store the data
        std::string m_dir;

        /// Handle for the incident service
        ServiceHandle< IIncidentSvc > m_incidentSvc;
        /// Handle for the D3PDSvc:
        ServiceHandle< ID3PDSvc > m_d3pdSvc;

        /// Storegate key for TrigMonEvent objects
        std::string m_keyEvent;
        /// D3PD prefix
        std::string m_prefix;

        //
        // The configuration variables:
        //
        ID3PD* m_tree; ///< D3PD object holding the configuration description
        bool m_onlySaveCostEvents; //!< Export non cost events?
        std::string m_writeMode; //!< What to export
        bool m_doBasic;
        bool m_doExtra;
        bool m_doChain;
        bool m_doL1;
        bool m_doROS;
        bool m_doROSSum;
        bool m_doRoI;
        bool m_doSeq;
        bool m_doTE;
        bool m_doDBKey;
        bool m_doEB;

        //////////////
        //BASIC DATA//
        //////////////
        uint32_t* m_event;              // Event number
        uint32_t* m_lumi;               // Lumi block (low 16 bits of TrigMonEvent.m_lumi) 
        float*    m_lumiLength;         // Lumi block length in seconds
        uint32_t* m_bunch_id;           // bunch crossing id (high 16 bits of TrigMonEvent.m_lumi)
        uint32_t* m_run;                // Run number
        uint32_t* m_sec;                // Event second
        uint32_t* m_nsec;               // Event ns
        float*    m_timer;              // Event timer
        float*    m_ebWeight;           // Enhanced bias weighting factor
        uint32_t* m_ebWeightBG;         // Enhanced bias weight bunch group
        uint8_t*  m_ebUnbiased;         // Enhanced Bias flag for unbiased online events
        uint8_t*  m_ranScaleTools;      // If this was a monitoring event
        uint32_t* m_appId;              // Application ID - hash of trigger XPU node

        //////////////
        //CHAIN DATA//
        //////////////
        uint16_t*                m_chainN; 
        std::vector< uint16_t >* m_chainCounter;
        std::vector< uint16_t >* m_chainLevel;    
        std::vector< float >*    m_chainTimer;         
        std::vector< uint8_t >*  m_chainIsPassed;   
        std::vector< uint8_t >*  m_chainIsPassedRaw;
        std::vector< uint8_t >*  m_chainIsPassthrough;
        std::vector< uint8_t >*  m_chainIsResurrected;
        std::vector< uint8_t >*  m_chainIsPrescaled;
        std::vector< uint8_t >*  m_chainWasL1AfterVeto;
        std::vector< uint8_t >*  m_chainWasL1BeforePrescale;
        std::vector< uint8_t >*  m_chainWasL1AfterPrescale;
        std::vector< uint8_t >*  m_chainIsExpressStream;

        //////////////
        //EXTRA DATA//
        //////////////
        float*    m_timerTrigCost;      // Detailed time - Time to process TrigCost OPI tools
        float*    m_timerEndSteer;      // Detailed time - End of OPI steering
        float*    m_timerChainProcess;  // Detailed time - chain processing
        float*    m_timerResultBuilder; // Detailed time - result building
        float*    m_timerMon;           // Detailed time - monitoring 
        float*    m_costEvent;          // Sequential cost event number
        uint32_t* m_costRunSec;         // Cost processing time second
        uint32_t* m_costRunNsec;        // Cost processing time ns

        ///////////
        //L1 DATA//
        ///////////
        uint16_t*                m_l1N; 
        std::vector< uint16_t >* m_l1CtpId;
        std::vector< uint8_t >*  m_l1PassedBeforePrescale;    
        std::vector< uint8_t >*  m_l1PassedAfterPrescale;    
        std::vector< uint8_t >*  m_l1PassedAfterVeto;
        std::vector< uint8_t >*  m_l1Passed;
        std::vector< uint8_t >*  m_l1Prescaled;
        std::vector< uint8_t >*  m_l1Vetoed; 

        ////////////
        //ROS DATA//
        ////////////
        uint16_t*                               m_robN;
        std::vector< uint32_t >*                m_robRequestorId;
        std::vector< double >*                  m_robTimer;
        std::vector< uint32_t >*                m_robTimeStartSec;
        std::vector< uint32_t >*                m_robTimeStartMicroSec;
        std::vector< uint32_t >*                m_robTimeStopSec;
        std::vector< uint32_t >*                m_robTimeStopMicroSec;
        std::vector< uint16_t >*                m_robDataN;
        std::vector< std::vector< uint32_t > >* m_robDataRobId;
        std::vector< std::vector< uint32_t > >* m_robDataRobSize;
        std::vector< std::vector< uint8_t > >*  m_robDataIsUnclassified;
        std::vector< std::vector< uint8_t > >*  m_robDataIsCached;
        std::vector< std::vector< uint8_t > >*  m_robDataIsRetrieved;
        std::vector< std::vector< uint8_t > >*  m_robDataIsIgnored;
        std::vector< std::vector< uint8_t > >*  m_robDataIsDisabled;
        std::vector< std::vector< uint8_t > >*  m_robDataIsStatusOk;
        std::vector< std::vector< uint8_t > >*  m_robDataIsPrefetched;   
        std::vector< uint16_t >*                m_robSumDataN;
        std::vector< std::vector< uint32_t > >* m_robSumDataSize;
        std::vector< std::vector< uint32_t > >* m_robSumDataNRob;
        std::vector< std::vector< uint32_t > >* m_robSumDataSubDet;  
        std::vector< std::vector< uint8_t > >*  m_robSumDataIsUnclassified; 
        std::vector< std::vector< uint8_t > >*  m_robSumDataIsCached;
        std::vector< std::vector< uint8_t > >*  m_robSumDataIsRetrieved;
        std::vector< std::vector< uint8_t > >*  m_robSumDataIsIgnored;
        std::vector< std::vector< uint8_t > >*  m_robSumDataIsDisabled;

        ////////////
        //RoI DATA//
        ////////////
        uint16_t*                 m_roiN;
        std::vector< uint8_t >*   m_roiIsNoneType;
        std::vector< uint8_t >*   m_roiIsMuonType;
        std::vector< uint8_t >*   m_roiIsEmTauType;
        std::vector< uint8_t >*   m_roiIsTau;
        std::vector< uint8_t >*   m_roiIsJetType;
        std::vector< uint8_t >*   m_roiIsJetEtType;
        std::vector< uint8_t >*   m_roiIsEnergyType;
        std::vector< uint32_t >*  m_roiId;
        std::vector< uint8_t >*   m_roiNL1thresholds;
        std::vector< float >*     m_roiEta;
        std::vector< float >*     m_roiPhi;
        std::vector< float >*     m_roiArea;
        //
        std::vector< float >*     m_roiEt;
        std::vector< float >*     m_roiEtLarge;
        std::vector< uint8_t >*   m_roiMuCharge;
        std::vector< uint32_t >*  m_roiIsolationBits;
        std::vector< float >*     m_roiVectorEX;
        std::vector< float >*     m_roiVectorEY;
        std::vector< uint8_t >*   m_roiOverflowEX;
        std::vector< uint8_t >*   m_roiOverflowEY;
        std::vector< uint8_t >*   m_roiOverflowET;

        ////////////////
        //SEQ/ALG DATA//
        ////////////////
        uint16_t*                               m_seqN;
        std::vector< uint8_t >*                 m_seqIsInitial;
        std::vector< uint8_t >*                 m_seqIsExecuted;
        std::vector< uint8_t >*                 m_seqIsAlreadyExecuted;
        std::vector< uint8_t >*                 m_seqIsPrevious;
        std::vector< uint16_t >*                m_seqLevel;
        std::vector< uint16_t >*                m_seqChannelCounter;
        std::vector< uint16_t >*                m_seqIndex; 
        std::vector< float >*                   m_seqSequenceTimer; 
        std::vector< double >*                  m_seqAlgTimerTot; 
        std::vector< uint16_t >*                m_seqAlgN; 
        std::vector< std::vector< uint8_t > >*  m_seqAlgPosition; 
        std::vector< std::vector< uint8_t > >*  m_seqAlgNRoIs; // RoI's are held in another varaible
        std::vector< std::vector< uint32_t > >* m_seqAlgRoIIndex;
        std::vector< std::vector< uint8_t > >*  m_seqRoI; // Hold the RoI data
        std::vector< std::vector< uint8_t > >*  m_seqAlgIsCached;
        std::vector< std::vector< uint8_t > >*  m_seqAlgIsCalled;
        std::vector< std::vector< double > >*   m_seqAlgTimer; 
        std::vector< std::vector< uint32_t > >* m_seqAlgTimeStartSec; 
        std::vector< std::vector< uint32_t > >* m_seqAlgTimeStartMicroSec; 
        std::vector< std::vector< uint32_t > >* m_seqAlgTimeStopSec; 
        std::vector< std::vector< uint32_t > >* m_seqAlgTimeStopMicroSec; 

        ///////////
        //TE DATA//
        ///////////
        uint16_t*                                 m_teN; 
        std::vector< uint16_t >*                  m_teId;
        std::vector< uint16_t >*                  m_teIndex;
        std::vector< uint8_t >*                   m_teIsRegularTE;
        std::vector< uint8_t >*                   m_teIsInitialTE;
        std::vector< uint8_t >*                   m_teIsRoITE;
        std::vector< uint8_t >*                   m_teIsL1ThreshTE;
        std::vector< uint8_t >*                   m_teActiveState;
        std::vector< uint8_t >*                   m_teErrorState;
        std::vector< uint8_t >*                   m_teIsTerminalNode;
        std::vector< uint8_t >*                   m_teIsOutputL2Node;
        std::vector< uint8_t >*                   m_teIsOutputEFNode;
        std::vector< uint8_t >*                   m_teIsTopologicalTE;
        std::vector< std::vector< uint16_t > >*   m_teChildIndex;
        std::vector< std::vector< uint16_t > >*   m_teParentIndex;
        std::vector< std::vector< uint8_t > >*    m_teRoiId;
        std::vector< std::vector< uint32_t > >*   m_teClid;

        ///////////
        //DB DATA//
        ///////////
        uint32_t* m_DB_SMK;
        uint32_t* m_DB_L1PSK;
        uint32_t* m_DB_HLTPSK;

    }; // class TrigCostD3PDMakerTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_TRIGCOSTD3PDMAKERTOOL_H
