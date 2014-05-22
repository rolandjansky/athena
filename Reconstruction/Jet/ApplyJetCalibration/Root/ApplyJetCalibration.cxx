/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  Applies preliminary jet calibrations
 *   1. Absolute EtaJES calibration 
 *   2. Jet pile-up correction
 *   3. Residual in-situ derived JES to data only
 *
 *  Extension of the OffsetEtaJES tool
 *
 *  Author: Dag Gillberg, dag.gillberg @ cern.ch
 *
 */

#include "ApplyJetCalibration/ApplyJetCalibration.h"
#include <TMath.h>
#include <TSystem.h>

namespace JetAnalysisCalib {
  JetCalibrationTool::JetCalibrationTool( TString jetAlgo, 
					  TString JESsettingFile, bool isData, TString dir)
    : _etaBinAxis(0), _minPt_JES(10), _minPt_EtaCorr(8), _maxE_EtaCorr(2500),
      _lowPtextrap(0), _lowPtMinR(0.3), 
      _mu_ref(0), _NPV_ref(1), _offsetBins(0), _resOffsetBins(0),
      _offsetDesc(""), _resOffsetDesc(""), _basePath(dir), _GeV(1000),
      _insituCorr(0),_insituEtaMax(0),_insituPtMin(0),_insituPtMax(3000),
      _gsc(0), _npv(0), _muSF(1.0), _rand(0)
  {
    init(jetAlgo,JESsettingFile,isData); 
  }

  JetCalibrationTool::JetCalibrationTool():
    _etaBinAxis(0), _minPt_JES(10), _minPt_EtaCorr(8), _maxE_EtaCorr(2500), 
    _lowPtextrap(0), _lowPtMinR(0.3),
    _mu_ref(0), _NPV_ref(1), _offsetBins(0), _resOffsetBins(0),
    _offsetDesc(""), _resOffsetDesc(""), _basePath(""), 
    _GeV(1000), _insituCorr(0),_insituEtaMax(0),_insituPtMin(0),_insituPtMax(3000),
    _useFullJetArea4vectorCorr(0),_gsc(0), _npv(0), _muSF(1.0), _rand(0) { }

  JetCalibrationTool::~JetCalibrationTool() {
  
    if(_insituCorr) delete _insituCorr;
    if(_etaBinAxis) delete _etaBinAxis;
    if(_offsetBins) delete _offsetBins;
    if(_gsc) delete _gsc;
    if(_npv) delete _npv;
    if(_rand) delete _rand;
  
  }
                                        
  // E_uncorr is the energy at the pile-up subtracted scale
  // eta_det is the eta of the raw, constituent-scale jet (constscale_eta)
  double JetCalibrationTool::GetJES(double E_uncorr, double eta_det) {

    double E = E_uncorr/_GeV; // E in GeV
    //Check if the Pt goes below the minimum value, if so use the special GetLowPtJES method
    if ( E/cosh(eta_det) < _minPt_JES ) {
      double R = GetLowPtJES(E,eta_det);
      return 1.0/R;
    }

    // Get the factors
    int ieta = GetEtaBin(eta_det);
    const double *factors = JES_factors[ieta];
  
    // Calculate the jet response and then the JES as 1/R
    double R = GetLogPolN(factors,E);
    return 1.0/R;
  }

  double JetCalibrationTool::GetLowPtJES(double E_uncorr, double eta_det) {
    int ieta = GetEtaBin(eta_det);
    if (_lowPtextrap == 0) {
      const double *factors = JES_factors[ieta];
      double E = _minPt_JES*cosh(eta_det);
      double R = GetLogPolN(factors,E);
      return R;
    }
    else if (_lowPtextrap == 1) {
      double Ecutoff = JES_minPt_E[ieta];
      double Rcutoff = JES_minPt_R[ieta];
      double slope = JES_minPt_slopes[ieta];
      double R = slope*(E_uncorr-Ecutoff)+Rcutoff;
      return R;
    }
    else if(_lowPtextrap == 2) {
      double minR = _lowPtMinR;
      double R = minR + JES_minPt_Param1[ieta]*E_uncorr + JES_minPt_Param2[ieta]*E_uncorr*E_uncorr;
      return R;
    }
    else error("Incorrect specification of low Pt JES extrapolation, please check the value of the LowPtJESExtrapolationMethod config flag.");
    return 1;
  }

  double JetCalibrationTool::GetMassCorr(double Ecorr, double eta_det) {

   if (!_applyMassCorrection) error("You can't apply the mass correction unless you specify ApplyMassCorrection: true in the configuration file!");

    int ieta = GetEtaBin(eta_det);
    const double *factors = JMS_factors[ieta];
    double E = ( Ecorr/cosh(eta_det)<5.0*_GeV ? 5.0*cosh(eta_det) : Ecorr/_GeV ); // E in GeV
    //double logE = log(E);

    double massR = GetLogPolN(factors,E);
    return 1.0/massR;
  }

  double JetCalibrationTool::GetNPVBeamspotCorrection(double NPV) {
    if(!_isData && _applyNPVBeamspotCorrection) return _npv->GetNVertexBsCorrection(NPV);
    return NPV;
  }

  // Returns an integer NPV from a float by random sampling between the two closest integers
  int JetCalibrationTool::GetNPVcorrApprox(int NPV, int seed) {
    if (_isData) return NPV;
    if ( seed >= 0 ) _rand->SetSeed(seed);
    double NPVfloat = _npv->GetNVertexBsCorrection(NPV);
    int NPVlow = int(NPVfloat); // cut off decimals
    // 13.2 - 13 = 20%, if rndm > 20% return 13, otherwise 14
    return _rand->Rndm() > ( NPVfloat - NPVlow ) ? NPVlow : NPVlow+1;
  }

  int JetCalibrationTool::GetNPVcorrApprox2(int NPV, int seed) {
    if (_isData) return NPV;
    if ( seed >= 0 ) _rand->SetSeed(seed);
    double P=_npv->GetNVertexBsCorrection(NPV)/NPV;
    int npv=0; for (int i=0;i<NPV;++i) if (_rand->Rndm()<P) ++npv;
    return npv;
  }

