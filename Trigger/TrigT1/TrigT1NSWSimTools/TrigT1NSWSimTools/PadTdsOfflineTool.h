/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PADTDSOFFLINETOOL_H
#define PADTDSOFFLINETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandGauss.h"
#include "CxxUtils/checker_macros.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "MuonDigitContainer/sTgcDigit.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonSimData/MuonSimData.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "TrigT1NSWSimTools/IPadTdsTool.h"
#include "TrigT1NSWSimTools/PadOfflineData.h"
#include "TrigT1NSWSimTools/tdr_compat_enum.h"
#include "PadTdsValidationTree.h"

#include "TTree.h"
#include <functional>
#include <algorithm>
#include <map>
#include <utility>

class sTgcDigit;
class TTree;
class MuonSimDataCollection;
class sTgcDigitContainer;

namespace CLHEP {
class HepRandomEngine;
}

namespace MuonGM {
class MuonDetectorManager;
}


// namespace for the NSW LVL1 related classes
namespace NSWL1 {
    /**
    *
    *   @short interface for the PadTDS tools
    *
    * This class implements the Pad TDS offline simulation. It loops over the input digits,
    * determines the BC tag and applies the additional processing of the VMM chip which is
    * not yet implemented in the digitization. The loop is executed over the full digit pool
    * once upon the first data request for an event and the PAD data are internally cached
    * and collected per trigger sectors. The run ID and event ID are cached; the processing
    * status is also cached to be propagated via a StatusCode at each data request.
    *
    * Supported processing:
    *  Time delay simulation by subtracting the time_of_flight and the electronic random jitter;
    *  VMM Dead Time simulation;
    *
    * It returns a vector of PadData to input the PadTrigger simulation.
    *
    *  @author Alessandro Di Mattia <dimattia@cern.ch>
    *
    * ----------------------------------------------------------------------------------------
    * 2022 Update: the internal cache has been removed for the code to deal with parallel
    * processing (athenaMT) in Release 22. It has been replaced by an event-by-event cache,
    * passed by reference throughout the workflow.
    *
    *  @modified by Francesco Giuseppe Gravili <francesco.giuseppe.gravili@cern.ch>
    *
    */
    class PadHits;
    class PadTrigger;

