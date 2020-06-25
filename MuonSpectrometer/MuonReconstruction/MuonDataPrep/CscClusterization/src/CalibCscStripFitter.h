/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CalibCscStripFitter_H
#define CalibCscStripFitter_H

// David Adams
// April 2007
//
// Strip fitter using the parabolic fit fron the CSC calibration tool.

#include "CscClusterization/ICscStripFitter.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "CscCalibTools/ICscCalibTool.h"

#include <vector>
#include <string>

class CalibCscStripFitter : virtual public ICscStripFitter, public AthAlgTool {

public:  // Ctors and dtor.
  enum NoiseOption {
    rms = 0, sigma, f001
  };

  CalibCscStripFitter(std::string, std::string, const IInterface*);

  ~CalibCscStripFitter()=default;

public:  // AlgTool methods

  StatusCode initialize();

public:  // Interface methods

  // Tell compiler not to hide other fit methods.
  using ICscStripFitter::fit;

  // Fit a list of sample charges using the calibration tool.
  // If that fit fails, the peak channel is used.
  //  Result fit(const ChargeList& charges, double samplingTime, IdentifierHash& stripHashId ) const;
  //  Result fit(const ChargeList& charges, double samplingTime, Identifier& stripId ) const;
  Result fit(const ChargeList& ChargeList,
             double samplingTime, bool samplingPhase, Identifier& sid) const;

private:  // data

  // Job options.
  //  double m_qerr;          // Charge error for a successful fit.
  double m_terr;              // Time error for a succcessful fit.
  //  double m_qerr_fail;     // Charge error for a failed fit.
  double m_terr_fail;         // Time error for a failed fit.
  double m_qerrprop;          // Charge calibration error
  bool   m_doCorrection;      // Correction for para to bipolar
  double m_chargeErrorScaler;       // Charge error scaler 

  std::string m_noiseOptionStr;
  NoiseOption m_noiseOption;

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  ToolHandle<ICscCalibTool> m_cscCalibTool;

};

#endif
