/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPOBJECTSELECTIONTOOLS_PHOTONMC16_H_
#define TOPOBJECTSELECTIONTOOLS_PHOTONMC16_H_

#include <memory>
#include <string>

#include "TopObjectSelectionTools/PhotonSelectionBase.h"
#include "TopObjectSelectionTools/IsolationTools.h"

#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"

namespace top {

/**
 * @brief Photon selection for top analyses.
 *        For Release 21 specific cleaning 
 */

class PhotonMC16 : public top::PhotonSelectionBase {
 public:
  /**
   * @brief Class to help select good photons.
   *
   * @param ptcut The minimum pT cut to apply to the photons.
   * @param etamax The maximum eta cut
   * @param isolation nullptr for un-isolated, or a new "isolation object" to
   * @param usePhotonShowerShapeVariables to turn on/off the reading of shower shape variables
   * apply isolation cuts
   */
   PhotonMC16(double ptcut, double etamax, IsolationBase* isolation, bool usePhotonShowerShapeVariables);

  /**
   * @brief Class to help select good photons.
   *
   * @param ptcut The minimum pT cut to apply to the photons.
   * @param etamax The maximum eta cut
   * @param tightID The ID level used to select tight photons.
   * @param looseID The ID level used to select loose photons.
   * @param isolation nullptr for un-isolated, or a new "isolation object" to
   * @param usePhotonShowerShapeVariables to turn on/off the reading of shower shape variables
   * apply isolation cuts
   */
  PhotonMC16(double ptcut, double etamax,
             const std::string& tightID,
             const std::string& looseID,
             IsolationBase* isolation,
	     bool usePhotonShowerShapeVariables);

  /**
   * @brief The cuts to select good photons for your analysis should be
   * implemented in here.
   *
   * @param ph The photon to cut on (all photons in the event are passed
   * to the tool)
   * @return True if this is a good photon, false otherwise.
   */
  bool passSelection(const xAOD::Photon& ph) const override;

  /**
   * @brief The loose selection.
   *
   * @param ph
   * @return
   */
  bool passSelectionLoose(const xAOD::Photon& ph) const override;

  /**
   * @brief Print some useful information about the photon selection.
   *
   * Usually this goes to the log file, so you know what you ran with.
   *
   * @param Where the print-out should go, e.g. cout.
   */
  void print(std::ostream&) const override;

 private:
  /**
   * @brief Do all the cuts except for the isolation.
   *
   * @param ph The photon in question
   * @return True if the photon passes
   */
  bool passSelectionNoIsolation(const xAOD::Photon& ph,
                                const std::string& photon_selection) const;

  std::string getConfigFile(int operatingPoint);

  // minimum pT cut to apply
  double m_ptcut;

  // max eta cut
  double m_etamax;

  std::string m_photon_selection;
  std::string m_loose_photon_selection;

  // Proper tool to select photons.
  // ToolHandle<AsgPhotonIsEMSelector> m_photonTightIsEMSelector;
  // ToolHandle<AsgPhotonIsEMSelector> m_photonMediumIsEMSelector;
  // ToolHandle<AsgPhotonIsEMSelector> m_photonLooseIsEMSelector;

  // The isolation tool, or nullptr if we don't want isolation
  std::unique_ptr<top::IsolationBase> m_isolation;

  mutable bool m_usePhotonShowerShapeVariables;
};

}  // namespace top
#endif  // TOPOBJECTSELECTIONTOOLS_PHOTONMC16_H_
