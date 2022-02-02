/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// This program is based in the example "main31.cc" from the Pythia8 examples, used to interface Pythia shower with Powheg events

// Copyright (C) 2012 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Adapted to be Athena and Pythia8_i compliant by giancarlo.panizzo@cern.ch

#include "Pythia8/Pythia.h"
#include "UserHooksUtils.h"
#include "UserSetting.h"
#include "Pythia8_i/UserHooksFactory.h"


using namespace Pythia8;

namespace Pythia8{
  struct si_data_type {
    int pythiamatching, pytune;
    bool use_photos, vetoqed, py8veto, nohad, savelhe, noQEDqopt;
  };

  struct si_event_info_type {
    double xphcut;    
    double vetoscale_isr;
    double vetoscale_fsr;
    int evtnumber;
  };

  class CustomV_EW;
  class PowhegV_EW;
}

Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::CustomV_EW> CustomV_EWCreator("CustomV_EW");
Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::PowhegV_EW> PowhegV_EWCreator("PowhegV_EW");


namespace Pythia8{

  // Use userhooks to veto PYTHIA emissions above the POWHEG scale.
  // CustomV_EW is intended to start from scalup the QED radiation from the 
  // resonance. It has to be used with SpaceShower:pTmaxMatch = 1 and "TimeShower:pTmaxMatch = 1"

  class CustomV_EW : public UserHooks {

  public:

    CustomV_EW() {

      std::cout<<"**********************************************************"<<std::endl;
      std::cout<<"*                                                        *"<<std::endl;
      std::cout<<"*       SI: Defining custom UserHook needed              *"<<std::endl;
      std::cout<<"*     to veto QED radiation  (ptmaxmatch = 1)            *"<<std::endl;
      std::cout<<"*                                                        *"<<std::endl;
      std::cout<<"*     WARNING: This code is unvalidated!!!               *"<<std::endl;
      std::cout<<"*                                                        *"<<std::endl;
      std::cout<<"**********************************************************"<<std::endl;


      Pythia8_UserHooks::UserHooksFactory::userSettings<bool>()["si_data_.vetoqed"]=true;
      Pythia8_UserHooks::UserHooksFactory::userSettings<bool>()["si_data_.py8veto"]=true;
      Pythia8_UserHooks::UserHooksFactory::userSettings<double>()["si_event_info_.vetoscale_fsr"]=10.0;

    }

    ~CustomV_EW() {;}

    // Initialize settings.

    bool initAfterBeams() {
      si_data_.vetoqed              = settingsPtr->mode("si_data_.vetoqed");
      si_data_.py8veto              = settingsPtr->mode("si_data_.py8veto");
      si_event_info_.vetoscale_fsr  = settingsPtr->mode("si_event_info_.vetoscale_fsr");
      return true;
    }


    // Allow process cross section to be modified..

    virtual bool canSetResonanceScale() {
      // If we are vetoing QED emissions, and ptmaxmatch = 1, and we are using PYTHIA8 based veto, set scale for QED radiation from leptons
      //  (the default would be the resonance mass)
      cout << "**** SI: Allow to set scale to veto QED emissions in PYTHIA" << endl;
      if ((si_data_.vetoqed) && (si_data_.py8veto)) return true;
      else return false;
    }

    virtual double scaleResonance( const int iRes, const Event& event) {
      // Set scale for the emissions from the resonace (FSR), equal to the scale stored in the LHE file
      //cout << "SI in scaleResonance. Setting scale: " << si_event_info_.vetoscale_fsr << endl; 
      //cout << event[iRes].id();
      //cout << "\n";
      return si_event_info_.vetoscale_fsr;
    }    

    virtual double EventList( const Event& event)
    {
      event.list();
      return false;
    }

  private:
    si_data_type si_data_;
    si_event_info_type si_event_info_;
  };

  // PowhegV_EW is intended to veto QCD radiation (ISR and FSR) 
  // and QED radiation from the resonance. So it has to be used with 
  // SpaceShower:pTmaxMatch = 2 and "TimeShower:pTmaxMatch = 2"

