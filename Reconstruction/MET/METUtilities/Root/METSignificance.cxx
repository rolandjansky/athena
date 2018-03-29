///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "xAODMissingET/MissingETAssociationMap.h"

// Jet EDM
#include "xAODJet/JetAttributes.h"

// Tracking EDM
#include "xAODTracking/TrackParticle.h"

// Shallow copy
#include "xAODCore/ShallowCopy.h"

// Muon EDM
#include "xAODMuon/MuonContainer.h"

// Tools
#include "JetResolution/JERTool.h"
#include "JetResolution/IJERTool.h"
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"
#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"
#include "EgammaAnalysisInterfaces/IEgammaCalibrationAndSmearingTool.h"
#include "tauRecTools/ITauToolBase.h"
#include "tauRecTools/CombinedP4FromRecoTaus.h"
#include "PathResolver/PathResolver.h"

namespace met {

    using std::vector;

    using xAOD::MissingET;
    using xAOD::MissingETContainer;
    using xAOD::MissingETAssociation;
    using xAOD::MissingETAssociationMap;
    using xAOD::MissingETAuxContainer;
    using xAOD::MissingETComposition;
    //
    using xAOD::IParticle;
    using xAOD::IParticleContainer;
    //
    using xAOD::JetContainer;
    using xAOD::JetConstituentVector;
    //
    using xAOD::TrackParticle;

    typedef ElementLink<xAOD::IParticleContainer> iplink_t;


  //static const SG::AuxElement::ConstAccessor<float> acc_varX("varX");
  //static const SG::AuxElement::ConstAccessor<float> acc_varY("varY");
  //static const SG::AuxElement::ConstAccessor<float> acc_covXY("covXY");
    static SG::AuxElement::Accessor<float> acc_varX("varX");
    static SG::AuxElement::Accessor<float> acc_varY("varY");
    static SG::AuxElement::Accessor<float> acc_covXY("covXY");
    static SG::AuxElement::Accessor<float> acc_jvt("Jvt");

    static const SG::AuxElement::Decorator< std::vector<iplink_t > > dec_constitObjLinks("ConstitObjectLinks");

    const static MissingETBase::Types::bitmask_t invisSource = 0x100000; // doesn't overlap with any other

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

    // Constructors
    ////////////////
    METSignificance::METSignificance(const std::string& name) :
      AsgTool(name),
      m_jerTool(""),
      m_muonCalibrationAndSmearingTool(""),
      m_egammaCalibTool(""),
      m_tCombinedP4FromRecoTaus(""),
      m_GeV(1.0e3),
      m_softTermParam(met::Random),
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
      h_phi_reso_pt20(0),
      h_phi_reso_pt50(0),
      h_phi_reso_pt100(0)
    {
      //
      // Property declaration
      //
      declareProperty("SoftTermParam",        m_softTermParam = met::Random );
      declareProperty("SoftTermReso",         m_softTermReso  = 10.0        );
      declareProperty("TreatPUJets",          m_treatPUJets   = true        );
      declareProperty("DoPhiReso",            m_doPhiReso     = false       );
      declareProperty("ApplyBias",            m_applyBias     = false       );
      declareProperty("ScalarBias",           m_scalarBias    = 0.0         );
      declareProperty("ConfigPrefix",         m_configPrefix  = "METUtilities/data17_13TeV/metsig_Aug15/");
      declareProperty("ConfigJetPhiResoFile", m_configJetPhiResoFile  = "jet_unc.root" );

      // properties to delete eventually
      declareProperty("IsData",      m_isData     = false   );
      declareProperty("IsAFII",      m_isAFII     = false   );

      // Phi resolution
      std::string configpath  = PathResolverFindCalibFile(m_configPrefix+m_configJetPhiResoFile);
      m_file = TFile::Open(configpath.c_str());
      if(m_file){
	h_phi_reso_pt20 = static_cast<TH2F *>(m_file->Get("phi_reso_pt20"));
	if(!h_phi_reso_pt20) ATH_MSG_ERROR("PU Jet Uncertainty Histogram not valid");
	h_phi_reso_pt50 = static_cast<TH2F *>(m_file->Get("phi_reso_pt50"));
	if(!h_phi_reso_pt50) ATH_MSG_ERROR("PU Jet Uncertainty Histogram not valid");
	h_phi_reso_pt100 = static_cast<TH2F *>(m_file->Get("phi_reso_pt100"));
	if(!h_phi_reso_pt100) ATH_MSG_ERROR("PU Jet Uncertainty Histogram not valid");
      } else{
	ATH_MSG_ERROR("PU Jet Uncertainty TFile is not valid: " << configpath);
      }
    }

