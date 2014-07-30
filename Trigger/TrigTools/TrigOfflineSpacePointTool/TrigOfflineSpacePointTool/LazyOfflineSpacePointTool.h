/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
//
// filename: LazyOfflineSpacePointTool.h
// 
// author: Nikos Konstantinidis 01/05/2004
//         nk@hep.ucl.ac.uk
//		 
// Description: reads in the ATLAS SpacePoints
// 
// Modified : J.Baines 10/5/04
// Modified : P. Conde Muino (04/02/05)
//
// -------------------------------
// ATLAS Collaboration
///////////////////////////////////////////////////////////////////////////////


#if ! defined( LAZYOFFLINESPACEPOINTTOOL_H )
#define LAZYOFFLINESPACEPOINTTOOL_H

#include <string>
#include <vector>

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigSiSpacePointTool/ISpacePointProvider.h"
#include "TrigInDetToolInterfaces/ITrigL2LayerNumberTool.h"
#include "SiClusterizationTool/ISCT_ClusteringTool.h"
#include "SiClusterizationTool/IPixelClusteringTool.h"
#include "SiClusterizationTool/ClusterMakerTool.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "SiSpacePointTool/SiSpacePointMakerTool.h"

#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "TrigInDetEvent/TrigSiSpacePointCollection.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "SiClusterizationTool/PixelGangedAmbiguitiesFinder.h"
#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"
#include "IRegionSelector/IRoiDescriptor.h"

class TrigSiSpacePoint;

class StoreGateSvc;

class SCT_ID;
class PixelID;
namespace InDetDD {
  class SiDetectorManager;
  class SiDetectorManager;
}
class IROBDataProviderSvc;
class TrigTimer;
namespace InDet { class ITrigSCT_SpacePointTool; }

class LazyOfflineSpacePointTool: public AlgTool, 
				 virtual public ISpacePointProvider
{
 public:

  LazyOfflineSpacePointTool( const std::string&, const std::string&, 
			     const IInterface* );
  virtual ~LazyOfflineSpacePointTool(){};
  
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  
  spacePointVec getSpacePoints( double roiEtaMin, double roiEtaMax, 
				double roiPhiMin, double roiPhiMax,
				bool getPixeSP=true, bool getSctSP=true);

  spacePointVec getSpacePoints(bool getPixeSP=true, bool getSctSP=true);

  StatusCode fillCollections(const IRoiDescriptor&, bool, bool,
			     std::vector<int>&, std::vector<int>&);

  StatusCode fillCollections(bool, bool,
			     std::vector<int>&, std::vector<int>&);

  const std::vector<int>* fillPixelDataErrors();
  const std::vector<int>* fillSCT_DataErrors();

  inline double getBStoClustersTimingPixel() const  {return 0.0;}
  inline double getBStoClustersTimingSCT() const  {return 0.0;}
  inline double getSPformationTimingPixel() const  {return 0.0;}
  inline double getSPformationTimingSCT() const  {return 0.0;}
  inline double getTotalTiming() const {return 0.0;}
  inline double getRegSelTiming() const {return 0.0;}

  // Transform a SpacePoint into a TrigSiSpacePoint
  // (Why do LVL2 algorithms use TrigSiSpacePoints instaid of SpacePoints????)

  TrigSiSpacePoint* makeTrigPixSP( const Trk::SpacePoint* ) const;
  TrigSiSpacePoint* makeTrigSctSP( const Trk::SpacePoint* ) const;

 private:
  int formPixelSpacePoints(const InDetRawDataCollection<PixelRDORawData>* 
			   RDO_collection, std::vector<int>&);
  void formSCT_spacePoints(const InDetRawDataCollection<SCT_RDORawData>* 
			   RDO_collection, 
			   spacePointVec& spVec);


 private:  // data members

  std::string m_regionSelectorToolName;
  //std::string m_sctSpToolName;
  //std::string m_pixelSpToolName;
  std::string m_sctSpContainerName,m_pixelSpContainerName;

  std::string m_sctRdoContainerName;
  std::string m_pixelRDOContainerName;

  //  std::string m_SCT_clusteringToolName;
  //std::string m_pixelClusteringToolName;

  TrigSiSpacePointContainer* m_sctSpContainer;
  TrigSiSpacePointContainer* m_pixelSpContainer;

  IRegSelSvc*                m_regionSelector;
  ToolHandle<InDet::SiSpacePointMakerTool>       m_pixelSpTool;
  ToolHandle<InDet::ITrigSCT_SpacePointTool>      m_sctSpTool;
  ToolHandle<InDet::ClusterMakerTool>            m_globalPosAlg;    

  StoreGateSvc*        m_StoreGate;

  const InDetDD::SiDetectorManager * m_SCT_manager;
  const InDetDD::SiDetectorManager * m_pixelManager;

  ToolHandle<InDet::ISCT_ClusteringTool>          m_SCT_clusteringTool;
  ToolHandle<InDet::IPixelClusteringTool>         m_pixelClusteringTool; 
  ToolHandle<InDet::PixelGangedAmbiguitiesFinder> m_gangedAmbiguitiesFinder; 
  InDet::SCT_ClusterContainer*                m_SCTclusterContainer;
  std::string                              m_SCTclustersName;

  int                     m_errorStrategy;  // error strategy

  int m_positionStrategy;                  // position strategy
                                           // see .cxx or jobOption example for
                                           // details

  int m_outputLevel;

  const SCT_ID*  m_sctId;
  const PixelID* m_pixelId;

  std::vector<int> m_pixelDataErrors;
  std::vector<int> m_sctDataErrors;
  
  std::string m_ROBDataProviderSvcName;
  IROBDataProviderSvc* m_robDataProvider;

  ToolHandle<ITrigL2LayerNumberTool> m_numberingTool;


#define LAZY_SP_NTIMERS 7
  TrigTimer* m_timer[LAZY_SP_NTIMERS];
  bool m_timers;

};
 

#endif