  //Function that returns the Global sequential calibration factor
  double JetCalibrationTool::GetGSC( TLorentzVector jet, double eta_det,
				     double trackWIDTH, double nTrk, double Tile0, double EM3, int Nsegments) {

    if (!_applyGSCCorrection) error("You can't apply the GSC correction unless you specify ApplyGSCCorrection: true in the configuration file!");
    //Apply the global sequential calibration correction if it was requested in the config file
    //Note: for now it expects to receive pT in GeV, hence feed it pT_corr/_GeV 
    jet.SetPtEtaPhiM(jet.Pt()/_GeV,jet.Eta(),jet.Phi(),jet.M()/_GeV);
    return _gsc->GetGSCCorrection(jet, fabs(eta_det), trackWIDTH, nTrk, Tile0, EM3, Nsegments);
  }

  double JetCalibrationTool::GetLogPolN(const double *factors, double x) {
    double y=0;
    for ( uint i=0; i<Npar; ++i )
      y += factors[i]*TMath::Power(log(x),Int_t(i));
    return y;
  }

  double JetCalibrationTool::GetLogPolNSlope(const double *factors, double x) {
    double y=0;
    for ( uint i=0; i<Npar; ++i )
      y += i*factors[i]*TMath::Power(log(x),Int_t(i-1))/x;
    return y;
  }

  double JetCalibrationTool::GetLogPolN2ndDeriv(const double *factors, double x) {
    double y=0;
    for ( uint i=2; i<Npar; ++i )
      y += (i*factors[i]*(i-1-log(x))*TMath::Power(log(x),Int_t(i-2)))/(x*x);
    return y;
  }

  int JetCalibrationTool::GetEtaBin(double eta_det) {
    /*int ieta=int(eta_det*10)+45;
      if ( eta_det<0 ) ieta -= 1;
      if ( ieta <  0 ) return 0;
      if ( ieta > 89 ) return 89;
      return ieta;*/ // old method: assumed binning in eta
    int bin = _etaBinAxis->FindBin(eta_det);
    if(bin<=0) return 0;
    if(bin>_etaBinAxis->GetNbins()) return bin-2; // overflow
    return bin-1;
  }

  TLorentzVector JetCalibrationTool::ApplyJES( double E_uncorr, double eta_det, double eta, 
					       double phi, double mass_uncorr) {
    TLorentzVector jet;
    double JES=GetJES(E_uncorr,eta_det);
  
    double p = ( E_uncorr > mass_uncorr ? sqrt(E_uncorr*E_uncorr-mass_uncorr*mass_uncorr) : 0 );
    jet.SetPtEtaPhiM(p/cosh(eta),eta,phi,mass_uncorr);
  
    jet *= JES; // Scale the jet by the JES
    if ( _isData && _insituCorr ) jet *= GetInsituCorr(jet.Pt(),eta_det);
    return jet;
  }

  TLorentzVector JetCalibrationTool::ApplyEtaJES( double E_uncorr, double eta_det, 
						  double eta, double phi, double mass_uncorr) {
    // Get corrections
    double JES = GetJES(E_uncorr,eta_det);
    double etaCorr = GetEtaCorr(E_uncorr*JES,eta_det);
  
    // Apply the correction
    TLorentzVector jet;
    double E=E_uncorr*JES, m=mass_uncorr*JES, corrEta=eta+etaCorr;
    double p= ( E>m ? sqrt(E*E-m*m) : 0 );
  
    jet.SetPtEtaPhiM(p/cosh(corrEta),corrEta,phi,m);
    if ( _isData && _insituCorr ) jet *= GetInsituCorr(jet.Pt(),eta_det);
    return jet;
  }

  TLorentzVector JetCalibrationTool::ApplyEtaMassJES( double E_uncorr, double eta_det,
						      double eta, double phi, double mass_uncorr) {
    // Get corrections
    double JES = GetJES(E_uncorr,eta_det);
    double etaCorr = GetEtaCorr(E_uncorr*JES,eta_det);
    double MassCorr = GetMassCorr(E_uncorr*JES,eta_det);

    // Apply the correction
    TLorentzVector jet;
    double E = E_uncorr*JES, m = mass_uncorr*MassCorr, corrEta = eta+etaCorr;
    double p= ( E>m ? sqrt(E*E-m*m) : 0 );

    jet.SetPtEtaPhiM(p/cosh(corrEta),corrEta,phi,m);
    if ( _isData && _insituCorr ) jet *= GetInsituCorr(jet.Pt(),eta_det);
    return jet;
  }

  double JetCalibrationTool::GetOffsetSF( double pT_det, double eta_det, 
					  double mu, double NPV, 
					  double mu_ref, double NPV_ref) {
    return GetOffsetScaleFactor(pT_det,GetOffset(eta_det,mu,NPV,mu_ref,NPV_ref));
  }

  double JetCalibrationTool::GetResidualOffsetSF( double pT_det, double eta_det, 
						  double mu, double NPV) {
    return GetOffsetScaleFactor(pT_det,GetResidualOffset(eta_det,mu,NPV));
  }

  // As input: raw, detector pt and eta (constscale pt/eta), and mu and NPV
  // Output: the jet scale factor
  double JetCalibrationTool::GetOffsetScaleFactor( double pT_det, double offsetET ) {

    if (pT_det<=0) return 0.001;
    double offset_sf = (pT_det-offsetET)/pT_det;

    // protect against negative energy
    if (offset_sf<=0.001) {
      offset_sf=0.01*_GeV/pT_det; // 10 MeV
      //static int Nwarn=0; static TString mname="JetCalibrationTool::ApplyOffsetEtaJES";
      //if (++Nwarn<20)
      //printf("%s Warning: Offset corr: %.1f GeV, jet pT: %.1f GeV. Setting pT to %.3f GeV\n",
      // mname.Data(),offsetET/_GeV,pT_det/_GeV,pT_det*offset_sf/_GeV);
    }
    return offset_sf;
  }

  // E_uncorr:  EM or LC scale jet energy
  // eta_det:   raw eta ("const-scale eta")
  // eta, phi,  mass_uncorr: after origin correction
  // mu, NPV:   <mu> and NPV of the event (average, not "actual" mu)
  // MUref, NPVref: Reference values
  TLorentzVector JetCalibrationTool::ApplyOffsetEtaJES( double E_uncorr, double eta_det, 
							double eta, double phi, double mass_uncorr,
							double mu, double NPV, double MUref, double NPVref) {

    // For MC11 JES, the offset ET is determined as a correction of the jet pT
    // Hence it should be "pT" not "ET"
    double offsetET = GetOffset(eta_det,mu,NPV,MUref,NPVref); 
    double pTuncorr = sqrt(E_uncorr*E_uncorr-mass_uncorr*mass_uncorr)/cosh(eta_det);
    double offset_r = (pTuncorr-offsetET)/(pTuncorr);

    // protect against negative energy
    if (offset_r<=0.001) {
      offset_r=0.001;
      static int Nwarn=0; static TString mname="JetCalibrationTool::ApplyOffsetEtaJES";
      if (++Nwarn<20)
	printf("%s Warning: Offset corr: %.1f GeV, jet pT: %.1f GeV. Setting pT to %.2f GeV\n",
	       mname.Data(),offsetET/_GeV,pTuncorr/_GeV,pTuncorr*offset_r/_GeV);
    }
  
    return ApplyEtaJES(E_uncorr*offset_r,eta_det,eta,phi,mass_uncorr*offset_r);
  }

