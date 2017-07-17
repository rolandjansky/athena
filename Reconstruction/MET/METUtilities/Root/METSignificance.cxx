///////////////////////// -*- C++ -*- /////////////////////////////
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
#include "MuonMomentumCorrections/IMuonCalibrationAndSmearingTool.h"
#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"
#include "ElectronPhotonFourMomentumCorrection/IEgammaCalibrationAndSmearingTool.h"
#include "tauRecTools/ITauToolBase.h"
#include "tauRecTools/CombinedP4FromRecoTaus.h"

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
    static SG::AuxElement::Accessor<float>  acc_jvt("Jvt");

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
      m_significance(0.0),
      m_rho(0.0),
      m_VarL(0.0),
      m_VarT(0.0),
      m_met(0.0),
      m_metphi(0.0),
      m_ht(0.0),
      m_sumet(0.0)
    {
        //
        // Property declaration
        //
        declareProperty("SoftTermParam",  m_softTermParam = 0       );
        declareProperty("SoftTermReso",   m_softTermReso  = 10.0    );
        declareProperty("TreatPUJets",    m_treatPUJets   = true    );
        declareProperty("DoPhiReso",      m_doPhiReso     = false   );

	// properties to delete eventually
        declareProperty("IsData",      m_isData     = false   );
        declareProperty("IsAFII",      m_isAFII     = false   );

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
	ATH_CHECK(m_egammaCalibTool.setProperty("ESModel", "es2016PRE"));
	ATH_CHECK(m_egammaCalibTool.setProperty("decorrelationModel", "1NP_v1"));
	if(m_isAFII) ATH_CHECK(m_egammaCalibTool.setProperty("useAFII", m_isAFII));

	toolName = "TauPerfTool";
	m_tCombinedP4FromRecoTaus.setTypeAndName("CombinedP4FromRecoTaus/STAutoConf_" + toolName);
	ATH_CHECK(m_tCombinedP4FromRecoTaus.setProperty("WeightFileName", "CalibLoopResult.root"));

        return StatusCode::SUCCESS;
    }
    
    StatusCode METSignificance::finalize()
    {
        ATH_MSG_INFO ("Finalizing " << name() << "...");
        
        return StatusCode::SUCCESS;
    }
    
    
    // **** Rebuild generic MET term ****
    
    StatusCode METSignificance::varianceMET(xAOD::MissingETContainer* metCont)
    {
        float varx=0;
        float vary=0;
        float covxy=0;
	m_metphi = 0.0; //Angle for rotation of the cov matrix
	m_met = -1.0; // Numerator
	m_sumet=-1.0;
	m_ht=0.0;
	double Var_L_j=0.0, Var_T_j=0.0, Var_L=0.0, Var_T=0.0, Cv_LT_j=0.0, Cv_LT=0.0;
        for(const auto& met : *metCont) {
	  // fill met total terms
	  if(MissingETBase::Source::isTotalTerm(met->source())){
	    m_met = met->met()/m_GeV;
	    m_metphi = met->phi();
	    m_sumet = met->sumet()/m_GeV;
	    m_ht   += m_sumet;
	    continue;
	  }
            if(met->source()==invisSource) continue;
	    // Soft term collection
            if(MissingETBase::Source::isSoftTerm(met->source())){
	      m_ht-=(met->sumet()/m_GeV);
	      if(m_softTermParam==0){
		
		ATH_MSG_VERBOSE("Resolution Soft term set to 10GeV");
		varx = m_softTermReso*m_softTermReso;
		vary = varx; covxy = varx;
		// Rotation (components)
		std::tie(Var_L, Var_T, Cv_LT) = CovMatrixRotation(varx , vary, covxy, m_metphi);
		//Covariance matrix rotated for all jets
		Var_L_j += Var_L;
		Var_T_j += Var_T;
		Cv_LT_j += Cv_LT;
		
	      }else{
		ATH_MSG_ERROR("Soft term parameterization is NOT defined for:" << m_softTermParam);
	      }
	      continue;
	    }
            ATH_MSG_VERBOSE("Add MET term " << met->name() );
            for(const auto& el : dec_constitObjLinks(*met)) {
                const IParticle* obj(*el);
		float pt_reso=0.0;
		if(obj->type()==xAOD::Type::Muon){
		  const xAOD::Muon* muon(static_cast<const xAOD::Muon*>(obj)); 

		  int dettype=-1;
		  if(muon->muonType()==0){//Combined
		    dettype=3;//CB
		  }
		  if(muon->muonType()==1){//MuonStandAlone
		    dettype=1;//MS
		  }
		  if(muon->muonType()>1){//Segment, Calo, Silicon
		    dettype=2;//ID
		  }

		  xAOD::Muon my_muon(*muon);

		  pt_reso=m_muonCalibrationAndSmearingTool->expectedResolution(dettype,my_muon,!m_isData);
		}else if(obj->type()==xAOD::Type::Jet){
		  const xAOD::Jet* jet(static_cast<const xAOD::Jet*>(obj));
		  if(m_isData) pt_reso = m_jerTool->getRelResolutionData(jet);
		  else         pt_reso = m_jerTool->getRelResolutionMC(jet);
		  
		  //
		  // Add extra uncertainty for PU jets based on JVT
		  //
		  if(m_treatPUJets){
		    double jet_pu_unc  = GetPUProb(jet->eta(), jet->phi(),jet->pt()/m_GeV, acc_jvt(*jet))/4.0;
		    pt_reso = sqrt(jet_pu_unc*jet_pu_unc + pt_reso*pt_reso);
		  }
		  //
		  // Use the phi resolution of the jets
		  // needs to be finished
		  //
		  //if(m_doPhiReso){
		  //  double jet_phi_unc = GetPhiUnc(jet->eta(), jet->phi(),jet->pt()/m_GeV);
		  //}

		}else if(obj->type()==xAOD::Type::Electron){
		  const xAOD::Electron* ele(static_cast<const xAOD::Electron*>(obj)); 
		  const auto cl_etaCalo = xAOD::get_eta_calo(*(ele->caloCluster()), ele->author());
		  pt_reso=m_egammaCalibTool->resolution(ele->e(),ele->caloCluster()->eta(),cl_etaCalo,PATCore::ParticleType::Electron);


		}else if(obj->type()==xAOD::Type::Photon){
		  //const xAOD::Photon* pho(static_cast<const xAOD::Photon*>(obj)); 
		  //const auto cl_etaCalo = xAOD::get_eta_calo(*(pho->caloCluster()), pho->author());
		  // need to fix
		  //pt_reso=m_egammaCalibTool->resolution(pho->e(),pho->caloCluster()->eta(),cl_etaCalo,PATCore::ParticleType::Photon);
		  //pt_reso=m_egammaCalibTool->resolution(pho->e(),pho->caloCluster()->eta(),cl_etaCalo,PATCore::ParticleType::Electron); // crashes with photons
		  // 
		}else if(obj->type()==xAOD::Type::Tau){
		  // needs to be implemented
		  const xAOD::TauJet* tau(static_cast<const xAOD::TauJet*>(obj)); 
		  pt_reso = dynamic_cast<CombinedP4FromRecoTaus*>(m_tCombinedP4FromRecoTaus.get())->GetCaloResolution(tau);
		}
		
		ATH_MSG_VERBOSE("Resolution: " << pt_reso);
		varx = (obj->p4().Px()*pt_reso)/m_GeV;
		vary = (obj->p4().Py()*pt_reso)/m_GeV;
		covxy = varx*vary;
		varx*=varx;
		vary*=vary;

		// Rotation (components)
		std::tie(Var_L, Var_T, Cv_LT) = CovMatrixRotation(varx , vary, covxy, m_metphi);
		//Covariance matrix rotated for all jets
		Var_L_j += Var_L;
		Var_T_j += Var_T;
		Cv_LT_j += Cv_LT;  
		
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

	// fill variables
	Var_L = Var_L_j;
	Var_T = Var_T_j;
	Cv_LT = Cv_LT_j;

	if( Var_L != 0 ){
	  Double_t Significance = Significance_LT(m_met,Var_L,Var_T,Cv_LT );
	  
	  Double_t Rho = Cv_LT  / sqrt( Var_L * Var_T ) ;
	  m_significance = Significance;
	  m_rho = Rho;
	  m_VarL = Var_L;
	  m_VarT = Var_T;
	  
	  
	  ATH_MSG_VERBOSE("     Significance (squared): " << Significance << " rho: " << GetRho()
			  << " MET: " << m_met << " sigmaL: " << GetVarL()
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


  double METSignificance::GetPhiUnc(double /*jet_eta*/, double /*jet_phi*/,double /*jet_pt*/)
  {
    /*
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
    */
    /*
    // Stored as bin content = Mean, error = RMS, we want to use the RMS.  
    if(jet_pt<50.0)
      return h_phi_reso_pt20->GetBinContent(xbin, ybin);
    else if(jet_pt<100.0)
      return h_phi_reso_pt50->GetBinContent(xbin, ybin);
    return h_phi_reso_pt100->GetBinContent(xbin, ybin);
    */
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
	Significance = pow( Numerator , 2 ) / (  var_parall  ) ;
      }
    else
    {
      Significance = pow( Numerator , 2 ) / (  var_parall * ( 1 - pow(rho,2) ) ) ;
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
    
} //> end namespace met
