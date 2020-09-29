/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONMONITORINGMT_MUONMATCHINGTOOL_H
#define TRIGMUONMONITORINGMT_MUONMATCHINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <string>
#include <memory>
#include <tuple>


/**
 * @brief Class that provides functionalities for searching for online muons close to a given offline muon
 * and judging they are matched with the specific criteria.
 */
class MuonMatchingTool : public AthAlgTool {

 public:
  MuonMatchingTool(const std::string& type, const std::string &name, const IInterface* parent);

  virtual StatusCode initialize() override;


  /**
   * @brief Function that searches for a Level 1 muon candidate and judges if it is matched to a given offline muon.
   * @param mu Offline muon around which Level 1 candidates are searched.
   * @param ctx Reference to the @c EventContext needed for accessing the @c LVL1MuonRoIs container.
   * @param trigger Considered level 1 threshold name, e.g. L1_MU10, etc.
   * @param pass True if a candidate is found.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   */
  const xAOD::MuonRoI* matchL1(const xAOD::Muon *mu, const EventContext& ctx, std::string trigger, bool &pass) const;

  /**
   * @brief Function that searches for an L2 standalone muon (L2MuonSA) candidate and judges if it is matched to a given offline muon.
   * @param mu Offline muon around which L2MuonSA candidates are searched.
   * @param trigger Considered chain name, e.g. HLT_mu26_ivarmedium_L1MU20, etc.
   * @param pass True if the matched candidate passed the hypothesis step.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   * Important: a valid pointer doesn't mean that it passed the hypothesis, users should check @c pass for the decision.
   */
  const xAOD::L2StandAloneMuon* matchL2SA(const xAOD::Muon *mu, std::string trigger, bool &pass) const;

  /**
   * @brief Function that searches for the L2 standalone muon (L2MuonSA) candidate closest to a given offline muon.
   * @param mu Offline muon around which L2MuonSA candidates are searched.
   * @param trigger Considered chain name, e.g. HLT_mu26_ivarmedium_L1MU20, etc.
   * @return LinkInfo to the found candidate. This is inValid link when there is no candidate found.
   * Important: a valid pointer doesn't mean that it passed the hypothesis and matched,
   * users should check @c pass for the decision and @c pass for knowing if it is really matched.
   * @todo Consider improving the argument list.
   */
  const TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer> searchL2SALinkInfo(const xAOD::Muon *mu, std::string trigger) const;

  /**
   * @brief Function that searches for an L2 standalone muon (L2MuonSA) candidate by ReadHandle and judges if it is matched to a given offlineSA muon.
   * @param mu Offline muon around which L2MuonSA candidates are searched.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   */
  const xAOD::L2StandAloneMuon* matchL2SAReadHandle( const EventContext& ctx, const xAOD::Muon *mu) const;

  /**
   * @brief Function that searches for an L2 combined muon (L2muComb) candidate and judges if it is matched to a given offline muon.
   * @param mu Offline muon around which L2muComb candidates are searched.
   * @param trigger Considered chain name, e.g. HLT_mu26_ivarmedium_L1MU20, etc.
   * @param pass True if the matched candidate passed the hypothesis step.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   * Important: a valid pointer doesn't mean that it passed the hypothesis, users should check @c pass for the decision.
   */
  const xAOD::L2CombinedMuon* matchL2CB(const xAOD::Muon *mu, std::string trigger, bool &pass) const;

  /**
   * @brief Function that searches for the L2 combined muon (L2muComb) candidate closest to a given offline muon.
   * @param mu Offline muon around which L2MuonSA candidates are searched.
   * @param trigger Considered chain name, e.g. HLT_mu26_ivarmedium_L1MU20, etc.
   * @return LinkInfo to the found candidate. This is inValid link when there is no candidate found. 
   * Important: a valid pointer doesn't mean that it passed the hypothesis and matched,
   * users should check @c pass for the decision and @c pass for knowing if it is really matched.
   * @todo Consider improving the argument list.
   */
  const TrigCompositeUtils::LinkInfo<xAOD::L2CombinedMuonContainer> searchL2CBLinkInfo(const xAOD::Muon *mu, std::string trigger) const;

  /**
   * @brief Function that searches for an L2 combined muon (L2muComb) candidate by ReadHandle and judges if it is matched to a given offlineCB muon.
   * @param mu Offline muon around which L2muComb candidates are searched.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   */
  const xAOD::L2CombinedMuon* matchL2CBReadHandle( const EventContext& ctx, const xAOD::Muon *mu) const;