  // Applies jet area offset correction given the constitutent scale jet 4-vector, the event density rho,
  // the jet area four vector, and mu and NPV
  //   "det" = raw, dector = constitituent scale ( which is equivalent to EM-scale for TopoEM jets )
  TLorentzVector  JetCalibrationTool::ApplyJetAreaOffset( double E_det, double eta_det, double phi_det, double m_det,
							  double Ax, double Ay, double Az, double Ae, double rho,
							  double mu, double NPV) {
    return ApplyJetAreaOffsetOrigin(E_det,eta_det,phi_det,m_det,eta_det,phi_det,m_det,Ax,Ay,Az,Ae,rho,mu,NPV);
  }

  // Same as above, but also applies the origin correction
  TLorentzVector  JetCalibrationTool::ApplyJetAreaOffsetOrigin( double E_det, double eta_det, double phi_det, double mass_uncorr,
								double eta_origin, double phi_origin, double m_origin,
								double Ax, double Ay, double Az, double Ae, double rho,
								double mu, double NPV) {
    static TString method = "JetCalibrationTool::ApplyJetAreaOffsetOrigin";
    if (E_det < m_origin) 
      error(Form("%s Current jet has mass=%.3f GeV, which is greater than its energy: %.3f GeV?? Aborting.",
		 method.Data(),m_origin/_GeV,E_det/_GeV));

    // calculate the pT using the origin corrected variables
    double pT_origin = sqrt(E_det*E_det - m_origin*m_origin)/cosh(eta_origin);
    double pT = sqrt(E_det*E_det - mass_uncorr*mass_uncorr)/cosh(eta_det);

    TLorentzVector jet, jetArea;
    jet.SetPtEtaPhiM(pT,eta_det,phi_det,mass_uncorr);
    jetArea.SetPxPyPzE(Ax,Ay,Az,Ae);
 
    // Full jet area 4 vector correction -- not implemented yet
    // REMINDER: The origin correction won't work in the current implementation of the jet area 4 vector correction.
    if ( _useFullJetArea4vectorCorr ) {
	jet -= rho*jetArea;
	jet *= GetResidualOffsetSF(jet.Pt(),eta_det,mu,NPV);
	//if the jet energy is negative after the pile up correction, return a small positive energy jet
	if ( jet.E() < 0 || jet.Pt() < 0 ) jet.SetPtEtaPhiE( 0.01*_GeV/jet.Pt(), jet.Eta(), jet.Phi(), 0.01*_GeV/jet.E() );
	//if the jet mass is negative after the pile up correction, set it to zero (this is the accepted practice for now)
	else if ( jet.M() < 0 ) jet.SetPtEtaPhiM( jet.Pt(),jet.Eta(),jet.Phi(),0 );
	return jet;
    }

    // "Normal" offset correction, applied as scale factor. 2012 default
    double pT_offset_corr = pT - rho*jetArea.Pt() - GetResidualOffset(eta_det,mu,NPV);
    double offset_SF = pT_offset_corr >=0 ? pT_offset_corr / pT: 0.01*_GeV/pT;
    jet.SetPtEtaPhiM(pT_origin,eta_origin,phi_origin,m_origin);
    jet*=offset_SF;
    return jet;
  }

  // Applies jet area offset correction given the constitutent scale jet 4-vector, the event density rho,
  // the jet area four vector, and mu and NPV
  //   "det" = raw, dector = constitituent scale ( which is equivalent to EM-scale for TopoEM jets )
  // Applies the eta and JES corrections after the jet area offset correction
  TLorentzVector JetCalibrationTool::ApplyJetAreaOffsetEtaJES( double E_det, double eta_det, double phi_det, double m_det,
							       double Ax, double Ay, double Az, double Ae, double rho,
							       double mu, double NPV) {
    // Uses const-scale eta,phi,m as "origin corrected" ones
    return ApplyJetAreaOffsetOriginEtaJES(E_det,eta_det,phi_det,m_det,eta_det,phi_det,m_det,Ax,Ay,Az,Ae,rho,mu,NPV);
  }

  // Same as above, but also applies the origin correction
  TLorentzVector  JetCalibrationTool::ApplyJetAreaOffsetOriginEtaJES( double E_det, double eta_det, double phi_det, double mass_uncorr,
								      double eta_origin, double phi_origin, double m_origin,
								      double Ax, double Ay, double Az, double Ae, double rho,
								      double mu, double NPV) {

    TLorentzVector jet = ApplyJetAreaOffsetOrigin(E_det,eta_det,phi_det,mass_uncorr,
						  eta_origin,phi_origin,m_origin,
						  Ax,Ay,Az,Ae,rho,mu,NPV);
						
    // apply EtaJES
    jet *= GetJES(jet.E(),eta_det);
    double eta = jet.Eta() + GetEtaCorr(jet.E(),eta_det);

    jet.SetPtEtaPhiM( jet.P()/cosh(eta), eta, jet.Phi(), jet.M() );
    if ( _isData && _insituCorr ) jet *= GetInsituCorr( jet.Pt(), eta_det );
  
    return jet;
  }

  //Applies the GSC correction after the Offset and JES corrections
  TLorentzVector JetCalibrationTool::ApplyOffsetEtaJESGSC( double E_uncorr, double eta_det,
							   double eta, double phi, double mass_uncorr,
							   double trackWIDTH, double nTrk, double Tile0, double EM3, double Nsegments,
							   double mu, double NPV) {

    TH2D *insituCorr_orig = _insituCorr;
    _insituCorr = 0; //GSC doesn't take insitu correction into account yet, so we make sure it doesn't get applied.
    TLorentzVector jet = ApplyOffsetEtaJES(E_uncorr, eta_det, eta, phi, mass_uncorr, mu, NPV);
    _insituCorr = insituCorr_orig;

    jet *= GetGSC(jet, eta_det, trackWIDTH, nTrk, Tile0, EM3, Nsegments);
    if ( _isData && _insituCorr ) jet *= GetInsituCorr(jet.Pt(),eta_det);

    return jet;
  }