  class PowhegV_EW : public UserHooks {

  public:  

    // Constructor and destructor.
    PowhegV_EW() {

      std::cout<<"**********************************************************"<<std::endl;
      std::cout<<"*                                                        *"<<std::endl;
      std::cout<<"*     SI: Defining modified PowhegHook to perform        *"<<std::endl;
      std::cout<<"*          the matching  (ptmaxmatch = 2)                *"<<std::endl;
      std::cout<<"*                                                        *"<<std::endl;
      std::cout<<"**********************************************************"<<std::endl;

      Pythia8_UserHooks::UserHooksFactory::userSettings<bool>()["si_data_.vetoqed"]=true;
      Pythia8_UserHooks::UserHooksFactory::userSettings<bool>()["si_data_.py8veto"]=true;
      Pythia8_UserHooks::UserHooksFactory::userSettings<double>()["si_event_info_.vetoscale_fsr"]=10.0;
      Pythia8_UserHooks::UserHooksFactory::userSettings<double>()["si_event_info_.vetoscale_isr"]=10.0;

    };

    ~PowhegV_EW() {}

    // Initialize settings, detailing merging strategy to use.
    bool initAfterBeams() {
      nFinal      = settingsPtr->mode("POWHEG:nFinal");
      vetoMode    = settingsPtr->mode("POWHEG:veto");
      vetoCount   = settingsPtr->mode("POWHEG:vetoCount");
      pThardMode  = settingsPtr->mode("POWHEG:pThard");
      pTemtMode   = settingsPtr->mode("POWHEG:pTemt");
      emittedMode = settingsPtr->mode("POWHEG:emitted");
      pTdefMode   = settingsPtr->mode("POWHEG:pTdef");
      MPIvetoMode = settingsPtr->mode("POWHEG:MPIveto");

      si_data_.vetoqed              = settingsPtr->mode("si_data_.vetoqed");
      si_data_.py8veto              = settingsPtr->mode("si_data_.py8veto");
      si_event_info_.vetoscale_fsr  = settingsPtr->mode("si_event_info_.vetoscale_fsr");
      si_event_info_.vetoscale_isr  = settingsPtr->mode("si_event_info_.vetoscale_isr");
      return true;
    }


    //--------------------------------------------------------------------------

    // Routines to calculate the pT (according to pTdefMode) in a splitting:
    //   ISR: i (radiator after)  -> j (emitted after) k (radiator before)
    //   FSR: i (radiator before) -> j (emitted after) k (radiator after)
    // For the Pythia pT definition, a recoiler (after) must be specified.

    // Compute the Pythia pT separation. Based on pTLund function in History.cc
    double pTpythia(const Event &e, int RadAfterBranch, int EmtAfterBranch,
		    int RecAfterBranch, bool FSR) {

      // Convenient shorthands for later
      Vec4 radVec = e[RadAfterBranch].p();
      Vec4 emtVec = e[EmtAfterBranch].p();
      Vec4 recVec = e[RecAfterBranch].p();
      int  radID  = e[RadAfterBranch].id();

      // Calculate virtuality of splitting
      double sign = (FSR) ? 1. : -1.;
      Vec4 Q(radVec + sign * emtVec); 
      double Qsq = sign * Q.m2Calc();

      // Mass term of radiator
      double m2Rad = (abs(radID) >= 4 && abs(radID) < 7) ?
	pow2(particleDataPtr->m0(radID)) : 0.;

      // z values for FSR and ISR
      double z, pTnow;
      if (FSR) {
	// Construct 2 -> 3 variables
	Vec4 sum = radVec + recVec + emtVec;
	double m2Dip = sum.m2Calc();
	double x1 = 2. * (sum * radVec) / m2Dip;
	double x3 = 2. * (sum * emtVec) / m2Dip;
	z     = x1 / (x1 + x3);
	pTnow = z * (1. - z);

      } else {
	// Construct dipoles before/after splitting
	Vec4 qBR(radVec - emtVec + recVec);
	Vec4 qAR(radVec + recVec);
	z     = qBR.m2Calc() / qAR.m2Calc();
	pTnow = (1. - z);
      }

      // Virtuality with correct sign
      pTnow *= (Qsq - sign * m2Rad);

      // Can get negative pT for massive splittings
      if (pTnow < 0.) {
	cout << "Warning: pTpythia was negative" << endl;
	return -1.;
      }

#ifdef DBGOUTPUT
      cout << "pTpythia: rad = " << RadAfterBranch << ", emt = "
	   << EmtAfterBranch << ", rec = " << RecAfterBranch
	   << ", pTnow = " << sqrt(pTnow) << endl;
#endif

      // Return pT
      return sqrt(pTnow);
    }

