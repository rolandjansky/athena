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
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"

#include "xAODPrimitives/IsolationType.h"
#include "xAODPrimitives/IsolationFlavour.h"
#include "RecoToolInterfaces/IsolationCommon.h"

#include "IsolationCorrections/IIsolationCorrectionTool.h"
#include "xAODMuon/MuonContainer.h"

#include "RecoToolInterfaces/ICaloCellIsolationTool.h"
#include "RecoToolInterfaces/ICaloTopoClusterIsolationTool.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "RecoToolInterfaces/INeutralEFlowIsolationTool.h"


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

  /// Containers (Is it best to make them as strings? Used by multiple handles)
  Gaudi::Property <std::string> m_ElectronContainerName {this,
      "ElectronCollectionContainerName", "Electrons"};

  Gaudi::Property <std::string> m_FwdElectronContainerName {this,
      "FwdElectronCollectionContainerName", "ForwardElectrons"};

  Gaudi::Property <std::string> m_PhotonContainerName {this,
      "PhotonCollectionContainerName", "Photons"};

  Gaudi::Property <std::string> m_MuonContainerName {this,
      "MuonCollectionContainerName", "Muons"};

  /** @brief Tool for cell isolation calculation */
  ToolHandle<xAOD::ICaloCellIsolationTool> m_cellIsolationTool {this, 
      "CaloCellIsolationTool", "", "Handle of the calo cell IsolationTool"};

  /** @brief Cell container*/
  SG::ReadHandleKey<CaloCellContainer> m_cellsKey {this,
            "CellCollectionName", "AllCalo", "Name of container which contain calo cells"};
  const CaloCellContainer* m_cellColl = nullptr;


  /** @brief Tool for topo isolation calculation */
  ToolHandle<xAOD::ICaloTopoClusterIsolationTool> m_topoIsolationTool {this,
      "CaloTopoIsolationTool", "", "Handle of the calo topo IsolationTool"};

  /** @brief Tool for neutral pflow isolation calculation */
  ToolHandle<xAOD::INeutralEFlowIsolationTool> m_pflowIsolationTool {this,
      "PFlowIsolationTool", "", "Handle of the pflow IsolationTool"};

  /** @brief Tool for neutral pflow isolation calculation */
  ToolHandle<xAOD::ITrackIsolationTool> m_trackIsolationTool {this,
      "TrackIsolationTool", "", "Handle of the track IsolationTool"};

  Gaudi::Property<bool> m_useBremAssoc {this, 
      "useBremAssoc", true, "use track to track assoc after brem"};

  /** @brief Isolation types (for the alg. properties, only vector<vector<double>> available */
  Gaudi::Property<std::vector<std::vector<double> > > m_egisoInts {this,
      "EgIsoTypes", {}, 
      "The isolation types to do for egamma: vector of vector of enum type Iso::IsolationType, stored as float"};

  Gaudi::Property<std::vector<std::vector<double> > > m_egcorInts {this,
      "EgCorTypes", {}, 
      "The correction types to do for egamma iso: vector of vector of enum type Iso::IsolationCalo/TrackCorrection, stored as float"};

  Gaudi::Property<std::vector<std::vector<double> > > m_muisoInts {this,
      "MuIsoTypes", {}, 
      "The isolation types to do for Muons : vector of vector of enum type Iso::IsolationType, stored as float"};

  Gaudi::Property<std::vector<std::vector<double> > > m_mucorInts {this,
      "MuCorTypes", {}, 
      "The correction types to do for Muon iso: vector of vector of enum type Iso::IsolationCalo/TrackCorrection, stored as float"};
  Gaudi::Property<std::vector<std::vector<double> > > m_fecorInts {this,
      "FeCorTypes", {}, 
      "The correction types to do for forward electron iso: vector of vector of enum type Iso::IsolationCalo/TrackCorrection, stored as float"};

  Gaudi::Property<std::vector<std::vector<double> > > m_feisoInts {this,
      "FeIsoTypes", {},
      "The isolation types to do for forward electron: vector of vector of enum type Iso::IsolationType, stored as float"};

  // template<class T> 
  // struct CaloIsoHelp {

  //   StatusCode initialize(const std::string &name); // note, not constructor

  //   std::vector<SG::WriteDecorHandleKey<T> > isoDeco;
  //   std::vector<SG::WriteDecorHandleKey<T> > nonecoreCorisoDeco;
  //   SG::WriteDecorHandleKey<T> coreCorisoDeco;
  //   std::vector<xAOD::Iso::IsolationType> isoTypes;
  //   xAOD::CaloCorrection CorrList;
  // };
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
  Gaudi::Property<bool> m_isolateEl {this, "IsolateEl", true, 
      "since egIsoTypes is common for el and ph, a new flag to control individually : electron"};

  Gaudi::Property<bool> m_isolatePh {this, "IsolatePh", true, 
      "since egIsoTypes is common for el and ph, a new flag to control individually : photon"};
  
  // Compute the isolation variables
  StatusCode IsolateEgamma(std::string egType);
  StatusCode IsolateMuon();

  Gaudi::Property<std::string> m_customConfig {this,
      "CustomConfigurationName", "", 
      "use a custom configuration"};

  // for the time being, only mu vs eg, no separation in eg
  Gaudi::Property<std::string> m_customConfigEl {this,
      "CustomConfigurationNameEl", "", 
      "use a custom configuration for electron"};

  Gaudi::Property<std::string> m_customConfigPh {this,
      "CustomConfigurationNamePh", "", 
      "use a custom configuration for photon"};

  Gaudi::Property<std::string> m_customConfigFwd {this,
      "CustomConfigurationNameFwd", "", 
      "use a custom configuration for forward electron"};

  Gaudi::Property<std::string> m_customConfigMu {this,
      "CustomConfigurationNameMu", "",
      "use a custom configuration for muon"}; 

  StatusCode DecorateEgamma(std::string egType);
  StatusCode DecorateMuon();

  Gaudi::Property<bool> m_allTrackRemoval {this, 
      "AllTrackRemoval", true};

  // For an AODFix
  // JM:  Not yet supported
  // Gaudi::Property<bool> m_isAODFix{this, "IsAODFix", false};
  // ToolHandle<CP::IIsolationCorrectionTool> m_leakTool {this,
  //     "LeakageTool", "", "Handle of the leakage Tool"};
  // StatusCode runLeakage();

  // From Attila, for a deep copy
  //  -- JM: these will have to change in the new code, and I propose moving them to a helper
  // template< class CONTAINER, class AUXSTORE > StatusCode deepCopy( const std::string& key ) const;
  // template< class CONTAINER, class AUXSTORE > StatusCode deepCopyImp( const std::string& key ) const;
  
  
}; 

#endif //> !ISOLATIONALGS_ISOLATIONBUILDER_H
