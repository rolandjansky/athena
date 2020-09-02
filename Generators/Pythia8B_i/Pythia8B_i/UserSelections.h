/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ======================================================================
// UserSelections
// James.Catmore@cern.ch
// This file replaces the old "user_finsel" where arbitrary selections
// can be made. There is a single bool method which takes
// (a) a string to point the code to the appropriate selection
// (b) a vector of doubles which can be used to specify cuts 
// Additional code can be included, as with "BsJpsiPhiAngles.h" below  
// ======================================================================
#include "CLHEP/Vector/LorentzVector.h"
#include "Pythia8B_i/BsJpsiPhiAngles.h"

CLHEP::HepLorentzVector convertVector(const Pythia8::Vec4 v) {
        CLHEP::HepLorentzVector vect;
	vect.setPx(v.px());
	vect.setPy(v.py());
	vect.setPz(v.pz());
	vect.setE(v.e());
	return vect;
}

double BsJpsiPhi_PDF(double *params, double *x, bool useHelicity) {

	//  if ( !checkInput(x) ) return 0.;

	// Human-readable aliases of the parameters and observables.
	// Human-readable aliases of the parameters and observables.
	double A0 = sqrt(params[0] * (1 - params[2]));
	double Al = sqrt(params[1] * (1 - params[2]));
	double As = sqrt(params[2]);
	double &GammaS = params[3];
	double &DeltaGamma = params[4];
	double &DeltaM = params[5];
	double &phiS = params[6];
	double &delta_p = params[7];
	double &delta_l = params[8];
	double delta_s = params[7] - params[9];

/*
	 cout << "a0 " << A0 << endl;
	 cout << "al " << Al << endl;
	 cout << "as " << As << endl;
	 cout << "GammaS " << GammaS << endl;
	 cout << "DeltaGamma " << DeltaGamma << endl;
	 cout << "phiS " << phiS << endl;
	 cout << "delta p " << delta_p << endl;
	 cout << "delta l " << delta_l << endl;
	 cout << "delta s " << delta_s << endl;
*/

	double &time = x[0];
	//  double &timeErr    = x[1]; // pico seconds
	double &tagprob = x[4]; // 0. anti-particle, 0.5 no tag, 1.0 particle
	
  // Calculate A perpendicular from the other parameters
  double Ap = 0;
  if ( 1. - As*As - A0*A0 - Al*Al >= 0 ) Ap = sqrt(1. - As*As - A0*A0 - Al*Al);

  // The tabulated function is Sum of: A[i] * ( B1[i] +/- B2[i] ) * C[i]
	
  double A[10], B1[10], B2[10], C[10];
	
  // Define A cells
  double sinphiS = std::sin(phiS);
  double cosphiS = std::cos(phiS);
	
  A[0] = 0.5 * A0*A0;
  A[1] = 0.5 * Al*Al;
  A[2] = 0.5 * Ap*Ap;
  A[3] = 0.5 * A0*Al * std::cos(delta_l);
  A[4] = Al*Ap;
  A[5] = A0*Ap;
  A[6] = 0.5 * As*As;
  A[7] = As*Al;
  A[8] = 0.5 * As*Ap * std::sin(delta_p - delta_s);
  A[9] = As*A0;


	
  // Define B cells
  double gammaH  = GammaS - DeltaGamma/2.;
  double gammaL  = GammaS + DeltaGamma/2.;
  double tauL = 1./gammaL;
  double tauH = 1./gammaH;
  double norm1 = (1./(tauL*(1.+cosphiS) + tauH*(1.-cosphiS)));
  double norm2 = (1./(tauL*(1.-cosphiS) + tauH*(1.+cosphiS)));
  double norm3 = (1./sqrt((tauL-tauH)*(tauL-tauH)*sinphiS*sinphiS+4.*tauL*tauH));
 
  double ExpGSSinMT = 0.0;
  double ExpGSCosMT = 0.0;

 

    // Calculate convolution of exp(-gammaL * time) with a gaussian. Not convoluted if sigma == 0.
    double ExpGL = std::exp(-time * gammaL);

    // Calculate convolution of exp(-gammaH * time) with a gaussian. Not convoluted if sigma == 0.		
    double ExpGH = std::exp(-time * gammaH);	

    B1[0] = ( (1. + cosphiS) * ExpGL + (1. - cosphiS) * ExpGH ) * norm1;
    B1[1] = B1[0];
    B1[2] = ( (1. - cosphiS) * ExpGL + (1. + cosphiS) * ExpGH ) * norm2;
    B1[3] = B1[0];
    B1[4] = 0.5 * std::cos(delta_p - delta_l) * sinphiS * (ExpGL - ExpGH) * norm3;
    B1[5] = 0.5 * std::cos(delta_p) * sinphiS * (ExpGL - ExpGH) * norm3;
    B1[6] = B1[2];
    B1[7] = 0.5 * sinphiS * std::sin(delta_l - delta_s) * (ExpGL - ExpGH) * norm3;
    B1[8] = B1[2];
    B1[9] = 0.5 * sinphiS * std::sin(delta_s) * (ExpGH - ExpGL) * norm3;

  



  // Tagged analysis
  if( std::abs(tagprob - 0.5) > 1e-6 ){   


    ExpGSSinMT = std::exp(-time * GammaS) * sin(DeltaM * time);
    ExpGSCosMT = std::exp(-time * GammaS) * cos(DeltaM * time);


    B2[0] = 2. * ExpGSSinMT * sinphiS * norm1;
    B2[1] = B2[0];
    B2[2] = -2. * ExpGSSinMT * sinphiS * norm2;
    B2[3] = B2[0];
    B2[4] = ( ExpGSCosMT * std::sin(delta_p - delta_l) - cosphiS * cos(delta_p - delta_l) * ExpGSSinMT ) * norm3;
    B2[5] = ( ExpGSCosMT * std::sin(delta_p) - cosphiS * std::cos(delta_p) * ExpGSSinMT ) * norm3;
    B2[6] = B2[2];
    B2[7] = ( ExpGSCosMT * std::cos(delta_l - delta_s) - cosphiS * sin(delta_l - delta_s) * ExpGSSinMT ) * norm3;
    B2[8] = B2[2];
    B2[9] = ( ExpGSCosMT * std::cos(delta_s) + cosphiS * std::sin(delta_s) * ExpGSSinMT ) * norm3;

  } // End of tagged terms
  else{
    // Untagged analysis
    for (int k = 0; k<10; k++) B2[k] = 0;

  }
  // Define C cells

    if (useHelicity) {

      // Helicity Basis: Input variables are costhetal, costhetak and chi
      double &costhetal = x[1];
      double &costhetak = x[2];
      double &chi = x[3];

      double sinsqthetal = 1. - (costhetal * costhetal);
      double sinsqthetak = 1. - (costhetak * costhetak);
      double cossqthetak = costhetak * costhetak;
      double coschi = std::cos(chi);
      double cossqchi = coschi * coschi;
      double sinchi = std::sin(chi);
      double sinsqchi = sinchi * sinchi;
      double sin2thetak = 2. * std::sqrt(1. - costhetak * costhetak) * costhetak;
      double sin2thetal = 2. * std::sqrt(1. - costhetal * costhetal) * costhetal;
      double sin2chi = std::sin(2. * chi);
      double sinthetak = sqrt(sinsqthetak);

      C[0] = 2. * cossqthetak * sinsqthetal; //cossqpsi * (1. - sinsqtheta * cossqphi);
      C[1] = sinsqthetak * (1 - sinsqthetal * cossqchi); //sinsqpsi * (1. - sinsqtheta * sinsqphi);
      C[2] = sinsqthetak * (1 - sinsqthetal * sinsqchi); //sinsqpsi * sinsqtheta;
      C[3] = 1. / sqrt(2.) * sin2thetak * sin2thetal * coschi; //* sin2psi * sinsqtheta * sin2phi;
      C[4] = -1. * sinsqthetak * sinsqthetal * sin2chi; //sinsqpsi * sin2theta * sinphi;
      C[5] = 1. / sqrt(2.) * sin2thetak * sin2thetal * sinchi; //* sin2psi * sin2theta * cosphi;
      C[6] = 2. / 3. * sinsqthetal; //(1. - sinsqtheta * cossqphi);
      C[7] = 1. / 3. * sqrt(6.) * sinthetak * sin2thetal * coschi; //sinpsi * sinsqtheta * sin2phi;
      C[8] = 1. / 3. * sqrt(6.) * sinthetak * sin2thetal * sinchi; //sinpsi * sin2theta * cosphi;
      C[9] = 4. / 3. * sqrt(3.) * costhetak * sinsqthetal; // cospsi * (1. - sinsqtheta * cossqphi); 

    }
    else {

      // Transversity Basis: Input variables are costheta, cospsi and phi
      double &costheta   = x[1];
      double &cospsi     = x[2];
      double &phi        = x[3];

      double sinsqtheta   = 1. - (costheta * costheta);
      double sinsqpsi     = 1. - (cospsi * cospsi);
      double cossqpsi     = cospsi * cospsi;
      double sin2theta    = 2. * std::sqrt(1. - costheta * costheta) * costheta;
      double sin2psi      = 2. * std::sqrt(1. - cospsi * cospsi) * cospsi;
      double cosphi       = std::cos(phi);
      double cossqphi     = cosphi * cosphi;
      double sinphi       = std::sin(phi);
      double sinsqphi     = sinphi * sinphi;
      double sin2phi      = std::sin(2. * phi);
      double sinpsi       = std::sqrt(1. - cospsi*cospsi);
  		
      C[0] = 2. * cossqpsi * (1. - sinsqtheta * cossqphi);
      C[1] = sinsqpsi * (1.-sinsqtheta * sinsqphi);
      C[2] = sinsqpsi * sinsqtheta;
      C[3] = 1. / sqrt(2.) * sin2psi * sinsqtheta * sin2phi;
      C[4] = -1. * sinsqpsi * sin2theta * sinphi;
      C[5] = 1. / sqrt(2.) * sin2psi * sin2theta * cosphi;
      C[6] = 2. / 3. *(1. - sinsqtheta * cossqphi);
      C[7] = 1. / 3. *sqrt(6.) *sinpsi * sinsqtheta * sin2phi;
      C[8] = 1. / 3. *sqrt(6.) *sinpsi * sin2theta * cosphi;
      C[9] = 4. / 3. *sqrt(3.) *cospsi * (1. - sinsqtheta * cossqphi);

    }

  
	
  // Sum the tabulated formula	
  double Wplus  = 0;
  double Wminus = 0;
	
	
  for ( int i=0; i<10; ++i ) {

    Wplus  += A[i] * ( B1[i] + B2[i] ) * C[i];
    Wminus += A[i] * ( B1[i] - B2[i] ) * C[i];

  }
	
  double value = tagprob * Wplus + (1. - tagprob) * Wminus;
	if (value < 0)
		value = 0;
	return value;
}

