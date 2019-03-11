/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

//#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
//#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>
#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"

#include "PixelConditionsData/PixelOfflineCalibData.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <atomic>

class IPixelCalibSvc;
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
  ~ClusterMakerTool();
  
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

  //  mutable MsgStream m_log;
  bool m_calibrateCharge;
  mutable std::atomic_bool m_issueErrorA;
  mutable std::atomic_bool m_forceErrorStrategy1A;
  mutable std::atomic_bool m_issueErrorB;
  mutable std::atomic_bool m_forceErrorStrategy1B;
  ServiceHandle<IPixelCalibSvc> m_calibSvc;
  ToolHandle<ISiLorentzAngleTool> m_pixelLorentzAngleTool{this, "PixelLorentzAngleTool", "SiLorentzAngleTool/PixelLorentzAngleTool", "Tool to retreive Lorentz angle of Pixel"};
  ToolHandle<ISiLorentzAngleTool> m_sctLorentzAngleTool{this, "SCTLorentzAngleTool", "SiLorentzAngleTool/SCTLorentzAngleTool", "Tool to retreive Lorentz angle of SCT"};

  // Parametrization of the Pixel errors
  // now moved in PixelConditionsData, except for CTB parametrization
  
  double getPixelCTBPhiError(int layer, int phi, int PhiClusterSize) const;
 
  SG::ReadCondHandleKey<PixelCalib::PixelOfflineCalibData> m_clusterErrorKey{this, "PixelOfflineCalibData", "PixelOfflineCalibData", "Output key of pixel cluster"};

};

}

#endif // INDETRIOMAKER_CLUSTERMAKERTOOL_H