  //Applies the GSC correction after the Jet area + residual offset and JES corrections
  TLorentzVector JetCalibrationTool::ApplyJetAreaOffsetEtaJESGSC( double E_uncorr, double eta_det, double phi, double mass_uncorr,
								  double Ax, double Ay, double Az, double Ae, double rho,
								  double trackWIDTH, double nTrk, double Tile0, double EM3, double Nsegments,
								  double mu, double NPV) {

    TH2D *insituCorr_orig = _insituCorr;
    _insituCorr = 0; //GSC doesn't take insitu correction into account yet, so we make sure it doesn't get applied.
    TLorentzVector jet = ApplyJetAreaOffsetEtaJES(E_uncorr, eta_det, phi, mass_uncorr, Ax, Ay, Az, Ae, rho, mu, NPV);
    _insituCorr = insituCorr_orig;

    jet *= GetGSC(jet, eta_det, trackWIDTH, nTrk, Tile0, EM3, Nsegments);
    if ( _isData && _insituCorr ) jet *= GetInsituCorr(jet.Pt(),eta_det);

    return jet;
  }

  TLorentzVector JetCalibrationTool::ApplyJetAreaOffsetOriginEtaJESGSC( double E_uncorr, double eta_det, double phi, double mass_uncorr,
									double eta_origin, double phi_origin, double m_origin,
									double Ax, double Ay, double Az, double Ae, double rho,
									double trackWIDTH, double nTrk, double Tile0, double EM3, double Nsegments,
									double mu, double NPV) {

    TH2D *insituCorr_orig = _insituCorr;
    _insituCorr = 0; //GSC doesn't take insitu correction into account yet, so we make sure it doesn't get applied.
    TLorentzVector jet = ApplyJetAreaOffsetOriginEtaJES(E_uncorr, eta_det, phi, mass_uncorr, eta_origin, phi_origin, m_origin, Ax, Ay, Az, Ae, rho, mu, NPV);
    _insituCorr = insituCorr_orig;
    jet *= GetGSC(jet, eta_det, trackWIDTH, nTrk, Tile0, EM3, Nsegments);
    if ( _isData && _insituCorr ) jet *= GetInsituCorr(jet.Pt(),eta_det);

    return jet;
  }

  TLorentzVector JetCalibrationTool::ApplyOffset( double E_uncorr, double eta_det,
						  double eta, double phi, double mass_uncorr,
						  double mu, double NPV, double MUref, double NPVref) {

    // For MC11 JES, the offset ET is determined as a correction of the jet pT
    // Hence it should be "pT" not "ET"
    double offsetET = GetOffset(eta_det,mu,NPV,MUref,NPVref);
    double pTuncorr = ( E_uncorr > mass_uncorr ? sqrt(E_uncorr*E_uncorr-mass_uncorr*mass_uncorr)/cosh(eta_det) : 0 );
    double offset_r = (pTuncorr-offsetET)/(pTuncorr);

    // protect against negative energy
    if (offset_r<=0.001) {
      offset_r=0.001;
      static int Nwarn=0; static TString mname="JetCalibrationTool::ApplyOffsetEtaJES";
      if (++Nwarn<20)
	printf("%s Warning: Offset corr: %.1f GeV, jet pT: %.1f GeV. Setting pT to %.2f GeV\n",
	       mname.Data(),offsetET/_GeV,pTuncorr/_GeV,pTuncorr*offset_r/_GeV);
    }

    // Apply the correction
    TLorentzVector jet;
    double E = E_uncorr*offset_r, m=mass_uncorr*offset_r;
    double p = ( E>m ? sqrt(E*E-m*m) : 0 );

    jet.SetPtEtaPhiM(p/cosh(eta),eta,phi,m);

    return jet;
  }


  // Also apply the bad jet correction
  TLorentzVector JetCalibrationTool::ApplyBadChnOffsetEtaJES( double E_uncorr, double eta_det, 
							      double eta, double phi, double mass_uncorr,
							      double bch_corr_cell, double bch_corr_jet,
							      double mu, double NPV, double MUref, double NPVref) {
    return ApplyOffsetEtaJES(E_uncorr*(1.0-bch_corr_cell)/(1.0-bch_corr_jet),
			     eta_det,eta,phi,mass_uncorr,mu,NPV,MUref,NPVref);
  }

  double JetCalibrationTool::GetEtaCorr(double Ecorr, double eta_det) {
    int ieta = GetEtaBin(eta_det);
    const double *factors = etaCorr_factors[ieta];
  
    double E = Ecorr/_GeV;
    if ( E < _minPt_EtaCorr*cosh(eta_det) ) 
      E = _minPt_EtaCorr*cosh(eta_det);
    if ( E>_maxE_EtaCorr ) E=_maxE_EtaCorr;
  
    double etaCorr = GetLogPolN(factors,E);
  
    // This is ( reco_eta - truth_eta )
    // to make it an additive correction return the negative value
    return -etaCorr;
  }

  int JetCalibrationTool::GetOffsetEtaBin(double eta_det) {
    int bin=_offsetBins->FindBin(fabs(eta_det)); // absolute value
    if (bin==0) return 0; // under-flow, should never happen!
    if (bin > _offsetBins->GetNbins()) return bin-2; // over-flow
    return bin-1;
  }

