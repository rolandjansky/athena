/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "VrtSecInclusive/SoftBtagTrackSelector.h"
// #include "VrtSecInclusive/Constants.h"
#include "AthContainers/ConstDataVector.h"
#include "xAODCore/AuxContainerBase.h"

#include <string>
#include <new>
#include <iostream>
#include <tuple>
#include <functional>
#include <chrono>

namespace VKalVrtAthena {

  //Constructor and destructor
  //__________________________________________________________________________
  SoftBtagTrackSelector::SoftBtagTrackSelector(const std::string& name, ISvcLocator* pSvcLocator)
    : AthAlgorithm( name, pSvcLocator ),
      m_primaryVertices              ( nullptr ),
      m_thePV                        ( nullptr )

  {
    declareProperties();
  }


  //__________________________________________________________________________
  SoftBtagTrackSelector::~SoftBtagTrackSelector()
  {

  }



  //__________________________________________________________________________
  StatusCode SoftBtagTrackSelector::beginRun()  {



    return StatusCode::SUCCESS;
  }



  //__________________________________________________________________________
  StatusCode SoftBtagTrackSelector::initialize()
  {
    ATH_MSG_INFO("initialize: Exit SoftBtagTrackSelector::initialize()");

    m_trackSelectionAlgs["pt"]             =  &SoftBtagTrackSelector::selectTrack_pTCut;
    m_trackSelectionAlgs["eta"]            =  &SoftBtagTrackSelector::selectTrack_etaCut;
    m_trackSelectionAlgs["d0"]             =  &SoftBtagTrackSelector::selectTrack_d0Cut;
    m_trackSelectionAlgs["d0err"]          =  &SoftBtagTrackSelector::selectTrack_d0errCut;
    m_trackSelectionAlgs["d0sig"]          =  &SoftBtagTrackSelector::selectTrack_d0signifCut;
    m_trackSelectionAlgs["z0"]             =  &SoftBtagTrackSelector::selectTrack_z0Cut;
    m_trackSelectionAlgs["z0err"]          =  &SoftBtagTrackSelector::selectTrack_z0errCut;
    m_trackSelectionAlgs["z0wrtPV"]        =  &SoftBtagTrackSelector::selectTrack_z0wrtPVCut;
    m_trackSelectionAlgs["z0sigwrtPV"]     =  &SoftBtagTrackSelector::selectTrack_z0signifwrtPVCut;
    m_trackSelectionAlgs["z0sinTheta"]     =  &SoftBtagTrackSelector::selectTrack_z0sinThetaCut;
    m_trackSelectionAlgs["chi2"]           =  &SoftBtagTrackSelector::selectTrack_chi2Cut;
    m_trackSelectionAlgs["minJetDeltaR"]   =  &SoftBtagTrackSelector::selectTrack_JetDeltaRCut;
    m_trackSelectionAlgs["pixelHits"]      =  &SoftBtagTrackSelector::selectTrack_pixelHits;
    m_trackSelectionAlgs["sctHits"]        =  &SoftBtagTrackSelector::selectTrack_sctHits;
    m_trackSelectionAlgs["bLayHits"]       =  &SoftBtagTrackSelector::selectTrack_bLayHits;

    // now make histograms/ntuples
    if( m_jp.FillHist ) {
      ITHistSvc*     hist_root=0;
      ATH_CHECK( service( "THistSvc", hist_root ) );

      ATH_MSG_INFO("initialize: Filling Histograms");

      const std::string config[] = {"precut","postcut","n1"};
      for(const auto conf : config){
	m_hists["ntrk_"+ conf]            = new TH1F(Form("ntrk_%s",conf.c_str()) ,         ";Track multiplicity;",            400,    0,   400  );
	m_hists["pt_"+ conf]              = new TH1F(Form("pt_%s",conf.c_str()) ,           ";p_{T} [GeV];",                   100,    0,    10  );
	m_hists["eta_"+ conf]             = new TH1F(Form("eta_%s",conf.c_str()) ,          ";|#eta|;",                        100,    0,     3  );
	m_hists["d0_"+ conf]              = new TH1F(Form("d0_%s",conf.c_str()) ,           ";d0 [mm];",                       300,    0,     2  );
	m_hists["z0_"+ conf]              = new TH1F(Form("z0_%s",conf.c_str()) ,           ";z0 [mm];",                       100, -100,   100  );
	m_hists["d0err_"+ conf]           = new TH1F(Form("d0err_%s",conf.c_str()) ,        ";d0 error [mm];",                 100,    0,   0.5  );
	m_hists["z0err_"+ conf]           = new TH1F(Form("z0err_%s",conf.c_str()) ,        ";z0 error [mm];",                 100,    0,     2  );
	m_hists["chi2_"+ conf]            = new TH1F(Form("chi2_%s",conf.c_str()) ,         ";Track chi2;",                    100,    0,    10  );
	m_hists["d0sig_"+ conf]           = new TH1F(Form("d0sig_%s",conf.c_str()) ,        ";d0 significance;",               100,    0,     5  );
	m_hists["z0sig_"+ conf]           = new TH1F(Form("z0sig_%s",conf.c_str()) ,        ";z0 significance;",               100,    0,     2  );
	m_hists["z0sigwrtPV_"+ conf]      = new TH1F(Form("z0sigwrtPV_%s",conf.c_str()) ,   ";z0 significance  wrt PV;",       100,    0,     5  );
	m_hists["z0wrtPV_"+ conf]         = new TH1F(Form("z0wrtPV_%s",conf.c_str()) ,      ";|z0 wrt PV| [mm];",              100,    0,     5  );
	m_hists["z0sinTheta_"+ conf]      = new TH1F(Form("z0sinTheta_%s",conf.c_str()) ,   ";|z0 sin#theta| [mm];",           100,    0,   2.5  );
	m_hists["minJetDeltaR_"+ conf]    = new TH1F(Form("minJetDeltaR_%s",conf.c_str()) , ";min dR(track,jet);",             100,    0,     4  );
	m_hists["pixelHits_"+ conf]       = new TH1F(Form("pixelHits_%s",conf.c_str()) ,    ";Pixel Hits;",                     21,  -0.5,  20.5 );
	m_hists["sctHits_"+ conf]         = new TH1F(Form("sctHits_%s",conf.c_str()) ,      ";SCT Hits;",                       21,  -0.5,  20.5 );
	m_hists["bLayHits_"+ conf]        = new TH1F(Form("bLayHits_%s",conf.c_str()) ,     ";BLayer Hits;",                    11,  -0.5,  10.5 );
	m_hists["siHits_"+ conf]          = new TH1F(Form("siHits_%s",conf.c_str()) ,       ";Silicon hits;",                   21,  -0.5,  20.5 );

      }

      std::string histDir("/AANT/SoftBtagTrackSelector/");
      for( auto& pair : m_hists ) {
	ATH_CHECK( hist_root->regHist( histDir + pair.first, pair.second ) );
      }
    }
    return StatusCode::SUCCESS; 
  }


