/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef EGAMMAMVACALIB_EGAMMAMVACALIBTOOL_H
#define EGAMMAMVACALIB_EGAMMAMVACALIBTOOL_H

// Package includes
#include "egammaInterfaces/IegammaMVACalibTool.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "MVAUtils/BDT.h"
#include "egammaMVACalib/egammaMVAFunctions.h"

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "TH2Poly.h"
#include "TObject.h"
#include "TString.h"
#include "TFormula.h"
#include "TTree.h"

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

  float getEnergy(const xAOD::CaloCluster& clus,
                  const xAOD::Egamma* eg) const override final;

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

  Gaudi::Property<bool> m_useLayerCorrected {this,
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

  /// a function called by initialize to setup the BDT, funcs, and shifts.
  StatusCode setupBDT(const egammaMVAFunctions::funcMap_t& funcLibrary,
		      const std::string& fileName);

  /// a utility to get a TString out of an TObjString pointer
  const TString& getString(TObject* obj) const;
  
};

#endif 