    // Destructor
    ///////////////
    METSignificance::~METSignificance()
    {
    }

    // Athena algtool's Hooks
    ////////////////////////////
    StatusCode METSignificance::initialize()
    {
        ATH_MSG_INFO ("Initializing " << name() << "...");


	ATH_MSG_INFO("Set up JER tools");
	std::string toolName;
	std::string jetcoll = "AntiKt4EMTopoJets";
	toolName = "JERTool_" + jetcoll;

	m_jerTool.setTypeAndName("JERTool/STAutoConf_"+toolName);
	ATH_CHECK(m_jerTool.setProperty("PlotFileName", "JetResolution/Prerec2015_xCalib_2012JER_ReducedTo9NP_Plots_v2.root"));
	ATH_CHECK(m_jerTool.setProperty("CollectionName", jetcoll));

	ATH_MSG_INFO("Set up MuonCalibrationAndSmearing tools");
	toolName = "MuonCalibrationAndSmearingTool";
	m_muonCalibrationAndSmearingTool.setTypeAndName("CP::MuonCalibrationAndSmearingTool/STAutoConf_"+toolName);

	ATH_MSG_DEBUG( "Initialising EgcalibTool " );
	toolName = "EgammaCalibrationAndSmearingTool";
	m_egammaCalibTool.setTypeAndName("CP::EgammaCalibrationAndSmearingTool/STAutoConf_" + toolName);
	ATH_CHECK(m_egammaCalibTool.setProperty("ESModel", "es2017_R21_PRE"));
	ATH_CHECK(m_egammaCalibTool.setProperty("decorrelationModel", "1NP_v1"));
	if(m_isAFII) ATH_CHECK(m_egammaCalibTool.setProperty("useAFII", 1));
	else ATH_CHECK(m_egammaCalibTool.setProperty("useAFII", 0));

	toolName = "TauPerfTool";
	m_tCombinedP4FromRecoTaus.setTypeAndName("CombinedP4FromRecoTaus/STAutoConf_" + toolName);
	ATH_CHECK(m_tCombinedP4FromRecoTaus.setProperty("WeightFileName", "CalibLoopResult_v04-04.root"));

        return StatusCode::SUCCESS;
    }

    StatusCode METSignificance::finalize()
    {
        ATH_MSG_INFO ("Finalizing " << name() << "...");
	delete h_phi_reso_pt20;
	delete h_phi_reso_pt50;
	delete h_phi_reso_pt100;

        return StatusCode::SUCCESS;
    }


    // **** Rebuild generic MET term ****

  StatusCode METSignificance::varianceMET(xAOD::MissingETContainer* metCont, std::string jetTermName, std::string softTermName, std::string totalMETName)
    {

      // reset variables
      m_VarL = 0.0;
      m_VarT = 0.0;
      m_CvLT = 0.0;

      double particle_sum[2][2] = {{0.0,0.0},
				   {0.0,0.0}};
      m_metphi = 0.0; //Angle for rotation of the cov matrix
      m_met = -1.0; // Numerator
      m_metsoft = 0.0;
      m_metsoftphi = 0.0;
      m_sumet=-1.0;
      m_ht=0.0;
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
	m_metphi = tot_met->phi();
	m_sumet  = tot_met->sumet()/m_GeV;
	m_ht     = m_sumet;
	ATH_MSG_VERBOSE("total MET: " << m_met << " phi: " << m_metphi << " name: " << tot_met->name());

      }else{
	ATH_MSG_ERROR("Could not find the total MET with name:" <<totalMETName);
	return StatusCode::SUCCESS;
      }
      m_met_vect.SetPtEtaPhi(m_met, 0.0, m_metphi);

