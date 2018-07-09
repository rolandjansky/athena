/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef EGAMMAMVACALIB_EGAMMAMVACALIBTOOL_H
#define EGAMMAMVACALIB_EGAMMAMVACALIBTOOL_H

// Package includes
#include "egammaInterfaces/IegammaMVACalibTool.h"
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "MVAUtils/BDT.h"

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "TH2Poly.h"
#include "TObject.h"
#include "TString.h"
#include "TFormula.h"

// STL includes
#include <string>
#include <memory>
#include <functional>


/**
 * @class egammaMVACalibTool
 * @brief A tool used by the egammaMVASvc to help manage the MVAs.
 **/
class egammaMVACalibTool : public extends<AthAlgTool, IegammaMVACalibTool> {
public:
  egammaMVACalibTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~egammaMVACalibTool() override;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
 
  enum CalibrationType {correctEcluster, correctEaccordion, 
			fullCalibration, NCalibrationTypes };

  enum ShiftType {NOSHIFT=0, PEAKTOTRUE, MEANTOTRUE, MEDIANTOTRUE, 
		  MEAN10TOTRUE, MEAN20TOTRUE, MEDIAN10TOTRUE, MEDIAN20TOTRUE, 
		  NSHIFTCORRECTIONS};

  float getEnergy(const xAOD::Egamma* eg,
		  const xAOD::CaloCluster* clus) const override final;

private:
  Gaudi::Property<int> m_particleType {this, 
      "ParticleType", xAOD::EgammaParameters::electron,
      "What type of particle do we use"};

  Gaudi::Property<int> m_shiftType {this, 
      "ShiftType", MEAN10TOTRUE, 
      "Shift corrections to apply to value"};

  Gaudi::Property<int> m_calibrationType {this, 
      "CalibrationType", correctEaccordion, 
      "What type of calibration to apply"};

  Gaudi::Property<bool> m_clusterEif0 {this, 
      "useClusterIf0", true, 
      "Use cluster energy if MVA response is 0"};

  /// string with folder for weight files
  Gaudi::Property<std::string> m_folder {this, 
      "folder", "",
      "string with folder for weight files"}; 

  Gaudi::Property<bool> m_use_layer_corrected {this,
      "use_layer_corrected", false,
      "whether to use layer corrections"};

  /// A TH2Poly used to extract bin numbers. Note there is an offset of 1
  std::unique_ptr<TH2Poly> m_hPoly;

  /// Where the BDTs are stored
  std::vector<MVAUtils::BDT> m_BDTs;

  /// where the pointers to the funcs to calculate the vars per BDT
  std::vector<std::vector<std::function<float(const xAOD::Egamma*, const xAOD::CaloCluster*)> > > m_funcs;

  /// shifts for mean10
  std::vector<TFormula> m_shifts;


  /// initialize the functions needed for electrons
  StatusCode initializeElectronFuncs(std::unordered_map<std::string,
				     std::function<float(const xAOD::Egamma*, 
							 const xAOD::CaloCluster*)> >& funcLibrary) const;
  /// initialize the functions needed for unconverted photons
  StatusCode initializeUnconvertedPhotonFuncs(std::unordered_map<std::string,
					      std::function<float(const xAOD::Egamma*, 
								  const xAOD::CaloCluster*)> >& funcLibrary) const;
  /// initialize the functions needed for converted photons
  StatusCode initializeConvertedPhotonFuncs(std::unordered_map<std::string,
					    std::function<float(const xAOD::Egamma*, 
								const xAOD::CaloCluster*)> >& funcLibrary) const;

  /// a function called by the above functions to setup the cluster funcs
  StatusCode initializeClusterFuncs(std::unordered_map<std::string,
				    std::function<float(const xAOD::Egamma*, 
							const xAOD::CaloCluster*)> >& funcLibrary,
				    const std::string& prefix) const;

  /// a function called by the above functions to setup the egamma funcs
  StatusCode initializeEgammaFuncs(std::unordered_map<std::string,
				   std::function<float(const xAOD::Egamma*, 
						       const xAOD::CaloCluster*)> >& funcLibrary,
				   const std::string& prefix) const;

  /// a function called by initialize to setup the BDT, funcs, and shifts.
  StatusCode setupBDT(const std::unordered_map<std::string,
		      std::function<float(const xAOD::Egamma*, 
					  const xAOD::CaloCluster*)> >& funcLibrary,
		      std::string fileName);

  /// a utility to get a TString out of an TObjString pointer
  const TString& getString(TObject* obj) const;
  
};

#endif 
