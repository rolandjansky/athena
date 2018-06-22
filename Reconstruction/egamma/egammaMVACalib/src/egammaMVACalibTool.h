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

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "TH2Poly.h"

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
      "folder", "egammaMVACalib/offline/v3",
      "string with folder for weight files"}; 

  Gaudi::Property<bool> m_use_layer_corrected {this,
      "use_layer_corrected", true,
      "whether to use layer corrections"};

  std::unique_ptr<TH2Poly> m_hPoly;

  std::vector<MVAUtils::BDT> m_BDTs;

  /// map of variable string to function (taking calo cluster)
  std::map<std::string, std::functional<float(const xAOD::CaloCluster&)> m_clusterFuncs;
  /// map of variable string to function (taking electron)
  std::map<std::string, std::functional<float(const xAOD::Electron&)> m_ElectronFuncs;
  /// map of variable string to function (taking photon)
  std::map<std::string, std::functional<float(const xAOD::Photon&)> m_PhotonFuncs;

  /// initialize the functions needed for electrons
  StatusCode initializeElectronFuncs();
  /// initialize the functions needed for unconverted photons
  StatusCode initializeUnconvertedPhotonFuncs();
  /// initialize the functions needed for converted photons
  StatusCode initializeConvertedPhotonFuncs();

};

#endif 