      // Fill the remaining terms
      for(const auto& met : *metCont) {

	// skip the invisible and total MET
	if(MissingETBase::Source::isTotalTerm(met->source())){
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
	  // done with the soft term. go to the next term.
	  continue;
	}
	ATH_MSG_VERBOSE("Add MET term " << met->name() );
	for(const auto& el : dec_constitObjLinks(*met)) {
	  const IParticle* obj(*el);
	  float pt_reso=0.0, phi_reso=0.0;
	  if(obj->type()==xAOD::Type::Muon){
	    ATH_CHECK(AddMuon(obj, pt_reso, phi_reso));
	  }else if(obj->type()==xAOD::Type::Jet){

	    // make sure the container name matches
	    if(met->name()!=jetTermName) continue;

	    AddJet(obj, pt_reso, phi_reso);

	  }else if(obj->type()==xAOD::Type::Electron){
	    AddElectron(obj, pt_reso, phi_reso);
	  }else if(obj->type()==xAOD::Type::Photon){
	    AddPhoton(obj, pt_reso, phi_reso);
	  }else if(obj->type()==xAOD::Type::Tau){
	    AddTau(obj, pt_reso, phi_reso);
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

	  RotateXY (particle_u,   particle_u_rot, obj->p4().Phi()); // positive phi rotation
	  AddMatrix(particle_sum, particle_u_rot, particle_sum);

	  // END compute NEW


	  ATH_MSG_VERBOSE("Resolution: " << pt_reso << " phi reso: " << phi_reso );

	  /*
	    if(acc_varX.isAvailable(*obj) && acc_varY.isAvailable(*obj) && acc_covXY.isAvailable(*obj)) {
	    ATH_MSG_VERBOSE("Add object with vars " << acc_varX(*obj)<<","<<acc_varY(*obj)<<","<<acc_covXY(*obj));
	    varx+=acc_varX(*obj);
	    vary+=acc_varY(*obj);
	    covxy+=acc_covXY(*obj);
	    //met->add(acc_varX(*el),acc_varY(*el),acc_covXY(*el));
	    }*/
	}
      }

      /*
      //
      // Compute the significance
      //     Multiply S = E . V-1. E
      //
      double particle_sum_invert[2][2] = {{0.0,0.0},
      {0.0,0.0}};

      InvertMatrix(particle_sum, particle_sum_invert);

      double MET[2]={0.0,0.0};
      MET[0]=met_vect.Px();
      MET[1]=met_vect.Py();

      double V1 = particle_sum_invert[0][0]*MET[0] + particle_sum_invert[1][0]*MET[1];
      double V2 = particle_sum_invert[0][1]*MET[0] + particle_sum_invert[1][1]*MET[1];
      double tmp_met_sig = V1*MET[0] + V2*MET[1];
      */

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

	  }else if(m_softTermParam==met::PthardParam){
	    m_soft_vect.SetPtEtaPhi(m_metsoft, 0.0, m_metsoftphi);
	    m_pthard_vect = m_soft_vect - m_met_vect;
	    Double_t PtSoftparaPH = m_pthard_vect.Mag()>0.0 ? (m_soft_vect.Dot(m_pthard_vect))/m_pthard_vect.Mag() : 0.0;
	    Double_t Bias_pthard = Bias_PtSoftParall(PtSoftparaPH);
	    Double_t MEx = m_met * cos(m_metphi) - Bias_pthard * cos(m_metsoftphi);
	    Double_t MEy = m_met * sin(m_metphi) - Bias_pthard * sin(m_metsoftphi);
	    met_vect.SetXYZ(MEx,MEy,0.0);
	  }
	  // Rotate  & compute
	  RotateToPhi(met_vect.Phi());
	  m_significance = Significance_LT(met_vect.Pt(), m_VarL, m_VarT, m_CvLT);
	  m_rho = m_CvLT / sqrt( m_VarL * m_VarT ) ;
	}else{
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
      else{
	ATH_MSG_DEBUG("Var_L is 0");
      }
      return StatusCode::SUCCESS;
    }


  // Add access to rotate the direction of the MET resolution to a new phi position
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

  ///////////////////////////////////////////////////////////////////
  //  Private methods
  ///////////////////////////////////////////////////////////////////
  //
  // Muon propagation of resolution
  //
  StatusCode METSignificance::AddMuon(const xAOD::IParticle* obj, float &pt_reso, float &phi_reso){
    const xAOD::Muon* muon(static_cast<const xAOD::Muon*>(obj));

    std::string dettype = "";
    if(muon->muonType()==0){//Combined
      dettype="CB";//CB
    }
    else if(muon->muonType()==1){//MuonStandAlone
      dettype="MS";//MS
    }
    else if(muon->muonType()>1){//Segment, Calo, Silicon
      dettype="ID";//ID
    }
    else{
      ATH_MSG_VERBOSE("This muon had none of the normal muon types (ID,MS,CB) - check this in detail");
      return StatusCode::FAILURE;
    }

    xAOD::Muon *my_muon = NULL;
    ATH_CHECK(m_muonCalibrationAndSmearingTool->correctedCopy(*muon,my_muon)==CP::CorrectionCode::Ok);
    pt_reso=m_muonCalibrationAndSmearingTool->expectedResolution(dettype,*my_muon,!m_isData);
    if(m_doPhiReso) phi_reso = my_muon->pt()*0.001;
    ATH_MSG_VERBOSE("muon: " << pt_reso << " dettype: " << dettype << " " << muon->pt() << " " << muon->p4().Eta() << " " << muon->p4().Phi());
    delete my_muon;

    return StatusCode::SUCCESS;
  }

  //
  // Electron propagation of resolution
  //
  void METSignificance::AddElectron(const xAOD::IParticle* obj, float &pt_reso, float &phi_reso){

    const xAOD::Electron* ele(static_cast<const xAOD::Electron*>(obj));
    const auto cl_etaCalo = xAOD::get_eta_calo(*(ele->caloCluster()), ele->author());
    pt_reso=m_egammaCalibTool->resolution(ele->e(),ele->caloCluster()->eta(),cl_etaCalo,PATCore::ParticleType::Electron);
    if(m_doPhiReso) phi_reso = ele->pt()*0.004;
    ATH_MSG_VERBOSE("el: " << pt_reso << " " << ele->pt() << " " << ele->p4().Eta() << " " << ele->p4().Phi());

  }

  //
  // Photon propagation of resolution
  //
  void METSignificance::AddPhoton(const xAOD::IParticle* obj, float &pt_reso, float &phi_reso){

    // photons
    const xAOD::Egamma* pho(static_cast<const xAOD::Egamma*>(obj));
    pt_reso=m_egammaCalibTool->getResolution(*pho);
    if(m_doPhiReso) phi_reso = pho->pt()*0.004;
    ATH_MSG_VERBOSE("pho: " << pt_reso << " " << pho->pt() << " " << pho->p4().Eta() << " " << pho->p4().Phi());
  }

  //
  // Jet propagation of resolution
  //
  void METSignificance::AddJet(const xAOD::IParticle* obj, float &pt_reso, float &phi_reso){

    const xAOD::Jet* jet(static_cast<const xAOD::Jet*>(obj));
    if(m_isData) pt_reso = m_jerTool->getRelResolutionData(jet);
    else         pt_reso = m_jerTool->getRelResolutionMC(jet);
    ATH_MSG_VERBOSE("jet: " << pt_reso  << " jetpT: " << jet->pt() << " " << jet->p4().Eta() << " " << jet->p4().Phi());
    //
    // Add extra uncertainty for PU jets based on JVT
    //
    if(m_treatPUJets){
      double jet_pu_unc  = GetPUProb(jet->eta(), jet->phi(),jet->pt()/m_GeV, acc_jvt(*jet))/4.0;
      pt_reso = sqrt(jet_pu_unc*jet_pu_unc + pt_reso*pt_reso);
      ATH_MSG_VERBOSE("jet_pu_unc: " << jet_pu_unc);
    }
    //
    // Use the phi resolution of the jets
    // needs to be finished
    //
    if(m_doPhiReso){
      double jet_phi_unc = fabs(GetPhiUnc(jet->eta(), jet->phi(),jet->pt()/m_GeV));
      phi_reso = jet->pt()*jet_phi_unc;
    }
  }

  //
  // Tau propagation of resolution
  //
  void METSignificance::AddTau(const xAOD::IParticle* obj, float &pt_reso, float &phi_reso){

    // tau objects
    const xAOD::TauJet* tau(static_cast<const xAOD::TauJet*>(obj));
    pt_reso = dynamic_cast<CombinedP4FromRecoTaus*>(m_tCombinedP4FromRecoTaus.get())->GetCaloResolution(tau);
    //for taus, this is not a relative resolution. so we divide by pT
    pt_reso /=tau->pt();
    if(m_doPhiReso) phi_reso = tau->pt()*0.01;
    ATH_MSG_VERBOSE("tau: " << pt_reso << " " << tau->pt() << " " << tau->p4().Eta() << " " << tau->p4().Phi() << " phi reso: " << phi_reso);
  }

  //
  // Soft term propagation of resolution
  //
  void METSignificance::AddSoftTerm(const xAOD::MissingET* soft, const TVector3 &met_vect, double (&particle_sum)[2][2]){

    if(m_softTermParam==met::Random){

      ATH_MSG_VERBOSE("Resolution Soft term set to 10GeV");

      m_soft_vect.SetPtEtaPhi(soft->met()/m_GeV, 0.0, soft->phi());

      double particle_u[2][2] = {{m_softTermReso*m_softTermReso,0.0},
				 {0.0,m_softTermReso*m_softTermReso}};
      double particle_u_rot[2][2] = {{m_softTermReso*m_softTermReso,0.0},
				     {0.0,m_softTermReso*m_softTermReso}};

      RotateXY(particle_u, particle_u_rot,met_vect.DeltaPhi(m_soft_vect));
      m_VarL+=particle_u_rot[0][0];
      m_VarT+=particle_u_rot[1][1];
      m_CvLT+=particle_u_rot[0][1];

      RotateXY (particle_u,   particle_u_rot,-1.0*soft->phi()); // negative phi rotation
      AddMatrix(particle_sum, particle_u_rot,     particle_sum);

      ATH_MSG_VERBOSE("SOFT " << soft->name() <<" - pt_reso: " << m_softTermReso << " soft: " << soft->met() << " phi: " << soft->phi()
		      << " Var_L: " << particle_u_rot[0][0] << " Var_T: " << particle_u_rot[1][1]
		      << " " << particle_u_rot[0][1]);

    }else if (m_softTermParam==met::PthardParam){

      ATH_MSG_VERBOSE("Resolution Soft term parameterized in pthard direction");

      m_soft_vect.SetPtEtaPhi(soft->met()/m_GeV, 0.0, soft->phi());

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

      RotateXY (particle_u,   particle_u_rot,-1.0*m_pthard_vect.Phi()); // negative phi rotation
      AddMatrix(particle_sum, particle_u_rot,     particle_sum);

    }else if (m_softTermParam==met::TSTParam){

      ATH_MSG_VERBOSE("Resolution Soft term parameterized in TST");

      m_soft_vect.SetPtEtaPhi(soft->met()/m_GeV, 0.0, soft->phi());

      double varTST = VarparPtSoftdir(soft->met()/m_GeV, soft->sumet()/m_GeV);

      double particle_u[2][2] = {{varTST,0.0},
				 {0.0,varTST}};
      double particle_u_rot[2][2] = {{varTST,0.0},
				     {0.0,varTST}};

      RotateXY(particle_u, particle_u_rot,met_vect.DeltaPhi(m_soft_vect));
      m_VarL+=particle_u_rot[0][0];
      m_VarT+=particle_u_rot[1][1];
      m_CvLT+=particle_u_rot[0][1];

      RotateXY (particle_u,   particle_u_rot,-1.0*soft->phi()); // negative phi rotation
      AddMatrix(particle_sum, particle_u_rot,     particle_sum);

    }else{
      ATH_MSG_ERROR("Soft term parameterization is NOT defined for:" << m_softTermParam);
    }

  }

    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////
  double METSignificance::GetPUProb(double jet_eta, double /*jet_phi*/,
				    double jet_pt,  double jet_jvt) {

    double unc=0.0;
    if(jet_jvt<0.05 && fabs(jet_eta)<2.7 && jet_pt<150.0e3){
      unc=0.95;
    }else if(jet_jvt<0.59 && fabs(jet_eta)<2.7 && jet_pt<100.0e3){
      unc=0.4;
    }else if(jet_jvt<0.59 && fabs(jet_eta)<2.7 && jet_pt<100.0e3){
      unc=0.4;
    }else if(jet_pt<30.0e3 && fabs(jet_eta)>2.7){
      unc=0.2;
    }else if(jet_pt<40.0e3 && fabs(jet_eta)>2.7){
      unc=0.07;
    }
    return unc;
    /*
    //  etaBins = [-4.5,-3.8,-3.5,-3.0,-2.7,-2.4,-1.5,-0.5,0.0,
    //           0.5,1.5,2.4,2.7,3.0,3.5,3.8,4.5]
    //pTBins = [0, 20.0, 30.0, 40.0, 60.0, 100.0, 150.0, 200.0]
    unsigned xbin=0, ybin=0, zbin=0;
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

    ybin = jet_phi>0.0? int(jet_phi/0.4)+9:int(jet_phi/0.4)+8;
    if(0.0<jet_pt && 20.0>=jet_pt)         zbin=1;
    else if(20.0<jet_pt  && 30.0>=jet_pt)  zbin=2;
    else if(30.0<jet_pt  && 40.0>=jet_pt)  zbin=3;
    else if(40.0<jet_pt  && 60.0>=jet_pt)  zbin=4;
    else if(60.0<jet_pt  && 100.0>=jet_pt) zbin=5;
    else if(100.0<jet_pt && 150.0>=jet_pt) zbin=6;
    else if(150.0<jet_pt)                  zbin=7;
    */
    /*
    if(jet_jvt<0.05)
      return h_pu_prob_Jvt05->GetBinContent(xbin, ybin, zbin);
    else if(jet_jvt<0.59)
      return h_pu_prob_Jvt59->GetBinContent(xbin, ybin, zbin);
    return h_pu_prob->GetBinContent(xbin, ybin, zbin);
    */
    return 0.0;
  }


  double METSignificance::GetPhiUnc(double jet_eta, double jet_phi,double jet_pt)
  {

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

    ybin = jet_phi>0.0? int(jet_phi/0.4)+9:int(jet_phi/0.4)+8;

    // Stored as bin content = Mean, error = RMS, we want to use the RMS.
    if(!h_phi_reso_pt20 || !h_phi_reso_pt50 || !h_phi_reso_pt100){
      ATH_MSG_ERROR("Jet Phi Resolution histograms are invalid.");
      return 0.0;
    }

    // collect the phi resolution
    if(jet_pt<50.0)
      return h_phi_reso_pt20->GetBinContent(xbin, ybin);
    else if(jet_pt<100.0)
      return h_phi_reso_pt50->GetBinContent(xbin, ybin);
    return h_phi_reso_pt100->GetBinContent(xbin, ybin);

    return 0.0;
  }

  std::tuple<double,double,double> METSignificance::CovMatrixRotation(double var_x, double var_y, double cv_xy, double Phi)
  {
    //Covariance matrix parallel and transverse to the Phi direction
    Double_t V11 = pow(cos(Phi),2)*var_x + 2*sin(Phi)*cos(Phi)*cv_xy + pow(sin(Phi),2)*var_y;
    Double_t V22 = pow(sin(Phi),2)*var_x - 2*sin(Phi)*cos(Phi)*cv_xy + pow(cos(Phi),2)*var_y;
    Double_t V12 = pow(cos(Phi),2)*cv_xy -sin(Phi)*cos(Phi)*var_x + sin(Phi)*cos(Phi)*var_y - pow(sin(Phi),2)*cv_xy;   // rho is equal to one for just one jet
    return  std::make_tuple( V11, V22, V12);
  }

  double METSignificance::Significance_LT(double Numerator, double var_parall, double var_perpen, double cov)
  {

    Double_t rho = cov / sqrt( var_parall * var_perpen ) ;
    Double_t Significance = 0;
    if (fabs( rho ) >= 0.9 )  //Cov Max not invertible -> Significance diverges
      {
	ATH_MSG_VERBOSE("rho is large: " << rho);
	Significance = pow( Numerator - m_scalarBias , 2 ) / (  var_parall  ) ;
      }
    else
    {
      Significance = pow( Numerator - m_scalarBias , 2 ) / (  var_parall * ( 1 - pow(rho,2) ) ) ;
    }

    if( fabs(Significance) >= 10e+15)
      {
	ATH_MSG_WARNING("warning -->"<< Significance);
      }

    return Significance;
  }

    ///////////////////////////////////////////////////////////////////
    // Non-const methods:
    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    // Protected methods:
    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    // Non-const methods:
    ///////////////////////////////////////////////////////////////////
  void METSignificance::InvertMatrix(double (&mat)[2][2], double (&m)[2][2])
  {
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
  void METSignificance::AddMatrix(double (&X)[2][2],double (&Y)[2][2], double (&mat_new)[2][2])
  {
    //double mat_new[2][2] = {0.0,0.0,
    //			 0.0,0.0};
    mat_new[0][0]=X[0][0]+Y[0][0];
    mat_new[0][1]=X[0][1]+Y[0][1];
    mat_new[1][0]=X[1][0]+Y[1][0];
    mat_new[1][1]=X[1][1]+Y[1][1];
  }

  void METSignificance::RotateXY(const double (&mat)[2][2], double (&mat_new)[2][2], double phi)
  {
    //double mat_new[2][2]={V11,V12,
    //                      V21,V22};
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
  double METSignificance::BiasPtSoftdir(const double PtSoft) //DeltaMet_Parall-(0.14-0.45*TST_Soft
  {
    if (PtSoft<60.) return (0.145)+(-0.45)*PtSoft;
    else return (0.145)+(-0.45)*(60.);
  }

  // variation in ptsoft direction
  double METSignificance::VarparPtSoftdir(const double PtSoft, const double SoftSumet) //41.9+3.8*TST_Soft+0.1*pow(TST_Soft,2)+(-12.7+ 1.39*SoftSumet-0.03*pow(SoftSumet,2))*(SoftSumet<25)
  {
    if (SoftSumet<25){
      if (PtSoft<50.) return 41.9+3.8*PtSoft+0.1*pow(PtSoft,2)-12.7+ 1.39*SoftSumet-0.03*pow(SoftSumet,2);
      else return 41.9+3.8*50.+0.1*pow(50.,2)-12.7+ 1.39*SoftSumet-0.03*pow(SoftSumet,2);
    }
    else{
      if (PtSoft<50.) return 41.9+3.8*PtSoft+0.1*pow(PtSoft,2);
      else return (40.5614)+(4.10965)*50.+(0.0955044)*pow(50.,2);
    }
  }

  double METSignificance::Var_Ptsoft(const double PtSoft)//40. + 2*TST_Soft + 0.1*pow(TST_Soft,2)
  {
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


} //> end namespace met