    // Compute the POWHEG pT separation between i and j
    // ISR: absolute pT of j
    // FSR: pT of j w.r.t. to i
    double pTpowheg(const Event &e, int i, int j, bool FSR) {

      // pT value for FSR and ISR
      double pTnow = 0.;
      if (FSR) {
	// POWHEG d_ij (in CM frame). Note that the incoming beams have not
	// been updated in the parton systems pointer yet (i.e. prior to any
	// potential recoil).
	int iInA = partonSystemsPtr->getInA(0);
	int iInB = partonSystemsPtr->getInB(0);
	double betaZ = - ( e[iInA].pz() + e[iInB].pz() ) /
	  ( e[iInA].e() + e[iInB].e()  );
	Vec4 iVecBst(e[i].p()), jVecBst(e[j].p());
	iVecBst.bst(0., 0., betaZ);
	jVecBst.bst(0., 0., betaZ);

	if ( e[i].id() == 21 && e[j].id() == 21) {
          pTnow = sqrt( (iVecBst + jVecBst).m2Calc() *
                        iVecBst.e() * jVecBst.e() /
                        pow2(iVecBst.e() + jVecBst.e()) );
	} else {
          pTnow = sqrt( (iVecBst + jVecBst).m2Calc() *
                        jVecBst.e() / iVecBst.e() );
	}
 
      } else {
	// POWHEG pT_ISR is just kinematic pT
	pTnow = e[j].pT();
      }

      // Check result
      if (pTnow < 0.) {
	cout << "Warning: pTpowheg was negative" << endl;
	return -1.;
      }

#ifdef DBGOUTPUT
      cout << "pTpowheg: i = " << i << ", j = " << j
	   << ", pTnow = " << pTnow << endl;
#endif

      return pTnow;
    }