  double JetCalibrationTool::GetOffset( double eta_det, double mu, double NPV, 
					double mu_ref, double NPV_ref) {

    // if input isn't given, use the default reference values from the JES setting file
    if ( ! _applyOffsetCorrection ) return 0.0;
    if ( _resOffsetBins != NULL ) error("Incorrect usage of the calibration tool: Current calibration requires jet area offset correction.");
    if ( mu < 0 ) mu=0.0;
    if (_offsetNewParameterization) return GetOffsetPt(fabs(eta_det), mu, NPV, _offsetIntercepts, _offsetSlopes, _offsetBins);

    double muRef  = ( mu_ref ==-99 ? _mu_ref  : mu_ref  );
    double npvRef = ( NPV_ref==-99 ? _NPV_ref : NPV_ref );

    //mu rescaling
    double muCorr = _isData ? mu : mu*_muSF;
    //NPV beamspot correction - data is not affected
    double NPVCorr = GetNPVBeamspotCorrection(NPV);

    int bin=GetOffsetEtaBin(eta_det);

    // "cut the edges"
    if ( _interpol == cut_corners ) {
      double etaMin = _offsetBins->GetBinLowEdge(bin+1), etaMax = _offsetBins->GetBinLowEdge(bin+2);
      double w=etaMax-etaMin, aeta=fabs(eta_det);
      if ( aeta < etaMin + w/4 && bin!=0 ) {
	double o2=(_offsetSlopes[bin]*(NPVCorr-npvRef) + _offsetIntercepts[bin]*(muCorr-muRef));
	double o1=(_offsetSlopes[bin-1]*(NPVCorr-npvRef) + _offsetIntercepts[bin-1]*(muCorr-muRef));
	return ((o1+o2)/2 + (o2-o1)/w*2*(aeta-etaMin)) * _GeV;
      }
      if ( aeta > etaMax - w/4 && bin!=int(_offsetIntercepts.size())-1 ) {
	double o1=(_offsetSlopes[bin]*(NPVCorr-npvRef) + _offsetIntercepts[bin]*(muCorr-muRef));
	double o2=(_offsetSlopes[bin+1]*(NPVCorr-npvRef) + _offsetIntercepts[bin+1]*(muCorr-muRef));
	return ((o1+o2)/2 + (o2-o1)/w*2*(aeta-etaMax)) * _GeV;
      }
    } else if ( _interpol == gaus ) {
      // Gaussian kernel !
      static const double etaW = 0.1; // representative eta-widht of an average jet?
      static const int Nsteps = 50; // Nsteps right and left
      static const double Nsigma = 3.0; // Nsigma
      double sumwO=0, sumw=0, w, etai;
      for (int istep=-Nsteps;istep<=Nsteps;++istep) {
	etai=eta_det+Nsigma*etaW*istep/Nsteps;
	w=TMath::Gaus(etai,eta_det,etaW);
	bin=GetOffsetEtaBin(etai);
	sumw += w; sumwO += w*(_offsetSlopes[bin]*(NPVCorr-npvRef) + _offsetIntercepts[bin]*(muCorr-muRef));
      }
      return sumwO/sumw * _GeV;
    }

    return (_offsetSlopes[bin]*(NPVCorr-npvRef) + _offsetIntercepts[bin]*(muCorr-muRef)) * _GeV;
  
  }
    
  double JetCalibrationTool::GetResidualOffset( double eta_det, double mu, double NPV ) { 
    // if input isn't given, use the default reference values from the JES setting file
    if ( ! _applyOffsetCorrection ) return 0.0;
    if ( _resOffsetBins == NULL ) 
      error("JetCalibrationTool::GetResidualOffset : No residual offset calibration factors loaded.");
    if ( mu < 0 ) mu=0.0;

    return GetOffsetPt(fabs(eta_det), mu, NPV, _resOffsetMu, _resOffsetNPV, _resOffsetBins);

  }

  double JetCalibrationTool::GetOffsetPt( double aeta, double mu, double NPV, VecD OffsetMu, VecD OffsetNPV, TAxis *OffsetBins ) {

    //mu rescaling
    double muCorr = _isData ? mu : mu*_muSF;

    //NPV beamspot correction
    double NPVCorr = GetNPVBeamspotCorrection(NPV);

    double alpha = OffsetMu[0], beta = OffsetNPV[0], etaEdge=0;
    int bin=1;
    for (;bin<=OffsetBins->GetNbins();++bin) {
      etaEdge = OffsetBins->GetBinLowEdge(bin);
      double width=OffsetBins->GetBinWidth(bin);
      if (aeta<etaEdge+width) break;
      alpha += width*OffsetMu[bin];
      beta  += width*OffsetNPV[bin];
    }
    alpha += OffsetMu[bin]*(aeta-etaEdge);
    beta  += OffsetNPV[bin]*(aeta-etaEdge);

    return (alpha*(muCorr-_mu_ref) + beta*(NPVCorr-_NPV_ref))*_GeV;

  }

  /***************************************
   *  InitializationTool
   *
   */

