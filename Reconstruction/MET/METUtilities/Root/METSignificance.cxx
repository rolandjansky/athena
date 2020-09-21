///////////////////////// -*- C++ -*- /////////////////////////////
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
// METSignificance.cxx
// Implementation file for class METSignificance
// Author: P.Francavilla<francav@cern.ch>
// Author: D.Schaefer<schae@cern.ch>
///////////////////////////////////////////////////////////////////

// METUtilities includes
#include "METUtilities/METSignificance.h"

// MET EDM
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"

// Jet EDM
#include "xAODJet/JetAttributes.h"

// Other xAOD EDM
#include "xAODTruth/TruthParticle.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"

#include "PathResolver/PathResolver.h"

// Needed for xAOD::get_eta_calo() function
#include "​ElectronPhotonFourMomentumCorrection/​EgammaCalibrationAndSmearingTool.h"

namespace met {
    
  typedef ElementLink<xAOD::IParticleContainer> iplink_t;
    
  static const SG::AuxElement::ConstAccessor<float> acc_varX("varX");
  static const SG::AuxElement::ConstAccessor<float> acc_varY("varY");
  static const SG::AuxElement::ConstAccessor<float> acc_covXY("covXY");
  static const SG::AuxElement::ConstAccessor<float> acc_jvt("Jvt");
  static const SG::AuxElement::ConstAccessor<float> acc_fjvt("fJvt");
  static const SG::AuxElement::ConstAccessor<float> acc_fjvt_der("DFCommonJets_fJvt");
    
  static const SG::AuxElement::ConstAccessor< std::vector<iplink_t > > dec_constitObjLinks("ConstitObjectLinks");
  const static MissingETBase::Types::bitmask_t invisSource = 0x100000; // doesn't overlap with any other
    
  METSignificance::METSignificance(const std::string& name) :
    AsgTool(name),
    m_jetCalibTool(""),
    m_muonCalibrationAndSmearingTool(""),
    m_egammaCalibTool(""),
    m_tCombinedP4FromRecoTaus(""),
    m_GeV(1.0e3),
    m_softTermParam(met::Random),
    m_jerForEMu(false),
    m_jetPtThr(-1.0),
    m_jetEtaThr(-1.0),
    m_significance(0.0),
    m_rho(0.0),
    m_VarL(0.0),
    m_VarT(0.0),
    m_CvLT(0.0),
    m_met_VarL(0.0),
    m_met_VarT(0.0),
    m_met_CvLT(0.0),
    m_met(0.0),
    m_metphi(0.0),
    m_ht(0.0),
    m_sumet(0.0),
    m_file(0),
    m_phi_reso_pt20(0),
    m_phi_reso_pt50(0),
    m_phi_reso_pt100(0)
  {
    declareProperty("SoftTermParam",        m_softTermParam = met::Random );
    declareProperty("SoftTermReso",         m_softTermReso  = 10.0        );
    declareProperty("TreatPUJets",          m_treatPUJets   = true        );
    declareProperty("TreatPUJetsOld",       m_treatPUJetsOld= false       );
    declareProperty("DoPhiReso",            m_doPhiReso     = false       );
    declareProperty("ApplyBias",            m_applyBias     = false       );
    declareProperty("DoJerForEMu",          m_jerForEMu     = false       ); // run jet resolution for all electrons and muons
    declareProperty("ScalarBias",           m_scalarBias    = 0.0         );
    declareProperty("JetPtThr",             m_jetPtThr      = -1.0        );
    declareProperty("JetEtaThr",            m_jetEtaThr     = -1.0        );
    declareProperty("ConfigPrefix",         m_configPrefix  = "METUtilities/data17_13TeV/metsig_Aug15/");
    declareProperty("ConfigJetPhiResoFile", m_configJetPhiResoFile  = "jet_unc.root" );
    declareProperty("JetResoAux",           m_JetResoAux            = ""  ); // relative pT resolution in addition to normal JES
    declareProperty("EMuResoAux",           m_EMuResoAux            = ""  ); // aux string sets a bool for the leptons to run the jet resolation
    declareProperty("JetCollection",        m_JetCollection         = "AntiKt4EMTopo" );

    // properties to delete eventually
    declareProperty("IsDataJet",   m_isDataJet     = false   );
    declareProperty("IsDataMuon",  m_isDataMuon    = false   );
    declareProperty("IsAFII",      m_isAFII        = false   );

    // Phi resolution
    std::string configpath  = PathResolverFindCalibFile(m_configPrefix+m_configJetPhiResoFile);
    m_file = TFile::Open(configpath.c_str());
    if(m_file){
      m_phi_reso_pt20 = static_cast<TH2F *>(m_file->Get("phi_reso_pt20"));
      if(!m_phi_reso_pt20) ATH_MSG_ERROR("PU Jet Uncertainty Histogram not valid");
      m_phi_reso_pt50 = static_cast<TH2F *>(m_file->Get("phi_reso_pt50"));
      if(!m_phi_reso_pt50) ATH_MSG_ERROR("PU Jet Uncertainty Histogram not valid");
      m_phi_reso_pt100 = static_cast<TH2F *>(m_file->Get("phi_reso_pt100"));
      if(!m_phi_reso_pt100) ATH_MSG_ERROR("PU Jet Uncertainty Histogram not valid");
    }
    else{
      ATH_MSG_ERROR("PU Jet Uncertainty TFile is not valid: " << configpath);
    }
  }
  
  METSignificance::~METSignificance(){}
  
  StatusCode METSignificance::initialize(){

    ATH_MSG_INFO ("Initializing " << name() << "...");
    ATH_MSG_INFO("Set up JER tools");
    std::string toolName;
    std::string jetcoll = "AntiKt4EMTopoJets";
    toolName = "JetCalibrationTool/jetCalibTool_"+m_JetCollection;
    ATH_MSG_INFO("Set up jet resolution tool");
    m_jetCalibTool.setTypeAndName(toolName);

    if( !m_jetCalibTool.isUserConfigured() ){

      std::string config = "JES_data2017_2016_2015_Recommendation_Aug2018_rel21.config";
      std::string calibSeq = "JetArea_Residual_EtaJES_GSC_Smear";
      std::string calibArea = "00-04-81";
      if(m_JetCollection=="AntiKt4EMPFlow"){
        config = "JES_data2017_2016_2015_Recommendation_PFlow_Aug2018_rel21.config";
        calibSeq = "JetArea_Residual_EtaJES_GSC_Smear";
        calibArea = "00-04-81";        
      }
    
      ANA_CHECK( ASG_MAKE_ANA_TOOL(m_jetCalibTool, JetCalibrationTool) );
      ANA_CHECK( m_jetCalibTool.setProperty("JetCollection",m_JetCollection) );
      ANA_CHECK( m_jetCalibTool.setProperty("ConfigFile",config) );
      ANA_CHECK( m_jetCalibTool.setProperty("CalibSequence",calibSeq) );
      ANA_CHECK( m_jetCalibTool.setProperty("CalibArea",calibArea) );
      ANA_CHECK( m_jetCalibTool.setProperty("IsData",false) ); // configure for MC due to technical reasons. Both data and MC smearing are available with this setting.
      ANA_CHECK( m_jetCalibTool.retrieve() );
    }

    ATH_MSG_INFO("Set up MuonCalibrationAndSmearing tools");
    toolName = "MuonCalibrationAndSmearingTool";
    m_muonCalibrationAndSmearingTool.setTypeAndName("CP::MuonCalibrationAndSmearingTool/METSigAutoConf_"+toolName);
    ATH_CHECK(m_muonCalibrationAndSmearingTool.retrieve());

    ATH_MSG_DEBUG( "Initialising EgcalibTool " );
    toolName = "EgammaCalibrationAndSmearingTool";
    m_egammaCalibTool.setTypeAndName("CP::EgammaCalibrationAndSmearingTool/METSigAutoConf_" + toolName);
    ATH_CHECK(m_egammaCalibTool.setProperty("ESModel", "es2017_R21_v0"));
    ATH_CHECK(m_egammaCalibTool.setProperty("decorrelationModel", "1NP_v1"));
    if(m_isAFII) ATH_CHECK(m_egammaCalibTool.setProperty("useAFII", 1));
    else ATH_CHECK(m_egammaCalibTool.setProperty("useAFII", 0));
    ATH_CHECK( m_egammaCalibTool.retrieve() );

    toolName = "TauPerfTool";
    m_tCombinedP4FromRecoTaus.setTypeAndName("CombinedP4FromRecoTaus/METSigAutoConf_" + toolName);
    ATH_CHECK(m_tCombinedP4FromRecoTaus.setProperty("WeightFileName", "CalibLoopResult_v04-04.root"));
    ATH_CHECK( m_tCombinedP4FromRecoTaus.retrieve() );

    return StatusCode::SUCCESS;
  }
  
