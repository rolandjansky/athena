///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// IsolationPflowCorrection.h 
// Header file for class IsolationPflowCorrection
/////////////////////////////////////////////////////////////////// 
#ifndef ISOLATIONALGS_ISOLATIONPFLOWCORRECTION_H
#define ISOLATIONALGS_ISOLATIONPFLOWCORRECTION_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/AuxElement.h"
#include "ParticlesInConeTools/IPFlowObjectsInConeTool.h"

#include "xAODPrimitives/IsolationType.h"
#include "xAODPrimitives/IsolationFlavour.h"
#include "RecoToolInterfaces/IsolationCommon.h"

#include "IsolationCorrections/IIsolationCorrectionTool.h"

namespace xAOD {
  class INeutralEFlowIsolationTool;
  class ICaloTopoClusterIsolationTool;
  class ICaloCellIsolationTool;
  class ITrackIsolationTool;
}
class CaloCellContainer;

class IsolationPflowCorrection
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  IsolationPflowCorrection( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~IsolationPflowCorrection()=default; 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  IsolationPflowCorrection() = delete;

  /// Containers
  std::string m_ElectronContainerName;
  std::string m_PhotonContainerName;

  StatusCode DecorateEgamma(std::string egType);

  template< class CONTAINER, class AUXSTORE > StatusCode deepCopy( const std::string& key ) const;
  template< class CONTAINER, class AUXSTORE > StatusCode deepCopyImp( const std::string& key ) const;

  // pflow objects in cone tool
  ToolHandle<xAOD::IPFlowObjectsInConeTool> m_pflowObjectsInConeTool; 

  
}; 

#endif //> !ISOLATIONALGS_ISOLATIONPFLOWCORRECTION_H