  void JetCalibrationTool::init(TString jetAlgo, TString JESfile, bool isData) {
  
    _isData = isData; 
    _rand = new TRandom3(12345678);

    printf("===================================\n\n");
    printf("  Initializing the OffsetEtaJES jet calibration tool\n");
    printf("  for %s jets\n\n",jetAlgo.Data());

    if (JESfile=="") error("No JES configuration file specified.");
    TString fn = FindFile(JESfile);

    printf("  Reading global JES settings from:\n    %s\n\n",fn.Data());

    TEnv *settings = new TEnv();
    int status=settings->ReadFile(FindFile(fn),EEnvLevel(0));
    if (status!=0) error("Cannot read file "+fn);

    //printf("  WARNING - This version of the code has not yet been validated.\n");
    //printf("            Please upgrade to a later version (JetCalibTool tag) when available.\n\n");

    //printf("  For 2011 data (Produced in rel 16.6 or later)\n");
    // read in the calibration factors


    /*
     *  1. initialization of the absolute JES calibration
     */

    TString calibFile = settings->GetValue("AbsoluteJES.CalibFile","");
    if (calibFile=="") error("AbsoluteJES.CalibFile not specified in "+JESfile);
    fn=FindFile(calibFile);
    settings->ReadFile(FindFile(fn), kEnvLocal);

    printf("  Reading absolute calibration factors from:\n    %s\n",fn.Data());
    _jesDesc = settings->GetValue("AbsoluteJES.Description","");
    printf("  Description:\n    %s\n",_jesDesc.Data());

    _applyMassCorrection = settings->GetValue("ApplyMassCorrection",false);
    if(_applyMassCorrection) printf("  Jet mass correction will be applied.\n");

    // minPt_JES always in GeV
    _minPt_JES = settings->GetValue(jetAlgo+".MinPtForETAJES",10);
    //Which extrapolation method to use at low Et (Et < _minPt_JES)
    _lowPtextrap = settings->GetValue("LowPtJESExtrapolationMethod",0);
    //For order 2 extrapolation only, set the minimum value of the response for Et = 0
    _lowPtMinR = settings->GetValue("LowPtJESExtrapolationMinimumResponse",0.25);

    // From mswiatlo, help from dag: variable eta binning
    VecD etaBins = VectorizeD(settings->GetValue("JES.EtaBins",""));
    if (etaBins.size()==0){ // default binning
      for (int i=0;i<=90; i++) 
	etaBins.push_back(0.1*i-4.5);
    }
    else if (etaBins.size()==0) error("JES.EtaBins incorrectly specified");
    else if (etaBins.size()>NetaBins+1) 
      error(Form("JES.EtaBins has %i bins, can be maximally 90!",(int)etaBins.size()-1));
    _etaBinAxis = new TAxis(etaBins.size()-1,&etaBins[0]);
    //for (uint ieta=0;ieta<NetaBins;++ieta) {
    for (uint ieta=0;ieta<etaBins.size()-1;++ieta) {
 
      // Read in absolute JES calibration factors
      TString key=Form("JES.%s_Bin%d",jetAlgo.Data(),ieta);
      std::vector<double> params = VectorizeD(settings->GetValue(key,""));
      if (params.size()!=Npar) error("Cannot read JES calib constants "+key+" from "+JESfile);
      for (uint ipar=0;ipar<Npar;++ipar) JES_factors[ieta][ipar] = params[ipar];

      //Protections for high order extrapolation methods at low Et (Et < _minPt_JES)
      if(_lowPtextrap > 0) {
	//Calculate the slope of the response curve at the minPt for each eta bin
	//Used in the GetLowPtJES method when Pt < minPt
	const double *factors = JES_factors[ieta];
	const double Ecutoff = _minPt_JES*cosh(etaBins[ieta]);
	const double Rcutoff = GetLogPolN(factors,Ecutoff);
	const double Slope = GetLogPolNSlope(factors,Ecutoff);
	if(Slope > Rcutoff/Ecutoff) error( Form("Slope of calibration curve at minimum ET is too steep for the JES factors of etabin %d, eta = %5.2f",ieta,etaBins[ieta]) );

	JES_minPt_E[ieta] = Ecutoff;
	JES_minPt_R[ieta] = Rcutoff;
	JES_minPt_slopes[ieta] = Slope;
      
	//Calculate the parameters for a 2nd order polynomial extension to the calibration curve below minimum ET
	//Used in the GetLowPtJES method when Pt < minPt
	if(_lowPtextrap == 2) {
	  const double h = _lowPtMinR;
	  const double Param1 = (2/Ecutoff)*(Rcutoff-h)-Slope;
	  const double Param2 = (0.5/Ecutoff)*(Slope-Param1);
	  //Slope of the calibration curve should always be positive
	  if( Param1 < 0 || Param1 + 2*Param2*Ecutoff < 0) error( Form("Polynomial extension to calibration curve below minimum ET is not monotonically increasing for etabin %d, eta = %5.2f",ieta,etaBins[ieta]) );
	  JES_minPt_Param1[ieta] = Param1;
	  JES_minPt_Param2[ieta] = Param2;
	}
      }

      // Read in jet eta calibration factors
      key=Form("EtaCorr.%s_Bin%d",jetAlgo.Data(),ieta);
      params = VectorizeD(settings->GetValue(key,""));
      if (params.size()!=Npar) error("Cannot read jet eta calib constants "+key+" from "+JESfile);
      for (uint ipar=0;ipar<Npar;++ipar) etaCorr_factors[ieta][ipar] = params[ipar];

      if(_applyMassCorrection) {
	// Read in absolute JMS calibration factors
	key=Form("MassCorr.%s_Bin%d",jetAlgo.Data(),ieta);
	params = VectorizeD(settings->GetValue(key,""));
	if (params.size()!=Npar) error("Cannot read JMS calib constants "+key+" from "+JESfile);
	for (uint ipar=0;ipar<Npar;++ipar) JMS_factors[ieta][ipar] = params[ipar];
      }
    }

    SetInterpolation(settings->GetValue("Interpolation","None"));

    // Read settings from other input files
    StrV includes = Vectorize(settings->GetValue("Includes",""));
    for (uint i=0;i<includes.size();++i)
      settings->ReadFile(FindFile(includes[i].Data()), kEnvLocal);

    /*
     *  2. initialization of the offset correction
     */

    TString offsetName = settings->GetValue("OffsetCorrection.Name","");
    if (offsetName=="") error("OffsetCorrection.Name not specified in "+JESfile);
    _applyOffsetCorrection = !offsetName.Contains("None");
    if (_applyOffsetCorrection) {
      _mu_ref = settings->GetValue("OffsetCorrection.DefaultMuRef",-99.0);
      if (_mu_ref==-99) error("OffsetCorrection.DefaultMuRef not specified in "+JESfile);
      _NPV_ref = settings->GetValue("OffsetCorrection.DefaultNPVRef",-99.0);
      if (_NPV_ref==-99) error("OffsetCorrection.DefaultNPVRef not specified in "+JESfile);
    
      _offsetDesc = settings->GetValue(offsetName+".Description","");
      printf("\n  Jet pile-up correction:\n    %s\n",_offsetDesc.Data());

      _offsetNewParameterization = TString(settings->GetValue(offsetName+".Parameterization","")).Contains("New");
    
      VecD offsetEtaBins = VectorizeD(settings->GetValue(offsetName+".AbsEtaBins",""));
      if (offsetEtaBins.size()<3) error(offsetName+".AbsEtaBins not specified");
      _offsetBins = new TAxis(offsetEtaBins.size()-1,&offsetEtaBins[0]);
    
      _offsetSlopes     = VectorizeD(settings->GetValue(offsetName+".Slope."+jetAlgo,""));
      if ((int)_offsetSlopes.size()!=_offsetBins->GetNbins()+int(_offsetNewParameterization)) 
	error("Incorrect specification of "+offsetName+".Slope."+jetAlgo);
      _offsetIntercepts = VectorizeD(settings->GetValue(offsetName+".Intercept."+jetAlgo,""));
      if ((int)_offsetIntercepts.size()!=_offsetBins->GetNbins()+int(_offsetNewParameterization)) 
	error("Incorrect specification of "+offsetName+".Intercept."+jetAlgo);
    }

    /*
     *  3. initialization of the jet areas and residual offset correction
     */

    _useFullJetArea4vectorCorr = settings->GetValue("ApplyFullJetArea4MomentumCorrection", false);
    if(_useFullJetArea4vectorCorr) printf("  Full 4-vector jet area offset correction will be applied.\n");
  
    offsetName = settings->GetValue("ResidualOffsetCorrection.Name","");
    _applyOffsetCorrection = !offsetName.Contains("None");
    if ( _applyOffsetCorrection && offsetName!="" ) {
      _resOffsetDesc = settings->GetValue(offsetName+".Description","");
      printf("\n  Residual jet-area pile-up correction:\n    %s\n",_resOffsetDesc.Data());
    
      VecD offsetEtaBins = VectorizeD(settings->GetValue(offsetName+".AbsEtaBins",""));
      if (offsetEtaBins.size()<3) error(offsetName+".AbsEtaBins not specified");
      _resOffsetBins = new TAxis(offsetEtaBins.size()-1,&offsetEtaBins[0]);
    
      _resOffsetMu = VectorizeD(settings->GetValue(offsetName+".MuTerm."+jetAlgo,""));
      if ((int)_resOffsetMu.size()!=_resOffsetBins->GetNbins()+1) 
	error("Incorrect specification of "+offsetName+".MuTerm."+jetAlgo);
      _resOffsetNPV = VectorizeD(settings->GetValue(offsetName+".NPVTerm."+jetAlgo,""));
      if ((int)_resOffsetNPV.size()!=_resOffsetBins->GetNbins()+1) 
	error("Incorrect specification of "+offsetName+".NPVTerm."+jetAlgo);
    }

    /*
     *  4. residual data correction
     */


    if (_isData) {
      TString insitu_fn = settings->GetValue("InsituCalibrationFile","None");
      TString rel_insitu_hn = settings->GetValue("RelativeInsituCalibrationHistogram","");
      TString abs_insitu_hn = settings->GetValue("AbsoluteInsituCalibrationHistogram","");
      TString insitu_desc = settings->GetValue("InsituCalibrationDescription","");
      if (insitu_fn!="None") {
	insitu_fn = FindFile(insitu_fn);
	TFile *file = TFile::Open(insitu_fn);
	if (file==NULL) error("Cannot open InsituCalibrationFile: "+insitu_fn);

	printf("\n  Reading In-situ correction factors from:\n    %s\n\n",insitu_fn.Data());
      
	rel_insitu_hn.ReplaceAll("JETALGO",jetAlgo); abs_insitu_hn.ReplaceAll("JETALGO",jetAlgo);
	TH2D *rel_h = (TH2D*)GetHisto(file,rel_insitu_hn);
	TH1D *abs_h = (TH1D*)GetHisto(file,abs_insitu_hn);
	if (rel_h==NULL||abs_h==NULL) {
	  printf("\n  Tool configured for data, but no residual in-situ correction can be applied!");
	} else {
	  gROOT->cd();
	  _insituCorr = CombineCalibration(rel_h,abs_h);
	  _insituEtaMax = _insituCorr->GetYaxis()->GetBinLowEdge(_insituCorr->GetNbinsY()+1);
	  _insituPtMin = _insituCorr->GetXaxis()->GetBinLowEdge(1);
	  _insituPtMax = _insituCorr->GetXaxis()->GetBinLowEdge(_insituCorr->GetNbinsX()+1);
	  file->Close();
      
	  printf("\n  Tool configured to calibrate data.\n  In-situ correction to be applied:\n    %s\n",
		 insitu_desc.Data());
	}
	delete file;
      } else 
	printf("\n  Tool configured to calibrate data.\n  No residual in-situ correction will be applied.\n");
    } else
      printf("\n  Tool configured to calibrate jets in MC.\n  No in-situ correction will be applied.\n");
  
    /*
     *  5. global sequential calibration. see Root/GSC.cxx and ApplyJetCalibration/GSC.h
     */

    _applyGSCCorrection = settings->GetValue("ApplyGSCCorrection",false);
    if(_applyGSCCorrection) { 
      printf("\n  Global Sequential Calibration correction will be applied.\n"); 
      TString GSCFile = settings->GetValue("GSCFactorsFile","empty");
      if ( GSCFile.EqualTo("empty") ) error("GSCFactorsFile flag not set in your config, can't the apply Global Sequential Calibration");
      TString GSCDepthString = settings->GetValue("GSCDepth","Full");
      if( !GSCDepthString.Contains("Tile0") && !GSCDepthString.Contains("EM3") && !GSCDepthString.Contains("nTrk") && !GSCDepthString.Contains("trackWIDTH") && !GSCDepthString.Contains("PunchThrough")
	  && !GSCDepthString.Contains("Full") )
	error("GSCDepth flag not properly set, please check your config file.");

      //Initialize the GSCTool class
      _gsc = new GSCTool(jetAlgo,GSCFile,GSCDepthString);

      //Set punchthrough eta binning and minimum pT
      if(GSCDepthString.Contains("PunchThrough") || GSCDepthString.Contains("Full")) {
	_gsc->SetPunchThroughEtaBins( VectorizeD( settings->GetValue("PunchThroughEtaBins","") ) );
	_gsc->SetPunchThroughMinPt( settings->GetValue("PunchThroughMinPt",50) );
      }

      printf("\n  Global sequential calibration depth set to: %s",GSCDepthString.Data());
    }

    /*
     *  6. mu rescaling and NPV beamspot correction. see Root/NPVBeamspotCorrection.cxx and ApplyJetCalibration/NPVBeamspotCorrection.h
     */

    _muSF = settings->GetValue("MuScaleFactor",1.0);
    _applyNPVBeamspotCorrection = settings->GetValue("ApplyNPVBeamspotCorrection",false);
    if( _applyNPVBeamspotCorrection) {
      _npv = new NPVBeamspotCorrectionTool();
      printf("\n  NPV beamspot correction will be applied.\n");
    }

    printf("\n===================================\n\n");
  
    //we don't need this anymore
    delete settings;
  
  }