    class PadTdsOfflineTool: virtual public IPadTdsTool,
                            public AthAlgTool,
                            public IIncidentListener {

    public:
        PadTdsOfflineTool(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);
        virtual ~PadTdsOfflineTool()=default;
        virtual StatusCode initialize ATLAS_NOT_THREAD_SAFE () override;
        virtual void handle (const Incident& inc) override;
        virtual StatusCode gather_pad_data(std::vector<std::shared_ptr<PadData>>& pads, int side=-1, int sector=-1) const override;

    public:
        /** @name Sector indices
            The current convention is to have sector [0,15] on side==0
            (side 'A') and sector [16,31] on side==1 (side 'C').
            Use the functions below rather than hardcoded values.
        */
        ///@{
        static size_t firstSector() { return 0; }
        static size_t lastSector() { return 31; }
        static size_t numberOfSectors() { return 32; }
        static size_t numberOfSectorsPerSide() { return 16; }
        static size_t firstSectorAside() { return 0; }
        static size_t lastSectorAside() { return 15; }
        static size_t firstSectorCside() { return 16; }
        static size_t lastSectorCside() { return 31; }
        static size_t firstSector(const int &side)
            { return (side==0 ? firstSectorAside() : side==1 ? firstSectorCside() : 0); }
        static size_t lastSector(const int &side)
            { return (side==0 ? lastSectorAside() : side==1 ? lastSectorCside() : 0); }
        static char sideLabel(const int &side)
            { return (side==0 ? 'A' : side==1 ? 'C' : '?'); }
        /** internal index. */
        static size_t sectorIndex(const int &side, const int &sector)
            { return side * numberOfSectorsPerSide() + sector; }
        ///@}

    private:
        // methods implementing the internal data processing
        StatusCode fill_pad_cache(std::vector< std::vector<std::shared_ptr<PadData>> > &pad_cache) const; //!< Apply the additional processing then fill the cache, locally
        StatusCode fill_pad_validation_id ATLAS_NOT_THREAD_SAFE (std::vector< std::vector<std::shared_ptr<PadData>> > &pad_cache) const; //!< Fill the ntuple branch for the PadTdsOffline

        double computeTof(const sTgcDigit* digit) const;        //!< compute the time of flight of particle giving the PAD hit
        double computeTimeJitter() const;                       //!< extract the time jitter t subtract from the PAD hit time
        void simulateDeadTime(std::vector<PadHits>& h) const;   //!< simulate the dead time of the VMM
        bool get_truth_hits_this_pad(const Identifier &pad_id, std::vector<MuonSimData::Deposit> &deposits) const;
        bool is_pad_digit(const sTgcDigit* digit) const;
        int cache_index(const sTgcDigit* digit) const;
        /*
         * Apply time delay and determine BC tag for the hit according to the trigger capture window.
         * BC tag:  bit nr. 8 correspond to the current bunch for simulated events
         */
        bool determine_delay_and_bc(const sTgcDigit* digit, const int &pad_hit_number, double &delayed_time, uint16_t &BCtag) const;
        void store_pads(const std::vector<PadHits> &pad_hits, std::vector< std::vector<std::shared_ptr<PadData>> > &pad_cache) const; //!< Store pad data in the cache
        void print_digit(const sTgcDigit* digit) const;

        void print_pad_time(const std::vector<PadHits> &pad_hits) const; //!< Print time for each pad
        void print_pad_cache(std::vector< std::vector<std::shared_ptr<PadData>> > &pad_cache) const; //!< Print all info stored in the pad cache

        // needed Servives, Tools and Helpers
        ServiceHandle<IIncidentSvc> m_incidentSvc{this, "IncidentSvc", "IncidentSvc"};  //!< Athena/Gaudi incident Service
        ServiceHandle<IAthRNGSvc>   m_rndmSvc{this, "RndmSvc", "AthRNGSvc", ""};        //!< Random number generator engine to use
        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
        const MuonGM::MuonDetectorManager* m_detManager;        //!< MuonDetectorManager

        // properties: container and service names
        Gaudi::Property<bool>         m_isMC          {this, "IsMC",            true,               "This is MC"};
        Gaudi::Property<std::string>  m_rndmEngineName{this, "RndmEngineName", "PadTdsOfflineTool", "Name of the random engine"};
        Gaudi::Property<bool>         m_doNtuple      {this, "DoNtuple",        false,              "Input PadTds branches into the analysis ntuple"};
        Gaudi::Property<float>        m_vmmTimeOverThreshold{this, "VMM_TimeOverThreshold", 0.,  "Time to form a digital signal"};
        Gaudi::Property<float>        m_vmmShapingTime      {this, "VMM_ShapingTime",       0.,  "Time from the leading edge of the signal and its peak"};
        Gaudi::Property<float>        m_vmmDeadTime         {this, "VMM_DeadTime",          50., "VMM chip dead time to produce another signal on the same channel"};
        Gaudi::Property<float>        m_triggerCaptureWindow{this, "TriggerCaptureWindow",  30., "Time window for valid hit coincidences"};
        Gaudi::Property<float>        m_timeJitter          {this, "TimeJitter",            2.,  "Time jitter"};

        Gaudi::Property<bool> m_applyTDS_TofSubtraction       {this, "ApplyTofSubtraction",        false, "Time of flight subtraction"};
        Gaudi::Property<bool> m_applyTDS_TimeJitterSubtraction{this, "ApplyTimeJitterSubtraction", false, "Time jitter subtraction"};
        Gaudi::Property<bool> m_applyVMM_ToT                  {this, "ApplyToT",                   false, "Time over threshold criteria"};
        Gaudi::Property<bool> m_applyVMM_ShapingTime          {this, "ApplyVMMShapingTime",        false, "VMM Shaping time condition"};
        Gaudi::Property<bool> m_applyVMM_DeadTime             {this, "ApplyVMMDeadTime",           false, "VMM Dead time condition"};

        std::unique_ptr<PadTdsValidationTree> m_validation_tree;
        SG::ReadHandleKey<sTgcDigitContainer> m_sTgcDigitContainer = {this,"sTGC_DigitContainerName","sTGC_DIGITS","the name of the sTGC digit container"};
        SG::ReadHandleKey<MuonSimDataCollection> m_sTgcSdoContainer = {this,"sTGC_SdoContainerName","sTGC_SDO","the name of the sTGC SDO container"};
    };
}
#endif
