/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
#ifndef NSWL1_PADTRIGGERLOGICOFFLINETOOL_H
#define NSWL1_PADTRIGGERLOGICOFFLINETOOL_H

//basic includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Property.h"

//local includes
#include "TrigT1NSWSimTools/IPadTriggerLogicTool.h"
#include "TrigT1NSWSimTools/PadTriggerValidationTree.h"
#include "TrigT1NSWSimTools/L1TdrStgcTriggerLogic.h"
#include "TrigT1NSWSimTools/TriggerTypes.h"


//forward declarations
class IIncidentSvc;
class IAtRndmGenSvc;
class TTree;
class sTgcDigitContainer;

namespace CLHEP {
class HepRandomEngine;
}

namespace MuonGM {
class MuonDetectorManager;
}

namespace NSWL1 {
    
    /**
        
    @short Simulation of the pad trigger

    // DG--todo This class implements the Pad TDS offline simulation. It loops over the input digits,
    // DG--todo determines the BC tag and applies the additional processing of the VMM chip which is
    // DG--todo not yet implemented in the digitization. The loop is executed over the full digit pool
    // DG--todo once upon the first data request for an event and the PAD data are internally cached
    // DG--todo and collected per trigger sectors. The run ID and event ID are cached; the processing
    // DG--todo status is also cached to be propagated via a StatusCode at each data request.
    // DG--todo
    // DG--todo Supported processing:
    // DG--todo Time delay simulation by subtracting the time_of_flight and the electronic random jitter;
    // DG--todo VMM Dead Time simulation;
    // DG--todo
    // DG--todo It returns a vector of PadData to input the PadTrigger simulation.

    davide.gerbaudo@gmail.com
    Oct 2015

    */
    class PadTriggerLogicOfflineTool:
            virtual public IPadTriggerLogicTool,
            public AthAlgTool,
            public IIncidentListener {
    public:
        enum CacheStatus {OK, FILL_ERROR, CLEARED};
        PadTriggerLogicOfflineTool(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);
        virtual ~PadTriggerLogicOfflineTool();
        virtual StatusCode initialize();
        virtual void handle (const Incident& inc);
        /// Log a message using the Athena controlled logging system
        
        StatusCode compute_pad_triggers(const std::vector<std::shared_ptr<PadData>>& pads, std::vector<std::unique_ptr<PadTrigger>> &triggers);

        /**
        @brief simplified trigger: require 4 aligned pads on 4 subsequent layers
        This is meant mainly to test the machinery.
        See test/test_4on4padTrigger.cxx
        */
        static std::vector<std::unique_ptr<PadTrigger>> build4of4SingleWedgeTriggers(const std::vector<std::shared_ptr<PadData>> &pads);
        ///// from PadData to the TDR-style PadWithHits
        //static PadWithHits convert(const PadData &pd);
        /**
        @brief transfer the geometric info from PadData to PadWithHits
        Note: it needs to access the MuonDetectorManager
        */
        bool fillGeometricInformation(const PadData &pd, PadWithHits &pwh);
        int Pad2BandId(const PadWithHits  &p, const float Yfrac);
        
        /// from TDR-style SectorTriggerCandidate to PadTrigger
        PadTrigger convert(const SectorTriggerCandidate &t);
            
    private:
        /// get the output tree from the athena histogram service
        TTree* get_tree_from_histsvc();
        // // methods implementing the internal data processing
        // CacheStatus fill_pad_cache();                           //!< loop over the digit container, apply the additional processing then fill the cache
        // void clear_cache();                                     //!< clear the pad hit cache deleting the PadData pointers

        // needed Servives, Tools and Helpers
        ServiceHandle< IIncidentSvc >      m_incidentSvc;       //!< Athena/Gaudi incident Service
        ServiceHandle< IAtRndmGenSvc >     m_rndmSvc;           //!< Athena random number service
        CLHEP::HepRandomEngine*            m_rndmEngine;        //!< Random number engine
        const MuonGM::MuonDetectorManager* m_detManager;        //!< MuonDetectorManager
        // DG-2015-10-02 probably not needed
        // const sTgcIdHelper*                m_sTgcIdHelper;      //!< sTgc offline Id helper

        // \todo
        // DG-2015-10-02 : can probably get rid of all this caching below

        // hidden variables
        // std::vector< std::vector<PadData*> > m_pad_cache;       //!< cache for the PAD hit data in the event (one per sector)
        int     m_pad_cache_runNumber;                          //!< run number associated to the current PAD cache
        int     m_pad_cache_eventNumber;                        //!< event number associated to the current PAD cache
        // CacheStatus m_pad_cache_status;                         //!< status of the current cache


        // properties: container and service names
        StringProperty   m_rndmEngineName;                      //!< property, todo
        StringProperty   m_sTgcDigitContainer;                  //!< property, todo
        StringProperty   m_sTgcSdoContainer;                    //!< property, todo
        // properties: configuration
        FloatProperty    m_PadEfficiency;                       //!< property, todo

        // properties: steering flags
        BooleanProperty  m_useSimple4of4;                       //!< property, todo
        BooleanProperty  m_doNtuple;                            //!< property, todo

        PadTriggerValidationTree m_validation_tree;
        size_t m_missingDetectorManagerErrorCounter;            ///< how many times we errored b/c of missing m_detManager
        size_t m_missingReadoutElementErrorCounter;             ///< how many times we errored b/c of missing sTgcReadoutElement
         L1TdrStgcTriggerLogic m_tdrLogic;
    };  // end of PadTriggerLogicOfflineTool class

} // namespace NSWL1

#endif
