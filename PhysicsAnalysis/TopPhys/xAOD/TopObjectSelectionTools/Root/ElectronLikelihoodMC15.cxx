/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopObjectSelectionTools/ElectronLikelihoodMC15.h"
#include "TopEvent/EventTools.h"
#include "ElectronPhotonSelectorTools/AsgElectronChargeIDSelectorTool.h"
#include <iostream>

namespace top {

  ElectronLikelihoodMC15::ElectronLikelihoodMC15(const double ptcut, const bool vetoCrack, const std::string& operatingPoint,
                                                 const std::string& operatingPointLoose, StandardIsolation* isolation, const bool applyTTVACut) :
    m_ptcut(ptcut),
    m_vetoCrack(vetoCrack),
    m_operatingPoint("SetMe"),
    m_operatingPointLoose("SetMe"),
    m_operatingPoint_DF("SetMe"),
    m_operatingPointLoose_DF("SetMe"),
    m_isolation(isolation),
    m_applyTTVACut(applyTTVACut),
    m_applyChargeIDCut(false)
  {
    /** Egamma use different naming styles for the likelihood in:
      *    Trigger = HLT_e24_lhmedium_iloose_L1EM20VH  HLT_e60_lhmedium
      *    Derivation Framework = DFCommonElectronsLHMedium
      *    ElectronPhotonSelectionTools (the LLH tools) = ElectronLikelihoodMediumOfflineConfig2015.conf
      *    Trigger SF = efficiencySF.e24vhi_medium1_e60_medium1.MediumLLH.2015.13TeV.rel20p0.v02.root
      *    Trigger SF = efficiencySF.AnyElectronTrigger.MediumLH.2015.13TeV.rel20p0.v01.root
      *    ID SF = efficiencySF.offline.MediumLH.2015.13TeV.rel20p0.v01.root
      * 
      *  It's a bit of a mess
      */
    
    std::string egammaNamesAreNotConsistantAnywhere("LikelihoodNotSetProperly");
    std::string egammaNamesAreNotConsistantAnywhereLoose("LikelihoodNotSetProperly");

    if (operatingPoint == "LooseLH") {egammaNamesAreNotConsistantAnywhere = "DFCommonElectronsLHLoose";}
    if (operatingPoint == "LooseAndBLayerLH") {egammaNamesAreNotConsistantAnywhere = "DFCommonElectronsLHLoose";}
    if (operatingPoint == "MediumLH") {egammaNamesAreNotConsistantAnywhere = "DFCommonElectronsLHMedium";}
    if (operatingPoint == "TightLH") {egammaNamesAreNotConsistantAnywhere = "DFCommonElectronsLHTight";}
    
    if (operatingPointLoose == "LooseLH") {egammaNamesAreNotConsistantAnywhereLoose = "DFCommonElectronsLHLoose";}
    if (operatingPointLoose == "LooseAndBLayerLH") {egammaNamesAreNotConsistantAnywhereLoose = "DFCommonElectronsLHLoose";}
    if (operatingPointLoose == "MediumLH") {egammaNamesAreNotConsistantAnywhereLoose = "DFCommonElectronsLHMedium";}
    if (operatingPointLoose == "TightLH") {egammaNamesAreNotConsistantAnywhereLoose = "DFCommonElectronsLHTight";}      
    
    m_operatingPoint_DF      = egammaNamesAreNotConsistantAnywhere;
    m_operatingPointLoose_DF = egammaNamesAreNotConsistantAnywhereLoose;
    m_operatingPoint         = operatingPoint;
    m_operatingPointLoose    = operatingPointLoose;

    // currently implemented only for this working point
    if (m_operatingPoint == "MediumLH" && m_isolation->tightLeptonIsolation() == "FixedCutTight")
      m_applyChargeIDCut = true;
  }

  ElectronLikelihoodMC15::ElectronLikelihoodMC15(const bool,
                                                 const double ptcut, const bool vetoCrack, const std::string& operatingPoint,
                                                 const std::string& operatingPointLoose, StandardIsolation* isolation, const bool applyTTVACut) :
  ElectronLikelihoodMC15::ElectronLikelihoodMC15(ptcut, vetoCrack, operatingPoint,
                                                 operatingPointLoose, isolation, applyTTVACut) {}

  bool ElectronLikelihoodMC15::passSelection(const xAOD::Electron& el) const {
    if (!passSelectionNoIsolation(el, m_operatingPoint_DF, m_operatingPoint))
      return false;

    if (m_isolation && !m_isolation->passSelection(el))
      return false;

    return true;
  }

  bool ElectronLikelihoodMC15::passSelectionLoose(const xAOD::Electron& el) const {
    if (!passSelectionNoIsolation(el, m_operatingPointLoose_DF, m_operatingPointLoose))
      return false;

    if (m_isolation && !m_isolation->passSelectionLoose(el))
      return false;

    return true;
  }

