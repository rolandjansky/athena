/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
#ifndef PADTDSOFFLINETOOL_H
#define PADTDSOFFLINETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Property.h"
#include "MuonSimData/MuonSimData.h" // cannot fw declare typedef MuonSimData::Deposit

#include "TrigT1NSWSimTools/IPadTdsTool.h"
#include "PadTdsValidationTree.h"


class IIncidentSvc;
class IAtRndmGenSvc;
class sTgcIdHelper;
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
    *
    */
    class PadHits;
    class PadTrigger;
    
    class PadTdsOfflineTool: virtual public IPadTdsTool,
                            public AthAlgTool,
                            public IIncidentListener {

    public:
        enum cStatus {OK, FILL_ERROR, CLEARED};
        PadTdsOfflineTool(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);
        virtual ~PadTdsOfflineTool();
        virtual StatusCode initialize();
        virtual void handle (const Incident& inc);
        StatusCode gather_pad_data(std::vector<std::shared_ptr<PadData>>& pads, int side=-1, int sector=-1);

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
        cStatus fill_pad_cache();                               //!< loop over the digit container, apply the additional processing then fill the cache
        void clear_cache();                                     //!< clear the pad hit cache deleting the PadData pointers

        void fill_pad_validation_id();                          //!< fill the ntuple branch for the PadTdsOffline

        double computeTof(const sTgcDigit* digit) const;        //!< compute the time of flight of particle giving the PAD hit
        double computeTimeJitter() const;                       //!< extract the time jitter t subtract from the PAD hit time
        void   simulateDeadTime(std::vector<PadHits>& h) const; //!< simulate the dead time of the VMM
        void printStgcGeometryFromAgdd() const; ///< test function to demonstrate the access to the AGDD parameters
        bool get_truth_hits_this_pad(const Identifier &pad_id, std::vector<MuonSimData::Deposit> &deposits); // simhits for a given pad
        /// get the output tree from the athena histogram service
        TTree* get_tree_from_histsvc();
        /// whether it's a pad digit (based on 'channelType'
        bool is_pad_digit(const sTgcDigit* digit) const;
        int cache_index(const sTgcDigit* digit) const;
        /// apply time delay and determine BC tag for the hit according to the trigger capture window
        /**
        BC tag:  bit nr. 8 correspond to the current bunch for simulated events
        */
        bool determine_delay_and_bc(const sTgcDigit* digit, const int &pad_hit_number,
                                    double &delayed_time, uint16_t &BCtag);
        bool retrieve_sim_data(const MuonSimDataCollection* &s);
        bool retrieve_digits(const sTgcDigitContainer* &d);
        /// store the pads in the internal pad cache
        void store_pads(const std::vector<PadHits> &pad_hits);
        /// print digit info (debugging)
        void print_digit(const sTgcDigit* digit) const;

        /// print time for each pad (debugging)
        void print_pad_time(const std::vector<PadHits> &pad_hits, const std::string &msg) const;
        /// print all info stored in the pad chache (debugging)
        void print_pad_cache() const;
        
        // needed Servives, Tools and Helpers
        ServiceHandle< IIncidentSvc >      m_incidentSvc;       //!< Athena/Gaudi incident Service
        ServiceHandle< IAtRndmGenSvc >     m_rndmSvc;           //!< Athena random number service
        CLHEP::HepRandomEngine*            m_rndmEngine;        //!< Random number engine
        const MuonGM::MuonDetectorManager* m_detManager;        //!< MuonDetectorManager
        const sTgcIdHelper*                m_sTgcIdHelper;      //!< sTgc offline Id helper

        // hidden variables
        std::vector< std::vector<std::shared_ptr<PadData>> > m_pad_cache;       //!< cache for the PAD hit data in the event (one per sector)
        int     m_pad_cache_runNumber;                          //!< run number associated to the current PAD cache
        int     m_pad_cache_eventNumber;                        //!< event number associated to the current PAD cache
        cStatus m_pad_cache_status;                             //!< status of the current cache


        // properties: container and service names
        StringProperty   m_rndmEngineName;                      //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink
        StringProperty   m_sTgcDigitContainer;                  //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink
        StringProperty   m_sTgcSdoContainer;                    //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink

        // properties: configuration
        FloatProperty    m_VMMTimeOverThreshold;                //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink
        FloatProperty    m_VMMShapingTime;                      //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink
        FloatProperty    m_VMMDeadTime;                         //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink
        FloatProperty    m_triggerCaptureWindow;                //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink
        FloatProperty    m_timeJitter;                          //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink

        // properties: steering flags
        BooleanProperty  m_applyTDS_TofSubtraction;             //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink
        BooleanProperty  m_applyTDS_TimeJitterSubtraction;      //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink
        BooleanProperty  m_applyVMM_ToT;                        //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink
        BooleanProperty  m_applyVMM_ShapingTime;                //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink
        BooleanProperty  m_applyVMM_DeadTime;                   //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink

        BooleanProperty  m_doNtuple;                            //!< property, see @link PadTdsOfflineTool::PadTdsOfflineTool @endlink

        PadTdsValidationTree m_validation_tree;

    };

}

#endif
