/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ORMETMakerAlg.h"
#include "METInterface/IMETMaker.h"
#include "METUtilities/METHelpers.h"

#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "xAODMissingET/MissingETAssociationHelper.h"

#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"
#include "PATCore/AcceptData.h"
#include "TauAnalysisTools/ITauSelectionTool.h"
#include <xAODCore/ShallowCopy.h>

#include "xAODPFlow/PFOAuxContainer.h"  //jetOR


using std::string;
using namespace xAOD;

namespace met {

  typedef ElementLink<xAOD::IParticleContainer> iplink_t;
  static const SG::AuxElement::ConstAccessor< std::vector<iplink_t > > acc_constitObjLinks("ConstitObjectLinks");

  //**********************************************************************

  ORMETMakerAlg::ORMETMakerAlg(const std::string& name,
			   ISvcLocator* pSvcLocator )
    : METMakerAlg(name, pSvcLocator) 
 {
    declareProperty("ORMETMapName",    m_ORMetMapKey = "ORMETAssoc" );
    declareProperty( "DoRetrieveORconstit", m_doRetrieveORconstit = false              );
    declareProperty( "RetainMuonConstit", m_retainMuonConstit = false              );
    declareProperty( "DoORMet", m_doORMet = false              );
    //pT thresholds
    declareProperty("ElectronPT", m_electronPT = 10000, "pT for electrons");
    declareProperty("MuonPT", m_muonPT = 10000, "pT for muons");
    declareProperty("PhotonPT", m_photonPT = 10000, "pT for photons");
    declareProperty("TauPT", m_tauPT = 10000, "pT for photons");
    //eta thresholds
    declareProperty("ElectronETA", m_electronETA = 2.47, "eta for electrons");
    declareProperty("MuonETA", m_muonETA = 2.7, "eta for muons");
    declareProperty("PhotonETA", m_photonETA = 2.47, "eta for photons");
    declareProperty("TauETA", m_tauETA = 2.47, "eta for photons");

    declareProperty("UseUnmatched", m_useUnmatched = "True", "Include or reject egamma with unmatched clusters");
    declareProperty("DoJVT", m_doJVT = "False");
    declareProperty("Soft", m_soft = "Clus");


  }

  //**********************************************************************

  ORMETMakerAlg::~ORMETMakerAlg() { }

  //**********************************************************************