  StatusCode METSignificance::finalize(){

    ATH_MSG_INFO ("Finalizing " << name() << "...");
    delete m_phi_reso_pt20;
    delete m_phi_reso_pt50;
    delete m_phi_reso_pt100;

    return StatusCode::SUCCESS;
  }
  
  // **** Rebuild generic MET term ****
  StatusCode METSignificance::varianceMET(xAOD::MissingETContainer* metCont, float avgmu, std::string jetTermName, std::string softTermName, std::string totalMETName){

    // reset variables
    m_VarL = 0.0;
    m_VarT = 0.0;
    m_CvLT = 0.0;

    int metTerm = 0;
    double particle_sum[2][2] = {{0.0,0.0}, {0.0,0.0}};
    m_metphi = 0.0; //Angle for rotation of the cov matrix
    m_met = -1.0; // Numerator
    m_metsoft = 0.0;
    m_metsoftphi = 0.0;
    m_sumet=-1.0;
    m_ht=0.0;
    m_term_VarL.clear();
    m_term_VarT.clear();
    m_term_CvLT.clear();

    unsigned nIterSoft=0;
    double softSumET=0.0;

    // first fill the total MET
    if(metCont->find(totalMETName)!=metCont->end()){
      const auto &tot_met = static_cast<xAOD::MissingET*>(*(metCont->find(totalMETName)));
      if(!MissingETBase::Source::isTotalTerm(tot_met->source())){
        ATH_MSG_ERROR("NOT the total MET with name:" <<totalMETName);
        return StatusCode::SUCCESS;
      }
      m_met    = tot_met->met()/m_GeV;
      m_metx   = tot_met->mpx()/m_GeV;
      m_mety   = tot_met->mpy()/m_GeV;
      m_metphi = tot_met->phi();
      m_sumet  = tot_met->sumet()/m_GeV;
      m_ht     = m_sumet;
      ATH_MSG_VERBOSE("total MET: " << m_met << " phi: " << m_metphi << " name: " << tot_met->name());
    }
    else{
      ATH_MSG_ERROR("Could not find the total MET with name:" <<totalMETName);
      return StatusCode::SUCCESS;
    }
    m_met_vect.SetXYZ(m_metx, m_mety, 0);

    // Fill the remaining terms
    for(const auto& met : *metCont) {

      // skip the invisible and total MET
      if(MissingETBase::Source::isTotalTerm(met->source())){
        ATH_MSG_VERBOSE("Total: " << met->name() << " val: " << met->met());
        continue;
      }
      if(met->source()==invisSource) continue;

      // Soft term collection
      if(MissingETBase::Source::isSoftTerm(met->source())){

        if(!MissingETBase::Source::hasPattern(met->source(),MissingETBase::Source::Track)) continue;
        ATH_MSG_VERBOSE("Soft Name: " << met->name());
        // make sure the container name matches
        if(met->name()!=softTermName || nIterSoft>0){
          if(nIterSoft>0) ATH_MSG_ERROR("Found multiple soft terms with the name:" <<softTermName << ". Your MET configuration is wrong!!!");
          continue;
        }
        ++nIterSoft;
        softSumET=(met->sumet()/m_GeV);

        AddSoftTerm(met, m_met_vect, particle_sum);
        m_metsoft = met->met()/m_GeV;
        m_metsoftphi = met->phi();
        metTerm = 2; // this is actually filled in AddSoftTerm
        // done with the soft term. go to the next term.
        continue;
      }
      ATH_MSG_VERBOSE("Add MET term " << met->name() );
      for(const auto& el : dec_constitObjLinks(*met)) {
        const xAOD::IParticle* obj(*el);
        float pt_reso=0.0, phi_reso=0.0;
        if(!obj){
          ATH_MSG_ERROR("Particle pointer is not valid. This will likely result in a crash " << obj);
          return StatusCode::FAILURE;
        }
        ATH_MSG_VERBOSE("pT: " << obj->pt() << " type: " << obj->type() << " truth: " << (obj->type()==xAOD::Type::TruthParticle));
        if(obj->type()==xAOD::Type::Muon || (obj->type()==xAOD::Type::TruthParticle && fabs(static_cast<const xAOD::TruthParticle*>(obj)->pdgId())==13)){
          ATH_CHECK(AddMuon(obj, pt_reso, phi_reso, avgmu));
          metTerm=4;
        }
        else if(obj->type()==xAOD::Type::Jet){
          // make sure the container name matches
          if(met->name()!=jetTermName) continue;
          ATH_CHECK(AddJet(obj, pt_reso, phi_reso, avgmu));
          metTerm=1;
        }
        else if(obj->type()==xAOD::Type::Electron || (obj->type()==xAOD::Type::TruthParticle && fabs(static_cast<const xAOD::TruthParticle*>(obj)->pdgId())==11)){
          ATH_CHECK(AddElectron(obj, pt_reso, phi_reso, avgmu));
          metTerm=3;
        }
        else if(obj->type()==xAOD::Type::Photon || (obj->type()==xAOD::Type::TruthParticle && fabs(static_cast<const xAOD::TruthParticle*>(obj)->pdgId())==22)){
          ATH_CHECK(AddPhoton(obj, pt_reso, phi_reso));
          metTerm=5;
        }
        else if(obj->type()==xAOD::Type::Tau || (obj->type()==xAOD::Type::TruthParticle && fabs(static_cast<const xAOD::TruthParticle*>(obj)->pdgId())==15)){
          AddTau(obj, pt_reso, phi_reso);
          metTerm=6;
        }

        // compute NEW
        double particle_u[2][2]     = {{pt_reso*pt_reso*obj->pt()*obj->pt()/m_GeV/m_GeV,0.0},
                                        {0.0,phi_reso*phi_reso/m_GeV/m_GeV}};
        double particle_u_rot[2][2] = {{pt_reso*pt_reso*obj->pt()*obj->pt()/m_GeV/m_GeV,0.0},
                                        {0.0,phi_reso*phi_reso/m_GeV/m_GeV}};
        RotateXY(particle_u, particle_u_rot,m_met_vect.DeltaPhi(obj->p4().Vect()));
        m_VarL+=particle_u_rot[0][0];
        m_VarT+=particle_u_rot[1][1];
        m_CvLT+=particle_u_rot[0][1];

        // Save the resolutions separated for each object type
        AddResoMap(particle_u_rot[0][0],
                    particle_u_rot[1][1],
                    particle_u_rot[0][1],
                    metTerm);

        RotateXY (particle_u,   particle_u_rot, obj->p4().Phi()); // positive phi rotation
        AddMatrix(particle_sum, particle_u_rot, particle_sum);
        // END compute NEW

        ATH_MSG_VERBOSE("Resolution: " << pt_reso << " phi reso: " << phi_reso );
      }
    }

    // setting the MET directed variables for later phi rotations if requested
    m_met_VarL=m_VarL;
    m_met_VarT=m_VarT;
    m_met_CvLT=m_CvLT;

    if( m_VarL != 0 ){

      if(m_applyBias){
        TVector3 met_vect = m_met_vect;
        TVector3 soft_vect = m_soft_vect;

        // should be done to reset the phi as well...
        if(m_softTermParam==met::TSTParam){
          Double_t Bias_TST = BiasPtSoftdir(m_metsoft);
          Double_t MEx = m_met * cos(m_metphi) - Bias_TST * cos(m_metsoftphi);
          Double_t MEy = m_met * sin(m_metphi) - Bias_TST * sin(m_metsoftphi);
          met_vect.SetXYZ(MEx,MEy,0.0);
        }
        else if(m_softTermParam==met::PthardParam){
          m_soft_vect.SetPtEtaPhi(m_metsoft, 0.0, m_metsoftphi);
          m_pthard_vect = m_soft_vect - m_met_vect;
          Double_t PtSoftparaPH = m_pthard_vect.Mag()>0.0 ? (m_soft_vect.Dot(m_pthard_vect))/m_pthard_vect.Mag() : 0.0;
          Double_t Bias_pthard = Bias_PtSoftParall(PtSoftparaPH);
          Double_t MEx = m_met * cos(m_metphi) - Bias_pthard * cos(m_metsoftphi);
          Double_t MEy = m_met * sin(m_metphi) - Bias_pthard * sin(m_metsoftphi);
          met_vect.SetXYZ(MEx,MEy,0.0);
        }
        // Rotate  & compute
        ATH_CHECK(RotateToPhi(met_vect.Phi()));
        m_significance = Significance_LT(met_vect.Pt(), m_VarL, m_VarT, m_CvLT);
        m_rho = m_CvLT / sqrt( m_VarL * m_VarT ) ;
      }
      else{
        // standard calculation
        m_significance = Significance_LT(m_met, m_VarL, m_VarT, m_CvLT);
        m_rho = m_CvLT / sqrt( m_VarL * m_VarT ) ;
      }
      m_ht-=softSumET;
      ATH_MSG_VERBOSE("     Significance (squared): " << m_significance << " rho: " << GetRho()
                    << " MET: " << m_met << " phi: " << m_metphi << " SUMET: " << m_sumet << " HT: " << m_ht << " sigmaL: " << GetVarL()
                    << " sigmaT: " << GetVarT() << " MET/sqrt(SumEt): " << GetMETOverSqrtSumET()
                    << " MET/sqrt(HT): " << GetMETOverSqrtHT()
                    << " sqrt(signif): " << GetSignificance()
                    << " sqrt(signifDirectional): " << GetSigDirectional());
    }
    else
      ATH_MSG_DEBUG("Var_L is 0");

    return StatusCode::SUCCESS;
  }