  TH2D* JetCalibrationTool::CombineCalibration(TH2D *h2d, TH1D *h) {
    TH2D *prod = (TH2D*)h2d->Clone();
    for (int xi=1;xi<=prod->GetNbinsX();xi++) {
      double pt=prod->GetXaxis()->GetBinCenter(xi);
      double R_abs=h->Interpolate(pt); // Rdata/RMC for the absolute scale
      //printf("pT = %7.1f GeV, abs calib: %.4f\n",pt,abs);
      for (int yi=1;yi<=prod->GetNbinsY();yi++) {
	double c_rel = h2d->GetBinContent(xi,yi); // 1/Rrel = RMC/Rdata
	prod->SetBinContent(xi,yi,c_rel/R_abs);
      }
    }
    return prod;
  }

  double JetCalibrationTool::GetInsituCorr(double pt, double eta) {
    if (_insituCorr==NULL) return 1.0;
    double myEta = eta, myPt = pt/_GeV;
    if (myEta <= -_insituEtaMax) myEta = 1e-6 - _insituEtaMax;
    else if (myEta >= _insituEtaMax) myEta = _insituEtaMax - 1e-6;
    if ( myPt <= _insituPtMin ) myPt = _insituPtMin + 1e-6;
    else if ( myPt >= _insituPtMax ) myPt = _insituPtMax - 1e-6;
    return _insituCorr->Interpolate(myPt,myEta);
  }

