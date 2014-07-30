/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: OfflineSpacePointProvider.h
// 
// author: Nikos Konstantinidis 26/06/2004
//         nk@hep.ucl.ac.uk
//		 
// Description: reads in the ATLAS offline SpacePoints
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////


#if ! defined( OFFLINESPACEPOINTPROVIDER_H )
#define OFFLINESPACEPOINTPROVIDER_H


#include <string>
#include <vector>

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigSiSpacePointTool/ISpacePointProvider.h"
#include "TrigInDetToolInterfaces/ITrigL2LayerNumberTool.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "TrigInDetEvent/TrigSiSpacePointCollection.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "IRegionSelector/IRoiDescriptor.h"

class TrigSiSpacePoint;
//class Trk::SpacePoint;

class StoreGateSvc;
class SCT_ID;
class PixelID;

typedef std::vector< TrigSiSpacePoint* >   spacePointVec;

class OfflineSpacePointProviderTool: public AlgTool, virtual public ISpacePointProvider
{
 public:

  OfflineSpacePointProviderTool( const std::string&, const std::string&, const IInterface* );
  virtual ~OfflineSpacePointProviderTool(){};
  
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  
  spacePointVec getSpacePoints(bool getPixeSP=true, bool getSctSP=true);

  StatusCode fillCollections(const IRoiDescriptor&, bool, bool, std::vector<int>&, std::vector<int>&);
  StatusCode fillCollections(bool, bool, std::vector<int>&, std::vector<int>&);

  const std::vector<int>* fillPixelDataErrors();
  const std::vector<int>* fillSCT_DataErrors();

  inline double getBStoClustersTimingPixel() const  {return 0.0;}
  inline double getBStoClustersTimingSCT() const  {return 0.0;}
  inline double getSPformationTimingPixel() const  {return 0.0;}
  inline double getSPformationTimingSCT() const  {return 0.0;}
  inline double getTotalTiming() const {return 0.0;}
  inline double getRegSelTiming() const {return 0.0;}
 private:  // data members

  TrigSiSpacePoint* makeTrigPixSP( const Trk::SpacePoint* ) const;
  TrigSiSpacePoint* makeTrigSctSP( const Trk::SpacePoint* ) const;

  std::string m_regionSelectorToolName;
  IRegSelSvc* m_regionSelector;

  std::string m_sctSpContainerName,m_pixelSpContainerName;
  TrigSiSpacePointContainer* m_sctSpContainer;
  TrigSiSpacePointContainer* m_pixelSpContainer;

  std::vector<int> m_pixelDataErrors;
  std::vector<int> m_sctDataErrors;

  StoreGateSvc*        m_StoreGate;

  const SCT_ID* m_sctId;
  const PixelID* m_pixelId;
  ToolHandle<ITrigL2LayerNumberTool> m_numberingTool;
};
 
#endif
