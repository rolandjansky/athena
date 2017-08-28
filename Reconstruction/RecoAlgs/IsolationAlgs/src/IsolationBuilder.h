///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IsolationBuilder.h 
// Header file for class IsolationBuilder
/////////////////////////////////////////////////////////////////// 
#ifndef ISOLATIONALGS_ISOLATIONBUILDER_H
#define ISOLATIONALGS_ISOLATIONBUILDER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/AuxElement.h"

#include "xAODPrimitives/IsolationType.h"
#include "xAODPrimitives/IsolationFlavour.h"
#include "RecoToolInterfaces/IsolationCommon.h"

#include "IsolationCorrections/IIsolationCorrectionTool.h"
#include "xAODMuon/MuonContainer.h"

namespace xAOD {
  class INeutralEFlowIsolationTool;
  class ICaloTopoClusterIsolationTool;
  class ICaloCellIsolationTool;
  class ITrackIsolationTool;
}
class CaloCellContainer;

class IsolationBuilder
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  IsolationBuilder( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~IsolationBuilder(); 

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
  IsolationBuilder();

  /// Containers
  std::string m_ElectronContainerName;
  std::string m_FwdElectronContainerName;
  std::string m_PhotonContainerName;
  SG::ReadHandleKey<xAOD::MuonContainer> m_MuonContainerName;

  /** @brief Tool for cell isolation calculation */
  ToolHandle<xAOD::ICaloCellIsolationTool> m_cellIsolationTool;
  std::string m_cellsName;     
  const CaloCellContainer* m_cellColl;


  /** @brief Tool for topo isolation calculation */
  ToolHandle<xAOD::ICaloTopoClusterIsolationTool> m_topoIsolationTool;

  /** @brief Tool for neutral pflow isolation calculation */
  ToolHandle<xAOD::INeutralEFlowIsolationTool> m_pflowIsolationTool;

  /** @brief Tool for neutral pflow isolation calculation */
  ToolHandle<xAOD::ITrackIsolationTool> m_trackIsolationTool;
  bool m_useBremAssoc;

  /** @brief Isolation types (for the alg. properties, only vector<vector<double>> available */
  std::vector<std::vector<double> > m_egisoInts, m_muisoInts;
  std::vector<std::vector<double> > m_egcorInts, m_mucorInts;
  std::vector<std::vector<double> > m_fecorInts, m_feisoInts;

  struct CaloIsoHelp {
    std::vector<SG::AuxElement::Decorator<float>*> isoDeco;
    std::vector<SG::AuxElement::Decorator<float>*> nonecoreCorisoDeco;
    SG::AuxElement::Decorator<float>* coreCorisoDeco;
    std::vector<xAOD::Iso::IsolationType> isoTypes;
    xAOD::CaloCorrection CorrList;
  };
  std::map<xAOD::Iso::IsolationFlavour,CaloIsoHelp> m_egCaloIso, m_feCaloIso, m_muCaloIso;

  struct TrackIsoHelp {
    std::vector<SG::AuxElement::Decorator<float>*> isoDeco;
    SG::AuxElement::Decorator<float>* coreCorisoDeco;
    std::vector<xAOD::Iso::IsolationType> isoTypes;
    xAOD::TrackCorrection CorrList;
  };
  std::map<xAOD::Iso::IsolationFlavour,TrackIsoHelp> m_egTrackIso, m_muTrackIso;

  // individual flags to run or not computation on electron or photon
  bool m_isolateEl, m_isolatePh;
  
  // Compute the isolation variables
  StatusCode IsolateEgamma(std::string egType);
  StatusCode IsolateMuon();

  std::string m_customConfig;
  std::string m_customConfigEl, m_customConfigPh, m_customConfigFwd, m_customConfigMu; // for the time being, only mu vs eg, no separation in eg
  StatusCode DecorateEgamma(std::string egType);
  StatusCode DecorateMuon();

  // For an AODFix
  bool m_isAODFix;
  bool m_allTrackRemoval;
  ToolHandle<CP::IIsolationCorrectionTool> m_leakTool;
  StatusCode runLeakage();
  // From Attila, for a deep copy
  template< class CONTAINER, class AUXSTORE > StatusCode deepCopy( const std::string& key ) const;
  template< class CONTAINER, class AUXSTORE > StatusCode deepCopyImp( const std::string& key ) const;

  
}; 

#endif //> !ISOLATIONALGS_ISOLATIONBUILDER_H
