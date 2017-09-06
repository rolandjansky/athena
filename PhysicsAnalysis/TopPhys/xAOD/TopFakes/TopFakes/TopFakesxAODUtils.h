/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPFAKESXAODUTILS_H
#define TOPFAKESXAODUTILS_H

#include "TopEvent/Event.h"

#include <vector>

namespace top {
  /** @namespace fakes
   *  @brief namespace containing usefull functions for matrix-method fakes estimates
   **/
  namespace fakes {
    /** @brief calculates the jet-environment-related quantities for a given electron
     *         quantities are calculated with and without taking into account the forward jets
     *         default values are -999. - it can occur in 0-jet cases
     *  @param el the selected electron (const)
     *  @param jets the whole jet container (const)
     *  @param dRClosetJet reference to the deltaR(electron, closest jet), including forward jets (i.e. jets can have |eta|>2.5)
     *  @param PtClosetJet reference to the pT of the closest Jet, including forward jets (i.e. jets can have |eta|>2.5)
     *  @param dRClosetCentralJet reference to the deltaR(electron, closest jet), excluding forward jets (i.e. considering only jets with |eta|<2.5)
     *  @param PtClosetCentralJet reference to the pT of the closest Jet, excluding forward jets (i.e. considering only jets with |eta|<2.5)
     **/
    void GetClosestJet (const xAOD::Electron& el, 
			const xAOD::JetContainer& jets,
			float& dRClosetJet, 
			float& PtClosetJet, 
			float& dROverPtClosetJet,
			float& dRClosetCentralJet, 
			float& PtClosetCentralJet,
			float& dROverPtClosetCentralJet);
    
    /** @brief calculates the jet-enrivronment-related quantities for a given muon
     *         quantities are calculated with and without taking into account the forward jets
     *         default values are -999. - it can occur in 0-jet cases
     *  @param mu the selected muon (const)
     *  @param jets the whole jet container (const)
     *  @param dRClosetJet reference to the deltaR(muon, closest jet), including forward jets (i.e. jets can have |eta|>2.5)
     *  @param PtClosetJet reference to the pT of the closest Jet, including forward jets (i.e. jets can have |eta|>2.5)
     *  @param dRClosetCentralJet reference to the deltaR(muon, closest jet), excluding forward jets (i.e. considering only jets with |eta|<2.5)
     *  @param PtClosetCentralJet reference to the pT of the closest Jet, excluding forward jets (i.e. considering only jets with |eta|<2.5)
     **/
    void GetClosestJet (const xAOD::Muon& mu, const xAOD::JetContainer& jets,
			float& dRClosetJet, 
			float& PtClosetJet, 
			float& dROverPtClosetJet,
			float& dRClosetCentralJet, 
			float& PtClosetCentralJet,
			float& dROverPtClosetCentralJet);
    
    /** @brief calculates the deltaPhi between an electron and the MET
     *  @param el reference to the selected electron (const)
     *  @param met reference to the MET (const)
     *  @param dPhi reference to the deltaPhi
     **/
    void GetdPhiLepMET (const xAOD::Electron& el, 
			const xAOD::MissingET& met, float& dPhi);
    
    /** @brief calculates the deltaPhi between a muon and the MET
     *  @param mu reference to the selected muon (const)
     *  @param met reference to the MET (const)
     *  @param dPhi reference to the deltaPhi
     **/
    void GetdPhiLepMET (const xAOD::Muon& mu, 
			const xAOD::MissingET& met, float& dPhi);
    
    /** @brief calculates the trigger-matching variable of an electron
     *  FIXME convention could be updated with new trigger recommendations
     *  @param runNumber allows to distinguish 2015 and 2016 data for triggers - default is 2015
     **/
    void GetTrigMatchVariable(const xAOD::Electron& el, int& trigmatch, const unsigned int runNumber=276262);
    
    /** @brief calculates the trigger-matching variable of a muon
     *  FIXME convention could be updated with new trigger recommendations
     *  @param runNumber allows to distinguish 2015 and 2016 data for triggers - default is 2015
     **/
    void GetTrigMatchVariable(const xAOD::Muon& mu, int& trigmatch, const unsigned int runNumber=276262);
    
    /** @brief get the indexes of the good high pT electrons
     *  FIXME may not be needed
     **/
    std::vector<unsigned int> GetGoodHighPtLeptons(const xAOD::ElectronContainer& els);
    
    /** @brief get the indexes of the good high pT muons
     *  FIXME may not be needed
     **/
    std::vector<unsigned int> GetGoodHighPtLeptons(const xAOD::MuonContainer& mus);
    
    
    /** @brief get the channel of the event, based on the good high pT electrons and muons
     *  FIXME may not be needed
     **/
    int GetChannel(const xAOD::ElectronContainer& els, const xAOD::MuonContainer& mus);
    
    /** @brief enum class listing the possible lepton definitions supported by the TopFakes package
     *  FIXME need to add more definitions (e.g. SSDilepton, TTbarResonance)
     **/
    enum class LeptonDefinitions { Standard, Other };
  }
}

#endif
