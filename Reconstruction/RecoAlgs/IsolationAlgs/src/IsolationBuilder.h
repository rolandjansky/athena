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
#include <set>
#include <vector>

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
  Gaudi::Property<std::vector<std::vector<int> > > m_elisoInts {this,
      "ElIsoTypes", {}, 
      "The isolation types to do for electrons: vector of vector of enum type Iso::IsolationType, stored as float"};

  Gaudi::Property<std::vector<std::vector<unsigned int> > > m_elcorInts {this,
      "ElCorTypes", {}, 
      "The correction types to do for electron iso: vector of vector of enum type Iso::IsolationCalo/TrackCorrection, stored as float"};

  /** @brief Isolation types (for the alg. properties, only vector<vector<double>> available */
  Gaudi::Property<std::vector<std::vector<int> > > m_phisoInts {this,
      "PhIsoTypes", {}, 
      "The isolation types to do for photons: vector of vector of enum type Iso::IsolationType, stored as float"};

  Gaudi::Property<std::vector<std::vector<unsigned int> > > m_phcorInts {this,
      "PhCorTypes", {}, 
      "The correction types to do for photons iso: vector of vector of enum type Iso::IsolationCalo/TrackCorrection, stored as float"};

  Gaudi::Property<std::vector<std::vector<int> > > m_muisoInts {this,
      "MuIsoTypes", {}, 
      "The isolation types to do for Muons : vector of vector of enum type Iso::IsolationType, stored as float"};

  Gaudi::Property<std::vector<std::vector<unsigned int> > > m_mucorInts {this,
      "MuCorTypes", {}, 
      "The correction types to do for Muon iso: vector of vector of enum type Iso::IsolationCalo/TrackCorrection, stored as float"};
  Gaudi::Property<std::vector<std::vector<unsigned int> > > m_fecorInts {this,
      "FeCorTypes", {}, 
      "The correction types to do for forward electron iso: vector of vector of enum type Iso::IsolationCalo/TrackCorrection, stored as float"};

  Gaudi::Property<std::vector<std::vector<int> > > m_feisoInts {this,
      "FeIsoTypes", {},
      "The isolation types to do for forward electron: vector of vector of enum type Iso::IsolationType, stored as float"};

  Gaudi::Property<bool> m_addCoreCorr{this, "AddCoreCorr", true, 
      "Add core correction for muon calo isolation"};

  template<class T> struct CaloIsoHelp {
    std::vector<SG::WriteDecorHandleKey<T> > isoDeco;
    std::vector<SG::WriteDecorHandleKey<T> > nonecoreCorisoDeco;
    SG::WriteDecorHandleKey<T> coreCorisoDeco;
    std::vector<xAOD::Iso::IsolationType> isoTypes;
    xAOD::CaloCorrection CorrList;
  };


  // struct CaloIsoHelp {
  //   std::vector<SG::AuxElement::Decorator<float>*> isoDeco;
  //   std::vector<SG::AuxElement::Decorator<float>*> nonecoreCorisoDeco;
  //   SG::AuxElement::Decorator<float>* coreCorisoDeco;
  //   std::vector<xAOD::Iso::IsolationType> isoTypes;
  //   xAOD::CaloCorrection CorrList;
  // };

  std::map<xAOD::Iso::IsolationFlavour,CaloIsoHelp<xAOD::ElectronContainer> > m_elCaloIso;
  std::map<xAOD::Iso::IsolationFlavour,CaloIsoHelp<xAOD::PhotonContainer> > m_phCaloIso;
  std::map<xAOD::Iso::IsolationFlavour,CaloIsoHelp<xAOD::ElectronContainer> > m_feCaloIso;
  std::map<xAOD::Iso::IsolationFlavour,CaloIsoHelp<xAOD::MuonContainer> > m_muCaloIso;

  template<class T> struct TrackIsoHelp {
    std::vector<SG::WriteDecorHandleKey<T> > isoDeco;
    std::vector<SG::WriteDecorHandleKey<T> > isoDecoV;
    SG::WriteDecorHandleKey<T> coreCorisoDeco;
    std::vector<xAOD::Iso::IsolationType> isoTypes;
    xAOD::TrackCorrection CorrList;
  };

  // struct TrackIsoHelp {
  //   std::vector<SG::AuxElement::Decorator<float>*> isoDeco;
  //   SG::AuxElement::Decorator<float>* coreCorisoDeco;
  //   std::vector<xAOD::Iso::IsolationType> isoTypes;
  //   xAOD::TrackCorrection CorrList;
  // };
  std::map<xAOD::Iso::IsolationFlavour,TrackIsoHelp<xAOD::ElectronContainer> > m_elTrackIso;
  std::map<xAOD::Iso::IsolationFlavour,TrackIsoHelp<xAOD::PhotonContainer> > m_phTrackIso;
  std::map<xAOD::Iso::IsolationFlavour,TrackIsoHelp<xAOD::MuonContainer> > m_muTrackIso;

  template<class T>
  StatusCode initializeIso(std::set<xAOD::Iso::IsolationFlavour>& runIsoType, // out
			   std::map<xAOD::Iso::IsolationFlavour,CaloIsoHelp<T> >* caloIsoMap, // out
			   std::map<xAOD::Iso::IsolationFlavour,TrackIsoHelp<T> >* trackIsoMap, // out
			   const std::string& containerName,
			   const std::vector<std::vector<int> >& isoInts,
			   const std::vector<std::vector<int> >& corrInts,
			   const std::string& customConfig,
			   bool addCoreCorr);
  
  // Compute the isolation variables
  StatusCode IsolateEgamma(std::string egType);
  StatusCode IsolateMuon();

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

