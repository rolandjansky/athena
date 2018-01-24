///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IsolationBuilder.h 
// Header file for class IsolationBuilder
/////////////////////////////////////////////////////////////////// 
#ifndef ISOLATIONALGS_ISOLATIONBUILDER_H
#define ISOLATIONALGS_ISOLATIONBUILDER_H 1

// to allow vector<vector<int>> properties
#include "VectorVectorIntParser.h"

// STL includes
#include <string>
#include <set>
#include <vector>
#include <utility>

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
#include "xAODPrimitives/IsolationConeSize.h"
#include "xAODPrimitives/IsolationHelpers.h"
#include "xAODPrimitives/IsolationFlavour.h"
#include "xAODBase/IParticleContainer.h"

#include "RecoToolInterfaces/ICaloCellIsolationTool.h"
#include "RecoToolInterfaces/ICaloTopoClusterIsolationTool.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "RecoToolInterfaces/INeutralEFlowIsolationTool.h"

#include "CaloEvent/CaloCellContainer.h"


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
  Gaudi::Property<std::vector<std::vector<int> > > m_elisoInts {this,
      "ElIsoTypes", {}, 
      "The isolation types to do for electrons: vector of vector of enum type Iso::IsolationType, stored as float"};

  Gaudi::Property<std::vector<std::vector<int> > > m_elcorInts {this,
      "ElCorTypes", {}, 
      "The correction types to do for electron iso: vector of vector of enum type Iso::IsolationCalo/TrackCorrection, stored as float"};

  /** @brief Isolation types (for the alg. properties, only vector<vector<double>> available */
  Gaudi::Property<std::vector<std::vector<int> > > m_phisoInts {this,
      "PhIsoTypes", {}, 
      "The isolation types to do for photons: vector of vector of enum type Iso::IsolationType, stored as float"};

  Gaudi::Property<std::vector<std::vector<int> > > m_phcorInts {this,
      "PhCorTypes", {}, 
      "The correction types to do for photons iso: vector of vector of enum type Iso::IsolationCalo/TrackCorrection, stored as float"};

  Gaudi::Property<std::vector<std::vector<int> > > m_muisoInts {this,
      "MuIsoTypes", {}, 
      "The isolation types to do for Muons : vector of vector of enum type Iso::IsolationType, stored as float"};

  Gaudi::Property<std::vector<std::vector<int> > > m_mucorInts {this,
      "MuCorTypes", {}, 
      "The correction types to do for Muon iso: vector of vector of enum type Iso::IsolationCalo/TrackCorrection, stored as float"};
  Gaudi::Property<std::vector<std::vector<int> > > m_fecorInts {this,
      "FeCorTypes", {}, 
      "The correction types to do for forward electron iso: vector of vector of enum type Iso::IsolationCalo/TrackCorrection, stored as float"};

  Gaudi::Property<std::vector<std::vector<int> > > m_feisoInts {this,
      "FeIsoTypes", {},
      "The isolation types to do for forward electron: vector of vector of enum type Iso::IsolationType, stored as float"};

  Gaudi::Property<bool> m_addCoreCorr{this, "AddCoreCorr", true, 
      "Add core correction for muon calo isolation"};

  struct CaloIsoHelpKey {
    std::vector<SG::WriteDecorHandleKey<xAOD::IParticleContainer> > isoDeco;
    bool addCoreCorr = false;
    SG::WriteDecorHandleKey<xAOD::IParticleContainer> coreCorisoDeco;
    std::vector<xAOD::Iso::IsolationType> isoTypes;
    xAOD::CaloCorrection CorrList;
  };

  std::vector<std::pair<xAOD::Iso::IsolationFlavour,CaloIsoHelpKey> > m_elCaloIso;
  std::vector<std::pair<xAOD::Iso::IsolationFlavour,CaloIsoHelpKey> > m_phCaloIso;
  std::vector<std::pair<xAOD::Iso::IsolationFlavour,CaloIsoHelpKey> > m_feCaloIso;
  std::vector<std::pair<xAOD::Iso::IsolationFlavour,CaloIsoHelpKey> > m_muCaloIso;

  struct TrackIsoHelpKey {
    std::vector<SG::WriteDecorHandleKey<xAOD::IParticleContainer> > isoDeco;
    std::vector<SG::WriteDecorHandleKey<xAOD::IParticleContainer> > isoDecoV;
    std::vector<xAOD::Iso::IsolationType> isoTypes;
    xAOD::TrackCorrection CorrList;
  };

  std::vector<std::pair<xAOD::Iso::IsolationFlavour,TrackIsoHelpKey> > m_elTrackIso;
  std::vector<std::pair<xAOD::Iso::IsolationFlavour,TrackIsoHelpKey> > m_phTrackIso;
  std::vector<std::pair<xAOD::Iso::IsolationFlavour,TrackIsoHelpKey> > m_muTrackIso;

  struct CaloIsoHelpHandles {

    CaloIsoHelpHandles(const CaloIsoHelpKey& keys);

    std::vector<SG::WriteDecorHandle<xAOD::IParticleContainer, float> > isoDeco;
    // SG::WriteDecorHandle<xAOD::IParticleContainer, float> coreCorisoDeco; // try moving it
  };


  struct TrackIsoHelpHandles {

    TrackIsoHelpHandles(const TrackIsoHelpKey& keys);

    std::vector<SG::WriteDecorHandle<xAOD::IParticleContainer, float> > isoDeco;
    std::vector<SG::WriteDecorHandle<xAOD::IParticleContainer, float> > isoDecoV;
  };


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

  Gaudi::Property<bool> m_allTrackRemoval {this, 
      "AllTrackRemoval", true};

  StatusCode initializeIso(std::set<xAOD::Iso::IsolationFlavour>& runIsoType, // out
			   std::vector<std::pair<xAOD::Iso::IsolationFlavour,CaloIsoHelpKey > >* caloIsoMap, // out
			   std::vector<std::pair<xAOD::Iso::IsolationFlavour,TrackIsoHelpKey > >* trackIsoMap, // out
			   const std::string& containerName,
			   const std::vector<std::vector<int> >& isoInts,
			   const std::vector<std::vector<int> >& corInts,
			   const std::string& customConfig,
			   bool addCoreCorr);

  StatusCode executeCaloIso(const std::vector<std::pair<xAOD::Iso::IsolationFlavour,CaloIsoHelpKey> >& caloIsoMap);

  StatusCode executeTrackIso(const std::vector<std::pair<xAOD::Iso::IsolationFlavour,TrackIsoHelpKey> >& trackIsoMap);

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
