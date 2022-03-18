/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2LONGLIVEDPARTICLES_MUONCLUSTER_H
#define TRIGL2LONGLIVEDPARTICLES_MUONCLUSTER_H
/*
  MuonCluster Algorithm:  Trigger/TrigAlgorithm/TrigMuonCluster

  Authors: Stefano.Giagu@cern.ch
           Antonio.Policicchio@cern.ch

  Algorithm for selection of Long-Lived neutral particle decays (Hidden Valley sector):
  Clustering of LVL1 muon RoIs, access to LVL2 jets and SITRACK tracks.
*/
#include <string>
#include <vector>
#include <set>
#include <map>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/MonitoredScalar.h"
#include "AthenaMonitoringKernel/MonitoredGroup.h"
#include "AthenaMonitoringKernel/MonitoredCollection.h"
#include "AthenaMonitoringKernel/MonitoredTimer.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"


#define kMAX_ROI 20

class MuonCluster : public AthReentrantAlgorithm {

public:
  /** Constructor */
  MuonCluster(const std::string& name, ISvcLocator* svc);
  ~MuonCluster();

  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey{ this,
      "RoIs",                                       // property name
      "HLT_muVtxCluster_RoIs",                      // default value of StoreGatekey
      "input RoICollection"};

  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_outputCompositesKey{ this,
      "TrigCompositeContainer",                             // property name
      "RoICluster_Composites",                              // default value of StoreGatekey
      "output Composites container"};

  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_outputRoiDescriptorKey{ this,
      "TrigRoiDescriptorDataVector",
      "RoICluster_Descriptors",
      "output RoI Descriptor container with descriptor for cluster with maximum number of RoIs. For ID."};

  /** hltInitialize() */
  virtual StatusCode initialize() override;
  /** hltExecute(), main code of the algorithm */
  virtual StatusCode execute(const EventContext& ctx) const override;


private:

    ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring Tool"};

protected:

  typedef struct  {
    double eta;
    double phi;
    int nroi;
  } lvl1_muclu_roi;

  // JobOption properties
  /** A property which specifies the radius of the cluster */
  Gaudi::Property<float> m_DeltaR{this, "DeltaR", 0.4, "radius of the muon cluster"};
  Gaudi::Property<std::string> m_featureLabel{"MuonCluLabel", "MuonClusterInput", "label for the MuonCluster feature in the HLT Navigation, for the xAOD::TrigCompositeContainer"};

  /** calculcate the deltaR between two Rois */
  float DeltaR(lvl1_muclu_roi , lvl1_muclu_roi ) const;

};

#endif // TRIGL2LONGLIVEDPARTICLES_MUONCLUSTER_H