  StatusCode METSignificance::RotateToPhi(float phi){

    // Rotation (components)
    std::tie(m_VarL, m_VarT, m_CvLT) = CovMatrixRotation(m_met_VarL , m_met_VarT, m_met_CvLT, (phi-m_metphi));

    if( m_VarL != 0 ){
      m_significance = Significance_LT(m_met,m_VarL,m_VarT,m_CvLT );
      m_rho = m_CvLT  / sqrt( m_VarL * m_VarT ) ;
    }
    ATH_MSG_DEBUG("     Significance (squared) at new phi: " << m_significance
                << " rho: " << GetRho()
                << " MET: " << m_met
                << " sigmaL: " << GetVarL()
                << " sigmaT: " << GetVarT() );

    return StatusCode::SUCCESS;
  }

  StatusCode METSignificance::SetLambda(const float px, const float py, const bool GeV){

    // compute the new direction
    double GeVConv = GeV ? 1.0 : m_GeV;
    m_lamda_vect.SetXYZ(px/GeVConv, py/GeVConv, 0.0);
    m_lamda_vect = (m_met_vect - m_lamda_vect);
    const double met_m_lamda = m_lamda_vect.Pt();

    // Rotation (components)
    std::tie(m_VarL, m_VarT, m_CvLT) = CovMatrixRotation(m_met_VarL , m_met_VarT, m_met_CvLT, (m_lamda_vect.Phi()-m_metphi));

    if( m_VarL != 0 ){
      m_significance = Significance_LT(met_m_lamda,m_VarL,m_VarT,m_CvLT );
      m_rho = m_CvLT  / sqrt( m_VarL * m_VarT ) ;
    }
    ATH_MSG_DEBUG("     Significance (squared) at new phi: " << m_significance
                << " rho: " << GetRho()
                << " MET: " << m_met
                << " sigmaL: " << GetVarL()
                << " sigmaT: " << GetVarT() );

    return StatusCode::SUCCESS;
  }

  // Muon propagation of resolution
  StatusCode METSignificance::AddMuon(const xAOD::IParticle* obj, float &pt_reso, float &phi_reso, float avgmu){

    int dettype = 0;
    bool DoEMuReso = false;
    ATH_MSG_VERBOSE("Particle type: " << obj->type());

    if(obj->type()==xAOD::Type::TruthParticle){
      pt_reso =0.01;
      if(obj->pt()>0.5e6) pt_reso=0.03;
      if(obj->pt()>1.0e6) pt_reso=0.1;// this is just a rough estimate for the time being until the interface can handle truth muons
    }
    else{
      const xAOD::Muon* muon(static_cast<const xAOD::Muon*>(obj));
      if(muon->muonType()==0){//Combined
        dettype=3;//CB
      }
      else if(muon->muonType()==1){//MuonStandAlone
        dettype=1;//MS
      }
      else if(muon->muonType()>1){//Segment, Calo, Silicon
        dettype=2;//ID
      }
      else{
        ATH_MSG_VERBOSE("This muon had none of the normal muon types (ID,MS,CB) - check this in detail");
        return StatusCode::FAILURE;
      }
    
      pt_reso=m_muonCalibrationAndSmearingTool->expectedResolution(dettype,*muon,!m_isDataMuon);
      if(m_doPhiReso) phi_reso = muon->pt()*0.001;
      // run the jet resolution for muons. for validation region extrapolation
      if(m_EMuResoAux!=""){
        SG::AuxElement::ConstAccessor<bool>  acc_EMReso(m_EMuResoAux);
        DoEMuReso = acc_EMReso.isAvailable(*muon) ? acc_EMReso(*muon) : false;
      }
      ATH_MSG_VERBOSE("muon: " << pt_reso << " dettype: " << dettype << " " << muon->pt() << " " << muon->p4().Eta() << " " << muon->p4().Phi());
    }// end reco setup
  
    // Common setup
    if(m_doPhiReso) phi_reso = obj->pt()*0.001;
    ATH_MSG_VERBOSE("muon: " << pt_reso << " dettype: " << dettype << " " << obj->pt() << " " << obj->p4().Eta() << " " << obj->p4().Phi());

    if(m_jerForEMu || DoEMuReso){
      bool treatPUJets = m_treatPUJets;
      m_treatPUJets=false; //turn off pileup jet treatement for this electron
      ATH_CHECK(AddJet(obj, pt_reso, phi_reso, avgmu));
      m_treatPUJets = treatPUJets; // reset value
    }

    return StatusCode::SUCCESS;
  }