    // Calculate pT for a splitting based on pTdefMode.
    // If j is -1, all final-state partons are tried.
    // If i, k, r and xSR are -1, then all incoming and outgoing 
    // partons are tried.
    // xSR set to 0 means ISR, while xSR set to 1 means FSR
    double pTcalc(const Event &e, int i, int j, int k, int r, int xSRin) {

      //    cout << "APPENA ENTRATO IN pTcalc" << endl;

      //cout << "idhep(i)= " << e[i].id() << endl;
      //cout << "idhep(j)= " << e[j].id() << endl;
      //cout << "idhep(k)= " << e[k].id() << endl;


      // Loop over ISR and FSR if necessary
      double pTemt = -1., pTnow;
      int xSR1 = (xSRin == -1) ? 0 : xSRin;
      int xSR2 = (xSRin == -1) ? 2 : xSRin + 1;
      for (int xSR = xSR1; xSR < xSR2; xSR++) {
	// FSR flag
	bool FSR = (xSR == 0) ? false : true;

	// If all necessary arguments have been given, then directly calculate.
	// POWHEG ISR and FSR, need i and j.
	if ((pTdefMode == 0 || pTdefMode == 1) && i > 0 && j > 0) {
	  pTemt = pTpowheg(e, i, j, (pTdefMode == 0) ? false : FSR);

	  // Pythia ISR, need i, j and r.
	} else if (!FSR && pTdefMode == 2 && i > 0 && j > 0 && r > 0) {
	  pTemt = pTpythia(e, i, j, r, FSR);

	  // Pythia FSR, need k, j and r.
	} else if (FSR && pTdefMode == 2 && j > 0 && k > 0 && r > 0) {
	  pTemt = pTpythia(e, k, j, r, FSR);

	  // Otherwise need to try all possible combintations.
	} else {
	  // Start by finding incoming legs to the hard system after
	  // branching (radiator after branching, i for ISR).
	  // Use partonSystemsPtr to find incoming just prior to the
	  // branching and track mothers.
	  int iInA = partonSystemsPtr->getInA(0);
	  int iInB = partonSystemsPtr->getInB(0);
	  while (e[iInA].mother1() != 1) { iInA = e[iInA].mother1(); }
	  while (e[iInB].mother1() != 2) { iInB = e[iInB].mother1(); }

	  // If we do not have j, then try all final-state partons
	  int jNow = (j > 0) ? j : 0;
	  int jMax = (j > 0) ? j + 1 : e.size();
	  for (; jNow < jMax; jNow++) {

	    // Final-state jNow only
	    if ( !e[jNow].isFinal() ) continue;

	    // POWHEG
	    if (pTdefMode == 0 || pTdefMode == 1) {

	      // ISR - only done once as just kinematical pT
	      if (!FSR) {
		pTnow = pTpowheg(e, iInA, jNow, (pTdefMode == 0) ? false : FSR);
		if (pTnow > 0.) pTemt = (pTemt < 0) ? pTnow : min(pTemt, pTnow);
  
		// FSR - try all outgoing partons from system before branching 
		// as i. Note that for the hard system, there is no 
		// "before branching" information.
              } else {

                int outSize = partonSystemsPtr->sizeOut(0);
                for (int iMem = 0; iMem < outSize; iMem++) {
                  int iNow = partonSystemsPtr->getOut(0, iMem);

                  // Coloured only, i != jNow and no carbon copies
                  if (iNow == jNow) continue;
                  if (jNow == e[iNow].daughter1() 
		      && jNow == e[iNow].daughter2()) continue;

                  pTnow = pTpowheg(e, iNow, jNow, (pTdefMode == 0) 
				   ? false : FSR);
                  if (pTnow > 0.) pTemt = (pTemt < 0) 
				    ? pTnow : min(pTemt, pTnow);
                } // for (iMem)

              } // if (!FSR)
  
	      // Pythia
	    } else if (pTdefMode == 2) {
  
	      // ISR - other incoming as recoiler
	      if (!FSR) {
		pTnow = pTpythia(e, iInA, jNow, iInB, FSR);
		if (pTnow > 0.) pTemt = (pTemt < 0) ? pTnow : min(pTemt, pTnow);
		pTnow = pTpythia(e, iInB, jNow, iInA, FSR);
		if (pTnow > 0.) pTemt = (pTemt < 0) ? pTnow : min(pTemt, pTnow);
  
		// FSR - try all final-state coloured partons as radiator
		//       after emission (k).
	      } else {
		for (int kNow = 0; kNow < e.size(); kNow++) {
		  if (kNow == jNow || !e[kNow].isFinal()) continue;
  
		  // For this kNow, need to have a recoiler.
		  // Try two incoming.
		  pTnow = pTpythia(e, kNow, jNow, iInA, FSR);
		  if (pTnow > 0.) pTemt = (pTemt < 0) 
				    ? pTnow : min(pTemt, pTnow);
		  pTnow = pTpythia(e, kNow, jNow, iInB, FSR);
		  if (pTnow > 0.) pTemt = (pTemt < 0) 
				    ? pTnow : min(pTemt, pTnow);

		  // Try all other outgoing.
		  for (int rNow = 0; rNow < e.size(); rNow++) {
		    if (rNow == kNow || rNow == jNow ||
			!e[rNow].isFinal()) continue;
		    pTnow = pTpythia(e, kNow, jNow, rNow, FSR);
		    if (pTnow > 0.) pTemt = (pTemt < 0) 
				      ? pTnow : min(pTemt, pTnow);
		  } // for (rNow)
  
		} // for (kNow)
	      } // if (!FSR)
	    } // if (pTdefMode)
	  } // for (j)
	}
      } // for (xSR)

#ifdef DBGOUTPUT
      cout << "pTcalc: i = " << i << ", j = " << j << ", k = " << k
	   << ", r = " << r << ", xSR = " << xSRin
	   << ", pTemt = " << pTemt << endl;
#endif

      return pTemt;
    }

