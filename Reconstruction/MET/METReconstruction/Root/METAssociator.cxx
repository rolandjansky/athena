///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METAssociator.cxx
// Implementation for class METAssociator
//
// This is the base class for tools that construct MET terms
// from other object collections.
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
///////////////////////////////////////////////////////////////////

// METReconstruction includes
#include "METReconstruction/METAssociator.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "AthContainers/ConstDataVector.h"

// Tracking EDM
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"

// Track errors
#include "EventPrimitives/EventPrimitivesHelpers.h"

// Tool interface headers
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "RecoToolInterfaces/ICaloTopoClusterIsolationTool.h"

// For DeltaR
#include "FourMomUtils/xAODP4Helpers.h"

namespace met {

  using namespace xAOD;

  // UE correction for each lepton
  const static SG::AuxElement::Decorator<float> dec_UEcorr("UEcorr_Pt");

  // all PFOs in maps
  const static SG::AuxElement::Decorator<int>   dec_NchsPFOsInMaps("NumOfChsPFOsInMaps");
  const static SG::AuxElement::Decorator<float> dec_SumOfChsPFOsInMap_Pt("SumOfChsPFOsInMap_Pt");
  const static SG::AuxElement::Decorator<float> dec_SumOfChsPFOsInMap_Eta("SumOfChsPFOsInMap_Eta");
  const static SG::AuxElement::Decorator<float> dec_SumOfChsPFOsInMap_Phi("SumOfChsPFOsInMap_Phi");
  const static SG::AuxElement::Decorator<float> dec_SumOfChsPFOsInMap_E("SumOfChsPFOsInMap_E");

  // neutral PFOs only in maps
  const static SG::AuxElement::Decorator<int>   dec_NchsPFOsInMaps_neutral("NumOfChsPFOsInMaps_neutralPFOsOnly");
  const static SG::AuxElement::Decorator<float> dec_SumOfChsPFOsInMap_Pt_neutral("SumOfChsPFOsInMap_Pt_neutralPFOsOnly");
  const static SG::AuxElement::Decorator<float> dec_SumOfChsPFOsInMap_Eta_neutral("SumOfChsPFOsInMap_Eta_neutralPFOsOnly");
  const static SG::AuxElement::Decorator<float> dec_SumOfChsPFOsInMap_Phi_neutral("SumOfChsPFOsInMap_Phi_neutralPFOsOnly");
  const static SG::AuxElement::Decorator<float> dec_SumOfChsPFOsInMap_E_neutral("SumOfChsPFOsInMap_E_neutralPFOsOnly");

  // charged PFOs only in maps
  const static SG::AuxElement::Decorator<int>   dec_NchsPFOsInMaps_charged("NumOfChsPFOsInMaps_chargedPFOsOnly");
  const static SG::AuxElement::Decorator<float> dec_SumOfChsPFOsInMap_Pt_charged("SumOfChsPFOsInMap_Pt_chargedPFOsOnly");
  const static SG::AuxElement::Decorator<float> dec_SumOfChsPFOsInMap_Eta_charged("SumOfChsPFOsInMap_Eta_chargedPFOsOnly");
  const static SG::AuxElement::Decorator<float> dec_SumOfChsPFOsInMap_Phi_charged("SumOfChsPFOsInMap_Phi_chargedPFOsOnly");
  const static SG::AuxElement::Decorator<float> dec_SumOfChsPFOsInMap_E_charged("SumOfChsPFOsInMap_E_chargedPFOsOnly");

  // all PFOs in event
  const static SG::AuxElement::Decorator<int>   dec_NchsPFOsAllInEvent("NumOfChsPFOsAllInEvent");
  const static SG::AuxElement::Decorator<float> dec_SumChsPFOsAllInEvent_Pt("SumChsPFOsAllInEvent_Pt");
  const static SG::AuxElement::Decorator<float> dec_SumChsPFOsAllInEvent_Eta("SumChsPFOsAllInEvent_Eta");
  const static SG::AuxElement::Decorator<float> dec_SumChsPFOsAllInEvent_Phi("SumChsPFOsAllInEvent_Phi");
  const static SG::AuxElement::Decorator<float> dec_SumChsPFOsAllInEvent_E("SumChsPFOsAllInEvent_E");

  // neutral PFOs only in event
  const static SG::AuxElement::Decorator<int>   dec_NchsPFOsAllInEvent_neutral("NumOfChsPFOsAllInEvent_neutralPFOsOnly");
  const static SG::AuxElement::Decorator<float> dec_SumChsPFOsAllInEvent_Pt_neutral("SumChsPFOsAllInEvent_Pt_neutralPFOsOnly");
  const static SG::AuxElement::Decorator<float> dec_SumChsPFOsAllInEvent_Eta_neutral("SumChsPFOsAllInEvent_Eta_neutralPFOsOnly");
  const static SG::AuxElement::Decorator<float> dec_SumChsPFOsAllInEvent_Phi_neutral("SumChsPFOsAllInEvent_Phi_neutralPFOsOnly");
  const static SG::AuxElement::Decorator<float> dec_SumChsPFOsAllInEvent_E_neutral("SumChsPFOsAllInEvent_E_neutralPFOsOnly");