  // Electron propagation of resolution
  StatusCode METSignificance::AddElectron(const xAOD::IParticle* obj, float &pt_reso, float &phi_reso, float avgmu){

    bool DoEMuReso = false;
    if(obj->type()==xAOD::Type::TruthParticle){
      pt_reso=m_egammaCalibTool->resolution(obj->e(),obj->eta(),obj->eta(),PATCore::ParticleType::Electron);
      if(m_doPhiReso) phi_reso = obj->pt()*0.004;
    }
    else{
      const xAOD::Electron* ele(static_cast<const xAOD::Electron*>(obj));
      const auto cl_etaCalo = xAOD::get_eta_calo(*(ele->caloCluster()), ele->author());
      pt_reso=m_egammaCalibTool->resolution(ele->e(),ele->caloCluster()->eta(),cl_etaCalo,PATCore::ParticleType::Electron);
      if(m_doPhiReso) phi_reso = ele->pt()*0.004;
      ATH_MSG_VERBOSE("el: " << pt_reso << " " << ele->pt() << " " << ele->p4().Eta() << " " << ele->p4().Phi());

      // run the jet resolution for muons. for validation region extrapolation
      if(m_EMuResoAux!=""){
        SG::AuxElement::ConstAccessor<bool>  acc_EMReso(m_EMuResoAux);
        DoEMuReso = acc_EMReso.isAvailable(*ele) ? acc_EMReso(*ele) : false;
      }
    }

    if(m_jerForEMu || DoEMuReso){
      bool treatPUJets = m_treatPUJets;
      m_treatPUJets=false; //turn off pileup jet treatement for this electron
      ATH_CHECK(AddJet(obj, pt_reso, phi_reso, avgmu));
      m_treatPUJets = treatPUJets; // reset value
    }
    return StatusCode::SUCCESS;
  }

  // Photon propagation of resolution
  StatusCode METSignificance::AddPhoton(const xAOD::IParticle* obj, float &pt_reso, float &phi_reso){

    if(obj->type()==xAOD::Type::TruthParticle){
      pt_reso=m_egammaCalibTool->resolution(obj->e(),obj->eta(),obj->eta(),PATCore::ParticleType::Electron); // leaving as an electron for the truth implementation rather than declaring a reco photon
      if(m_doPhiReso) phi_reso = obj->pt()*0.004;
    }
    else{
      const xAOD::Egamma* pho(static_cast<const xAOD::Egamma*>(obj));
      pt_reso=m_egammaCalibTool->getResolution(*pho);
      if(m_doPhiReso) phi_reso = pho->pt()*0.004;
      ATH_MSG_VERBOSE("pho: " << pt_reso << " " << pho->pt() << " " << pho->p4().Eta() << " " << pho->p4().Phi());
    }
    return StatusCode::SUCCESS;
  }

  // Jet propagation of resolution. returns the relative pT and phi resolution.
  StatusCode METSignificance::AddJet(const xAOD::IParticle* obj, float &pt_reso, float &phi_reso, float &avgmu){

    const xAOD::Jet* jet(static_cast<const xAOD::Jet*>(obj));
    double pt_reso_dbl_data=0.0, pt_reso_dbl_mc=0.0, pt_reso_dbl_max=0.0;

    // setting limits on jets if requested
    if(m_jetPtThr>0.0 && m_jetPtThr>jet->pt())          return StatusCode::SUCCESS;
    if(m_jetEtaThr>0.0 && m_jetEtaThr<fabs(jet->eta())) return StatusCode::SUCCESS;

    ATH_CHECK(m_jetCalibTool->getNominalResolutionData(*jet, pt_reso_dbl_data));
    ATH_CHECK(m_jetCalibTool->getNominalResolutionMC(*jet, pt_reso_dbl_mc));
    pt_reso_dbl_max = std::max(pt_reso_dbl_data,pt_reso_dbl_mc);
    pt_reso = pt_reso_dbl_max;

    ATH_MSG_VERBOSE("jet: " << pt_reso  << " jetpT: " << jet->pt() << " " << jet->p4().Eta() << " " << jet->p4().Phi());

    // Add extra uncertainty for PU jets based on JVT
    if(m_treatPUJets){
      double jet_pu_unc  = 0.;
      if(acc_fjvt.isAvailable(*jet))
        jet_pu_unc = GetPUProb(jet->eta(), jet->phi(),jet->pt()/m_GeV, acc_jvt(*jet), acc_fjvt(*jet), avgmu);
      else if(acc_fjvt_der.isAvailable(*jet))
        jet_pu_unc = GetPUProb(jet->eta(), jet->phi(),jet->pt()/m_GeV, acc_jvt(*jet), acc_fjvt_der(*jet), avgmu);
      else{
        ATH_MSG_ERROR("No fJVT decoration available - must have treat pileup jets set to off or provide fJVT!");
        return StatusCode::FAILURE;
      }
      pt_reso = sqrt(jet_pu_unc*jet_pu_unc + pt_reso*pt_reso);
      ATH_MSG_VERBOSE("jet_pu_unc: " << jet_pu_unc);
    }

    // Use the phi resolution of the jets
    // needs to be finished
    if(m_doPhiReso){
      double jet_phi_unc = fabs(GetPhiUnc(jet->eta(), jet->phi(),jet->pt()/m_GeV));
      phi_reso = jet->pt()*jet_phi_unc;
    }
    
    // Add user defined additional resolutions. For example, b-tagged jets
    if(m_JetResoAux!=""){
      SG::AuxElement::ConstAccessor<float> acc_extra(m_JetResoAux);
      if(acc_extra.isAvailable(*jet)){
        float extra_relative_pt_reso = acc_extra(*jet);
        pt_reso = sqrt(pt_reso*pt_reso + extra_relative_pt_reso*extra_relative_pt_reso);
      }
    }

    return StatusCode::SUCCESS;
  }

  // Tau propagation of resolution
  void METSignificance::AddTau(const xAOD::IParticle* obj, float &pt_reso, float &phi_reso){

    // tau objects
    if(obj->type()==xAOD::Type::TruthParticle){
      pt_reso= 0.1;
      if(m_doPhiReso) phi_reso = obj->pt()*0.01;
    }
    else{
      const xAOD::TauJet* tau(static_cast<const xAOD::TauJet*>(obj));
      pt_reso = dynamic_cast<CombinedP4FromRecoTaus*>(m_tCombinedP4FromRecoTaus.get())->GetCaloResolution(tau);
      // For taus, this is not a relative resolution. So we divide by pT
      pt_reso /=tau->pt();
      if(m_doPhiReso) phi_reso = tau->pt()*0.01;
      ATH_MSG_VERBOSE("tau: " << pt_reso << " " << tau->pt() << " " << tau->p4().Eta() << " " << tau->p4().Phi() << " phi reso: " << phi_reso);
    }
  }