    //--------------------------------------------------------------------------

    // Extraction of pThard based on the incoming event.
    // Assume that all the final-state particles are in a continuous block
    // at the end of the event and the final entry is the POWHEG emission.
    // If there is no POWHEG emission, then pThard is set to SCALUP.

    bool canVetoMPIStep()    { return true; }
    int  numberVetoMPIStep() { return 1; }
    bool doVetoMPIStep(int nMPI, const Event &e) {

      if (nMPI > 1) return false;

      // Find if there is a POWHEG emission. Go backwards through the
      // event record until there is a non-final particle. Also sum pT and
      // find pT_1 for possible MPI vetoing
      int    count = 0;
      double pT1 = 0., pTsum = 0.;
      for (int i = e.size() - 1; i > 0; i--) {
	if (e[i].isFinal()) {
	  count++;
	  pT1    = e[i].pT();
	  pTsum += e[i].pT();
	} else break;
      }
      // Extra check that we have the correct final state
      if (count != nFinal && count != nFinal + 1) {
	cout << "Error: wrong number of final state particles in event" << endl;
	exit(1);
      }
      // Flag if POWHEG radiation present and index
      bool isEmt = (count == nFinal) ? false : true;
      int  iEmt  = (isEmt) ? e.size() - 1 : -1;

      // If there is no radiation or if pThardMode is 0 then set pThard = SCALUP.
      pThard = -1;
      // pThardMode is 0
      if (!isEmt || pThardMode == 0) {
	// This sets the scale to veto emissions in the QCD shower by Pythia
	// This scale is used for all emissions, except if they come from the resonance
	//cout << "SI: in doVetoMPIStep: " << si_event_info_.vetoscale_isr << endl;
	pThard = si_event_info_.vetoscale_isr;
	// Not using directly scalup, because the special file LHE (two scales)
	//      pThard = infoPtr->scalup();
      
	// If pThardMode is 1 then the pT of the POWHEG emission is checked against
	// all other incoming and outgoing partons, with the minimal value taken
      } else if (pThardMode == 1) {
	pThard = pTcalc(e, -1, iEmt, -1, -1, -1);

	// If pThardMode is 2, then the pT of all final-state partons is checked
	// against all other incoming and outgoing partons, with the minimal value
	// taken
      } else if (pThardMode == 2) {
	pThard = pTcalc(e, -1, -1, -1, -1, -1);
      }

      // Find MPI veto pT if necessary
      if (MPIvetoMode == 1) {
	//pTMPI = infoPtr->QFac();
	pTMPI = (isEmt) ? pTsum / 2. : pT1;
      }

#ifdef DBGOUTPUT
      cout << "doVetoMPIStep: Qfac = " << infoPtr->scalup()
	   << ", pThard = " << pThard << endl << endl;
#endif

      // Initialise other variables
      accepted   = false;
      nAcceptSeq = nISRveto = nFSRveto = 0;

      if(pThard < 0)
	{
	  cout << "something wrong with pThard = " << pThard << endl;
	  exit(1);
	}
      //    pThard = infoPtr->QFac();

      // Do not veto the event
      return false;
    }

    //--------------------------------------------------------------------------

    // ISR veto