  // charged PFOs only in event
  const static SG::AuxElement::Decorator<int>   dec_NchsPFOsAllInEvent_charged("NumOfChsPFOsAllInEvent_chargedPFOsOnly");
  const static SG::AuxElement::Decorator<float> dec_SumChsPFOsAllInEvent_Pt_charged("SumChsPFOsAllInEvent_Pt_chargedPFOsOnly");
  const static SG::AuxElement::Decorator<float> dec_SumChsPFOsAllInEvent_Eta_charged("SumChsPFOsAllInEvent_Eta_chargedPFOsOnly");
  const static SG::AuxElement::Decorator<float> dec_SumChsPFOsAllInEvent_Phi_charged("SumChsPFOsAllInEvent_Phi_chargedPFOsOnly");
  const static SG::AuxElement::Decorator<float> dec_SumChsPFOsAllInEvent_E_charged("SumChsPFOsAllInEvent_E_chargedPFOsOnly");


  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

  // Constructors
  ////////////////
  METAssociator::METAssociator(const std::string& name) :
    AsgTool(name),
    m_trkseltool(""),
    m_trkIsolationTool(""),
    m_caloIsolationTool("")
  {
    ATH_MSG_INFO("METAssoc constructor");
    declareProperty( "InputCollection",    m_input_data_key                      );
    declareProperty( "PrimVxColl",         m_pvcoll      = "PrimaryVertices"     );
    declareProperty( "TrkColl",            m_trkcoll     = "InDetTrackParticles" );
    declareProperty( "ClusColl",           m_clcoll      = "CaloCalTopoClusters" );
    declareProperty( "PFlowColl",          m_pfcoll      = "CHSParticleFlowObjects" );
    declareProperty( "UseModifiedClus",    m_useModifiedClus = false            );
    declareProperty( "UseTracks",          m_useTracks   = true                  );
    declareProperty( "PFlow",              m_pflow       = false                 );
    declareProperty( "HRecoil",            m_recoil      = false                 );
    declareProperty( "UseRapidity",        m_useRapidity = false                 );
    declareProperty( "TrackSelectorTool",  m_trkseltool                          );
    declareProperty( "TrackIsolationTool", m_trkIsolationTool                    );
    declareProperty( "CaloIsolationTool",  m_caloIsolationTool                   );
    declareProperty( "IgnoreJetConst",     m_skipconst = false                   );
    declareProperty( "ForwardColl",        m_forcoll   = ""                       );
    declareProperty( "ForwardDef",         m_foreta    = 2.5                     );
    declareProperty( "CentralTrackPtThr",  m_cenTrackPtThr = 30e+3               );
    declareProperty( "ForwardTrackPtThr",  m_forTrackPtThr = 30e+3               );
    declareProperty( "CleanCPFO",          m_cleanChargedPFO = true              );
  }

  // Destructor
  ///////////////
  METAssociator::~METAssociator()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METAssociator::initialize()
  {
    ATH_MSG_DEBUG ("Initializing " << name() << "...");

    ATH_CHECK( m_trkseltool.retrieve() );
    ATH_CHECK(m_trkIsolationTool.retrieve());
    ATH_CHECK(m_caloIsolationTool.retrieve());

    if(m_clcoll == "CaloCalTopoClusters") {
      if(m_useModifiedClus) {
	ATH_MSG_ERROR("Configured to use modified topocluster collection but \"CaloCalTopoClusters\" collection specified!");
	return StatusCode::FAILURE;
      } else {
	ATH_MSG_INFO("Configured to use standard topocluster collection.");
      }
    } else {
      if(m_useModifiedClus) {
	ATH_MSG_INFO("Configured to use modified topocluster collection \"" << m_clcoll << "\".");
      } else {
	ATH_MSG_ERROR("Configured to use topocluster collection \"" << m_clcoll << "\", but modified clusters flag not set!");
	return StatusCode::FAILURE;
      }
    }

    if(m_pfcoll == "JetETMissParticleFlowObjects") {      
      ATH_MSG_ERROR("Configured to use standard pflow collection \"" << m_pfcoll << "\".");
      ATH_MSG_ERROR("This is no longer supported -- please use the CHSParticleFlowObjects collection, which has the four-vector corrections built in.");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("Configured to use PFlow collection \"" << m_pfcoll << "\".");
    }

    return StatusCode::SUCCESS;
  }

