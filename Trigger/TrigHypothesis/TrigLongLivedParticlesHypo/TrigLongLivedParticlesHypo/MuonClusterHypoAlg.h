/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     MuonClusterHypoAlg.h
// PACKAGE:  Trigger/TrigHypotheis/TrigMuonHypo
//
// AUTHORS:   S. Giagu <stefano.giagu@cern.ch>
//            A.Policicchio <antonio.policicchio@cern.ch>
//
// PURPOSE:  LVL2 Muon Cluster Hypothesis Algorithm: V1.0
// ********************************************************************

#ifndef TRIG_MUCLUHYPOALG_H
#define TRIG_MUCLUHYPOALG_H

#include <string>
#include <vector>
#include <set>
#include <map>

#include "DecisionHandling/HypoBase.h"
#include "GaudiKernel/ToolHandle.h"

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/MonitoredScalar.h"
#include "AthenaMonitoringKernel/MonitoredGroup.h"
#include "AthenaMonitoringKernel/MonitoredCollection.h"
#include "AthenaMonitoringKernel/MonitoredTimer.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
// #include "TrigT1Interfaces/RecMuonRoI.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include "TrigLongLivedParticlesHypo/MuonClusterHypoTool.h"

class StoreGateSvc;

/*
 * \class MuionClusterHypo
 * \brief MuonClusterHypoAlg is a Trigger Hypo Algorithm that retrieves the L1 Muon RoIs and then
 *        requires that these L1 RoIs form a cluster with number of RoI larger than 3, with no Jets with Log(H/E)<0.5 in the cluster cone and no SITRACKs around the cluster direction.
 *        A TE will be set active if the selection cuts are fullfilled.
*/

class MuonClusterHypoAlg: public HypoBase {
  public:
    /** Constructor. */
    MuonClusterHypoAlg(const std::string& name, ISvcLocator* pSvcLocator);
    /** Destructor. */
    ~MuonClusterHypoAlg();

    SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_outputCompositesKey{ this,
        "TrigCluCompositeContainer",                  // property name
        "RoICluster_Composites",                      // default value of StoreGatekey
        "output Composites container"};

    /** method to initialize. */
    StatusCode initialize();
    /** method to execute. */
    StatusCode execute(const EventContext& ctx) const;

private:

    ToolHandleArray<MuonClusterHypoTool> m_hypoTools{this,"HypoTools", {}, "Hypothesis Tools"};
    ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool", "", "Monitoring Tool"};

};

#endif  // TRIG_MUCLUHYPO_H