  //
  // Soft term propagation of resolution
  //
  void METSignificance::AddSoftTerm(const xAOD::MissingET* soft, const TVector3 &met_vect, double (&particle_sum)[2][2]){

    if(m_softTermParam==met::Random){

      ATH_MSG_VERBOSE("Resolution Soft term set to 10GeV");

      m_soft_vect.SetXYZ(soft->mpx()/m_GeV, soft->mpy()/m_GeV, 0);

      double particle_u[2][2] = {{m_softTermReso*m_softTermReso,0.0},
                                 {0.0,m_softTermReso*m_softTermReso}};
      double particle_u_rot[2][2] = {{m_softTermReso*m_softTermReso,0.0},
                                     {0.0,m_softTermReso*m_softTermReso}};

      RotateXY(particle_u, particle_u_rot,met_vect.DeltaPhi(m_soft_vect));
      m_VarL+=particle_u_rot[0][0];
      m_VarT+=particle_u_rot[1][1];
      m_CvLT+=particle_u_rot[0][1];
      
      // Save the resolutions separated for each object type
      AddResoMap(particle_u_rot[0][0],
                 particle_u_rot[1][1],
                 particle_u_rot[0][1],
                 met::ResoSoft);

      RotateXY (particle_u,   particle_u_rot,-1.0*soft->phi()); // negative phi rotation
      AddMatrix(particle_sum, particle_u_rot,     particle_sum);

      ATH_MSG_VERBOSE("SOFT " << soft->name() <<" - pt_reso: " << m_softTermReso << " soft: " << soft->met() << " phi: " << soft->phi()
                   << " Var_L: " << particle_u_rot[0][0] << " Var_T: " << particle_u_rot[1][1]
                   << " " << particle_u_rot[0][1]);
    }
    else if (m_softTermParam==met::PthardParam){

      ATH_MSG_VERBOSE("Resolution Soft term parameterized in pthard direction");

      m_soft_vect.SetXYZ(soft->mpx()/m_GeV, soft->mpy()/m_GeV, 0);

      m_pthard_vect =  m_soft_vect - met_vect;

      double varTST = Var_Ptsoft(soft->met()/m_GeV);

      double particle_u[2][2] = {{varTST,0.0},
                                 {0.0,varTST}};
      double particle_u_rot[2][2] = {{varTST,0.0},
                                     {0.0,varTST}};

      RotateXY(particle_u, particle_u_rot,met_vect.DeltaPhi(m_pthard_vect));
      m_VarL+=particle_u_rot[0][0];
      m_VarT+=particle_u_rot[1][1];
      m_CvLT+=particle_u_rot[0][1];

      // Save the resolutions separated for each object type
      AddResoMap(particle_u_rot[0][0],
                 particle_u_rot[1][1],
                 particle_u_rot[0][1],
                 met::ResoSoft);

      RotateXY (particle_u,   particle_u_rot,-1.0*m_pthard_vect.Phi()); // negative phi rotation
      AddMatrix(particle_sum, particle_u_rot,     particle_sum);

    }
    else if (m_softTermParam==met::TSTParam){

      ATH_MSG_VERBOSE("Resolution Soft term parameterized in TST");

      m_soft_vect.SetXYZ(soft->mpx()/m_GeV, soft->mpy()/m_GeV, 0);

      double varTST = VarparPtSoftdir(soft->met()/m_GeV, soft->sumet()/m_GeV);

      double particle_u[2][2] = {{varTST,0.0},
                                 {0.0,varTST}};
      double particle_u_rot[2][2] = {{varTST,0.0},
                                     {0.0,varTST}};

      RotateXY(particle_u, particle_u_rot,met_vect.DeltaPhi(m_soft_vect));
      m_VarL+=particle_u_rot[0][0];
      m_VarT+=particle_u_rot[1][1];
      m_CvLT+=particle_u_rot[0][1];

      // Save the resolutions separated for each object type
      AddResoMap(particle_u_rot[0][0],
                 particle_u_rot[1][1],
                 particle_u_rot[0][1],
                 met::ResoSoft);

      RotateXY (particle_u,   particle_u_rot,-1.0*soft->phi()); // negative phi rotation
      AddMatrix(particle_sum, particle_u_rot,     particle_sum);

    }
    else{
      ATH_MSG_ERROR("Soft term parameterization is NOT defined for:" << m_softTermParam);
    }

  }

