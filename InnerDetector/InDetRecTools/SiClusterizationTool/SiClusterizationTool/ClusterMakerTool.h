/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//ClusterMaker.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Fill the global position fields of the PrepRawData
///////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////
// First version 04/08/2003 Tommaso Lari
//
///////////////////////////////////////////////////////////////////

#ifndef INDETRIOMAKER_CLUSTERMAKERTOOL_H
#define INDETRIOMAKER_CLUSTERMAKERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "PixelCabling/IPixelCablingSvc.h"
#include "PixelConditionsData/PixelChargeCalibCondData.h"
#include "PixelConditionsData/PixelOfflineCalibData.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "GaudiKernel/ToolHandle.h"

#include <atomic>
#include <vector>

template <class T> class ServiceHandle;
class Identifier;
class StatusCode;

namespace InDetDD {
  class SiDetectorElement;
}

class PixelID;

namespace InDet {

static const InterfaceID IID_ClusterMakerTool("InDet::ClusterMakerTool", 1, 0);

class PixelCluster;
class SCT_Cluster;
class SiWidth;

class ClusterMakerTool : public AthAlgTool {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  ClusterMakerTool(const std::string &type,
                   const std::string &name,
                   const IInterface *parent);
  ~ClusterMakerTool() = default;
  
  static const InterfaceID& interfaceID() { return IID_ClusterMakerTool; };

  StatusCode initialize();
  
  //  void SetErrorStrategy(int strategy);

  // Compute the pixel cluster global position, and the error associated 
  // to the position.
  // Called by the pixel clustering tools
  // 
  // Input parameters
  // - the cluster Identifier 
  // - the position in local reference frame 
  // - the list of identifiers of the Raw Data Objects belonging to the cluster
  // - the width of the cluster
  // - the module the cluster belongs to  
  // - wheter the cluster contains ganged pixels
  // - the error strategy, currently
  //    0: cluster width/sqrt(12.)
  //    1: pixel pitch/sqrt(12.)
  //    2: parametrized as a function ofpseudorapidity and cluster size 
  //       (default)
  // - TOT interpolation variable in local x and y directions 
  //   [ Omegax = TOT1/(TOT1+TOT2), where TOT1 and TOT2 are the sum of the 
  //     charges of the first and last row of the cluster respectively  
  //     Omegay: similar definition with columns rather than rows ]

  PixelCluster* pixelCluster(const Identifier& clusterID,
                             const Amg::Vector2D& localPos,
                             const std::vector<Identifier>& rdoList,
                             const int lvl1a,
                             const std::vector<int>& totList,
                             const SiWidth& width,
                             const InDetDD::SiDetectorElement* element, 
                             bool ganged,
                             int errorStrategy,
                             const float omegax,
                             const float omegay,
                             bool split = false,
                             double splitProb1 = 0.,
                             double splitProb2 = 0.) const;

  PixelCluster* pixelCluster(const Identifier& clusterID,
                             const Amg::Vector2D& localPos,
                             const std::vector<Identifier>& rdoList,
                             const int lvl1a,
                             const std::vector<int>& totList,
                             const SiWidth& width,
                             const InDetDD::SiDetectorElement* element, 
                             bool ganged,
                             int errorStrategy,
                             const PixelID& pixelID,
                             bool split = false,
                             double splitProb1 = 0.,
                             double splitProb2 = 0.) const;



  // Computes global position and errors for SCT cluster.
  // Called by SCT Clustering tools
  // 
  // Input parameters
  // - the cluster Identifier 
  // - the position in local reference frame 
  // - the list of identifiers of the Raw Data Objects belonging to the cluster
  // - the width of the cluster
  // - the module the cluster belongs to  
  // - the error strategy, currently
  //    0: Cluster Width/sqrt(12.)
  //    1: Set to a different values for one and two-strip clusters (def.)

  SCT_Cluster* sctCluster(const Identifier& clusterID,
                         const Amg::Vector2D& localPos,
                         const std::vector<Identifier>& rdoList,
                         const SiWidth& width,
                         const InDetDD::SiDetectorElement* element,
                         int errorStrategy) const;
 
private:

  ServiceHandle<IPixelCablingSvc> m_pixelCabling
  {this, "PixelCablingSvc", "PixelCablingSvc", "Pixel cabling service"};

  SG::ReadCondHandleKey<PixelChargeCalibCondData> m_chargeDataKey
  {this, "PixelChargeCalibCondData", "PixelChargeCalibCondData", "Pixel charge calibration data"};

  ToolHandle<ISiLorentzAngleTool> m_pixelLorentzAngleTool
  {this, "PixelLorentzAngleTool", "SiLorentzAngleTool/PixelLorentzAngleTool", "Tool to retreive Lorentz angle of Pixel"};

  ToolHandle<ISiLorentzAngleTool> m_sctLorentzAngleTool
  {this, "SCTLorentzAngleTool", "SiLorentzAngleTool/SCTLorentzAngleTool", "Tool to retreive Lorentz angle of SCT"};

  // These std::atomic_bool may be dropped.
  // m_issueErrorA and m_issueErrorB are changed in pixelCluster but do not affect any computation.
  // The default values of m_forceErrorStrategy1A and m_forceErrorStrategy1B are unchanged.
  // If they are changed in event processing and affect some computation, they are not thread-safe.
  mutable std::atomic_bool m_issueErrorA{true};
  mutable std::atomic_bool m_forceErrorStrategy1A{false};
  mutable std::atomic_bool m_issueErrorB{true};
  mutable std::atomic_bool m_forceErrorStrategy1B{false};

  // Parametrization of the Pixel errors
  // now moved in PixelConditionsData, except for CTB parametrization
  
  double getPixelCTBPhiError(int layer, int phi, int PhiClusterSize) const;
 
  SG::ReadCondHandleKey<PixelCalib::PixelOfflineCalibData> m_clusterErrorKey{this, "PixelOfflineCalibData", "PixelOfflineCalibData", "Output key of pixel cluster"};

};

}

#endif // INDETRIOMAKER_CLUSTERMAKERTOOL_H
