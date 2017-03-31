/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CalibCscStripFitter.h

#ifndef CalibCscStripFitter_H
#define CalibCscStripFitter_H

// David Adams
// April 2007
//
// Strip fitter using the parabolic fit fron the CSC calibration tool.

#include <vector>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CscClusterization/ICscStripFitter.h"

class ICscCalibTool;
class CscIdHelper;
namespace Muon {
  class CscStripPrepData;
}
namespace MuonGM 
{
    class MuonDetectorManager;
}


class CalibCscStripFitter : virtual public ICscStripFitter, public AthAlgTool {

public:  // Ctors and dtor.
  enum NoiseOption {
    rms = 0, sigma, f001
  };

  // Constructor.
  CalibCscStripFitter(std::string, std::string, const IInterface*);

  // Destructor.
  ~CalibCscStripFitter();

public:  // AlgTool methods

  // Initialization.
  StatusCode initialize();

  // Finalization.
  StatusCode finalize();

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

  // Pointer to muon geometry manager.
  const MuonGM::MuonDetectorManager* m_pmuon_detmgr;
  const CscIdHelper* m_phelper;

  // Calibration tool.
  ToolHandle<ICscCalibTool> m_cscCalibTool;

};

#endif
