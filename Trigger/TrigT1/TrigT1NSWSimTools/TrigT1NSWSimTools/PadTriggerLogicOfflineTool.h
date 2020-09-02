/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWL1_PADTRIGGERLOGICOFFLINETOOL_H
#define NSWL1_PADTRIGGERLOGICOFFLINETOOL_H

#include "TrigT1NSWSimTools/IPadTriggerLogicTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigT1NSWSimTools/PadTriggerValidationTree.h"
#include "TrigT1NSWSimTools/L1TdrStgcTriggerLogic.h"
#include "TrigT1NSWSimTools/TriggerTypes.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "RegSelLUT/IRegionIDLUT_Creator.h"

//forward declarations
class IIncidentSvc;
class TTree;

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

    Oct 2015

    */
    class PadTriggerLogicOfflineTool:
            virtual public IPadTriggerLogicTool, public AthAlgTool, public IIncidentListener {
    public:
        enum CacheStatus {OK, FILL_ERROR, CLEARED};
        PadTriggerLogicOfflineTool(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);
        virtual ~PadTriggerLogicOfflineTool()=default;
        virtual StatusCode initialize() override;
        virtual void handle (const Incident& inc) override;
        /// Log a message using the Athena controlled logging system

        virtual
        StatusCode compute_pad_triggers(const std::vector<std::shared_ptr<PadData>>& pads, std::vector<std::unique_ptr<PadTrigger>> &triggers) override;


        int ROI2BandId(const float &EtaTrigAtCenter, const int &SectorType);//Recipe From Y.R (based on eta slicing of triggering bands see the implementation) 
        
        
        static std::vector<std::unique_ptr<PadTrigger>> build4of4SingleWedgeTriggers(const std::vector<std::shared_ptr<PadData>> &pads);

        /**
        @brief transfer the geometric info from PadData to std::shared_ptr<PadData>
        Note: it needs to access the MuonDetectorManager
        */
        
        /// from TDR-style SectorTriggerCandidate to PadTrigger
        PadTrigger convert(const SectorTriggerCandidate &t);
            
    private:
        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
        /// get the output tree from the athena histogram service
         const std::vector<float> m_etaBandsLargeSector;
         const std::vector<float> m_etaBandsSmallSector;



        StatusCode get_tree_from_histsvc(TTree*&);

        ServiceHandle< IIncidentSvc >      m_incidentSvc;       //!< Athena/Gaudi incident Service
        const MuonGM::MuonDetectorManager* m_detManager;        //!< MuonDetectorManager

        StringProperty   m_rndmEngineName;                      //!< property, todo
        StringProperty   m_sTgcDigitContainer;                  //!< property, todo
        StringProperty   m_sTgcSdoContainer;                    //!< property, todo
        FloatProperty    m_PadEfficiency;                       //!< property, todo
        IntegerProperty m_phiIdBits;
        BooleanProperty  m_useSimple4of4;                       //!< property, todo
        BooleanProperty  m_doNtuple;                            //!< property, todo

        PadTriggerValidationTree m_validation_tree;

        void fillGeometricInformation(const std::shared_ptr<PadOfflineData>&);
         L1TdrStgcTriggerLogic m_tdrLogic;

         ToolHandle<IRegionIDLUT_Creator> m_lutCreatorToolsTGC; 
    };  

} // namespace NSWL1

#endif
