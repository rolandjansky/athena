/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_TAUDECAYMODENNCLASSIFIER_H
#define TAURECTOOLS_TAUDECAYMODENNCLASSIFIER_H

// base class include(s)
#include "tauRecTools/TauRecToolBase.h"

// xAOD include(s)
#include "xAODTau/TauJet.h"

// lwtnn include(s)
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"
#include "lwtnn/Exceptions.hh"

// standard library include(s)
#include <memory>
#include <vector>
#include <set>
#include <map>

/**
 * @brief Tau decay mode classifier using a neural network
 *
 * @author B. Zhang, C. Deutsch
 *
 */

class TauDecayModeNNClassifier : public TauRecToolBase
{
public:
  ASG_TOOL_CLASS2(TauDecayModeNNClassifier, TauRecToolBase, ITauToolBase)

  explicit TauDecayModeNNClassifier(const std::string &name = "TauDecayModeNNClassifier");
  virtual ~TauDecayModeNNClassifier();

  virtual StatusCode initialize() override;
  virtual StatusCode execute(xAOD::TauJet &xTau) const override;
  virtual StatusCode finalize() override;

private:
  /// properties of the tool
  std::string m_outputName;      //!
  std::string m_probPrefix;      //!
  std::string m_weightFile;      //!
  std::size_t m_maxChargedPFOs;  //!
  std::size_t m_maxNeutralPFOs;  //!
  std::size_t m_maxShotPFOs;     //!
  std::size_t m_maxConvTracks;   //!
  float m_neutralPFOPtCut;       //!
  bool m_ensureTrackConsistency; //!
  bool m_decorateProb;           //!
  /**
   * @brief retrieve the input variables from a TauJet
   * @param xTau a TauJet object
   * @param inputSeqMap a map that contain several sequences
   * 
   * each sequence contains its input variables stored in a vector
   * this map is used by the lwtnn graph
   */
  virtual StatusCode getInputs(const xAOD::TauJet &xTau,
                               std::map<std::string, std::map<std::string, std::vector<double>>> &inputSeqMap) const;
  /// lwtnn graph
  std::unique_ptr<const lwt::LightweightGraph> m_lwtGraph; //!
};

namespace tauRecTools
{
  /**
   * @brief A closely related class that calculates the input variables 
   */
  class TauDecayModeNNVariable
  {
  public:
    TauDecayModeNNVariable() = delete;
    static const std::size_t nClasses = 5;
    static const std::set<std::string> sCommonP4Vars;
    static const std::set<std::string> sNeutralPFOVars;
    static const std::array<std::string, nClasses> sModeNames;
    static float deltaPhi(const TLorentzVector &p4, const TLorentzVector &p4_tau);
    static float deltaEta(const TLorentzVector &p4, const TLorentzVector &p4_tau);
    static float deltaPhiECal(const TLorentzVector &p4, const std::pair<float, bool> &tau_phiTrkECal);
    static float deltaEtaECal(const TLorentzVector &p4, const std::pair<float, bool> &tau_etaTrkECal);
    /**
     * @brief retrieve the PFO attributes
     * @param pfo a pointer to PFO object
     * @param attr an attribute object
     * @throw std::runtime_error if the attribute cannot be retrieved
     *
     * the template type must match the type of the attribute
     */
    template <typename T>
    static T pfoAttr(const xAOD::PFO *pfo, const xAOD::PFODetails::PFOAttributes &attr);
    static float ptSubRatio(const xAOD::PFO *pfo);
    static float energyFracEM2(const xAOD::PFO *pfo, float energy_em2);
  };

  /**
   * @brief A closely related class that provides helper functions
   */
  class TauDecayModeNNHelper
  {
  public:
    TauDecayModeNNHelper() = delete;
    static float Log10Robust(const float val, const float min_val = 0.);
    /**
     * @brief sort the objects and only keep the leading N objects in the vector
     * @param vec a vector of physics object pointers
     * @param n_obj number of leading pT objects to keep
     *
     * the template type must have pt() method (e.g. PFO*, TauTrack*)
     * objects are sorted by pT in descending order
     * the redundant objects will not be used
     */
    template <typename T>
    static void sortAndKeep(std::vector<T> &vec, const std::size_t n_obj);
    /**
     * @brief initialise the map with a set of defined keys
     * @param empty_map the map to be initialised
     * @param keys a set that contains the keys for the map
     *
     * the template type can be any type
     * the map does not necessary need to be empty
     */
    template <typename T>
    static void initMapKeys(std::map<std::string, T> &empty_map, const std::set<std::string> &keys);
  };
} // namespace tauRecTools

#endif // TAURECTOOLS_TAUDECAYMODENNCLASSIFIER_H