  /**
   * @brief Function that searches for an EF standalone muon (EFSA) candidate and judges if it is matched to a given offline muon.
   * @param mu Offline muon around which EFSA candidates are searched.
   * @param trigger Considered chain name, e.g. HLT_mu26_ivarmedium_L1MU20, etc.
   * @param pass True if the matched candidate passed the hypothesis step.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   * Important: a valid pointer doesn't mean that it passed the hypothesis, users should check @c pass for the decision.
   */
  const xAOD::Muon* matchEFSA(const xAOD::Muon *mu, std::string trigger, bool &pass) const;

  /**
   * @brief Function that searches for an EF standalone muon (EFSA) candidate and judges if it is matched to a given track particle.
   * @param mu Offline muon around which EFSA candidates are searched.
   * @param trigger Considered chain name, e.g. HLT_mu26_ivarmedium_L1MU20, etc.
   * @return Pointer to the matched candidate. This is inValid link when there is no candidate found.
   * Important: a valid pointer doesn't mean that it passed the hypothesis, users should check @c pass for the decision.
   */
  const TrigCompositeUtils::LinkInfo<xAOD::MuonContainer> matchEFSALinkInfo( const xAOD::Muon *mu, std::string trig) const;

  /**
   * @brief Function that searches for an EF standalone muon (EFSA) candidate by ReadHandle and judges if it is matched to a given offlineSA muon.
   * @param mu Offline muon around which EFSA candidates are searched.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   */
  const xAOD::Muon* matchEFSAReadHandle( const EventContext& ctx, const xAOD::Muon *mu) const;

  /**
   * @brief Function that searches for an EF combined muon (EFCB) candidate and judges if it is matched to a given offline muon.
   * @param mu Offline muon around which EFCB candidates are searched.
   * @param trigger Considered chain name, e.g. HLT_mu26_ivarmedium_L1MU20, etc.
   * @param pass True if the matched candidate passed the hypothesis step.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   * Important: a valid pointer doesn't mean that it passed the hypothesis, users should check @c pass for the decision.
   */
  const xAOD::Muon* matchEFCB(const xAOD::Muon *mu, std::string trigger, bool &pass) const;

  /**
   * @brief Function that searches for an EF combined muon (EFCB) candidate and judges if it is matched to a given track particle.
   * @param mu Offline muon around which EFCB candidates are searched.
   * @param trigger Considered chain name, e.g. HLT_mu26_ivarmedium_L1MU20, etc.
   * @return Pointer to the matched candidate. This is inValid link when there is no candidate found.
   * Important: a valid pointer doesn't mean that it passed the hypothesis, users should check @c pass for the decision.
   */
  const TrigCompositeUtils::LinkInfo<xAOD::MuonContainer> matchEFCBLinkInfo( const xAOD::Muon *mu, std::string trig) const;

  /**
   * @brief Function that searches for an EF combined muon (EFCB) candidate by ReadHandle and judges if it is matched to a given offlineCB muon.
   * @param mu Offline muon around which EFCB candidates are searched.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   */
  const xAOD::Muon* matchEFCBReadHandle( const EventContext& ctx, const xAOD::Muon *mu) const;

  /**
   * @brief Function that searches for an EF isolation muon (EFIso) candidate and judges if it is matched to a given offline muon.
   * @param mu Offline muon around which EFIso candidates are searched.
   * @param trigger Considered chain name, e.g. HLT_mu26_ivarmedium_L1MU20, etc.
   * @param pass True if the matched candidate passed the hypothesis step.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   * Important: a valid pointer doesn't mean that it passed the hypothesis, users should check @c pass for the decision.
   */
  const xAOD::Muon* matchEFIso(const xAOD::Muon *mu, std::string trigger, bool &pass) const;

  /**
   * @brief Function that searches for an EF isolation muon (EFIso) candidate and judges if it is matched to a given track particle.
   * @param mu Offline muon around which EFIso candidates are searched.
   * @param trigger Considered chain name, e.g. HLT_mu26_ivarmedium_L1MU20, etc.
   * @return Pointer to the matched candidate. This is inValid link when there is no candidate found.
   * Important: a valid pointer doesn't mean that it passed the hypothesis, users should check @c pass for the decision.
   */
  const TrigCompositeUtils::LinkInfo<xAOD::MuonContainer> matchEFIsoLinkInfo( const xAOD::Muon *mu, std::string trig) const;


  /**
   * @brief Function that searches for an offline muon matched to L2SA muon
   * @param ctx Reference to the @c EventContext needed for accessing the @c Muons container.
   * @param samu the given online muon
   * @return Pointer to the matched offline muon. This is @c nullptr when there is no muon found.
   */
  const xAOD::Muon* matchL2SAtoOff( const EventContext& ctx, const xAOD::L2StandAloneMuon* samu) const;