  double METSignificance::GetPUProb(double jet_eta, double /*jet_phi*/,
                                    double jet_pt,  double jet_jvt,
                                    double jet_fjvt,
                                    float avgmu) {

    double unc=0.0;
    if(m_treatPUJetsOld){
      if(jet_jvt<0.05 && fabs(jet_eta)<2.7 && jet_pt<150.0){
        unc=0.95;
      }else if(jet_jvt<0.59 && fabs(jet_eta)<2.7 && jet_pt<100.0){
        unc=0.4;
      }else if(jet_jvt<0.59 && fabs(jet_eta)<2.7 && jet_pt<100.0){
        unc=0.4;
      }else if(jet_pt<30.0 && fabs(jet_eta)>2.7){
        unc=0.2;
      }else if(jet_pt<40.0 && fabs(jet_eta)>2.7){
        unc=0.07;
      }
      return unc;
    }

    if(m_JetCollection == "AntiKt4EMTopoJets"){
      if(jet_eta<2.4){
        if(jet_pt<30){
          if(jet_jvt<0.11)      unc = 1;
          else if(jet_jvt<0.25) unc = 0.0730 + 0.0024 * avgmu + 0.00001 * avgmu * avgmu;
          else if(jet_jvt<0.85) unc = 0.0995 + 0.0031 * avgmu + 0.00005 * avgmu * avgmu;
          else if(jet_jvt<0.95) unc = 0.0311 + 0.0025 * avgmu + 0.00005 * avgmu * avgmu;
          else                  unc = 0.0308 -0.0010 * avgmu + 0.00006 * avgmu * avgmu ;
        }else if(jet_pt<40){
          if(jet_jvt<0.11)      unc = 1.;
          else if(jet_jvt<0.25) unc = 1.;
          else if(jet_jvt<0.85) unc = -0.0188 + 0.0039 * avgmu + 0.00002 * avgmu * avgmu;
          else if(jet_jvt<0.95) unc = 0.0252 -0.0009 * avgmu + 0.00006 * avgmu * avgmu  ;
          else                  unc = 0.0085 -0.0003 * avgmu + 0.00002 * avgmu * avgmu  ;
        }else if(jet_pt<50){
          if(jet_jvt<0.11)      unc = 1;
          else if(jet_jvt<0.25) unc = 0.0345 -0.0006 * avgmu + 0.00004 * avgmu * avgmu  ;
          else if(jet_jvt<0.85) unc = 0.1078 -0.0051 * avgmu + 0.00011 * avgmu * avgmu  ;
          else if(jet_jvt<0.95) unc = -0.0026 + 0.0005 * avgmu + 0.00002 * avgmu * avgmu;
          else                  unc = 0.0090 -0.0004 * avgmu + 0.00001 * avgmu * avgmu  ;
        }else if(jet_pt<60){
          if(jet_jvt<0.11)      unc = 1;
          else if(jet_jvt<0.25) unc = -0.0321 + 0.0030 * avgmu -0.00002 * avgmu * avgmu;
          else if(jet_jvt<0.85) unc = 0.0260 -0.0007 * avgmu + 0.00003 * avgmu * avgmu ;
          else                  unc = -0.0040 + 0.0003 * avgmu;
        }else if(jet_pt<100){
          unc = 0.9492 -2.0757 * jet_jvt + 1.13328 * jet_jvt * jet_jvt;
        }else if(jet_pt<150){
          unc = 0.7888 -1.8372 * jet_jvt + 1.05539 * jet_jvt * jet_jvt;
        }
      }else if(jet_eta<2.6){
        if(jet_pt<30){
          if(jet_jvt<0.11)      unc = 0.2633 + 0.0091 * avgmu + -0.00009 * avgmu * avgmu;
          else if(jet_jvt<0.25) unc = 0.1841 + 0.0144 * avgmu + -0.00008 * avgmu * avgmu;
          else if(jet_jvt<0.85) unc = 0.1401 + 0.0048 * avgmu + 0.00006 * avgmu * avgmu ;
          else if(jet_jvt<0.95) unc = -0.0118 + 0.0076 * avgmu + 0.00003 * avgmu * avgmu;
          else                  unc = 0.0534 + -0.0011 * avgmu + 0.00010 * avgmu * avgmu;
        }else if(jet_pt<40){
          if(jet_jvt<0.11)      unc = 0.1497 + 0.0133 * avgmu + -0.00015 * avgmu * avgmu  ;
          else if(jet_jvt<0.25) unc = -0.2260 + 0.0276 * avgmu + -0.00021 * avgmu * avgmu ;
          else if(jet_jvt<0.85) unc = 0.2743 + -0.0093 * avgmu + 0.00022 * avgmu * avgmu  ;
          else if(jet_jvt<0.95) unc = 0.0604 + 0.0006 * avgmu + 0.00006 * avgmu * avgmu   ;
          else                  unc = 0.0478 + -0.0009 * avgmu + 0.00004 * avgmu * avgmu  ;
        }else if(jet_pt<50){
          if(jet_jvt<0.11)      unc = -0.2187 + 0.0317 * avgmu + -0.00037 * avgmu * avgmu ;
          else if(jet_jvt<0.25) unc = 0.0964 + 0.0053 * avgmu + 0.00002 * avgmu * avgmu   ;
          else if(jet_jvt<0.85) unc = 1.1730 + -0.0624 * avgmu + 0.00088 * avgmu * avgmu  ;
          else if(jet_jvt<0.95) unc = -0.2011 + 0.0151 * avgmu + -0.00018 * avgmu * avgmu ;
          else                  unc = 0.0145 + -0.0003 * avgmu + 0.00002 * avgmu * avgmu  ;
        }else if(jet_pt<60){
          if(jet_jvt<0.11)      unc = 0.0051 + 0.0113 * avgmu + -0.00008 * avgmu * avgmu  ;
          else if(jet_jvt<0.25) unc = -0.1024 + 0.0109 * avgmu + -0.00006 * avgmu * avgmu ;
          else if(jet_jvt<0.85) unc = 1.2491 + -0.0501 * avgmu + 0.00052 * avgmu * avgmu  ;
          else                  unc = 0.0267 + -0.0014 * avgmu + 0.00003 * avgmu * avgmu  ;
        }else if(jet_pt<100){
          unc = 0.8951 -2.4995 * jet_jvt + 1.63229 * jet_jvt * jet_jvt;
        }else if(jet_pt<150){
          unc = 0.9998 -1.7319 * jet_jvt + 0.72680 * jet_jvt * jet_jvt;
        }
      }else if(jet_eta<2.7){
        if(jet_pt<30){
          if(jet_jvt<0.11)      unc = 0.3001 + 0.0054 * avgmu -0.00004 * avgmu * avgmu  ;
          else if(jet_jvt<0.25) unc = 0.0663 + 0.0198 * avgmu -0.00013 * avgmu * avgmu  ;
          else if(jet_jvt<0.85) unc = -0.0842 + 0.0163 * avgmu -0.00008 * avgmu * avgmu ;
          else if(jet_jvt<0.95) unc = -0.0219 + 0.0080 * avgmu + 0.00003 * avgmu * avgmu;
          else                  unc = 0.0461 -0.0003 * avgmu + 0.00012 * avgmu * avgmu  ;
        }else if(jet_pt<40){
          if(jet_jvt<0.11)      unc = 0.1885 + 0.0083 * avgmu -0.00006 * avgmu * avgmu ;
          else if(jet_jvt<0.25) unc = -0.0286 + 0.0150 * avgmu -0.00007 * avgmu * avgmu;
          else if(jet_jvt<0.85) unc = 0.0152 + 0.0028 * avgmu + 0.00005 * avgmu * avgmu;
          else if(jet_jvt<0.95) unc = 0.1815 -0.0076 * avgmu + 0.00018 * avgmu * avgmu ;
          else                  unc = 0.0192 -0.0003 * avgmu + 0.00007 * avgmu * avgmu ;
        }else if(jet_pt<50){
          if(jet_jvt<0.11)      unc = 0.1257 + 0.0074 * avgmu -0.00004 * avgmu * avgmu  ;
          else if(jet_jvt<0.25) unc = -0.0276 + 0.0080 * avgmu + 0.00000 * avgmu * avgmu;
          else if(jet_jvt<0.85) unc = 0.1403 -0.0051 * avgmu + 0.00009 * avgmu * avgmu  ;
          else if(jet_jvt<0.95) unc = 0.2078 -0.0101 * avgmu + 0.00017 * avgmu * avgmu  ;
          else                  unc = 0.2597 -0.0132 * avgmu + 0.00020 * avgmu * avgmu  ;
        }else if(jet_pt<60){
          if(jet_jvt<0.11)      unc = 0.1111 + 0.0045 * avgmu -0.00000 * avgmu * avgmu ;
          else if(jet_jvt<0.25) unc = 0.0975 -0.0011 * avgmu + 0.00008 * avgmu * avgmu ;
          else if(jet_jvt<0.85) unc = 0.0920 -0.0053 * avgmu + 0.00013 * avgmu * avgmu ;
          else                  unc = -0.0071 + 0.0016 * avgmu -0.00001 * avgmu * avgmu;
        }else if(jet_pt<100){
          unc = 0.4660 -1.2116 * jet_jvt + 0.78807 * jet_jvt * jet_jvt;
        }else if(jet_pt<150){
          unc = 0.2254 -0.5476 * jet_jvt + 0.32617 * jet_jvt * jet_jvt;
        }
      }// end eta 2.7
      else{//forward jets
        float fjvt = jet_fjvt>0.6 ? 0.6 : jet_fjvt; // the pileup more or less plateaus at 0.6
        if(jet_pt<30)       unc = 0.5106 + 1.2566 * fjvt -1.15060  * fjvt * fjvt;
        else if(jet_pt<40)  unc = 0.2972 + 1.9418 * fjvt -1.82694  * fjvt * fjvt;
        else if(jet_pt<50)  unc = 0.1543 + 1.9864 * fjvt -1.48429  * fjvt * fjvt;
        else if(jet_pt<60)  unc = 0.1050 + 1.3196 * fjvt + 0.03554 * fjvt * fjvt;
        else if(jet_pt<120) unc = 0.0400 + 0.5653 * fjvt + 1.96323 * fjvt * fjvt;
        // max of 0.9 seems reasonable
        if(jet_fjvt>0.6) unc = 0.9; 
      }
      // end emtopo
    }else{//p-flow inputs
      if(jet_eta<2.4){
        if(jet_pt<30){
          if(jet_jvt<0.11)      unc = 1;
          else if(jet_jvt<0.25) unc = 0.2494 + 0.0076 * avgmu -0.00001 * avgmu * avgmu ;
          else if(jet_jvt<0.85) unc = 0.0626 + 0.0037 * avgmu + 0.00004 * avgmu * avgmu;
          else if(jet_jvt<0.95) unc = 0.0192 + 0.0017 * avgmu + 0.00005 * avgmu * avgmu;
          else                  unc = 0.0147 -0.0003 * avgmu + 0.00004 * avgmu * avgmu ;
        }else if(jet_pt<40){
          if(jet_jvt<0.11)      unc = 1;
          else if(jet_jvt<0.25) unc = 0.1979 + 0.0034 * avgmu + 0.00003 * avgmu * avgmu;
          else if(jet_jvt<0.85) unc = 0.0731 -0.0022 * avgmu + 0.00009 * avgmu * avgmu ;
          else if(jet_jvt<0.95) unc = 0.0281 -0.0012 * avgmu + 0.00006 * avgmu * avgmu ;
          else                  unc = 0.0086 -0.0003 * avgmu + 0.00002 * avgmu * avgmu ;
        }else if(jet_pt<50){
          if(jet_jvt<0.11)      unc = 1;
          else if(jet_jvt<0.25) unc = 0.2242 -0.0010 * avgmu + 0.00006 * avgmu * avgmu  ;
          else if(jet_jvt<0.85) unc = 0.0568 -0.0019 * avgmu + 0.00006 * avgmu * avgmu  ;
          else if(jet_jvt<0.95) unc = -0.0050 + 0.0008 * avgmu + 0.00001 * avgmu * avgmu;
          else                  unc = 0.0037 -0.0000 * avgmu + 0.00000 * avgmu * avgmu  ;
        }else if(jet_pt<60){
          if(jet_jvt<0.11)      unc = 1;
          else if(jet_jvt<0.25) unc = 0.0027 + 0.0058 * avgmu -0.00001 * avgmu * avgmu  ;
          else if(jet_jvt<0.85) unc = -0.0143 + 0.0008 * avgmu + 0.00001 * avgmu * avgmu;
          else                  unc = -0.0012 + 0.0001 * avgmu + 0.00000 * avgmu * avgmu;
        }else if(jet_pt<100){          
          unc = 0.8558 -1.8519 * jet_jvt + 1.00208 * jet_jvt * jet_jvt;
        }else if(jet_pt<150){
          unc = 0.6474 -1.4491 * jet_jvt + 0.80591 * jet_jvt * jet_jvt;
        }
      }else if(jet_eta<2.6){
        if(jet_pt<30){
          if(jet_jvt<0.11)      unc = 0.2633 + 0.0091 * avgmu + -0.00009 * avgmu * avgmu;
          else if(jet_jvt<0.25) unc = 0.1841 + 0.0144 * avgmu + -0.00008 * avgmu * avgmu;
          else if(jet_jvt<0.85) unc = 0.1401 + 0.0048 * avgmu + 0.00006 * avgmu * avgmu ;
          else if(jet_jvt<0.95) unc = -0.0118 + 0.0076 * avgmu + 0.00003 * avgmu * avgmu;
          else                  unc = 0.0534 + -0.0011 * avgmu + 0.00010 * avgmu * avgmu;
        }else if(jet_pt<40){
          if(jet_jvt<0.11)      unc = 0.1497 + 0.0133 * avgmu + -0.00015 * avgmu * avgmu  ;
          else if(jet_jvt<0.25) unc = -0.2260 + 0.0276 * avgmu + -0.00021 * avgmu * avgmu ;
          else if(jet_jvt<0.85) unc = 0.2743 + -0.0093 * avgmu + 0.00022 * avgmu * avgmu  ;
          else if(jet_jvt<0.95) unc = 0.0604 + 0.0006 * avgmu + 0.00006 * avgmu * avgmu   ;
          else                  unc = 0.0478 + -0.0009 * avgmu + 0.00004 * avgmu * avgmu  ;
        }else if(jet_pt<50){
          if(jet_jvt<0.11)      unc = -0.2187 + 0.0317 * avgmu + -0.00037 * avgmu * avgmu ;
          else if(jet_jvt<0.25) unc = 0.0964 + 0.0053 * avgmu + 0.00002 * avgmu * avgmu   ;
          else if(jet_jvt<0.85) unc = 1.1730 + -0.0624 * avgmu + 0.00088 * avgmu * avgmu  ;
          else if(jet_jvt<0.95) unc = -0.2011 + 0.0151 * avgmu + -0.00018 * avgmu * avgmu ;
          else                  unc = 0.0145 + -0.0003 * avgmu + 0.00002 * avgmu * avgmu  ;
        }else if(jet_pt<60){
          if(jet_jvt<0.11)      unc = 0.0051 + 0.0113 * avgmu + -0.00008 * avgmu * avgmu  ;
          else if(jet_jvt<0.25) unc = -0.1024 + 0.0109 * avgmu + -0.00006 * avgmu * avgmu ;
          else if(jet_jvt<0.85) unc = 1.2491 + -0.0501 * avgmu + 0.00052 * avgmu * avgmu  ;
          else                  unc = 0.0267 + -0.0014 * avgmu + 0.00003 * avgmu * avgmu  ;
        }else if(jet_pt<100){
          unc = 0.8802 -1.6233 * jet_jvt + 0.74604 * jet_jvt * jet_jvt;
        }else if(jet_pt<150){
          unc = 0.9762 -2.4160 * jet_jvt + 1.45763 * jet_jvt * jet_jvt;
        }
      }else if(jet_eta<2.7){
        if(jet_pt<30){
          if(jet_jvt<0.11)      unc = 0.2877 + 0.0056 * avgmu -0.00004 * avgmu * avgmu;
          else if(jet_jvt<0.25) unc = 0.0353 + 0.0196 * avgmu -0.00012 * avgmu * avgmu;
          else if(jet_jvt<0.85) unc = -0.1616 + 0.0188 * avgmu -0.00009 * avgmu * avgmu;
          else if(jet_jvt<0.95) unc =  0.0373 + 0.0048 * avgmu + 0.00006 * avgmu * avgmu;
          else                  unc = 0.0666 -0.0007 * avgmu + 0.00013 * avgmu * avgmu;
        }else if(jet_pt<40){
          if(jet_jvt<0.11)      unc = 0.1331 + 0.0098 * avgmu -0.00007 * avgmu * avgmu;
          else if(jet_jvt<0.25) unc = 0.0570 + 0.0096 * avgmu  -0.00000 * avgmu * avgmu;
          else if(jet_jvt<0.85) unc = 0.2338  -0.0094 * avgmu + 0.00019 * avgmu * avgmu;
          else if(jet_jvt<0.95) unc = 0.2930  -0.0127 * avgmu + 0.00023 * avgmu * avgmu;
          else                  unc = 0.0152  -0.0003 * avgmu + 0.00007 * avgmu * avgmu;
        }else if(jet_pt<50){
          if(jet_jvt<0.11)      unc = 0.1582 + 0.0060 * avgmu  -0.00003 * avgmu * avgmu;
          else if(jet_jvt<0.25) unc = -0.0079 + 0.0057 * avgmu + 0.00003 * avgmu * avgmu;
          else if(jet_jvt<0.85) unc = 0.1865  -0.0081 * avgmu + 0.00013 * avgmu * avgmu;
          else if(jet_jvt<0.95) unc = 0.9103  -0.0405 * avgmu + 0.00049 * avgmu * avgmu;
          else                  unc = 0.1183  -0.0048 * avgmu + 0.00009 * avgmu * avgmu;
        }else if(jet_pt<60){
          if(jet_jvt<0.11)      unc = 0.0859 + 0.0047 * avgmu  -0.00000 * avgmu * avgmu;
          else if(jet_jvt<0.25) unc = 0.0249 + 0.0027 * avgmu + 0.00004 * avgmu * avgmu;
          else if(jet_jvt<0.85) unc = 0.1865  -0.0087 * avgmu + 0.00012 * avgmu * avgmu;
          else                  unc = 0.2069  -0.0087 * avgmu + 0.00011 * avgmu * avgmu;
        }else if(jet_pt<100){
          unc = 0.4281 -1.1109 * jet_jvt + 0.71551 * jet_jvt * jet_jvt;
        }else if(jet_pt<150){
          unc = 0.2033 -0.5162 * jet_jvt + 0.33810 * jet_jvt * jet_jvt;
        }
      }// end eta 2.7
      else{//forward jets
        float fjvt = jet_fjvt>0.6 ? 0.6 : jet_fjvt; // the pileup more or less plateaus at 0.6
        if(jet_pt<30)       unc = 0.5295 + 1.2467 * fjvt -1.13946  * fjvt * fjvt;
        else if(jet_pt<40)  unc = 0.3118 + 1.9951 * fjvt -1.86882  * fjvt * fjvt;
        else if(jet_pt<50)  unc = 0.1347 + 2.3884 * fjvt -1.96891  * fjvt * fjvt;
        else if(jet_pt<60)  unc = 0.0872 + 1.5718 * fjvt + 0.02135 * fjvt * fjvt;
        else if(jet_pt<120) unc = 0.0303 + 0.8560 * fjvt + 1.89537 * fjvt * fjvt;
        // max of 0.9 seems reasonable
        if(jet_fjvt>0.6) unc = 0.9; 
      }
    }// end pflow

    unc = std::min(unc, 1.0);
    unc = std::max(unc, 0.0);

    return unc;
  }

