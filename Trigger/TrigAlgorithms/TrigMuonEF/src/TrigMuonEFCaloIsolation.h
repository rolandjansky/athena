// This file is really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEF_TRIGMUONEFCALOISOLATION_H__
#define TRIGMUONEF_TRIGMUONEFCALOISOLATION_H__

#include "TrigInterfaces/FexAlgo.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCellContainer.h"

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // legacy trigger code

class StoreGateSvc;
class IsoMuonFeature;


namespace xAOD {
class ICaloCellIsolationTool;
class ICaloTopoClusterIsolationTool;
}

/**
 * @class TrigMuonEFCaloIsolation
 *
 * @brief Algorithm to fill etcone or topoetcone calorimeter isolation for EF muons
 *
 * This class calculates calorimeter isolation for EF muons. It uses
 * the last created calorimeter topo cluster container from TrigCaloRec as input
 * to the CaloIsolationTool to calculate the isolation.
 *
 * @author Martin Spangenberg (Martin.Spangenberg@cern.ch)
 */
class TrigMuonEFCaloIsolation : public virtual HLT::FexAlgo,
    public virtual IIncidentListener {
public:

    /// Constructor
    TrigMuonEFCaloIsolation (const std::string &name, ISvcLocator *pSvcLocator);
    /// Destructor
    ~TrigMuonEFCaloIsolation();

    /// Initialize the algorithm
    HLT::ErrorCode hltInitialize();
    /// Execute - called per trigger element
    HLT::ErrorCode hltExecute(const HLT::TriggerElement *, HLT::TriggerElement *);
    /// Finalize the algorithm
    HLT::ErrorCode hltFinalize();

private:

    /// Fill et-cone isolation values for xAOD muons
    void fillCaloIsolation(const xAOD::MuonContainer *muons,
                           const xAOD::CaloClusterContainer *clustercont);


    /// Require that EF muons are combined
    bool m_requireCombined;

    /// Select if pileup correction should be applied
    bool m_applyPileupCorrection;

    /// flag to see if debug is enabled
    bool m_debug;

    /// Tool to calculate the isolation
    ToolHandle<xAOD::ICaloTopoClusterIsolationTool> m_caloTopoClusterIsolationTool;

    /// Monitoring Histograms
    StringProperty m_histo_path_base;     // set the histo path for Monitoring
    //std::vector<double > m_etiso_cone1;
    std::vector<double > m_etiso_cone2;
    std::vector<double > m_etiso_cone3;
    std::vector<double > m_etiso_cone4;

    void handle(const Incident &);

}; //class TrigMuonEFCaloIsolation

#endif //TRIGMUONEF_TRIGMUONEFCALOISOLATION_H__