  void JetCalibrationTool::SetInterpolation(TString interpolStr) {
    if      (interpolStr=="Gaus"||interpolStr=="GaussianKernel") _interpol=gaus;
    else if (interpolStr=="CutCorners") _interpol=cut_corners;
    else if (interpolStr=="None") _interpol=none;
    else error("Invalid value for interpolation: "+interpolStr);
  }

  std::vector<TString> JetCalibrationTool::Vectorize(TString str, TString sep) {
    std::vector<TString> result; TObjArray *strings = str.Tokenize(sep.Data());
    if (strings->GetEntries()==0) {
      delete strings;
      return result;
    }
    TIter istr(strings);
    while (TObjString* os=(TObjString*)istr())
      if (os->GetString()[0]!='#') result.push_back(os->GetString());
      else break;
    delete strings;
    return result;
  }

  std::vector<double> JetCalibrationTool::VectorizeD(TString str, TString sep) {
    std::vector<double> result; std::vector<TString> vecS = Vectorize(str,sep);
    for (uint i=0;i<vecS.size();++i) result.push_back(atof(vecS[i]));
    return result;
  }

  // Find configuration file
  TString JetCalibrationTool::FindFile(TString filename) {
    TString fn(filename); gSystem->ExpandPathName(fn); gSystem->ExpandPathName(_basePath);

    // First check the actual path
    if (gSystem->AccessPathName(fn)==false) { 
      TString path(fn); path.ReplaceAll(gSystem->BaseName(fn),"");
      path.ReplaceAll("CalibrationConfigs/",""); path.ReplaceAll("CalibrationFactors/","");
      _basePath=path;
      // account for possible subfolders, e.g. muScan2013
      std::string temp_path = path.Data();
      std::size_t found = temp_path.find("CalibrationConfigs/");
      if (found!=std::string::npos) temp_path.erase(found);
      found = temp_path.find("CalibrationFactors/");
      if (found!=std::string::npos) temp_path.erase(found);
      _basePath=temp_path;
      // path.ReplaceAll("CalibrationConfigs/",""); path.ReplaceAll("CalibrationFactors/","");
      //_basePath=path;
      return fn;
    }

    // Add / to path name if needed
    if (_basePath.Length()&&_basePath[_basePath.Length()-1]!='/') _basePath+='/';
  
    // if not there, check the directory were the last found file was ...
    TString lastpath=_basePath;
    if (gSystem->AccessPathName(_basePath+fn)==false) return _basePath+fn;

    // Let's try to pick up the calibration files in the RootCore folder
    TString RootCoreDir = gSystem->Getenv("ROOTCOREBIN");
    if (RootCoreDir == "") RootCoreDir = gSystem->Getenv("ROOTCOREDIR");
    if (RootCoreDir != "") {
      TString path=RootCoreDir+"/data/ApplyJetCalibration/"; // this should always work
      if (gSystem->AccessPathName(path+fn)==false) { _basePath=path; return _basePath+fn; }
      path=RootCoreDir+"../ApplyJetCalibration/data/";
      if (gSystem->AccessPathName(path+fn)==false) { _basePath=path; return _basePath+fn; }
    }

    TString TestAreaDir = gSystem->Getenv("TestArea");
    if(TestAreaDir != "") {
      TString path=TestAreaDir+"/InstallArea/XML/ApplyJetCalibration/CalibrationConfigs/"+gSystem->BaseName(fn);
      if(gSystem->AccessPathName(path)==false) {
	_basePath = path;
	_basePath.ReplaceAll(gSystem->BaseName(fn),""); _basePath.ReplaceAll("CalibrationConfigs/","");
	return path;
      }
      path=TestAreaDir+"/InstallArea/XML/ApplyJetCalibration/"+fn;
      if(gSystem->AccessPathName(path)==false) {
	_basePath = TestAreaDir+"/InstallArea/XML/ApplyJetCalibration/";
	return _basePath+fn;
      }
      path=TestAreaDir+"/Reconstruction/Jet/ApplyJetCalibration/data/CalibrationConfigs/"+gSystem->BaseName(fn);
      if(gSystem->AccessPathName(path)==false) {
	_basePath = path;
	_basePath.ReplaceAll(gSystem->BaseName(fn),""); _basePath.ReplaceAll("CalibrationConfigs/","");
	return path;
      }
      path=TestAreaDir+"/Reconstruction/Jet/ApplyJetCalibration/data/"+fn;
      if(gSystem->AccessPathName(path)==false) {
	_basePath = TestAreaDir+"/Reconstruction/Jet/ApplyJetCalibration/data/";
	return _basePath+fn;
      }
    }			 
  
    // getting a bit desperate here... check if its one level up
    TString path="ApplyJetCalibration/data/";
    if (gSystem->AccessPathName(path+fn)==false) { _basePath=path; return _basePath+fn; }

    // if needed, can here add a loop over the gSystem->GetIncludePath() list of directories

    printf("Cannot find file %s\n",filename.Data());
    printf("Searched in:\n  ./%s\n",filename.Data());
    printf("  %s\n",(lastpath+filename).Data());
    if (RootCoreDir!="") {
      printf("  %s\n  %s\n",
	     (RootCoreDir+"/data/ApplyJetCalibration/"+fn).Data(),
	     (RootCoreDir+"/../ApplyJetCalibration/data/"+fn).Data());
    }
    if(TestAreaDir!="") { 
      printf(" %s\n",
	     (TestAreaDir+"/InstallArea/XML/ApplyJetCalibration/CalibrationConfigs/"+gSystem->BaseName(fn)).Data());
      printf(" %s\n",
	     (TestAreaDir+"/InstallArea/XML/ApplyJetCalibration/"+fn).Data());
      printf(" %s\n",
	     (TestAreaDir+"/Reconstruction/Jet/ApplyJetCalibration/data/CalibrationConfigs/"+gSystem->BaseName(fn)).Data());
      printf(" %s\n",
	     (TestAreaDir+"/Reconstruction/Jet/ApplyJetCalibration/data/"+fn).Data());
    }
    printf("  ./ApplyJetCalibration/data/%s\n",filename.Data());
    error("Cannot find file "+filename);
    return "";
  }

}
