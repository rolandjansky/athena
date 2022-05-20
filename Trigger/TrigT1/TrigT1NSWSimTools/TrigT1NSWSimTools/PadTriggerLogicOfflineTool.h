/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWL1_PADTRIGGERLOGICOFFLINETOOL_H
#define NSWL1_PADTRIGGERLOGICOFFLINETOOL_H

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "MuonDigitContainer/sTgcDigit.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"
#include "TrigT1NSWSimTools/IPadTriggerLogicTool.h"
#include "TrigT1NSWSimTools/PadTriggerValidationTree.h"
#include "TrigT1NSWSimTools/L1TdrStgcTriggerLogic.h"
#include "TrigT1NSWSimTools/TriggerTypes.h"
#include "TrigT1NSWSimTools/PadData.h"
#include "TrigT1NSWSimTools/PadOfflineData.h"
#include "TrigT1NSWSimTools/PadTrigger.h"
#include "TrigT1NSWSimTools/SectorTriggerCandidate.h"
#include "TrigT1NSWSimTools/SingleWedgePadTrigger.h"
#include "TrigT1NSWSimTools/tdr_compat_enum.h"
#include "TrigT1NSWSimTools/sTGCTriggerBandsInEta.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/TrapezoidBounds.h"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include "TTree.h"
#include "Math/Vector3D.h"
#include <functional>
#include <algorithm>
#include <map>
#include <utility>
#include <cmath>

namespace MuonGM {
    class MuonDetectorManager;
}

namespace NSWL1 {
    /*
     * @short Simulation of the pad trigger
     *
     * DG--todo This class implements the Pad TDS offline simulation. It loops over the input digits,
     * DG--todo determines the BC tag and applies the additional processing of the VMM chip which is
     * DG--todo not yet implemented in the digitization. The loop is executed over the full digit pool
     * DG--todo once upon the first data request for an event and the PAD data are internally cached
     * DG--todo and collected per trigger sectors. The run ID and event ID are cached; the processing
     * DG--todo status is also cached to be propagated via a StatusCode at each data request.
     * DG--todo
     * DG--todo Supported processing:
     * DG--todo Time delay simulation by subtracting the time_of_flight and the electronic random jitter;
     * DG--todo VMM Dead Time simulation;
     * DG--todo
     * DG--todo It returns a vector of PadData to input the PadTrigger simulation.
     * Oct 2015
     *
     * Major updates for release 22 processing: francesco.giuseppe.gravili@cern.ch
     */
    class PadTriggerLogicOfflineTool:
            virtual public IPadTriggerLogicTool, public AthAlgTool, public IIncidentListener {
    public:
        PadTriggerLogicOfflineTool(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);
        virtual ~PadTriggerLogicOfflineTool()=default;
        virtual StatusCode initialize ATLAS_NOT_THREAD_SAFE () override;
        virtual void handle (const Incident& inc) override;
        /// Log a message using the Athena controlled logging system

        virtual StatusCode compute_pad_triggers(const std::vector<std::shared_ptr<PadData>>& pads, std::vector<std::unique_ptr<PadTrigger>> &triggers) const override;

        int ROI2BandId(const float &EtaTrigAtCenter, const int &SectorType) const;//Recipe From Y.R (based on eta slicing of triggering bands see the implementation) 

        static std::vector<std::unique_ptr<PadTrigger>> build4of4SingleWedgeTriggers(const std::vector<std::shared_ptr<PadData>> &pads);

        /// from TDR-style SectorTriggerCandidate to PadTrigger
        PadTrigger convert(const SectorTriggerCandidate &t) const;

    private:
        /// get the output tree from the athena histogram service
        const std::vector<float> m_etaBandsLargeSector;
        const std::vector<float> m_etaBandsSmallSector;

        std::map<IdentifierHash, std::pair<double,double> > m_phiTable;

        ServiceHandle<IIncidentSvc> m_incidentSvc{this, "IncidentSvc", "IncidentSvc"};  //!< Athena/Gaudi incident Service
        const MuonGM::MuonDetectorManager* m_detManager;        //!< MuonDetectorManager

        Gaudi::Property<float> m_PadEfficiency {this, "PadEfficiency",   1.0,   "Pad trigger efficiency"};
        Gaudi::Property<int>   m_phiIdBits     {this, "PhiIdBits",       6,     "Number of bit to compute Phi-Id of pad triggers"};
        Gaudi::Property<bool>  m_useSimple4of4 {this, "UseSimple4of4",   false, "Use simplified logic requiring 4 hits on 4 gas gaps"};
        Gaudi::Property<bool>  m_doNtuple      {this, "DoNtuple",        false, "Save the trigger outputs in an analysis ntuple"};

        std::unique_ptr<PadTriggerValidationTree> m_validation_tree;
        void fillGeometricInformation(const std::shared_ptr<PadOfflineData>&) const;
        void fillPhiTable();
        L1TdrStgcTriggerLogic m_tdrLogic;
    };
} // namespace NSWL1
#endif