  bool ElectronLikelihoodMC15::passSelectionNoIsolation(const xAOD::Electron& el, const std::string& operatingPoint_DF, const std::string& operatingPoint) const {

    if (el.pt() < m_ptcut)
        return false;

    // removing bad electron cluser - see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EGammaIdentificationRun2#Bad_Electron_Photon_Cluster
    if( !el.isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON) ) return false;

    try {
      if (el.auxdataConst<int>(operatingPoint_DF) != 1)
        return false;
    } catch(std::exception& e) {
      if (el.auxdataConst<char>(operatingPoint_DF) != 1)
        return false;
    }


    if(operatingPoint == "LooseAndBLayerLH"){

      if(!passBLayerCuts(el))
      	return false;

    }

    //WARNING: Not all electrons keep clusters in the derivation
    //i.e. bad electrons (which is why we moved the check on the likelihood
    //before the check on the calo cluster)
    //This stops a crash
    //Good electrons should always have a cluster, if not then crash to warn us
    //Better than checking and silently doing nothing...
    if (std::fabs(el.caloCluster()->etaBE(2)) > 2.47)
        return false;

    if (m_vetoCrack && std::fabs(el.caloCluster()->etaBE(2)) > 1.37 && std::fabs(el.caloCluster()->etaBE(2)) < 1.52)
        return false;

    // Track-to-vertex association
    if (m_applyTTVACut){
      if (!passTTVACuts(el))
	return false;
    }

    // Electron Charge ID Selector Tool
    // apply decoration only
    if (m_applyChargeIDCut && !passChargeIDCut(el))
      el.auxdecor<char>("passChargeID") = 0;
    else
      el.auxdecor<char>("passChargeID") = 1;
    
    return true;
  }

bool ElectronLikelihoodMC15::passBLayerCuts(const xAOD::Electron& el) const
{
  
  // this is taken from ElectronPhotonSelectorTools/Root/AsgElectronLikelihoodTool.cxx                                                                                                                       
  const xAOD::TrackParticle* t = el.trackParticle();

  if (!t) return false;
  
  uint8_t expectBlayer(true);
  uint8_t nBlayerHits(0);
  uint8_t nBlayerOutliers(0);
  
  t -> summaryValue(expectBlayer,    xAOD::expectBLayerHit);
  t -> summaryValue(nBlayerHits,     xAOD::numberOfBLayerHits);
  t -> summaryValue(nBlayerOutliers, xAOD::numberOfBLayerOutliers);
  
  if(expectBlayer && (nBlayerHits+nBlayerOutliers) < 1) return false;
  else return true;
  
}

bool ElectronLikelihoodMC15::passTTVACuts(const xAOD::Electron& el) const
{

  // TTVA:
  // see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TrackingCPEOYE2015#Track_to_Vertex_Association    
  if( !el.isAvailable<float>("d0sig") ){
    std::cout << "d0 significance not found for electron. "
	      << "Maybe no primary vertex? Won't accept." << std::endl;
    return false;
  }
  
  float d0sig = el.auxdataConst<float>("d0sig");
  if( std::abs(d0sig) >= 5 )
    return false;
  
  if( !el.isAvailable<float>("delta_z0_sintheta") ){
    std::cout << "delta z0*sin(theta) not found for electron. "
      	      << "Maybe no primary vertex? Won't accept." << std::endl;
    return false;
  }
  
  float delta_z0_sintheta = el.auxdataConst<float>("delta_z0_sintheta");
  if( std::abs(delta_z0_sintheta) >= 0.5 )
    return false;
  
  return true;
  
}

bool ElectronLikelihoodMC15::passChargeIDCut(const xAOD::Electron& el) const
{

  // Electron Charge ID Selector Tool
  // see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/ElectronChargeFlipTaggerTool
  if ( asg::ToolStore::contains<AsgElectronChargeIDSelectorTool> ("ECIDS_medium") ) {
    AsgElectronChargeIDSelectorTool* electronChargeIDSelectorTool = asg::ToolStore::get<AsgElectronChargeIDSelectorTool> ("ECIDS_medium");
    if (!electronChargeIDSelectorTool->accept(&el)) return false;
  }
  return true;
}

  void ElectronLikelihoodMC15::print(std::ostream& os) const {
      os << "ElectronLikelihoodMC15\n";
      os << "    * pT > " << m_ptcut << "\n";
      os << "    * Currently disabled --- |cluster_eta| < 2.47 \n";
      os << "    * Veto 1.37 < |cluster_eta| < 1.52? " << std::boolalpha << m_vetoCrack << "\n";
      if (!m_isolation) {
        os << "    * No isolation requirement\n";
      } else {
        m_isolation->print(os);
      }
      os << "    * LH Tool Operating Point: " << m_operatingPoint_DF << " \t\t LH Tool Loose Operating Point: " <<m_operatingPointLoose_DF << " \n";
  }
}