  /**
   * @brief Function that searches for an offline muon matched to L2CB muon
   * @param ctx Reference to the @c EventContext needed for accessing the @c Muons container.
   * @param cbmu the given online muon
   * @return Pointer to the matched offline muon. This is @c nullptr when there is no muon found.
   */
  const xAOD::Muon* matchL2CBtoOff( const EventContext& ctx, const xAOD::L2CombinedMuon* cbmu) const;


  bool isMatchedL2SA(const xAOD::L2StandAloneMuon*, const xAOD::Muon*) const;
  bool isMatchedL2CB(const xAOD::L2CombinedMuon*, const xAOD::Muon*) const;

  /**
   * @brief Function to extrapolate a Inner Detector track to the pivot plane i.e. the middle layers of the Muon Spectrometer where the level 1 RoI is defined.
   * This functionality is not available yet.
   * @param track Inner Detector track that is extrapolated.
   * @return Pointer to the extrapolated track parameters.
   * @see @c reqdRL1byPt
   * @todo Return std::unique_ptr when this is available.
   */
  const Trk::TrackParameters* extTrackToPivot(const xAOD::TrackParticle *track) const;

  /**
   * @brief Function compute dR used for matching offline muons and level 1 RoIs at the pivot plane.
   * This is needed that the accuracy of the extrapolation by @c extTrackToPivot is dependent on the pt of the offline muon.
   * @param mupt Offline muon pt
   * @return Required dR between the offline muon and Level 1 muons
   */
  static double reqdRL1byPt(double mupt);


  static std::tuple<bool,double,double> PosForMatchSATrack(const xAOD::Muon *mu);
  static std::tuple<bool,double,double> PosForMatchCBTrack(const xAOD::Muon *mu);

  
 private:

  enum L1Items{ L1_MU4=1, L1_MU6, L1_MU10, L1_MU11, L1_MU15, L1_MU20, L1_MU21,ERROR};
  int L1ItemStringToInt(std::string l1item) const {
    if( "L1_MU4"==l1item) return L1Items::L1_MU4;
    if( "L1_MU6"==l1item) return L1Items::L1_MU6;
    if( "L1_MU10"==l1item) return L1Items::L1_MU10;
    if( "L1_MU11"==l1item) return L1Items::L1_MU11;
    if( "L1_MU15"==l1item) return L1Items::L1_MU15;
    if( "L1_MU20"==l1item) return L1Items::L1_MU20;
    if( "L1_MU21"==l1item) return L1Items::L1_MU21;
    return L1Items::ERROR;
  }

  const float m_L2SAreqdR = 0.25;
  const float m_L2CBreqdR = 0.03;
  const float m_EFreqdR = 0.03;

  // private methods
  /**
   * @brief Function that searches for an online muon candidate of type T by ReadHandle and judges if it is matched to a given offline muon.
   * @param offl Position of the offline muon used for computing dR.
   * @param reqdR Requirement of dR used for the matching. Note that reqdR is updated with the dR of the found candidate.
   * @param ReadHandleKey SG::ReadHandleKey of online muon.
   * @param ctx EventContext. 
   * @param trigPosForMatchFunc Function pointer that implements cuts for the online muon candidates.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   * @see MuonMatchingTool.icc for the implementation and MuonMatchingTool.cxx for the instantiation.
   * @todo Consider improving the argument list.
   */
  template<class T, class OFFL> const T* matchReadHandle(const OFFL* offl, float reqdR,
                                                         SG::ReadHandleKey<DataVector<T> > ReadHandleKey, const EventContext& ctx,
                                                         std::tuple<bool,double,double> (*trigPosForMatchFunc)(const T*) = &MuonMatchingTool::trigPosForMatch<T>) const;


  /**
   * @brief Function that searches for an online muon candidate of type T closest to a given offline muon. 
   * @param offl Position of the offline muon used for computing dR.
   * @param trigger Considered chain name, e.g. HLT_mu26_ivarmedium_L1MU20, etc.
   * @param trigPosForMatchFunc Function pointer that implements cuts for the online muon candidates.
   * @return LinkInfo to the matched candidate. This is inValid link when there is no candidate found.
   * Important: a valid pointer doesn't mean that it passed the hypothesis, users should check @c pass for the decision.
   * @see MuonMatchingTool.icc for the implementation and MuonMatchingTool.cxx for the instantiation.
   * @todo Consider improving the argument list.
   */
  template<class T, class OFFL> const TrigCompositeUtils::LinkInfo<DataVector<T> > matchLinkInfo(const OFFL *offl, std::string trigger, float reqdR, bool &pass,
                                   const std::string containerSGKey = "",
				   std::tuple<bool,double,double> (*trigPosForMatchFunc)(const T*) = &MuonMatchingTool::trigPosForMatch<T>) const;