  StatusCode ORMETMakerAlg::initialize() {

    ATH_CHECK( METMakerAlg::initialize() );
    ATH_MSG_INFO("Initializing " << name() << "...");
    ATH_MSG_INFO("Retrieving tools...");

    ATH_CHECK( m_ORMetMapKey.initialize() );
    ATH_CHECK(m_chargedPFOContainerWriteHandleKey.initialize()); 
    ATH_CHECK(m_neutralPFOContainerWriteHandleKey.initialize()); 
    ATH_CHECK(m_PFOContainerWriteHandleKey.initialize()); 

    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode ORMETMakerAlg::finalize() {
    ATH_MSG_INFO ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode ORMETMakerAlg::execute() {
    ATH_MSG_VERBOSE("Executing " << name() << "...");
    // Create a MissingETContainer with its aux store
    auto ctx = getContext();
    auto metHandle= SG::makeHandle (m_metKey,ctx);
    ATH_CHECK( metHandle.record (std::make_unique<xAOD::MissingETContainer>(),                      std::make_unique<xAOD::MissingETAuxContainer>()) );
    xAOD::MissingETContainer* newMet=metHandle.ptr();


    SG::ReadHandle<xAOD::MissingETAssociationMap> metMap(m_metMapKey);
    if (!metMap.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve MissingETAssociationMap: " << m_metMapKey.key());
      return StatusCode::FAILURE;
    }

    MissingETAssociationHelper metHelper(&(*metMap));

    SG::ReadHandle<xAOD::MissingETAssociationMap> ORMetMap(m_ORMetMapKey);
    if (m_doORMet && !ORMetMap.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve MissingETAssociationMap: " << m_ORMetMapKey.key());
      return StatusCode::FAILURE;
    }

    
    MissingETAssociationHelper ORMetHelper((m_doORMet ? &(*ORMetMap) : 0));

    // Retrieve containers ***********************************************

    /// MET
    SG::ReadHandle<xAOD::MissingETContainer> coreMet(m_CoreMetKey);
    if (!coreMet.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve MissingETContainer: " << m_CoreMetKey.key());
      return StatusCode::FAILURE;
    }

    /// Jets
    SG::ReadHandle<xAOD::JetContainer> Jets(m_JetContainerKey);
    if (!Jets.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve JetContainer: " << Jets.key());
      return StatusCode::FAILURE;
    }

    /// Electrons
    SG::ReadHandle<xAOD::ElectronContainer> Electrons(m_ElectronContainerKey);
    if (!Electrons.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve ElectronContainer: " << Electrons.key());
      return StatusCode::FAILURE;
    }

    /// Photons
    SG::ReadHandle<xAOD::PhotonContainer> Gamma(m_PhotonContainerKey);
    if (!Gamma.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve GammaContainer: " << Gamma.key());
      return StatusCode::FAILURE;
    }

    /// Taus
    SG::ReadHandle<xAOD::TauJetContainer> TauJets(m_TauJetContainerKey);
    if (!TauJets.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve TauJetContainer: " << TauJets.key());
      return StatusCode::FAILURE;
    }

    /// Muons
    SG::ReadHandle<xAOD::MuonContainer> Muons(m_MuonContainerKey);
    if (!Muons.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve MuonContainer: "  << Muons.key());
      return StatusCode::FAILURE;
    }

    ///PFOs for OR constituents
    SG::ReadHandle<xAOD::PFOContainer> PFOs("CHSParticleFlowObjects");
    if (!PFOs.isValid() && m_doRetrieveORconstit ) {
      ATH_MSG_WARNING("Unable to retrieve PFOContainer: " << PFOs.key());
      return StatusCode::FAILURE;
    }

    // Select and flag objects for final MET building ***************************

    MissingETBase::UsageHandler::Policy objScale = MissingETBase::UsageHandler::PhysicsObject;
    if(m_doTruthLep) objScale = MissingETBase::UsageHandler::TruthParticle;
    // Electrons
    if(!m_ElectronContainerKey.empty()) {
      ConstDataVector<ElectronContainer> metElectrons(SG::VIEW_ELEMENTS);
      for(const auto& el : *Electrons) {
    	if(accept(el)) {
    	  metElectrons.push_back(el);

    	}
      }
      if( m_metmaker->rebuildMET("RefEle", xAOD::Type::Electron, newMet,
    				 metElectrons.asDataVector(),
    				 &metHelper, objScale).isFailure() ) {
    	ATH_MSG_WARNING("Failed to build electron term.");
      }
      ATH_MSG_DEBUG("Selected " << metElectrons.size() << " MET electrons. "
    		    << acc_constitObjLinks(*(*newMet)["RefEle"]).size() << " are non-overlapping.");
    }

    // Photons
    if(!m_PhotonContainerKey.empty()) {
      ConstDataVector<PhotonContainer> metPhotons(SG::VIEW_ELEMENTS);
      for(const auto& ph : *Gamma) {
    	if(accept(ph)) {
    	  metPhotons.push_back(ph);
    	}
      }
      if( m_metmaker->rebuildMET("RefGamma", xAOD::Type::Photon, newMet,
    				 metPhotons.asDataVector(),
    				 &metHelper, objScale).isFailure() ) {
    	ATH_MSG_WARNING("Failed to build photon term.");
      }
      ATH_MSG_DEBUG("Selected " << metPhotons.size() << " MET photons. "
    		    << acc_constitObjLinks(*(*newMet)["RefGamma"]).size() << " are non-overlapping.");
    }

    // Taus
    if(!m_TauJetContainerKey.empty()) {
      ConstDataVector<TauJetContainer> metTaus(SG::VIEW_ELEMENTS);
      for(const auto& tau : *TauJets) {
    	if(accept(tau)) {
    	  metTaus.push_back(tau);
    	}
      }
      if( m_metmaker->rebuildMET("RefTau", xAOD::Type::Tau, newMet,
    				 metTaus.asDataVector(),
    				 &metHelper, objScale).isFailure() ){
    	ATH_MSG_WARNING("Failed to build tau term.");
      }
      ATH_MSG_DEBUG("Selected " << metTaus.size() << " MET taus. "
    		    << acc_constitObjLinks(*(*newMet)["RefTau"]).size() << " are non-overlapping.");
    }

    // Muons
    ConstDataVector<MuonContainer> metMuons(SG::VIEW_ELEMENTS);
    if(!m_MuonContainerKey.empty()) {
      for(const auto& mu : *Muons) {
    	if(accept(mu)) {
    	  metMuons.push_back(mu);
    	}
      }
      
      if(m_doTruthLep) objScale = MissingETBase::UsageHandler::OnlyTrack;

      if (m_doORMet && m_retainMuonConstit) {
        if( m_metmaker->rebuildMET("Muons", xAOD::Type::Muon, newMet,
    				 metMuons.asDataVector(),
    				 &ORMetHelper, objScale).isFailure() ) {
    	  ATH_MSG_WARNING("Failed to build muon term.");
        }
      } else {
        if( m_metmaker->rebuildMET("Muons", xAOD::Type::Muon, newMet,
    				 metMuons.asDataVector(),
    				 &metHelper, objScale).isFailure() ) {
    	  ATH_MSG_WARNING("Failed to build muon term.");
        }
      }
      ATH_MSG_DEBUG("Selected " << metMuons.size() << " MET muons. "
    		    << acc_constitObjLinks(*(*newMet)["Muons"]).size() << " are non-overlapping.");
    }

    if (m_doRetrieveORconstit) {

      /*** Fill overlapRemovedPFlowObjects ***/
      SG::WriteHandle<xAOD::PFOContainer> PFOContainerWriteHandle(m_PFOContainerWriteHandleKey,ctx); 
      ATH_CHECK(PFOContainerWriteHandle.record(std::make_unique<xAOD::PFOContainer>(SG::VIEW_ELEMENTS)));

      ATH_MSG_INFO("Retrieve OR constituents");
      ConstDataVector<PFOContainer> met_PFO(SG::VIEW_ELEMENTS);
      for(const auto& pfo : *PFOs) {met_PFO.push_back(pfo);}

      const xAOD::PFOContainer *OR_pfos; 
   
      if (metMuons.size()>0 && m_retainMuonConstit) {ATH_CHECK(m_metmaker->retrieveOverlapRemovedConstituents(met_PFO.asDataVector(),&metHelper, &OR_pfos, true, metMuons.asDataVector()));}
      else {ATH_CHECK(m_metmaker->retrieveOverlapRemovedConstituents(met_PFO.asDataVector(),&metHelper, &OR_pfos, false));}

      *PFOContainerWriteHandle=*OR_pfos;

     /**** If want to write CHScharged/neutralOverlapRemovedPFlowObjects container to AOD ****
      SG::WriteHandle<xAOD::PFOContainer> PFOContainerWriteHandle(m_PFOContainerWriteHandleKey,ctx); 
      ATH_CHECK(PFOContainerWriteHandle.record(std::make_unique<xAOD::PFOContainer>(SG::VIEW_ELEMENTS)));
      //cPFO
      SG::ReadHandle<xAOD::PFOContainer> cPFOs("CHSChargedParticleFlowObjects");
      if (!cPFOs.isValid()) {
        ATH_MSG_WARNING("Unable to retrieve PFOContainer: " << cPFOs.key());
        return StatusCode::FAILURE;
      }
      //nPFO
      SG::ReadHandle<xAOD::PFOContainer> nPFOs("CHSNeutralParticleFlowObjects");
      if (!nPFOs.isValid()) {
        ATH_MSG_WARNING("Unable to retrieve PFOContainer: " << nPFOs.key());
         return StatusCode::FAILURE;
      }
      SG::WriteHandle<xAOD::PFOContainer> chargedPFOContainerWriteHandle(m_chargedPFOContainerWriteHandleKey,ctx); 
      SG::WriteHandle<xAOD::PFOContainer> neutralPFOContainerWriteHandle(m_neutralPFOContainerWriteHandleKey,ctx); 
      ATH_CHECK(chargedPFOContainerWriteHandle.record(std::make_unique<xAOD::PFOContainer>(),std::make_unique<xAOD::PFOAuxContainer>())); 
      ATH_CHECK(neutralPFOContainerWriteHandle.record(std::make_unique<xAOD::PFOContainer>(),std::make_unique<xAOD::PFOAuxContainer>())); 
      

      ConstDataVector<PFOContainer> met_cPFO(SG::VIEW_ELEMENTS);
      for(const auto& pfo : *cPFOs) {met_cPFO.push_back(pfo);}
      ConstDataVector<PFOContainer> met_nPFO(SG::VIEW_ELEMENTS);
      for(const auto& pfo : *nPFOs) {met_nPFO.push_back(pfo);}

      xAOD::PFOContainer* or_cPFO = chargedPFOContainerWriteHandle.ptr();
      xAOD::PFOContainer* or_nPFO = neutralPFOContainerWriteHandle.ptr();

      if (metMuons.size()>0 && m_retainMuonConstit) {ATH_CHECK(m_metmaker->retrieveOverlapRemovedConstituents(met_cPFO.asDataVector(), met_nPFO.asDataVector(),&metHelper,or_cPFO,or_nPFO,true, metMuons.asDataVector()));}
      else {ATH_CHECK(m_metmaker->retrieveOverlapRemovedConstituents(met_cPFO.asDataVector(), met_nPFO.asDataVector(),&metHelper,or_cPFO,or_nPFO,false));}

    (*PFOContainerWriteHandle).assign((*neutralPFOContainerWriteHandle).begin(), (*neutralPFOContainerWriteHandle).end());
    (*PFOContainerWriteHandle).insert((*PFOContainerWriteHandle).end(),
		  (*chargedPFOContainerWriteHandle).begin(), 
		  (*chargedPFOContainerWriteHandle).end());***/

     for (auto tmp_constit : *PFOContainerWriteHandle){ATH_MSG_VERBOSE("ORMETMaker constit with index " << tmp_constit->index() << ", charge " << tmp_constit->charge()<< " pT " << tmp_constit->pt() << " in OverlapRemovedCHSParticleFlowObjects");}

    }






    
    m_doJVT= (m_soft=="Clus" ? false : true);
    if (m_doORMet) {
      if( m_metmaker->rebuildJetMET("RefJet", (m_soft=="Clus" ? m_softclname : m_softtrkname), newMet,
				  Jets.cptr(), coreMet.cptr(), &ORMetHelper, m_doJVT ).isFailure() ) {
        ATH_MSG_WARNING("Failed to build jet and soft terms.");
      } 
      ATH_MSG_DEBUG("Of " << Jets.cptr()->size()  << " jets, "
		  << acc_constitObjLinks(*(*newMet)["RefJet"]).size() << " are non-overlapping, "
		  << acc_constitObjLinks(*(*newMet)[(m_soft=="Clus" ? m_softclname : m_softtrkname)]).size() << " are soft");
    } else {
      if( m_metmaker->rebuildJetMET("RefJet", (m_soft=="Clus" ? m_softclname : m_softtrkname), newMet,
				  Jets.cptr(), coreMet.cptr(), &metHelper, m_doJVT ).isFailure() ) {
        ATH_MSG_WARNING("Failed to build jet and soft terms.");
      }
      ATH_MSG_DEBUG("Of " << Jets.cptr()->size()  << " jets, "
		  << acc_constitObjLinks(*(*newMet)["RefJet"]).size() << " are non-overlapping, "
		  << acc_constitObjLinks(*(*newMet)[(m_soft=="Clus" ? m_softclname : m_softtrkname)]).size() << " are soft");
    }


    ATH_CHECK( buildMETSum("Final"+m_soft, 
			    newMet,
			    (m_soft=="Clus")? (*newMet)["SoftClus"]->source() : (*newMet)["PVSoftTrk"]->source()) 
  	     ); 

    xAOD::MissingET* metSum = (*newMet)["Final"+m_soft];
    ATH_MSG_DEBUG("MET sum = " << 0.001 * metSum->met() << " GeV");
    ATH_MSG_DEBUG("MET jet = " << 0.001 * (*newMet)["RefJet"]->met() << " GeV");
    ATH_MSG_DEBUG("MET mu = " << 0.001 * (*newMet)["Muons"]->met() << " GeV");
    ATH_MSG_DEBUG("MET el = " << 0.001 * (*newMet)["RefEle"]->met() << " GeV");
    ATH_MSG_DEBUG("MET ph = " << 0.001 * (*newMet)["RefGamma"]->met() << " GeV");
    ATH_MSG_DEBUG("MET tau = " << 0.001 * (*newMet)["RefTau"]->met() << " GeV");
    ATH_MSG_DEBUG("MET soft = " << 0.001 * (*newMet)["Soft"+m_soft]->met() << " GeV");

    return StatusCode::SUCCESS;
  }


  bool ORMETMakerAlg::accept(const xAOD::Muon* mu){
        if( mu->pt()<m_muonPT && fabs(mu->eta())>m_muonETA)  return false;
	return static_cast<bool>(m_muonSelTool->accept(*mu));
  }

  bool ORMETMakerAlg::accept(const xAOD::Electron* el){
	if( fabs(el->eta())>m_electronETA || el->pt()<m_electronPT) return false;
	return static_cast<bool> (m_elecSelLHTool->accept(el));
  }

  bool ORMETMakerAlg::accept(const xAOD::Photon* ph){
	if( !(ph->author()&20) || fabs(ph->eta())>m_photonETA || ph->pt()<m_photonPT) return false;
	return static_cast<bool> (m_photonSelIsEMTool->accept(ph));
  }

  bool ORMETMakerAlg::accept(const xAOD::TauJet* tau){ 
	return static_cast<bool>(m_tauSelTool->accept( *tau ));
  }
}