//double transversityAnglePhi(const HepLorentzVector &jpsi,
//		const HepLorentzVector &phi, const HepLorentzVector &kp,
//		const HepLorentzVector &mup) {
//
//	HepLorentzVector phi_jpsi = phi;
//	HepLorentzVector kp_jpsi = kp;
//	HepLorentzVector mup_jpsi = mup;
//	phi_jpsi.boost(-jpsi.boostVector());
//	kp_jpsi .boost(-jpsi.boostVector());
//	mup_jpsi.boost(-jpsi.boostVector());
//
//	Hep3Vector phi_direction = phi_jpsi.vect().unit();
//	Hep3Vector phi_orthogonal = (kp_jpsi.vect().unit() - phi_direction
//			* (kp_jpsi.vect().unit().dot(phi_direction))).unit();
//
//	return atan2(mup_jpsi.vect().unit().dot(phi_orthogonal),
//			mup_jpsi.vect().unit().dot(phi_direction));
//}

bool Pythia8B_i::userSelection(Pythia8::Event &event, std::string userString,
		std::vector<double> userVars) {

        using CLHEP::HepLorentzVector;
	bool accept(false);

	// ==================================================
	// code up your selection here; as many blocks as
	// needed can be inserted
	if (userString == "EXAMPLE" && event.size() > 0 && userVars.size() > 0) {

		accept = true;

	}

	// ==================================================
	// selects decays where a B-hadron decays to a J/psi
	// by any route
	// Accepts event if event contains a B which (eventually)
	// goes to a J/psi

	else if (userString == "BJPSIINCLUSIVE" && event.size() > 0) {
		int chargeConj = 1;
		if (userVars.size()==0) {
			ATH_MSG_INFO("User selection BJPSIINCLUSIVE with B-state");
		}
		if (userVars.size()>0) {
			if (userVars.size()>1) ATH_MSG_WARNING("User selection BJPSIINCLUSIVE with more than one argument! Check job options");
			if (userVars.at(0)>0) ATH_MSG_DEBUG("User selection BJPSIINCLUSIVE with B-state");
			if (userVars.at(0)<0) {ATH_MSG_DEBUG("User selection BJPSIINCLUSIVE with anti-B-state"); chargeConj = -1;}
		}		

		// B-decay codes which can go to charmonium
		std::vector<int> bToCharmoniaCodes;
		bToCharmoniaCodes.push_back(511*chargeConj);
		bToCharmoniaCodes.push_back(521*chargeConj);
		bToCharmoniaCodes.push_back(531*chargeConj);
		bToCharmoniaCodes.push_back(541*chargeConj);
		bToCharmoniaCodes.push_back(-5122*chargeConj);
		bToCharmoniaCodes.push_back(-5132*chargeConj);
		bToCharmoniaCodes.push_back(-5232*chargeConj);
		bToCharmoniaCodes.push_back(-5332*chargeConj);

		int eventSize = event.size();

		bool isBtoJpsi(false);
		bool containsB(false);
		// loop over all particles in the event
		for (int i = 0; i < eventSize; i++) {
			int pdgID = event[i].id();
			std::vector < Pythia8::Particle > decayMembers;
			bool isB(false);
			// Does event contain B which can go to J/psi?
			for (unsigned int k = 0; k < bToCharmoniaCodes.size(); ++k) {
				if (pdgID == bToCharmoniaCodes[k]) {
					containsB = true;
					isB = true;
					break;
				}
			}
			// Get decay chain of the B; see if there is a J/psi
			if (isB) {
				descendThroughDecay(event, decayMembers, i);
				std::vector<int> pdgCodes = getCodes(decayMembers);
				for (unsigned int k = 0; k < pdgCodes.size(); ++k) {
					if (pdgCodes[k] == 443)
						isBtoJpsi = true;
				}
			}
			if (isBtoJpsi)
				break;
		}
		if (containsB && isBtoJpsi)
			accept = true;
		if (containsB && !isBtoJpsi)
			accept = false;
		if (!containsB)
			accept = false;

	}

	// ==================================================
	// prototype for Bs->J/psi phi angular distribution
	// (Adam Barton)

	else if ((userString == "BJPSIPHI_TRANS" || userString == "BJPSIPHI_HEL")
			&& event.size() > 0) {

		const bool debug = false;
		bool flat;

		//Read
		//userVars[0] 0(flat)/1(angle)
		//[1] prob_limit
		//[2] tag mode
		//[3] A0
		//[4] Al
		//[5] As
		//[6] GammaS
		//[7] DeltaGamma
		//[8] DeltaM
		//[9] phiS
		//[10] delta_p
		//[11] delta_l
		//[12]delta_s

		if (userVars.size() < 13) {
			ATH_MSG_WARNING(
					"REQUIRED userVARs not provided for BsJpsiPhi pdf defaulting to flat");
			flat = true;
		} else {
			flat = userVars[0] == 0.0;

		}

		//		for(int i=0; i<10;i++){
		//			ATH_MSG_INFO("BJPSIPHI_TRANS: " << i << " random number: " << Rdmengine->flat());
		//
		//		}

		//	std::vector<Pythia8::Particle> decayMembers;

		int i_Bs = 0;
		int i_Muplus = 0;
		int i_Muminus = 0;
		int i_Kplus = 0;
		int i_Kminus = 0;
		int i_Phi = 0;
		int i_Jpsi = 0;

		bool isBsJpsiPhi = false;
		int eventSize = event.size();
		for (int i = 0; i < eventSize; i++) {

			int pID = event[i].id();
			if (std::abs(pID) == 531) { //NOTE THIS WILL FIND BS AND ANTIBS
				i_Bs = i;
				std::vector<int> daughterlist = event.daughterList(i);

				if (daughterlist.size() != 2)
					continue;
				bool isjpsi = false;
				bool isphi = false;
				if (event[daughterlist[0]].id() == 443) {
					isjpsi = true;
					i_Jpsi = daughterlist[0];
				}
				if (event[daughterlist[1]].id() == 443) {
					isjpsi = true;
					i_Jpsi = daughterlist[1];
				}
				if (event[daughterlist[0]].id() == 333) {
					isphi = true;
					i_Phi = daughterlist[0];
				}
				if (event[daughterlist[1]].id() == 333) {
					isphi = true;
					i_Phi = daughterlist[1];
				}
				if (!isphi || !isjpsi)
					continue;
				std::vector<int> daughterlistJpsi = event.daughterList(i_Jpsi);
				std::vector<int> daughterlistPhi = event.daughterList(i_Phi);
				if (daughterlistJpsi.size() != 2 || daughterlistPhi.size() != 2)
					continue;
				//resets values to avoid possible bug

				if (event[daughterlistJpsi[0]].id() == 13)
					i_Muminus = daughterlistJpsi[0];
				else if (event[daughterlistJpsi[1]].id() == 13)
					i_Muminus = daughterlistJpsi[1];
				else
					i_Muminus = 0;

				if (event[daughterlistJpsi[0]].id() == -13)
					i_Muplus = daughterlistJpsi[0];
				else if (event[daughterlistJpsi[1]].id() == -13)
					i_Muplus = daughterlistJpsi[1];
				else
					i_Muplus = 0;

				if (event[daughterlistPhi[0]].id() == 321)
					i_Kplus = daughterlistPhi[0];
				else if (event[daughterlistPhi[1]].id() == 321)
					i_Kplus = daughterlistPhi[1];
				else
					i_Kplus = 0;

				if (event[daughterlistPhi[0]].id() == -321)
					i_Kminus = daughterlistPhi[0];
				else if (event[daughterlistPhi[1]].id() == -321)
					i_Kminus = daughterlistPhi[1];
				else
					i_Kminus = 0;
				if (i_Muminus == 0 || i_Muplus == 0 || i_Kminus == 0 || i_Kplus
						== 0)
					continue;
				else {
					if (debug)
						ATH_MSG_INFO(
								"found entire Bs->Jpsi(mumu)phi(KK) decay ");
					isBsJpsiPhi = true;
					break;
				}
			}
		}

		if (!isBsJpsiPhi)
			return false;
		if (flat)
			return true;

		Pythia8::Particle &p_Bs = event[i_Bs];
		Pythia8::Particle &p_Muplus = event[i_Muplus];
		Pythia8::Particle &p_Muminus = event[i_Muminus];
		Pythia8::Particle &p_Kplus = event[i_Kplus];
		Pythia8::Particle &p_Kminus = event[i_Kminus];
		Pythia8::Particle &p_Phi = event[i_Phi];
		Pythia8::Particle &p_Jpsi = event[i_Jpsi];

		//		cout << "Bs " << p_Bs.id() << endl;
		//		cout << "|> " << p_Jpsi.id() << " +  " << p_Phi.id() << endl;
		//		cout << "|> " << p_Muplus.id() << " + " << p_Muminus.id() << " + " << p_Kplus.id() << " + " << p_Kminus.id() << endl;


		HepLorentzVector v_Bs = convertVector(p_Bs.p());
		HepLorentzVector v_Muplus = convertVector(p_Muplus.p());
		HepLorentzVector v_Muminus = convertVector(p_Muminus.p());
		HepLorentzVector v_Kplus = convertVector(p_Kplus.p());
		HepLorentzVector v_Kminus = convertVector(p_Kminus.p());
		HepLorentzVector v_Phi = convertVector(p_Phi.p());
		HepLorentzVector v_Jpsi = convertVector(p_Jpsi.p());

		BsJpsiPhiAngles angles(v_Kplus, v_Muplus, v_Phi, v_Jpsi, v_Bs);

		CLHEP::HepRandomEngine* Rdmengine =
				Pythia8B_i::p_AtRndmGenSvc->GetEngine(Pythia8_i::pythia_stream);
		const double gentau = Pythia8_i::m_pythia->particleData.tau0(531);
		const double correctionfactor = 0.299792458;
		const double gentauCorrect = gentau / correctionfactor;
		if (debug) {
			ATH_MSG_INFO("Lifetime for 531: " << gentau);
			ATH_MSG_INFO("correct lifetime " << gentauCorrect);
		}
		const double Bstau = p_Bs.tau() / correctionfactor;
		double prob1 = 1000;

		//PUT PDFS HERE
		if (userString == "BJPSIPHI_TRANS") {
			double x[5];
			x[0] = Bstau;
			x[1] = angles.thetatrfix();
			x[2] = angles.thetaKfix();
			x[3] = angles.phitrfix();
			x[4] = userVars[2];

			prob1 = BsJpsiPhi_PDF(&userVars[3], x, false);
		} else if (userString == "BJPSIPHI_HEL") {
			double x[5];
			x[0] = Bstau;
			x[1] = angles.thetaLfix();
			x[2] = angles.thetaKfix();
			x[3] = angles.chifix();
			x[4] = userVars[2];

			prob1 = BsJpsiPhi_PDF(&userVars[3], x, true);
		}

		const double prob2 = exp(Bstau / gentauCorrect) * gentauCorrect;
		if (Bstau > 20)
			ATH_MSG_WARNING("Warning Bstau > 20 ps, this could indicate a bug");
		const double prob_norm = userVars[1];
		if (debug) {
			ATH_MSG_INFO("prob limit set to " << prob_norm);
			ATH_MSG_INFO("This Bs has lifetime  " << Bstau);
		}
		double rand = Rdmengine->flat() * prob_norm;
		if (prob1 * prob2 > prob_norm) {
			ATH_MSG_WARNING(
					"Max prob exceeded, too many of these indicates a problem, a few is fine");
		}
		if (debug)
		  std::cout << "totalprob " << prob1 * prob2 << std::endl;
		if (rand < (prob1 * prob2)) {

			if (debug)
				ATH_MSG_INFO("Passed PDF filter");
			accept = true;
		} else {
			if (debug)
				ATH_MSG_INFO("Rejected PDF filter");
			accept = false;
		}

	} // end of Bs->J/psiphi
	else if ((userString == "BDJPSIKSTAR_TRANS") && event.size() > 0) {
		const bool debug = false;
		bool flat;
		if (userVars.size() < 13) {
			ATH_MSG_WARNING(
					"REQUIRED userVARs not provided for BdJpsiKstar pdf defaulting to flat");
			flat = true;
		} else {
			flat = userVars[0] == 0.0;
		}

		int i_Bd = 0;
		int i_Muplus = 0;
		int i_Muminus = 0;
		int i_Kplus = 0;
		int i_piminus = 0;
		int i_Kstar = 0;
		int i_Jpsi = 0;

		bool isBsJpsiKstar = false;
		const int eventSize = event.size();
		for (int i = 0; i < eventSize; i++) {

			const int pID = event[i].id();
			if (std::abs(pID) == 511) { //NOTE THIS FIND BD and Anti-Bd
				i_Bd = i;
				std::vector<int> daughterlist = event.daughterList(i);

				if (daughterlist.size() != 2)
					continue;
				bool isjpsi = false;
				bool iskstar = false;
				if (event[daughterlist[0]].id() == 443) {
					isjpsi = true;
					i_Jpsi = daughterlist[0];
				}
				if (event[daughterlist[1]].id() == 443) {
					isjpsi = true;
					i_Jpsi = daughterlist[1];
				}
				if (std::abs(event[daughterlist[0]].id()) == 313) { //This will find kstar or KstarBar
					iskstar = true;
					i_Kstar = daughterlist[0];
				}
				if (std::abs(event[daughterlist[1]].id()) == 313) { //This will find kstar or KstarBar
					iskstar = true;
					i_Kstar = daughterlist[1];
				}
				if (!iskstar || !isjpsi)
					continue;
				std::vector<int> daughterlistJpsi = event.daughterList(i_Jpsi);
				std::vector<int> daughterlistKstar = event.daughterList(i_Kstar);
				if (daughterlistJpsi.size() != 2 || daughterlistKstar.size() != 2)
					continue;
				//resets values to avoid possible bug

				if (event[daughterlistJpsi[0]].id() == 13)
					i_Muminus = daughterlistJpsi[0];
				else if (event[daughterlistJpsi[1]].id() == 13)
					i_Muminus = daughterlistJpsi[1];
				else
					i_Muminus = 0;

				if (event[daughterlistJpsi[0]].id() == -13)
					i_Muplus = daughterlistJpsi[0];
				else if (event[daughterlistJpsi[1]].id() == -13)
					i_Muplus = daughterlistJpsi[1];
				else
					i_Muplus = 0;

				if (std::abs(event[daughterlistKstar[0]].id()) == 321)
					i_Kplus = daughterlistKstar[0];
				else if (std::abs(event[daughterlistKstar[1]].id()) == 321)
					i_Kplus = daughterlistKstar[1];
				else
					i_Kplus = 0;

				if (abs(event[daughterlistKstar[0]].id()) == 211)
					i_piminus = daughterlistKstar[0];
				else if (abs(event[daughterlistKstar[1]].id()) == 211)
					i_piminus = daughterlistKstar[1];
				else
					i_piminus = 0;

				if (i_Muminus == 0 || i_Muplus == 0 || i_piminus == 0 || i_Kplus
						== 0)
					continue;

				if (debug)
					ATH_MSG_INFO(
							"found entire Bd->Jpsi(mumu)Kstar(KPi) decay ");
				isBsJpsiKstar = true;
				break;

			}
		}

		if (!isBsJpsiKstar)
			return false;
		if (flat)
			return true;

		Pythia8::Particle &p_Bd = event[i_Bd];
		Pythia8::Particle &p_Muplus = event[i_Muplus];
		Pythia8::Particle &p_Muminus = event[i_Muminus];
		Pythia8::Particle &p_Kplus = event[i_Kplus];
		//Pythia8::Particle &p_piminus = event[i_piminus];
		Pythia8::Particle &p_Kstar = event[i_Kstar];
		Pythia8::Particle &p_Jpsi = event[i_Jpsi];

		//		cout << "Bs " << p_Bs.id() << endl;
		//		cout << "|> " << p_Jpsi.id() << " +  " << p_Phi.id() << endl;
		//		cout << "|> " << p_Muplus.id() << " + " << p_Muminus.id() << " + " << p_Kplus.id() << " + " << p_Kminus.id() << endl;


		HepLorentzVector v_Bd = convertVector(p_Bd.p());
		HepLorentzVector v_Muplus = convertVector(p_Muplus.p());
		HepLorentzVector v_Muminus = convertVector(p_Muminus.p());
		HepLorentzVector v_Kplus = convertVector(p_Kplus.p());
		//HepLorentzVector v_piminus = convertVector(p_piminus.p());
		HepLorentzVector v_Kstar = convertVector(p_Kstar.p());
		HepLorentzVector v_Jpsi = convertVector(p_Jpsi.p());

		BsJpsiPhiAngles angles(v_Kplus, v_Muplus, v_Kstar, v_Jpsi, v_Bd);

		CLHEP::HepRandomEngine* Rdmengine =
				Pythia8B_i::p_AtRndmGenSvc->GetEngine(Pythia8_i::pythia_stream);
		const double gentau = Pythia8_i::m_pythia->particleData.tau0(511);
		const double correctionfactor = 0.299792458;
		const double gentauCorrect = gentau / correctionfactor;
		if (debug) {
			ATH_MSG_INFO("Lifetime for 511: " << gentau);
			ATH_MSG_INFO("correct lifetime " << gentauCorrect);
		}
		const double Bdtau = p_Bd.tau() / correctionfactor;
		double prob1;

		//PUT PDFS HERE

		double x[5];
		x[0] = Bdtau;
		x[1] = angles.thetatrfix();
		x[2] = angles.thetaKfix();
		x[3] = angles.phitrfix();
		x[4] = userVars[2];
		prob1 = BsJpsiPhi_PDF(&userVars[3], x, false);


		const double prob2 = exp(Bdtau / gentauCorrect) * gentauCorrect;
		if (Bdtau > 20)
			ATH_MSG_WARNING("Warning Bdtau > 20 ps, this could indicate a bug");
		const double prob_norm = userVars[1];
		if (debug) {
			ATH_MSG_INFO("prob limit set to " << prob_norm);
			ATH_MSG_INFO("This Bd has lifetime  " << Bdtau);
		}
		const double rand = Rdmengine->flat() * prob_norm;
		if (prob1 * prob2 > prob_norm) {
			ATH_MSG_WARNING(
					"Max prob exceeded, too many of these indicates a problem, a few is fine");
		}
		if (debug)
		  std::cout << "totalprob " << prob1 * prob2 << std::endl;
		if (rand < (prob1 * prob2)) {
			if (debug)
				ATH_MSG_INFO("Passed PDF filter");
			accept = true;
		} else {
			if (debug)
				ATH_MSG_INFO("Rejected PDF filter");
			accept = false;
		}
	}

	return (accept);

}