    bool canVetoISREmission() { return (vetoMode == 0) ? false : true; }
    bool doVetoISREmission(int, const Event &e, int iSys) {

      // Must be radiation from the hard system, otherwise return
      if (iSys != 0) return false;

      // If vetocount != 0 and we already have accepted 'vetoCount' emissions in a row,
      // do nothing; if vetocount = 0 check all emissions
      if (vetoCount != 0 && nAcceptSeq >= vetoCount) return false;

      // Pythia radiator after, emitted and recoiler after.
      int iRadAft = -1, iEmt = -1, iRecAft = -1;
      for (int i = e.size() - 1; i > 0; i--) {
	if      (iRadAft == -1 && e[i].status() == -41) iRadAft = i;
	else if (iEmt    == -1 && e[i].status() ==  43) iEmt    = i;
	else if (iRecAft == -1 && e[i].status() == -42) iRecAft = i;
	if (iRadAft != -1 && iEmt != -1 && iRecAft != -1) break;
      }
      if (iRadAft == -1 || iEmt == -1 || iRecAft == -1) {
	e.list();
	cout << "Error: couldn't find Pythia ISR emission" << endl;
	exit(1);
      }

      // pTemtMode == 0: pT of emitted w.r.t. radiator
      // pTemtMode == 1: min(pT of emitted w.r.t. all incoming/outgoing)
      // pTemtMode == 2: min(pT of all outgoing w.r.t. all incoming/outgoing)
      int xSR      = (pTemtMode == 0) ? 0       : -1;
      int i        = (pTemtMode == 0) ? iRadAft : -1;
      int j        = (pTemtMode != 2) ? iEmt    : -1;
      int k        = -1;
      int r        = (pTemtMode == 0) ? iRecAft : -1;
      double pTemt = pTcalc(e, i, j, k, r, xSR);

#ifdef DBGOUTPUT
      cout << "doVetoISREmission: pTemt = " << pTemt << endl << endl;
#endif

      // Veto if pTemt > pThard
      //cout << "SI In doVetoISREmission with pThard: " << pThard << endl;
      if (pTemt > pThard) {
	nAcceptSeq = 0;
	nISRveto++;
	return true;
      }

      // Else mark that an emission has been accepted and continue
      nAcceptSeq++;
      accepted = true;

      return false;
    }

    //--------------------------------------------------------------------------

    // FSR veto