template<class T>
StatusCode IsolationBuilder::initializeIso(std::set<xAOD::Iso::IsolationFlavour>& runIsoType, // out
					   std::map<xAOD::Iso::IsolationFlavour,CaloIsoHelp<T> >* caloIsoMap, // out
					   std::map<xAOD::Iso::IsolationFlavour,TrackIsoHelp<T> >* trackIsoMap, // out

					   const std::string& containerName,
					   const std::vector<std::vector<int> >& isoInts,
					   const std::vector<std::vector<int> >& corrInts,
					   const std::string& customConfig,
					   bool addCoreCorr)
{

  std::string prefix = containerName + ".";

  for (size_t flavor = 0; flavor < isoInts.size(); flavor++) {
    // iterate over the flavor (cell, topo, eflow, track
    //   Note: it is a configuration error if different types
    //         are included in one inner vector

    CaloIsoHelp<T> cisoH;
    TrackIsoHelp<T> tisoH

    //std::vector<SG::AuxElement::Decorator<float>*> Deco;
    xAOD::Iso::IsolationFlavour isoFlav =
      xAOD::Iso::numIsolationFlavours;
    xAOD::Iso::IsolationFlavour oldIsoFlav =
      xAOD::Iso::numIsolationFlavours;
    for (size_t type = 0; type < isoInts[flavor].size();type++) { // the cone size
      xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(isoInts[flavor][type]);
      isoFlav = xAOD::Iso::isolationFlavour(isoType);
      if (oldIsoFlav != xAOD::Iso::numIsolationFlavours && oldIsoFlav != isoFlav) {
	ATH_MSG_ERROR("Configuration error:  can only have one type of isolation in inner vector");
	return StuatusCode::FAILURE;
      }
      oldIsoFlav = isoFlav;
      std::string isoName = prefix + xAOD::Iso::toString(isoType);
      if (customConfig != "") {
	isoName += "_" + customConfig;
      }
      if (isoFlav == xAOD::Iso::etcone || isoFlav == xAOD::Iso::topoetcone || isoFlav == xAOD::Iso::neflowisol) {
	cisoH.isoTypes.push_back(isoType);
	cisoH.isoDeco.emplace_back(isoName);
	ATH_CHECK(cisoH.isoDeco.back().initialize());
      } else if (isoFlav == xAOD::Iso::ptcone) {
	tisoH.isoTypes.push_back(isoType);
	tisoH.isoDeco.emplace_back(isoName);
	ATH_CHECK(tisoH.isoDeco.back().initialize());
	std::string isoNameV = prefix + "ptvarcone" + std::to_string(100*xAOD::Iso::coneSize(isoType));
	if (customConfig != "") {
	  isoNameV += "_" + customConfig;
	}
	tisoH.isoDecoV.emplace_back(isoNameV);
	ATH_CHECK(tisoH.isoDeco.back().initialize());
      } else {
	ATH_MSG_ERROR("Configuration error: Isolation flavor " << isFlav << " not supported.");
	return StuatusCode::FAILURE;
      }	
    }
    if (isoFlav == xAOD::Iso::etcone || isoFlav == xAOD::Iso::topoetcone || isoFlav == xAOD::Iso::neflowisol) {
      
      for (size_t corrType = 0; corrType < corInts[flavor].size(); corrType++) {
	const unsigned int cor = corInts[flavor][corrType];
	cisoH.CorrList.calobitset.set(cor);
	const xAOD::Iso::IsolationCaloCorrection isoCor = static_cast<xAOD::Iso::IsolationCaloCorrection>(cor);
	if (addCore && (isoCor == xAOD::Iso::coreCone || isoCor == xAOD::Iso::coreMuon) ) {
	  std::string isoCorName = prefix;
	  if (isoFlav == xAOD::Iso::topoetcone || isoFlav == xAOD::Iso::neflowisol) {
	    isoCorName += xAOD::Iso::toString(isoFlav);
	  }
	  isoCorName += xAOD::Iso::toString(isoCor) + xAOD::Iso::toString(xAOD::Iso::coreEnergy) 
	    + "Correction"; // hard coded since we never store the core area in fact
	  if (customConfig != "") {
	    isoCorName += "_" + customConfig;
	  }
	  cisoH.coreCorisoDeco = isoCorName;
	  ATH_CHECK(cisoH.coreCorisoDeco.initialize());
	}
      }
      if (caloIsoMap) {
	caloIsoMap->insert(std::make_pair(isoFlav,cisoH));
      } else {
	ATH_MSG_ERROR("caloIsoMap was nullptr but the configuration attempted to use it");
	return StuatusCode::FAILURE;
      }
    } else if (isoFlav == xAOD::Iso::ptcone) {
      tisoH.CorrListtrackbitset.set(static_cast<unsigned int>(xAOD::Iso::coreTrackPtr));
      if (trackIsoMap) {
	trackIsoMap->insert(std::make_pair(isoFlav,tisoH));
      } else {
	ATH_MSG_ERROR("trackIsoMap was nullptr but the configuration attempted to use it");
	return StuatusCode::FAILURE;
      }	
    } else 
      ATH_MSG_WARNING("Isolation flavour " << xAOD::Iso::toString(isoFlav) << " does not exist ! Check your inputs");
    runIsoType.insert(isoFlav);
  }
}


#endif //> !ISOLATIONALGS_ISOLATIONBUILDER_H