  //__________________________________________________________________________
  StatusCode SoftBtagTrackSelector::finalize()
  {

    ATH_MSG_INFO("finalize: SoftBtagTrackSelector finalize()");
    return StatusCode::SUCCESS; 
  }

  //__________________________________________________________________________
  StatusCode SoftBtagTrackSelector::initEvent()
  {

    return StatusCode::SUCCESS;

  }
  

  //__________________________________________________________________________
  StatusCode SoftBtagTrackSelector::execute()
  {
    //
    // ATH_MSG_DEBUG( __FUNCTION__ );
    StatusCode sc = this->initEvent();
    const xAOD::EventInfo * eventInfo = 0;
    CHECK( evtStore()->retrieve( eventInfo, "EventInfo" ) );

    m_isMC = false;
    if(eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION )) m_isMC = true;


    //--------------------------------------------------------
    //  Primary vertex processing
    //
    sc = this->processPrimaryVertices(); // fetch the 1st primary reconstructed vertex
    if( sc.isFailure() or !m_thePV ) {
      ATH_MSG_WARNING("processPrimaryVertices() failed");
      return StatusCode::SUCCESS;
    }


    //--------------------------------------------------------
    //  Jets processing
    //
    sc = this->processJets(); // fetch the jet container
    if( sc.isFailure() or !m_jets ) {
      ATH_MSG_WARNING("processJets() failed");
      return StatusCode::SUCCESS;
    }