  StatusCode METAssociator::execute(xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap) const
  {
    ATH_MSG_DEBUG ("In execute: " << name() << "...");
    if(!metCont) {
      ATH_MSG_WARNING("Invalid pointer to MissingETContainer supplied! Abort.");
      return StatusCode::FAILURE;
    }

    if(!metMap) {
      ATH_MSG_WARNING("Invalid pointer to MissingETAssociationMap supplied! Abort.");
      return StatusCode::FAILURE;
    }
    if(m_pflow && !m_useTracks ){
      ATH_MSG_WARNING("Attempting to build PFlow without a track collection.");
      return StatusCode::FAILURE;
    }

    return this->executeTool(metCont, metMap);
  }

  StatusCode METAssociator::retrieveConstituents(met::METAssociator::ConstitHolder& constits) const
  {
    ATH_MSG_DEBUG ("In execute: " << name() << "...");
    if (!m_skipconst || m_forcoll.empty()) {
      if( evtStore()->retrieve(constits.tcCont, m_clcoll).isFailure() ) {
        ATH_MSG_WARNING("Unable to retrieve topocluster container " << m_clcoll << " for overlap removal");
        return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("Successfully retrieved topocluster collection");
    } else {
      std::string hybridname = "Etmiss";
      hybridname += m_clcoll;
      hybridname += m_foreta;
      hybridname += m_forcoll;
      if( evtStore()->contains<IParticleContainer>(hybridname) ) {
        ATH_CHECK(evtStore()->retrieve(constits.tcCont,hybridname));
      } else {
        ConstDataVector<IParticleContainer> *hybridCont = new ConstDataVector<IParticleContainer>(SG::VIEW_ELEMENTS);

        const IParticleContainer *centCont = 0;
        const IParticleContainer *forCont = 0;
        if( evtStore()->retrieve(centCont, m_clcoll).isFailure() ) {
          ATH_MSG_WARNING("Unable to retrieve central container " << m_clcoll << " for overlap removal");
          return StatusCode::FAILURE;
        }
        if( evtStore()->retrieve(forCont, m_forcoll).isFailure() ) {
          ATH_MSG_WARNING("Unable to retrieve forward container " << m_forcoll << " for overlap removal");
          return StatusCode::FAILURE;
        }
        for(const auto& clus : *centCont) if (fabs(clus->eta())<m_foreta) hybridCont->push_back(clus);
        for(const auto& clus : *forCont) if (fabs(clus->eta())>=m_foreta) hybridCont->push_back(clus);
        ATH_CHECK( evtStore()->record(hybridCont,hybridname));
        constits.tcCont = hybridCont->asDataVector();
      }
    }

    if( !m_useTracks){
      //if you want to skip tracks, set the track collection empty manually
      ATH_MSG_DEBUG("Skipping tracks");
    }else{
      const VertexContainer *vxCont = 0;
      if( evtStore()->retrieve(vxCont, m_pvcoll).isFailure() ) {
	ATH_MSG_WARNING("Unable to retrieve primary vertex container " << m_pvcoll);
	//this is actually really bad.  If it's empty that's okay
	return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("Successfully retrieved primary vertex container");
      ATH_MSG_DEBUG("Container holds " << vxCont->size() << " vertices");

      for(const auto& vx : *vxCont) {
	ATH_MSG_VERBOSE( "Testing vertex " << vx->index() );
	if(vx->vertexType()==VxType::PriVtx)
	  {constits.pv = vx; break;}
      }
      if(!constits.pv) {
	ATH_MSG_DEBUG("Failed to find primary vertex! Reject all tracks.");
      } else {
	ATH_MSG_VERBOSE("Primary vertex has z = " << constits.pv->z());
      }

      constits.trkCont=0;
      ATH_MSG_DEBUG("Retrieving Track collection " << m_trkcoll);
      ATH_CHECK( evtStore()->retrieve(constits.trkCont, m_trkcoll) );

      if(m_pflow) {
	ATH_MSG_DEBUG("Retrieving PFlow collection " << m_pfcoll);
	constits.pfoCont = 0;
	ATH_CHECK( evtStore()->retrieve(constits.pfoCont, m_pfcoll ) );
      }//pflow
    }//retrieve track/pfo containers

    return StatusCode::SUCCESS;
  }

  StatusCode METAssociator::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  ///////////////////////////////////////////////////////////////////
  // Protected methods:
  ///////////////////////////////////////////////////////////////////

  StatusCode METAssociator::fillAssocMap(xAOD::MissingETAssociationMap* metMap,
					 const xAOD::IParticleContainer* hardObjs) const
  {
    ConstitHolder constits;

    if (retrieveConstituents(constits).isFailure()) {
      ATH_MSG_DEBUG("Unable to retrieve constituent containers");
      return StatusCode::FAILURE;
    }

    std::vector<const IParticle*> constlist;
    constlist.reserve(20);
    std::vector<const IParticle*> hardObjs_tmp;
    for(const auto& obj : *hardObjs) {
      hardObjs_tmp.push_back(obj);
    }
    std::sort(hardObjs_tmp.begin(),hardObjs_tmp.end(),greaterPt);

    // artur
    //std::cout<<std::endl<<std::endl;
    //std::cout<<"------------------- begin HR and random Phi calculation --------------------------"<<std::endl;
    TLorentzVector HR;
    float UEcorr_Pt = 0.;
    unsigned int lept_count = 0;
    std::vector<double> vPhiRnd;
    if (m_pflow && m_useTracks && m_recoil) {
      constlist.clear();
      ATH_CHECK( this->hadrecoil_PFO(hardObjs_tmp, constits, HR, vPhiRnd) );
    }

    // all pfos in the maps
    int   N_chsPFOsInMaps = 0;
    float SumOfChsPFOsInMap_Pt  = 0.;
    float SumOfChsPFOsInMap_Eta = 0.;
    float SumOfChsPFOsInMap_Phi = 0.;
    float SumOfChsPFOsInMap_E   = 0.;

    // neutral pfos in maps
    int   N_chsPFOsInMaps_neutralPFOsOnly = 0;
    float SumOfChsPFOsInMap_Pt_neutralPFOsOnly  = 0.;
    float SumOfChsPFOsInMap_Eta_neutralPFOsOnly = 0.;
    float SumOfChsPFOsInMap_Phi_neutralPFOsOnly = 0.;
    float SumOfChsPFOsInMap_E_neutralPFOsOnly   = 0.;

    // charged pfos in maps
    int   N_chsPFOsInMaps_chargedPFOsOnly = 0;
    float SumOfChsPFOsInMap_Pt_chargedPFOsOnly  = 0.;
    float SumOfChsPFOsInMap_Eta_chargedPFOsOnly = 0.;
    float SumOfChsPFOsInMap_Phi_chargedPFOsOnly = 0.;
    float SumOfChsPFOsInMap_E_chargedPFOsOnly   = 0.;

    // all pfos in event
    int   N_chsPFOsAllInEvent      = 0;
    float SumChsPFOsAllInEvent_Pt  = 0.;
    float SumChsPFOsAllInEvent_Eta = 0.;
    float SumChsPFOsAllInEvent_Phi = 0.;
    float SumChsPFOsAllInEvent_E   = 0.;

    // neutral pfos 
    int   N_chsPFOsAllInEvent_neutralPFOsOnly      = 0;
    float SumChsPFOsAllInEvent_Pt_neutralPFOsOnly  = 0.;
    float SumChsPFOsAllInEvent_Eta_neutralPFOsOnly = 0.;
    float SumChsPFOsAllInEvent_Phi_neutralPFOsOnly = 0.;
    float SumChsPFOsAllInEvent_E_neutralPFOsOnly   = 0.;

    // charged pfos
    int   N_chsPFOsAllInEvent_chargedPFOsOnly      = 0;
    float SumChsPFOsAllInEvent_Pt_chargedPFOsOnly  = 0.;
    float SumChsPFOsAllInEvent_Eta_chargedPFOsOnly = 0.;
    float SumChsPFOsAllInEvent_Phi_chargedPFOsOnly = 0.;
    float SumChsPFOsAllInEvent_E_chargedPFOsOnly   = 0.;    
    
    //std::cout<<"------------------- end HR and random Phi calculation --------------------------"<<std::endl;

    //std::cout<<"------------------- begin loop over leptins --------------------------"<<std::endl;

    for(const auto& obj : hardObjs_tmp) {
      if(obj->pt()<5e3 && obj->type()!=xAOD::Type::Muon) continue;
      constlist.clear();
      ATH_MSG_VERBOSE( "Object type, pt, eta, phi = " << obj->type() << ", " << obj->pt() << ", " << obj->eta() << "," << obj->phi() );
      if (m_pflow) {
        if(!m_useTracks){
          ATH_MSG_DEBUG("Attempting to build PFlow without a track collection.");
          return StatusCode::FAILURE;
        }else{
          std::map<const IParticle*,MissingETBase::Types::constvec_t> momentumOverride;
          if(m_recoil){
            //std::cout<<"METAssociator::fillAssocMap: m_recoil = true, GetPFOWana "<<std::endl;
            UEcorr_Pt = 0.;
            
            ATH_CHECK( this->GetPFOWana(obj,constlist,constits,momentumOverride, vPhiRnd, lept_count, UEcorr_Pt) );
            dec_UEcorr(*obj) = UEcorr_Pt;            

            // Save numbeer of chsPFOs and all components of 4-vector of the vetro sum of chsPFOs in the MAPs as decorators for each electron
            N_chsPFOsInMaps       = 0;
            SumOfChsPFOsInMap_Pt  = 0.;
            SumOfChsPFOsInMap_Eta = 0.;
            SumOfChsPFOsInMap_Phi = 0.;
            SumOfChsPFOsInMap_E   = 0;

            N_chsPFOsInMaps_neutralPFOsOnly = 0;
            SumOfChsPFOsInMap_Pt_neutralPFOsOnly  = 0.;
            SumOfChsPFOsInMap_Eta_neutralPFOsOnly = 0.;
            SumOfChsPFOsInMap_Phi_neutralPFOsOnly = 0.;
            SumOfChsPFOsInMap_E_neutralPFOsOnly   = 0.;
        
            N_chsPFOsInMaps_chargedPFOsOnly = 0;
            SumOfChsPFOsInMap_Pt_chargedPFOsOnly  = 0.;
            SumOfChsPFOsInMap_Eta_chargedPFOsOnly = 0.;
            SumOfChsPFOsInMap_Phi_chargedPFOsOnly = 0.;
            SumOfChsPFOsInMap_E_chargedPFOsOnly   = 0.;
            
            TLorentzVector SumOfAllPFOsInMaps;
            TLorentzVector SumOfAllPFOsInMaps_neutralPFOsOnly;
            TLorentzVector SumOfAllPFOsInMaps_chargedPFOsOnly;

            for(const auto& pfo_j : constlist) {
              const xAOD::PFO* pfo_i = static_cast<const xAOD::PFO*>(pfo_j);
              TLorentzVector pfoCurrVect;
              pfoCurrVect.SetPtEtaPhiE( pfo_i->pt(), pfo_i->eta(), pfo_i->phi(), pfo_i->e() );
              SumOfAllPFOsInMaps += pfoCurrVect;
              if( fabs(pfo_i->charge()) > FLT_MIN ){ // charged
                N_chsPFOsInMaps_chargedPFOsOnly++;
                TLorentzVector pfoCurrVect_chargedPFOsOnly;
                pfoCurrVect_chargedPFOsOnly.SetPtEtaPhiE( pfo_i->pt(), pfo_i->eta(), pfo_i->phi(), pfo_i->e() );
                SumOfAllPFOsInMaps_chargedPFOsOnly += pfoCurrVect_chargedPFOsOnly;
              }
              else{ // neutral
                N_chsPFOsInMaps_neutralPFOsOnly++;
                TLorentzVector pfoCurrVect_neutralPFOsOnly;
                pfoCurrVect_neutralPFOsOnly.SetPtEtaPhiE( pfo_i->pt(), pfo_i->eta(), pfo_i->phi(), pfo_i->e() );
                SumOfAllPFOsInMaps_neutralPFOsOnly += pfoCurrVect_neutralPFOsOnly;
              }
            }

            N_chsPFOsInMaps       = constlist.size();
            SumOfChsPFOsInMap_Pt  = SumOfAllPFOsInMaps.Pt();
            SumOfChsPFOsInMap_Eta = SumOfAllPFOsInMaps.Eta();
            SumOfChsPFOsInMap_Phi = SumOfAllPFOsInMaps.Phi();
            SumOfChsPFOsInMap_E   = SumOfAllPFOsInMaps.E();

            SumOfChsPFOsInMap_Pt_chargedPFOsOnly  = SumOfAllPFOsInMaps_chargedPFOsOnly.Pt();
            SumOfChsPFOsInMap_Eta_chargedPFOsOnly = SumOfAllPFOsInMaps_chargedPFOsOnly.Eta();
            SumOfChsPFOsInMap_Phi_chargedPFOsOnly = SumOfAllPFOsInMaps_chargedPFOsOnly.Phi();
            SumOfChsPFOsInMap_E_chargedPFOsOnly   = SumOfAllPFOsInMaps_chargedPFOsOnly.E();

            SumOfChsPFOsInMap_Pt_neutralPFOsOnly  = SumOfAllPFOsInMaps_neutralPFOsOnly.Pt();
            SumOfChsPFOsInMap_Eta_neutralPFOsOnly = SumOfAllPFOsInMaps_neutralPFOsOnly.Eta();
            SumOfChsPFOsInMap_Phi_neutralPFOsOnly = SumOfAllPFOsInMaps_neutralPFOsOnly.Phi();
            SumOfChsPFOsInMap_E_neutralPFOsOnly   = SumOfAllPFOsInMaps_neutralPFOsOnly.E();

            // all PFOs in maps
            dec_NchsPFOsInMaps(*obj)        = N_chsPFOsInMaps;
            dec_SumOfChsPFOsInMap_Pt(*obj)  = SumOfChsPFOsInMap_Pt;
            dec_SumOfChsPFOsInMap_Eta(*obj) = SumOfChsPFOsInMap_Eta;
            dec_SumOfChsPFOsInMap_Phi(*obj) = SumOfChsPFOsInMap_Phi;
            dec_SumOfChsPFOsInMap_E(*obj)   = SumOfChsPFOsInMap_E;

            // charged PFOs only in maps
            dec_NchsPFOsInMaps_charged(*obj) = N_chsPFOsInMaps_chargedPFOsOnly;
            dec_SumOfChsPFOsInMap_Pt_charged(*obj) = SumOfChsPFOsInMap_Pt_chargedPFOsOnly;
            dec_SumOfChsPFOsInMap_Eta_charged(*obj) = SumOfChsPFOsInMap_Eta_chargedPFOsOnly;
            dec_SumOfChsPFOsInMap_Phi_charged(*obj) = SumOfChsPFOsInMap_Phi_chargedPFOsOnly;
            dec_SumOfChsPFOsInMap_E_charged(*obj) = SumOfChsPFOsInMap_E_chargedPFOsOnly;

            // neutral PFOs only in maps
            dec_NchsPFOsInMaps_neutral(*obj) = N_chsPFOsInMaps_neutralPFOsOnly;
            dec_SumOfChsPFOsInMap_Pt_neutral(*obj) = SumOfChsPFOsInMap_Pt_neutralPFOsOnly;
            dec_SumOfChsPFOsInMap_Eta_neutral(*obj) = SumOfChsPFOsInMap_Eta_neutralPFOsOnly;
            dec_SumOfChsPFOsInMap_Phi_neutral(*obj) = SumOfChsPFOsInMap_Phi_neutralPFOsOnly;
            dec_SumOfChsPFOsInMap_E_neutral(*obj) = SumOfChsPFOsInMap_E_neutralPFOsOnly;            
            /*
            std::cout<<"!!!!! N_chsPFOsInMaps       = "<<N_chsPFOsInMaps<<std::endl;
            std::cout<<"!!!!! SumOfChsPFOsInMap_Pt  = "<<SumOfChsPFOsInMap_Pt<<std::endl;
            std::cout<<"!!!!! SumOfChsPFOsInMap_Eta = "<<SumOfChsPFOsInMap_Eta<<std::endl;
            std::cout<<"!!!!! SumOfChsPFOsInMap_Phi = "<<SumOfChsPFOsInMap_Phi<<std::endl;
            std::cout<<"!!!!! SumOfChsPFOsInMap_E   = "<<SumOfChsPFOsInMap_E<<std::endl;
            */
            
            // Save numbeer of chsPFOs and all components of 4-vector of the vetro sum of chsPFOs (all chsPFOs in the cevents -> will be the same for each lepton)
            N_chsPFOsAllInEvent      = 0; 
            SumChsPFOsAllInEvent_Pt  = 0.;
            SumChsPFOsAllInEvent_Eta = 0.;
            SumChsPFOsAllInEvent_Phi = 0.;
            SumChsPFOsAllInEvent_E   = 0.;

            // neutral pfos 
            N_chsPFOsAllInEvent_neutralPFOsOnly      = 0;
            SumChsPFOsAllInEvent_Pt_neutralPFOsOnly  = 0.;
            SumChsPFOsAllInEvent_Eta_neutralPFOsOnly = 0.;
            SumChsPFOsAllInEvent_Phi_neutralPFOsOnly = 0.;
            SumChsPFOsAllInEvent_E_neutralPFOsOnly   = 0.;

            // charged pfos
            N_chsPFOsAllInEvent_chargedPFOsOnly      = 0;
            SumChsPFOsAllInEvent_Pt_chargedPFOsOnly  = 0.;
            SumChsPFOsAllInEvent_Eta_chargedPFOsOnly = 0.;
            SumChsPFOsAllInEvent_Phi_chargedPFOsOnly = 0.;
            SumChsPFOsAllInEvent_E_chargedPFOsOnly   = 0.; 

            TLorentzVector SumOfAllPFOs;
            TLorentzVector SumOfAllPFOs_neutralPFOsOnly;
            TLorentzVector SumOfAllPFOs_chargedPFOsOnly;

            for(const auto& pfo_i : *constits.pfoCont){
              TLorentzVector pfoCurrVect;
              pfoCurrVect.SetPtEtaPhiE( pfo_i->pt(), pfo_i->eta(), pfo_i->phi(), pfo_i->e() );
              SumOfAllPFOs += pfoCurrVect;
              N_chsPFOsAllInEvent++;
              if( fabs(pfo_i->charge()) > FLT_MIN ){ // charged
                N_chsPFOsAllInEvent_chargedPFOsOnly++;
                TLorentzVector pfoCurrVect_chargedPFOsOnly;
                pfoCurrVect_chargedPFOsOnly.SetPtEtaPhiE( pfo_i->pt(), pfo_i->eta(), pfo_i->phi(), pfo_i->e() );
                SumOfAllPFOs_chargedPFOsOnly += pfoCurrVect_chargedPFOsOnly;
              }
              else{ // neutral
                N_chsPFOsAllInEvent_neutralPFOsOnly++;
                TLorentzVector pfoCurrVect_neutralPFOsOnly;
                pfoCurrVect_neutralPFOsOnly.SetPtEtaPhiE( pfo_i->pt(), pfo_i->eta(), pfo_i->phi(), pfo_i->e() );
                SumOfAllPFOs_neutralPFOsOnly += pfoCurrVect_neutralPFOsOnly;
              }
            }

            // all pfos in event
            SumChsPFOsAllInEvent_Pt  = SumOfAllPFOs.Pt();
            SumChsPFOsAllInEvent_Eta = SumOfAllPFOs.Eta();
            SumChsPFOsAllInEvent_Phi = SumOfAllPFOs.Phi();
            SumChsPFOsAllInEvent_E   = SumOfAllPFOs.E();

            // charged pfos in maps
            SumChsPFOsAllInEvent_Pt_chargedPFOsOnly  = SumOfAllPFOs_chargedPFOsOnly.Pt();
            SumChsPFOsAllInEvent_Eta_chargedPFOsOnly = SumOfAllPFOs_chargedPFOsOnly.Eta();
            SumChsPFOsAllInEvent_Phi_chargedPFOsOnly = SumOfAllPFOs_chargedPFOsOnly.Phi();
            SumChsPFOsAllInEvent_E_chargedPFOsOnly   = SumOfAllPFOs_chargedPFOsOnly.E(); 

            // neutral pfos in maps
            SumChsPFOsAllInEvent_Pt_neutralPFOsOnly  = SumOfAllPFOs_neutralPFOsOnly.Pt();
            SumChsPFOsAllInEvent_Eta_neutralPFOsOnly = SumOfAllPFOs_neutralPFOsOnly.Eta();
            SumChsPFOsAllInEvent_Phi_neutralPFOsOnly = SumOfAllPFOs_neutralPFOsOnly.Phi();
            SumChsPFOsAllInEvent_E_neutralPFOsOnly   = SumOfAllPFOs_neutralPFOsOnly.E();

            // all pfos in event
            dec_NchsPFOsAllInEvent(*obj)       = N_chsPFOsAllInEvent;
            dec_SumChsPFOsAllInEvent_Pt(*obj)  = SumChsPFOsAllInEvent_Pt;
            dec_SumChsPFOsAllInEvent_Eta(*obj) = SumChsPFOsAllInEvent_Eta;
            dec_SumChsPFOsAllInEvent_Phi(*obj) = SumChsPFOsAllInEvent_Phi;
            dec_SumChsPFOsAllInEvent_E(*obj)   = SumChsPFOsAllInEvent_E;

            // charged PFOs only in event
            dec_NchsPFOsAllInEvent_charged(*obj)       = N_chsPFOsAllInEvent_chargedPFOsOnly;
            dec_SumChsPFOsAllInEvent_Pt_charged(*obj)  = SumChsPFOsAllInEvent_Pt_chargedPFOsOnly;
            dec_SumChsPFOsAllInEvent_Eta_charged(*obj) = SumChsPFOsAllInEvent_Eta_chargedPFOsOnly;
            dec_SumChsPFOsAllInEvent_Phi_charged(*obj) = SumChsPFOsAllInEvent_Phi_chargedPFOsOnly;
            dec_SumChsPFOsAllInEvent_E_charged(*obj)   = SumChsPFOsAllInEvent_E_chargedPFOsOnly;
   
            // neutral PFOs only in event
            dec_NchsPFOsAllInEvent_neutral(*obj)       = N_chsPFOsAllInEvent_neutralPFOsOnly;
            dec_SumChsPFOsAllInEvent_Pt_neutral(*obj)  = SumChsPFOsAllInEvent_Pt_neutralPFOsOnly;
            dec_SumChsPFOsAllInEvent_Eta_neutral(*obj) = SumChsPFOsAllInEvent_Eta_neutralPFOsOnly;
            dec_SumChsPFOsAllInEvent_Phi_neutral(*obj) = SumChsPFOsAllInEvent_Phi_neutralPFOsOnly;
            dec_SumChsPFOsAllInEvent_E_neutral(*obj)   = SumChsPFOsAllInEvent_E_neutralPFOsOnly;
            /*
            std::cout<<"!!!!! N_chsPFOsAllInEvent       = "<<N_chsPFOsAllInEvent<<std::endl;
            std::cout<<"!!!!! SumChsPFOsAllInEvent_Pt   = "<<SumChsPFOsAllInEvent_Pt<<std::endl;
            std::cout<<"!!!!! SumChsPFOsAllInEvent_Eta  = "<<SumChsPFOsAllInEvent_Eta<<std::endl;
            std::cout<<"!!!!! SumChsPFOsAllInEvent_Phi  = "<<SumChsPFOsAllInEvent_Phi<<std::endl;
            std::cout<<"!!!!! SumChsPFOsAllInEvent_E    = "<<SumChsPFOsAllInEvent_E<<std::endl;            
            std::cout<<"----------------------------------"<<std::endl;
            */

            //std::cout<<"UEcorr_Pt = "<<UEcorr_Pt<<std::endl;
            //std::cout<<"lept_count after = "<<lept_count<<std::endl;
          }
          else{
            //std::cout<<"METAssociator::fillAssocMap: m_recoil = false, extractPFO "<<std::endl;
            ATH_CHECK( this->extractPFO(obj,constlist,constits,momentumOverride) );
          }
          MissingETComposition::insert(metMap,obj,constlist,momentumOverride);
          
        }
      } 
      else {
        std::vector<const IParticle*> tclist;
        tclist.reserve(20);
        ATH_CHECK( this->extractTopoClusters(obj,tclist,constits) );
        if(m_useModifiedClus) {
          for(const auto& cl : tclist) {
          // use index-parallelism to identify shallow copied constituents
            constlist.push_back((*constits.tcCont)[cl->index()]);
          }
        } 
        else {
          constlist = tclist;
        }
        if(m_useTracks) ATH_CHECK( this->extractTracks(obj,constlist,constits) );
        MissingETComposition::insert(metMap,obj,constlist);
      }
    }
    return StatusCode::SUCCESS;
  }

  // Accept Track
  ////////////////
  bool METAssociator::acceptTrack(const xAOD::TrackParticle* trk, const xAOD::Vertex* vx) const
  {

    if (!vx) return false;//in events with no pv, we will just reject all tracks, and therefore build only the calo MET
    const Root::TAccept& accept = m_trkseltool->accept( *trk, vx );
    return accept;
  }


  bool METAssociator::isGoodEoverP(const xAOD::TrackParticle* trk) const
  {

    if( (fabs(trk->eta())<1.5 && trk->pt()>m_cenTrackPtThr) ||
	(fabs(trk->eta())>=1.5 && trk->pt()>m_forTrackPtThr) ) {

      // Get relative error on qoverp
      float Rerr = Amg::error(trk->definingParametersCovMatrix(),4)/fabs(trk->qOverP());
      ATH_MSG_VERBOSE( "Track momentum error (%): " << Rerr*100 );

      // first compute track and calo isolation variables
      float ptcone20 = 0., isolfrac = 0., etcone10 = 0., EoverP = 0.;
      // ptcone
      TrackIsolation trkIsoResult;
      std::vector<Iso::IsolationType> trkIsoCones; 
      trkIsoCones.push_back(xAOD::Iso::IsolationType::ptcone20);
      xAOD::TrackCorrection trkIsoCorr;
      trkIsoCorr.trackbitset.set(xAOD::Iso::IsolationTrackCorrection::coreTrackPtr); 
      m_trkIsolationTool->trackIsolation(trkIsoResult,
					 *trk,
					 trkIsoCones,
					 trkIsoCorr);
      ptcone20 = trkIsoResult.ptcones.size() > 0 ? trkIsoResult.ptcones[0] : 0;
      isolfrac = ptcone20/trk->pt();
      // etcone
      CaloIsolation caloIsoResult;
      std::vector<Iso::IsolationType> caloIsoCones; 
      // We can't actually configure the tool to give etcone10, so instead we have to compute etcone20,
      // applying the core cone correction.
      // Then, we retrieve the correction value, which is etcone10, rather than the isolation value
      caloIsoCones.push_back(xAOD::Iso::IsolationType::etcone20); 
      xAOD::CaloCorrection caloIsoCorr_coreCone;
      caloIsoCorr_coreCone.calobitset.set(xAOD::Iso::IsolationCaloCorrection::coreCone); // this is etcone10
      m_caloIsolationTool->caloTopoClusterIsolation(caloIsoResult,
						    *trk,
						    caloIsoCones,
						    caloIsoCorr_coreCone);
      if(caloIsoResult.etcones.size() > 0) {
	// retrieve the correction value for the core cone
	etcone10 = caloIsoResult.coreCorrections[xAOD::Iso::IsolationCaloCorrection::coreCone][xAOD::Iso::IsolationCorrectionParameter::coreEnergy];
      } else {
	ATH_MSG_WARNING("isGoodEoverP: Failed to retrieve the isolation core correction (etcone10)! Setting etcone10=0");
	etcone10 = 0.;
      }
      EoverP   =  etcone10/trk->pt(); 
      /////////////////////////////////////////////////////////////////////////
      ATH_MSG_VERBOSE( "Track isolation fraction: " << isolfrac );
      ATH_MSG_VERBOSE( "Track E/P = " << EoverP );

      if(isolfrac<0.1) {
	    // isolated track cuts
	    if(Rerr>0.4) return false;
	    else if (EoverP<0.65 && ((EoverP>0.1 && Rerr>0.05) || Rerr>0.1)) return false;
      } else {
	    // non-isolated track cuts
	    float trkptsum = ptcone20+trk->pt();
	    if(etcone10/trkptsum<0.6 && trk->pt()/trkptsum>0.6) return false;
      }
    }
    return true;
  }

}
