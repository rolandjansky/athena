/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOINTERFACE_ICALOTOPOTOWERBUILDERTOOLBASE_H
#define CALOINTERFACE_ICALOTOPOTOWERBUILDERTOOLBASE_H
///////////////////////////////////////////////////////////////////////////////
/// \brief ICaloTopoTowerBuilderToolBase is abstract interface for CALOTOPOTOWERBUILDERTOOLBASE
///
/// ICaloTopoTowerBuilderToolBase is an abstract class to access tower builder 
/// tool.
///
///     Concrete tools, derived from the CaloTowerBuilderToolBase base abstract
///	class are controlled via this interface.
///
/// \author Luca Fiorini <Luca.Fiorini@cern.ch>
/// \date October 4, 2007 -
///
///////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include <string>

class CaloTopoTowerContainer;
class CaloTowerSeg;
class CaloCellContainer;
class CaloClusterContainer;
class EventContext;

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_ICaloTopoTowerBuilderToolBase("ICaloTopoTowerBuilderToolBase", 1 , 0); 

class ICaloTopoTowerBuilderToolBase : virtual public IAlgTool
{
 public:
  
  //Virtual destructor
  virtual ~ICaloTopoTowerBuilderToolBase() {}

  /// common initialization

  /// execute is abstract
  virtual StatusCode execute(const EventContext& ctx,
                             CaloTopoTowerContainer* theContainer,const CaloCellContainer* theCell=0) const = 0;
  virtual StatusCode initialize() =0;

  virtual StatusCode initializeTool() = 0;

  virtual void setTowerSeg(const CaloTowerSeg& theTowerSeg) = 0;
  // virtual void setSelectrionCriteria( double minimumCellEnergy, double minimumClusterEnergy, bool   useCellWeights)=0;
  //virtual void setNoiseCriteria(bool m_useNoiseTool, bool m_usePileUpNoise, float m_noiseSigma, float m_cellESignificanceThreshold)=0;

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) = 0;

  static const InterfaceID& interfaceID() {
    static const InterfaceID IID("ICaloTopoTowerBuilderToolBase", 1 , 0);
    return IID;
  }


 protected:


};
#endif