    // Perform track selection    

    const xAOD::TrackParticleContainer* inputTrackParticles { nullptr };
    ATH_CHECK( evtStore()->retrieve( inputTrackParticles, m_jp.inputTrackContainer ) );

    auto *filteredTrackParticleContainer = new ConstDataVector<xAOD::TrackParticleContainer>( SG::VIEW_ELEMENTS );
    ATH_CHECK( evtStore()->record( filteredTrackParticleContainer, m_jp.outputTrackContainer ) );

    int ntrk_precut = 0;
    int ntrk_postcut = 0;

    for( auto* trk : *inputTrackParticles ) {
      bool isFromSV = false;

      clearTrackVariables();

      if( m_isMC and (m_jp.useTruthMatchedTrack or m_jp.FillHist ) ){
      	int apdgId_parent = abs(TrackTruthMatchParent( trk ));
      	if( ( apdgId_parent > 400 and apdgId_parent < 500 ) or ( apdgId_parent > 4000 and apdgId_parent < 5000 ) )
      	  isFromSV = true; // c-hadron
      	if( ( apdgId_parent > 500 and apdgId_parent < 600 ) or ( apdgId_parent > 5000 and apdgId_parent < 6000 ) )
      	  isFromSV = true; // b-hadron
      	if( !TrackTruthMatchFromSV(trk) ) isFromSV = false;
      }

      bool flag = true;

      for( auto& pair : m_trackSelectionAlgs ){
      	if ( !(this->*(pair.second))( trk ) ){
	  m_flags[pair.first] = false;
	  flag = false;
	}else{
	  m_flags[pair.first] = true;
	}
      }
      if(m_jp.FillHist)	FillHists(isFromSV);


      if(m_jp.skipTrackSelection) flag = true;

      if(flag){
	bool storeTrk = false;
	if (m_jp.useTruthMatchedTrack){
	  if( isFromSV ) storeTrk = true;
	}else{
	  storeTrk = true;
	}

	if(storeTrk){
	  if( flag ) filteredTrackParticleContainer->push_back( trk );

	}
      } // selected tracks

      ntrk_precut++;
      if(flag) ntrk_postcut++;

    } // loop over tracks

    ATH_MSG_DEBUG( __FUNCTION__ << Form(": filtered %4d out of %4d   tracks",
					ntrk_postcut,
					ntrk_precut));

    ATH_MSG_VERBOSE( "execute: process done." );