  /**
   * @brief Function that searches for an online muon candidate of type T and judges if it is matched to a given offline muon.
   * @param offl Position of the offline muon used for computing dR.
   * @param trigger Considered chain name, e.g. HLT_mu26_ivarmedium_L1MU20, etc.
   * @param reqdR Requirement of dR used for the matching. Note that reqdR is updated with the dR of the found candidate.
   * @param pass True if the matched candidate passed the hypothesis step.
   * @param trigPosForMatchFunc Function pointer that implements cuts for the online muon candidates.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   * Important: a valid pointer doesn't mean that it passed the hypothesis, users should check @c pass for the decision.
   * @see MuonMatchingTool.icc for the implementation and MuonMatchingTool.cxx for the instantiation.
   * @todo Consider improving the argument list.
   */
  template<class T, class OFFL> const T* match(const OFFL *offl, std::string trigger, float reqdR, bool &pass,
                                   const std::string containerSGKey = "",
				   std::tuple<bool,double,double> (*trigPosForMatchFunc)(const T*) = &MuonMatchingTool::trigPosForMatch<T>) const;

  /**
   * @brief Function that searches for an offline muon candidate matched to online muon of type T.
   * @param trig Position of the online muon used for computing dR.
   * @param reqdR Requirement of dR used for the matching. Note that reqdR is updated with the dR of the found candidate.
   * @param offlinePosForMatchFunc Function pointer that implements cuts for the offline muon candidates.
   * @param trigPosForMatchFunc Function pointer that implements cuts for the online muon candidates.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   * Important: a valid pointer doesn't mean that it passed the hypothesis, users should check @c pass for the decision.
   * @see MuonMatchingTool.icc for the implementation and MuonMatchingTool.cxx for the instantiation.
   * @todo Consider improving the argument list.
   */
  template<class T> const xAOD::Muon* matchOff(const EventContext& ctx, const T* trig, float reqdR,
                                               std::tuple<bool,double,double> (*offlinePosForMatchFunc)(const xAOD::Muon*),
                                               std::tuple<bool,double,double> (*trigPosForMatchFunc)(const T*) = &MuonMatchingTool::trigPosForMatch<T>) const;


  const Amg::Vector3D offlineMuonAtPivot(const xAOD::Muon *mu) const;
  double FermiFunction(double x, double x0, double w) const;
  const Trk::TrackParameters* extTrackToTGC(const xAOD::TrackParticle *track) const;
  const Trk::TrackParameters* extTrackToRPC(const xAOD::TrackParticle *track) const;

  // static methods
  // Template methods that perform different matching schemes for T=xAOD::L2StandAloneMuon, xAOD::L2CombinedMuon and xAOD::Muon (EF).
  template<class T> static inline std::tuple<bool,double,double> trigPosForMatch(const T *trig);

  SG::ReadHandleKey<xAOD::MuonRoIContainer> m_MuonRoIContainerKey {this, "MuonRoIContainerName", "LVL1MuonRoIs", "Level 1 muon container"};
  SG::ReadHandleKey<xAOD::MuonContainer> m_MuonContainerKey {this, "MuonContainerName", "Muons", "Offline muon container"};
  SG::ReadHandleKey<xAOD::L2StandAloneMuonContainer> m_L2MuonSAContainerKey {this, "L2StandAloneMuonContainerName", "HLT_MuonL2SAInfo", "L2MuonSA container"};
  SG::ReadHandleKey<xAOD::L2CombinedMuonContainer> m_L2muCombContainerKey {this, "L2CombinedMuonContainerName", "HLT_MuonL2CBInfo", "L2muComb container"};
  SG::ReadHandleKey<xAOD::MuonContainer> m_EFSAMuonContainerKey {this, "EFSAMuonContainerName", "HLT_Muons_RoI", "EFSAMuon container"};
  SG::ReadHandleKey<xAOD::MuonContainer> m_EFCBMuonContainerKey {this, "EFCBMuonContainerName", "HLT_MuonsCB_RoI", "EFCBMuon container"};

  // properties
  Gaudi::Property<bool> m_use_extrapolator {this, "UseExtrapolator", false, "Flag to enable the extrapolator for matching offline and trigger muons"};

  // tools
  PublicToolHandle<Trig::TrigDecisionTool> m_trigDec {this, "TrigDecisionTool", "Trig::TrigDecisionTool/TrigDecisionTool", "TrigDecisionTool"};
  PublicToolHandle<Trk::IExtrapolator> m_extrapolator {"Trk::Extrapolator/AtlasExtrapolator"};
  //The extrapolator is currently not available. Once it gets available, initialize it with the following, which attempts to retrieve:
  //{this, "Extrapolator", "Trk::Extrapolator/AtlasExtrapolator", "Track extrapolator"}; 
};

#include "MuonMatchingTool.icc"

#endif //TRIGMUONMONITORINGMT_MUONMATCHINGTOOL_H