    bool canVetoFSREmission() { return (vetoMode == 0) ? false : true; }
    bool doVetoFSREmission(int, const Event &e, int iSys, bool inr) {
      // radiation from the hard system: isys=0
      // radiation from resonances: isys!=0 and inr=1
      // MPI radiation: isys!=0 and inr=0

      // we do not veto MPI radiation
      // if we veto here gamma from resonance (inr==1), 
      // we do not have to use canSetResonanceScale 
      if (iSys != 0 && inr != 1) return false;

      // In case of radiation from resonance we veto
      // This is used for ptmaxmatch = 2. 
      // If py8veto = 1, this method is also used to veto photons, otherwise, use external function
      // force the radiation scale, pThard, to be equal to the one set in the LHE file
      if (inr == 1) {
	if ((si_data_.vetoqed == false) || (si_data_.py8veto == false)) {
	  //cout << "SI: not using doVetoFSREmission" << endl;
	  return false;
	}
	else {
	  // Set scale for FSR from the resonance
	  pThard = si_event_info_.vetoscale_fsr;
	  //cout << "SI: Using PYTHIA8 based veto with ptmaxmatch = 2 for FSR, pthard: " << pThard << endl;
	}
      }
      

      // If vetocount != 0 and we already have accepted 'vetoCount' emissions in a row,
      // do nothing; if vetocount = 0 check all emissions
      if (vetoCount != 0 && nAcceptSeq >= vetoCount) return false;

      // Pythia radiator (before and after), emitted and recoiler (after)
      int iRecAft = e.size() - 1;
      int iEmt    = e.size() - 2;
      int iRadAft = e.size() - 3;
      int iRadBef = e[iEmt].mother1();
      if ( (e[iRecAft].status() != 52 && e[iRecAft].status() != -53) ||
	   e[iEmt].status() != 51 || e[iRadAft].status() != 51) {
	e.list();
	cout << "Error: couldn't find Pythia FSR emission" << endl;
	exit(1);
      }

      // Behaviour based on pTemtMode:
      //  0 - pT of emitted w.r.t. radiator before
      //  1 - min(pT of emitted w.r.t. all incoming/outgoing)
      //  2 - min(pT of all outgoing w.r.t. all incoming/outgoing)
      int xSR = (pTemtMode == 0) ? 1       : -1;

      int i   = (pTemtMode == 0) ? iRadBef : -1;
      i = (pTdefMode == 1) ? iRadAft : iRadBef;
      // using POWHEG pT definition i should be iRadAft (daugther)
      int k   = (pTemtMode == 0) ? iRadAft : -1;
      int r   = (pTemtMode == 0) ? iRecAft : -1;

      // When pTemtMode is 0 or 1, iEmt has been selected
      double pTemt = 0.;
      if (pTemtMode == 0 || pTemtMode == 1) {
	// Which parton is emitted, based on emittedMode:
	//  0 - Pythia definition of emitted
	//  1 - Pythia definition of radiated after emission
	//  2 - Random selection of emitted or radiated after emission
	//  3 - Try both emitted and radiated after emission

	// j = radiator after

	int j = iRadAft;
	//emittedMode = 0 -> j = iRadAft + 1 = iEmt 
	if (emittedMode == 0 || (emittedMode == 2 && rndmPtr->flat() < 0.5)) j++;

	for (int jLoop = 0; jLoop < 2; jLoop++) {
	  if      (jLoop == 0) pTemt = pTcalc(e, i, j, k, r, xSR);
	  else if (jLoop == 1) pTemt = min(pTemt, pTcalc(e, i, j, k, r, xSR));
  
	  // For emittedMode == 3, have tried iRadAft, now try iEmt
	  if (emittedMode != 3) break;
	  if (k != -1) swap(j, k); else j = iEmt;
	}

	// If pTemtMode is 2, then try all final-state partons as emitted
      } else if (pTemtMode == 2) {
	pTemt = pTcalc(e, i, -1, k, r, xSR);
      }

#ifdef DBGOUTPUT
      cout << "doVetoFSREmission: pTemt = " << pTemt << endl << endl;
#endif

      // Veto if pTemt > pThard
      //cout << "SI In doVetoFSREmission with pThard: " << pThard << endl;
      if (pTemt > pThard) {
	nAcceptSeq = 0;
	nFSRveto++;
	return true;
      }

      // Else mark that an emission has been accepted and continue
      nAcceptSeq++;
      accepted = true;
      return false;
    }

    //--------------------------------------------------------------------------

    // MPI veto

    bool canVetoMPIEmission() { return (MPIvetoMode == 0) ? false : true; }
    bool doVetoMPIEmission(int, const Event &e) {

      if (MPIvetoMode == 1) {
	if (e[e.size() - 1].pT() > pTMPI) {
#ifdef DBGOUTPUT
	  cout << "doVetoMPIEmission: pTnow = " << e[e.size() - 1].pT()
	       << ", pTMPI = " << pTMPI << endl << endl;
#endif
	  return true;
	}
      }
      return false;
    }

    //--------------------------------------------------------------------------

    // Functions to return information

    int    getNISRveto() { return nISRveto; }
    int    getNFSRveto() { return nFSRveto; }

  private:
    int    nFinal, vetoMode, vetoCount, pThardMode, pTemtMode,
					   emittedMode, pTdefMode, MPIvetoMode;
    double pThard, pTMPI;
    bool   accepted;
    // The number of accepted emissions (in a row)
    int nAcceptSeq;
    // Statistics on vetos
    unsigned long int nISRveto, nFSRveto;
    si_data_type si_data_;
    si_event_info_type si_event_info_;
  };

} // end namespace Pythia8