    return StatusCode::SUCCESS;   
 
  }

  //__________________________________________________________________________
  void SoftBtagTrackSelector::declareProperties()
  {
    declareProperty("FillHist",               m_jp.FillHist               = false        );
    declareProperty("StoreSelectedTracks",    m_jp.StoreSelectedTracks    = false        );

    declareProperty("Jetcontainer",           m_jp.jetContainer           = "AntiKt4EMTopoJets"           );
    declareProperty("Inputtrackcontainer",    m_jp.inputTrackContainer    = "InDetTrackParticles"         );
    declareProperty("OutputTrackContainer",   m_jp.outputTrackContainer   = "SoftBtagSeedTrackParticles"  );

    declareProperty("pTMinCut",               m_jp.pTMinCut               = 500.         );
    declareProperty("etaMinCut",              m_jp.etaMinCut              = 0.           );
    declareProperty("etaMaxCut",              m_jp.etaMaxCut              = 2.0          );
    declareProperty("chi2MaxCut",             m_jp.chi2MaxCut             = 4.0          );
    declareProperty("d0MinCut",               m_jp.d0MinCut               = 0.05         );
    declareProperty("d0MaxCut",               m_jp.d0MaxCut               = 10.0         );
    declareProperty("z0MinCut",               m_jp.z0MinCut               = 0.0          );
    declareProperty("z0MaxCut",               m_jp.z0MaxCut               = 100.         );
    declareProperty("d0errMaxCut",            m_jp.d0errMaxCut            = 0.13         );
    declareProperty("z0errMaxCut",            m_jp.z0errMaxCut            = 0.3          );
    declareProperty("z0wrtPVMinCut",          m_jp.z0wrtPVMinCut          = 0.           );
    declareProperty("z0wrtPVMaxCut",          m_jp.z0wrtPVMaxCut          = 2.           );
    declareProperty("z0sinThetaMinCut",       m_jp.z0sinThetaMinCut       = 0.           );
    declareProperty("z0sinThetaMaxCut",       m_jp.z0sinThetaMaxCut       = 1.2          );    

    declareProperty("d0signifMinCut",         m_jp.d0signifMinCut         = 1.7          );
    declareProperty("z0signifwrtPVMinCut",    m_jp.z0signifwrtPVMinCut    = 0.5          );

    declareProperty("JetDeltaRMinCut",        m_jp.jetDeltaRCut           = 0.4          );
    declareProperty("JetPtCut",               m_jp.jetPtCut               = 30.e3        );

    declareProperty("CutPixelHits",           m_jp.CutPixelHits           = 3            );
    declareProperty("CutSctHits",             m_jp.CutSctHits             = 1            );
    declareProperty("CutBLayHits",            m_jp.CutBLayHits             = 1           );

    declareProperty("truthSVMinDist",         m_jp.truthSVMinDist          = 0.2         );
    declareProperty("UseTruthMatchedTrack",   m_jp.useTruthMatchedTrack   = false        );
    declareProperty("SkipTrackSelection",     m_jp.skipTrackSelection     = false        );

  }

  void SoftBtagTrackSelector::clearTrackVariables(){

    m_trackVariables.clear();
    m_flags.clear();

  }

  //__________________________________________________________________________
  bool SoftBtagTrackSelector::selectTrack_pTCut( const xAOD::TrackParticle* trk ) {
    double pt = trk->pt();
    m_trackVariables["pt"] = pt/1000.;
    return ( pt > m_jp.pTMinCut );
  }


  bool SoftBtagTrackSelector::selectTrack_etaCut( const xAOD::TrackParticle* trk ) {
    double abseta = fabs(trk->eta());
    m_trackVariables["eta"] = abseta;
    return ( abseta > m_jp.etaMinCut and abseta < m_jp.etaMaxCut );
  }


  bool SoftBtagTrackSelector::selectTrack_d0Cut( const xAOD::TrackParticle* trk ){
    double absd0 = fabs(trk->d0());
    m_trackVariables["d0"] = absd0;
    return ( absd0 > m_jp.d0MinCut && absd0 < m_jp.d0MaxCut );
  }


  bool SoftBtagTrackSelector::selectTrack_d0errCut    ( const xAOD::TrackParticle* trk ) {    
    double d0err = sqrt(fabs(trk->definingParametersCovMatrix()(0,0)));
    m_trackVariables["d0err"] =  d0err;
    return ( d0err < m_jp.d0errMaxCut );
  }


  bool SoftBtagTrackSelector::selectTrack_d0signifCut ( const xAOD::TrackParticle* trk ) {
    double d0signif = fabs(trk->d0())/sqrt(trk->definingParametersCovMatrix()(0,0));
    m_trackVariables["d0sig"] = d0signif;
    return (d0signif > m_jp.d0signifMinCut);
  }


  bool SoftBtagTrackSelector::selectTrack_z0Cut       ( const xAOD::TrackParticle* trk ) {
    double z0 = trk->z0();
    m_trackVariables["z0"] = z0;

    return ( fabs(z0) > m_jp.z0MinCut && fabs(z0) < m_jp.z0MaxCut );
  }


  bool SoftBtagTrackSelector::selectTrack_z0errCut    ( const xAOD::TrackParticle* trk ) {
    double z0err = sqrt(fabs(trk->definingParametersCovMatrix()(1,1)));
    m_trackVariables["z0err"] =  z0err;
    return z0err < m_jp.z0errMaxCut;
  }


  bool SoftBtagTrackSelector::selectTrack_z0wrtPVCut ( const xAOD::TrackParticle* trk ) {
    double z0wrtPV = trk->z0() + trk->vz() - m_thePV->z();
    m_trackVariables["z0wrtPV"] = z0wrtPV;
    return (fabs(z0wrtPV) > m_jp.z0wrtPVMinCut and fabs(z0wrtPV) < m_jp.z0wrtPVMaxCut);
  }

  bool SoftBtagTrackSelector::selectTrack_z0sinThetaCut ( const xAOD::TrackParticle* trk ) {
    double dz0 = trk->z0() + trk->vz() - m_thePV->z();
    double z0sinTheta = dz0 * sin( trk->theta() );
    m_trackVariables["z0sinTheta"] = z0sinTheta;

    return (fabs(z0sinTheta) > m_jp.z0sinThetaMinCut and fabs(z0sinTheta) < m_jp.z0sinThetaMaxCut );
  }


  bool SoftBtagTrackSelector::selectTrack_z0signifwrtPVCut ( const xAOD::TrackParticle* trk ) {
    const double z0wrtPV = trk->z0() - m_thePV->z();
    const double trk_z0err = sqrt(trk->definingParametersCovMatrix()(1,1));
    double z0signifwrtPV = fabs(z0wrtPV/trk_z0err);
    m_trackVariables["z0sigwrtPV"] = z0signifwrtPV;

    return z0signifwrtPV > m_jp.z0signifwrtPVMinCut;
  }
  

  bool SoftBtagTrackSelector::selectTrack_chi2Cut( const xAOD::TrackParticle* trk ) {

    double chi2 = trk->chiSquared()/(trk->numberDoF()+AlgConsts::infinitesimal);
    m_trackVariables["chi2"] = trk->chiSquared()/(trk->numberDoF()+AlgConsts::infinitesimal);

    return ( chi2 < m_jp.chi2MaxCut );
  }


  bool SoftBtagTrackSelector::selectTrack_JetDeltaRCut( const xAOD::TrackParticle* trk ) {

    TLorentzVector trk_tlv;
    trk_tlv.SetPtEtaPhiM(trk->pt(),trk->eta(),trk->phi(),trk->m());

    double min_dr = 99;
    double min_dr_pt = -1;
    for( auto *jet : *m_jets){
      TLorentzVector jet_tlv;
      if( jet->pt() < m_jp.jetPtCut ) continue;
      jet_tlv.SetPtEtaPhiM(jet->pt(),jet->eta(),jet->phi(),jet->m());
      double dr = trk_tlv.DeltaR(jet_tlv);

      if( dr < min_dr ){
	min_dr = dr;
	min_dr_pt = jet->pt();
      }
    }
    m_trackVariables["minJetDeltaR"] = min_dr;
    m_trackVariables["minJetDeltaR_pt"] = min_dr_pt;
    return ( min_dr > m_jp.jetDeltaRCut );
  }


  bool SoftBtagTrackSelector::selectTrack_pixelHits( const xAOD::TrackParticle* trk ) {
    uint8_t pixelHits;
    if( !(trk->summaryValue( pixelHits, xAOD::numberOfPixelHits ) ) ) pixelHits =0;
    m_trackVariables["pixelHits"] = pixelHits;
    return ( pixelHits	>= m_jp.CutPixelHits);
  }

  bool SoftBtagTrackSelector::selectTrack_sctHits( const xAOD::TrackParticle* trk ) {
    uint8_t sctHits;
    if( !(trk->summaryValue( sctHits, xAOD::numberOfSCTHits ) ) ) sctHits =0;
    m_trackVariables["sctHits"] = sctHits;
    return ( sctHits >= m_jp.CutSctHits);
  }

  bool SoftBtagTrackSelector::selectTrack_bLayHits( const xAOD::TrackParticle* trk ) {
    uint8_t bLayHits;
    if( !(trk->summaryValue( bLayHits, xAOD::numberOfInnermostPixelLayerHits ) ) ) bLayHits =0;
    m_trackVariables["bLayHits"] = bLayHits;
    return ( bLayHits >= m_jp.CutBLayHits);
  }

  StatusCode SoftBtagTrackSelector::processJets() {

    ATH_CHECK( evtStore()->retrieve( m_jets, m_jp.jetContainer) );
    return StatusCode::SUCCESS;
  }


  StatusCode SoftBtagTrackSelector::processPrimaryVertices() {
    
    ATH_CHECK( evtStore()->retrieve( m_primaryVertices, "PrimaryVertices") );

    m_thePV = nullptr;

    for( auto *vertex : *m_primaryVertices ) {
      
      if( xAOD::VxType::PriVtx != vertex->vertexType() ) continue;
      m_thePV = vertex;
      
    } // loop over PVs
    
    if( !m_thePV ) {
      ATH_MSG_DEBUG("No PV is found in the PV collection!");
      return StatusCode::FAILURE;
    }

    m_truthPV = nullptr;
    if( m_isMC ){
      const xAOD::TruthVertexContainer* truthVertices (  nullptr );
      ATH_CHECK(  evtStore()->retrieve( truthVertices, "TruthVertices" ) );
      for(const auto* vtx : *truthVertices ){
	m_truthPV = vtx;
	break;
      }
    }
    
    return StatusCode::SUCCESS;
  }


  int SoftBtagTrackSelector::TrackTruthMatchParent(const xAOD::TrackParticle* track) {

    if(!m_isMC) return 0;

    const xAOD::TruthParticle* TruthTrack = nullptr;

    if(track->isAvailable<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink")) {
      ElementLink<xAOD::TruthParticleContainer> link = track->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink");
      if(link.isValid() && track->auxdataConst<float>("truthMatchProbability") > 0.7) TruthTrack = *link;
    }

    if(TruthTrack != nullptr) {
      if(TruthTrack->nParents() > 0) {
	if(TruthTrack->parent(0)) return TruthTrack->parent(0)->absPdgId();
      }
    }
    return 0;
  }



  bool SoftBtagTrackSelector::TrackTruthMatchFromSV(const xAOD::TrackParticle* track) {

    if(!m_isMC) return false;
    if(!m_truthPV) return false;

    const xAOD::TruthParticle* TruthTrack = nullptr;

    if(track->isAvailable<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink")) {
      ElementLink<xAOD::TruthParticleContainer> link = track->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink");
      if(link.isValid() && track->auxdataConst<float>("truthMatchProbability") > 0.7) TruthTrack = *link;
    }

    if(TruthTrack == nullptr) return false;
    if(TruthTrack->nParents() == 0) return false;
    if(!TruthTrack->parent(0)) return false;

    if(!TruthTrack->hasProdVtx()) return false;
    const xAOD::TruthVertex *prodVtx = TruthTrack->prodVtx();

    Amg::Vector3D pv(m_truthPV->x(), m_truthPV->y(), m_truthPV->z());
    Amg::Vector3D sv(prodVtx->x(), prodVtx->y(), prodVtx->z());    
    double dist = (pv-sv).norm();

    return dist > m_jp.truthSVMinDist;
  }


  void SoftBtagTrackSelector::FillHists(const bool isFromSV){
    if( m_jp.useTruthMatchedTrack and !isFromSV ) return;
    for( auto& cut1: m_flags ){
      std::string name = cut1.first;
      bool passN1  = true;
      bool passAll = true;      

      for( auto& cut2 : m_flags){
	if( !cut2.second ){
	  passAll = false;
	  if(cut1 != cut2) passN1 = false;	
	}
      }

      m_hists[name + "_precut"]->Fill(m_trackVariables[name]);
      if(passAll) m_hists[name + "_postcut"]->Fill(m_trackVariables[name]);
      if(passN1) m_hists[name + "_n1"]->Fill(m_trackVariables[name]);      

    }
  }

} // end of namespace bracket