  double METSignificance::GetPhiUnc(double jet_eta, double jet_phi,double jet_pt){

    unsigned xbin=0, ybin=0;
    if(-4.5<jet_eta && -3.8>=jet_eta)      xbin=1;
    else if(-3.8<jet_eta && -3.5>=jet_eta) xbin=2;
    else if(-3.5<jet_eta && -3.0>=jet_eta) xbin=3;
    else if(-3.0<jet_eta && -2.7>=jet_eta) xbin=4;
    else if(-2.7<jet_eta && -2.4>=jet_eta) xbin=5;
    else if(-2.4<jet_eta && -1.5>=jet_eta) xbin=6;
    else if(-1.5<jet_eta && -0.5>=jet_eta) xbin=7;
    else if(-0.5<jet_eta &&  0.0>=jet_eta) xbin=8;
    else if(0.0<jet_eta  &&  0.5>=jet_eta) xbin=9;
    else if(0.5<jet_eta  &&  1.5>=jet_eta) xbin=10;
    else if(1.5<jet_eta  &&  2.4>=jet_eta) xbin=11;
    else if(2.4<jet_eta  &&  2.7>=jet_eta) xbin=12;
    else if(2.7<jet_eta  &&  3.0>=jet_eta) xbin=13;
    else if(3.0<jet_eta  &&  3.5>=jet_eta) xbin=14;
    else if(3.5<jet_eta  &&  3.8>=jet_eta) xbin=15;
    else if(3.8<jet_eta                  ) xbin=16;

    ybin = jet_phi>0.0 ? int(jet_phi/0.4)+9 : int(jet_phi/0.4)+8;

    // Stored as bin content = Mean, error = RMS, we want to use the RMS.
    if(!m_phi_reso_pt20 || !m_phi_reso_pt50 || !m_phi_reso_pt100){
      ATH_MSG_ERROR("Jet Phi Resolution histograms are invalid.");
      return 0.0;
    }

    // Collect the phi resolution
    if(jet_pt<50.0)
      return m_phi_reso_pt20->GetBinError(xbin, ybin);
    else if(jet_pt<100.0)
      return m_phi_reso_pt50->GetBinError(xbin, ybin);
    return m_phi_reso_pt100->GetBinError(xbin, ybin);
  }

