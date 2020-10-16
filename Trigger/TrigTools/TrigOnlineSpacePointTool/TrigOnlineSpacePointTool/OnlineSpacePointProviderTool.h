/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: OnlineSpacePointProviderTool.h
// 
// author: Dmitry Emeliyanov 16/05/2006
//         D.Emeliyanov@rl.ac.uk
//		 
// Description: produces LVL2 data: clusters and spacepoints
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////


#ifndef __ONLINE_SPACEPOINTPROVIDER_TOOL_H__
#define __ONLINE_SPACEPOINTPROVIDER_TOOL_H__

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigSiSpacePointTool/ISpacePointProvider.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "GaudiKernel/IIncidentListener.h"

class TrigSiSpacePoint;
class IRoiDescriptor;

class PixelSpacePointTool;
class SCT_SpacePointTool;
class IPixelClusterCacheTool;
class ISCT_ClusterCacheTool;
class SCT_ID;
class PixelID;
class IROBDataProviderSvc;
class TrigTimer;

class OnlineSpacePointProviderTool: public AthAlgTool, virtual public ISpacePointProvider, virtual IIncidentListener

{
 public:
 
  OnlineSpacePointProviderTool( const std::string&, const std::string&, const IInterface* );
  virtual ~OnlineSpacePointProviderTool(){};
  
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  
  spacePointVec getSpacePoints(bool getPixeSP=true, bool getSctSP=true, std::map<Identifier, std::vector<long int> >  *clustermap=nullptr);
	StatusCode fillCollections(const IRoiDescriptor&, bool, bool, 
			std::vector<int>&, std::vector<int>&);
  StatusCode fillCollections(bool, bool, std::vector<int>&, std::vector<int>&);


  const std::vector<int>* fillPixelDataErrors();
  const std::vector<int>* fillSCT_DataErrors();
  //! Incident listener
  virtual void handle( const Incident &incident );


  inline double getBStoClustersTimingPixel() const {return m_bstoclustersPixelTime;}
  inline double getBStoClustersTimingSCT() const {return m_bstoclustersSCTTime;}
  inline double getSPformationTimingPixel() const {return m_spformationPixelTime;}
  inline double getSPformationTimingSCT() const {return m_spformationSCTTime;}
  inline double getTotalTiming() const {return m_l2DataTime;}
  inline double getRegSelTiming() const {return m_regSelTime;}
 private:  // data members

  bool m_fsPixelDataReady;
  bool m_fsSCT_DataReady;
  bool m_isFullScan;
  std::string m_ROBDataProviderSvcName;
  std::string m_regionSelectorToolName;
  std::string m_sctSpToolName;
  std::string m_pixelSpToolName;
  std::string m_sctClusterContainerName;
  std::string m_pixelClusterContainerName;
  std::string m_sctClusterCacheName;
  std::string m_pixelClusterCacheName;
  std::string m_sctSpContainerName;
  std::string m_pixelSpContainerName;

  IRegSelSvc* m_regionSelector;
  PixelSpacePointTool* m_pixelSpTool;
  SCT_SpacePointTool*  m_sctSpTool;
  ToolHandle<IPixelClusterCacheTool> m_pixelCacheTool;
  ToolHandle<ISCT_ClusterCacheTool>  m_sctCacheTool;

  IROBDataProviderSvc* m_robDataProvider;

  const SCT_ID* m_sctId;
  const PixelID* m_pixelId;

  std::vector<int> m_sctDataErrors;
  std::vector<int> m_pixelDataErrors;
  std::vector<int> m_fsPixelIDs;
  std::vector<int> m_fsSCT_IDs;

#define SP_NTIMERS 12
  TrigTimer* m_timer[SP_NTIMERS];
  bool m_timers;

  double m_bstoclustersPixelTime;
  double m_bstoclustersSCTTime;
  double m_spformationPixelTime;
  double m_spformationSCTTime;
  double m_l2DataTime;
  double m_regSelTime;
  int nColl[4];
  bool m_useStoreGate;

  bool m_useSctClustThreshold, m_usePixelClustThreshold;
  unsigned int m_sctClustThreshold,m_pixClustThreshold;

  bool m_doBS;

};
 

#endif

