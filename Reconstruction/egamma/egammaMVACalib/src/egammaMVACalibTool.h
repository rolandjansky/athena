/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAMVACALIB_EGAMMAMVACALIBTOOL_H
#define EGAMMAMVACALIB_EGAMMAMVACALIBTOOL_H

// Package includes
#include "egammaInterfaces/IegammaMVACalibTool.h"
#include "xAODEgamma/EgammaEnums.h"
#include "MVAUtils/BDT.h"
#include "egammaMVACalib/egammaMVAFunctions.h"

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
 * @brief A tool used by the egammaMVASvc to help calibrate energy for one particle type.
 *
 * The particle type to be calibrated must be specified by the property ParticleType.
 * The property folder must be set with the path to a folder containig three files:
 * - MVACalib_electron.weights.root
 * - MVACalib_unconvertedPhoton.weights.root
 * - MVACalib_convertedPhoton.weights.root
 *
 * The ROOT files should have in the following content:
 * - a TH2Poly object named "hPoly". This is used to decide which BDT to use. The
 *   x-axis is cluster eta, the y-axis is the raw-accordion-Et in GeV. The BDT that
 *   will be used for each event is the one with index equal to the content of the
 *   TH2Poly - 1, e.g. const int bin = m_hPoly->FindBin(etaVar, energyVarGeV) - 1.
 *   (the minimum value in the TH2Poly is 1)
 * - several TTree named BDTX where X is an integer index (not padded). X here counts
 *   from 0. Each TTree has the weights of a BDT and it is the input to MVAUtils::BDT.
 *   Alternatively the TTree can be inside a TObjArray named "trees".
 * - a TObjArray named "variables" containing many TObjString, each one for each BDT.
 *   Each TObjString is a string containing the names of the input variables for each BDT,
 *   separated by ";". The names should be understandable by
 *   egammaMVAFunctions::initializeElectronFuncs,
 *   egammaMVAFunctions::initializeUnconvertedPhotonFuncs,
 *   egammaMVAFunctions::initializeConvertedPhotonFuncs.
 * - a TObjectArray named "shifts" containing many TObjString, each one for each BDT.
 *   Each TObjString is a string which represent the formula to compute the shift
 *   (used to construct a TFormula). The variables is the Et in GeV after the calibration.
 *   The value of the shift is divided by the energy calibrated by the BDT.
 *   
 * 
 * On data the property use_layer_corrected should be set to true. In reconstruction
 * this flag is always false. In PhysicsAnalysis it should be set appropriately.
 * When set to true when using the layer energies as input the data-driver-corrected
 * version are used.
 **/

class egammaMVACalibTool : public extends<AthAlgTool, IegammaMVACalibTool> {
public:
  egammaMVACalibTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~egammaMVACalibTool() override;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  /** how the output of the BDT is used
   * correctEaccordion: energy = raw energy * BDT
   * fullCalibration: energy = BDT
   */
  enum CalibrationType {correctEaccordion, fullCalibration};

  /** shift to be applied after the BDT.
   *  Only MEAN10TOTRUE and MEAN10TOTRUE are supperted
   */
  enum ShiftType {NOSHIFT=0, MEAN10TOTRUE};

  /** returns the calibrated energy **/
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

  /// shifts formulas
  std::vector<TFormula> m_shifts;

  /// a function called by initialize to setup the BDT, funcs, and shifts.
  StatusCode setupBDT(const egammaMVAFunctions::funcMap_t& funcLibrary,
                      const std::string& fileName);

  /// a utility to get a TString out of an TObjString pointer
  const TString& getString(TObject* obj) const;

};

#endif
