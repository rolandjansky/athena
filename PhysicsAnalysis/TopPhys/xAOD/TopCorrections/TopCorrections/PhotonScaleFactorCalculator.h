/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PhotonScaleFactorCalculator.h 802226 2017-04-04 16:13:10Z grancagn $
#ifndef TOPCORRECTIONS_PHOTONSCALEFACTORCALCULATOR_H_
#define TOPCORRECTIONS_PHOTONSCALEFACTORCALCULATOR_H_

/**
  * @author John Morris <john.morris@cern.ch>
  *
  * @brief PhotonScaleFactorCalculator
  *   Calculate all photon scale factors and decorate
  *
  * $Revision: 802226 $
  * $Date: 2017-04-04 18:13:10 +0200 (Tue, 04 Apr 2017) $
  *
  **/

// system include(s):
#include <memory>
#include <set>
#include <string>

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

// CP Tool include(s):
#include "PhotonEfficiencyCorrection/IAsgPhotonEfficiencyCorrectionTool.h"

namespace top {
// Forward declaration(s):
class TopConfig;

class PhotonScaleFactorCalculator final : public asg::AsgTool {
 public:
  explicit PhotonScaleFactorCalculator(const std::string& name);
  virtual ~PhotonScaleFactorCalculator() {}

  // Delete Standard constructors
  PhotonScaleFactorCalculator(const PhotonScaleFactorCalculator& rhs) = delete;
  PhotonScaleFactorCalculator(PhotonScaleFactorCalculator&& rhs) = delete;
  PhotonScaleFactorCalculator& operator=(const PhotonScaleFactorCalculator& rhs) = delete;

  StatusCode initialize();
  StatusCode execute();

 private:
  std::shared_ptr<top::TopConfig> m_config;

  CP::SystematicSet m_systNominal;
  CP::SystematicSet m_systEffIDUp;
  CP::SystematicSet m_systEffIDDown;
  CP::SystematicSet m_systEffLowPtIsoUp;
  CP::SystematicSet m_systEffLowPtIsoDown;
  CP::SystematicSet m_systEffTrkIsoUp;
  CP::SystematicSet m_systEffTrkIsoDown;
  CP::SystematicSet m_systIsoDDonoffUp;
  CP::SystematicSet m_systIsoDDonoffDown;

  ToolHandle<IAsgPhotonEfficiencyCorrectionTool> m_photonEffSF;
  ToolHandle<IAsgPhotonEfficiencyCorrectionTool> m_photonIsoSF;
  ToolHandle<IAsgPhotonEfficiencyCorrectionTool> m_photonLooseIsoSF;

  bool m_photonIsoSF_exists;
  bool m_photonLooseIsoSF_exists;

  std::string m_decor_isoSF;
  std::string m_decor_isoSF_loose;        

};
}  // namespace top
#endif  // TOPCORRECTIONS_PHOTONSCALEFACTORCALCULATOR_H_