  std::tuple<double,double,double> METSignificance::CovMatrixRotation(double var_x, double var_y, double cv_xy, double Phi){
    // Covariance matrix parallel and transverse to the Phi direction
    Double_t V11 = pow(cos(Phi),2)*var_x + 2*sin(Phi)*cos(Phi)*cv_xy + pow(sin(Phi),2)*var_y;
    Double_t V22 = pow(sin(Phi),2)*var_x - 2*sin(Phi)*cos(Phi)*cv_xy + pow(cos(Phi),2)*var_y;
    Double_t V12 = pow(cos(Phi),2)*cv_xy -sin(Phi)*cos(Phi)*var_x + sin(Phi)*cos(Phi)*var_y - pow(sin(Phi),2)*cv_xy;   // rho is equal to one for just one jet
    return  std::make_tuple( V11, V22, V12);
  }

  double METSignificance::Significance_LT(double Numerator, double var_parall, double var_perpen, double cov){

    Double_t rho = cov / sqrt( var_parall * var_perpen ) ;
    Double_t Significance = 0;
    if (fabs( rho ) >= 0.9 ){  //Cov Max not invertible -> Significance diverges
      ATH_MSG_VERBOSE("rho is large: " << rho);
      Significance = pow( Numerator - m_scalarBias , 2 ) / (  var_parall  ) ;
    }
    else
      Significance = pow( Numerator - m_scalarBias , 2 ) / (  var_parall * ( 1 - pow(rho,2) ) ) ;

    if( fabs(Significance) >= 10e+15)
      ATH_MSG_WARNING("warning -->"<< Significance);

    return Significance;
  }

  void METSignificance::InvertMatrix(double (&mat)[2][2], double (&m)[2][2]){

    // determinant
    double det = mat[0][0]*mat[1][1]-mat[0][1]*mat[1][0];

    m[0][0]=0.0;
    m[0][1]=0.0;
    m[1][0]=0.0;
    m[1][1]=0.0;

    if(det==0.0) return;

    m[0][0]= 1.0/det*(mat[1][1]);
    m[1][0]=-1.0/det*(mat[1][0]);
    m[0][1]=-1.0/det*(mat[0][1]);
    m[1][1]= 1.0/det*(mat[0][0]);
  }

  void METSignificance::AddMatrix(double (&X)[2][2],double (&Y)[2][2], double (&mat_new)[2][2]){
    mat_new[0][0]=X[0][0]+Y[0][0];
    mat_new[0][1]=X[0][1]+Y[0][1];
    mat_new[1][0]=X[1][0]+Y[1][0];
    mat_new[1][1]=X[1][1]+Y[1][1];
  }

  void METSignificance::RotateXY(const double (&mat)[2][2], double (&mat_new)[2][2], double phi){

    double c = cos(phi);
    double s = sin(phi);
    double cc = c*c;
    double ss = s*s;
    double cs = c*s;

    double V11 = mat[0][0]*cc + mat[1][1]*ss - cs*(mat[1][0] + mat[0][1]);
    double V12 = mat[0][1]*cc - mat[1][0]*ss + cs*(mat[0][0] - mat[1][1]);
    double V21 = mat[1][0]*cc - mat[0][1]*ss + cs*(mat[0][0] - mat[1][1]);
    double V22 = mat[0][0]*ss + mat[1][1]*cc + cs*(mat[1][0] + mat[0][1]);

    mat_new[0][0]=V11;
    mat_new[0][1]=V12;
    mat_new[1][0]=V21;
    mat_new[1][1]=V22;
  }

  /// Parameterization with PtSoft Direction //
  double METSignificance::BiasPtSoftdir(const double PtSoft){
    if (PtSoft<60.) return (0.145)+(-0.45)*PtSoft;
    else return (0.145)+(-0.45)*(60.);
  }

  // variation in ptsoft direction
  double METSignificance::VarparPtSoftdir(const double PtSoft, const double SoftSumet){
    if (SoftSumet<25){
      if (PtSoft<50.) return 41.9+3.8*PtSoft+0.1*pow(PtSoft,2)-12.7+ 1.39*SoftSumet-0.03*pow(SoftSumet,2);
      else return 41.9+3.8*50.+0.1*pow(50.,2)-12.7+ 1.39*SoftSumet-0.03*pow(SoftSumet,2);
    }
    else{
      if (PtSoft<50.) return 41.9+3.8*PtSoft+0.1*pow(PtSoft,2);
      else return (40.5614)+(4.10965)*50.+(0.0955044)*pow(50.,2);
    }
  }

  double METSignificance::Var_Ptsoft(const double PtSoft){
    if (PtSoft<45.) return 40. + 2*PtSoft + 0.1*pow(PtSoft,2);
    else return 40. + 2*45 + 0.1*pow(45,2);
  }

  double METSignificance::Bias_PtSoftParall(const double PtSoft_Parall)
  {
    if (-60.<=PtSoft_Parall && PtSoft_Parall<0.) return -8. -0.4*PtSoft_Parall;
    if (-60.>PtSoft_Parall) return -8. -0.4 * (-60.);
    if( PtSoft_Parall>=0. && PtSoft_Parall<60.) return -8. -PtSoft_Parall;
    if(PtSoft_Parall>60.) return -8. -60.;
    return 0.0;
  }

  void METSignificance::AddResoMap(const double varL, const double varT, const double CvLT, const int term){

    if(m_term_VarL.find(term)==m_term_VarL.end()){
      m_term_VarL[term] = 0.0;
      m_term_VarT[term] = 0.0;
      m_term_CvLT[term] = 0.0;
    }

    m_term_VarL[term] += varL;
    m_term_VarT[term] += varT;
    m_term_CvLT[term] += CvLT;
    
  }

} //> end namespace met
