/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// PowhegHooksBB4L.h 
// Copyright (C) 2017 Silvia Ferrario Ravasio, Tomas Jezo, Paolo Nason, Markus Seidel
// based on PowhegHooks.h by Richard Corke
// Adapted to be Athena and Pythia8_i compliant by Simone Amoroso (amoroso@cern.ch)

#include "Pythia8/Pythia.h"
#include "UserHooksUtils.h"
#include "Pythia8_i/UserHooksFactory.h"
#include "Pythia8Plugins/PowhegHooks.h"
#include "UserSetting.h"
#include <iostream>
#include <cassert>
#include "boost/lexical_cast.hpp"
#include <stdexcept>

struct{
  int radtype;
} radtype_;




namespace Pythia8 { class PowhegBB4Ltms; }
Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::PowhegBB4Ltms> powhegBB4LtmsCreator("PowhegBB4Ltms");



namespace Pythia8 {  
  using namespace std;

  //==========================================================================
  // Use userhooks to veto PYTHIA emissions above the POWHEG scale.
  class PowhegBB4Ltms : public PowhegHooks {    
  public:
    
    // Constructor
    PowhegBB4Ltms()
      :	m_debug("Powheg:bb4l:DEBUG", false),
	m_vetoFSREmission("Powheg:bb4l:FSREmission:veto", true),
	m_dryRunFSR("Powheg:bb4l:dryRunFSR", false),
	m_onlyDistance1("Powheg:bb4l:onlyDistance1", false),
	m_vetoAtPL("Powheg:bb4l:vetoAtPL", false),
	m_vetoQED("Powheg:bb4l:vetoQED", false),
	m_vetoPartonLevel("Powheg:bb4l:PartonLevel:veto", 0),
        m_wouldVetoFsr(false), 
        m_topresscale(-1.), m_vetoDecScale(-1.), m_atopresscale(-1.), 
	m_nISRveto(0), m_nFSRveto(0), nFSRvetoBB4l(0),
	m_pTmin("Powheg:bb4l:pTminVeto", 0.5),
        m_vetoTopCharge(-1),
	m_vetoProduction("Powheg:veto", 0),
	m_pTpythiaVeto("Powheg:bb4l:pTpythiaVeto", 0),
	m_vetoDipoleFrame("Powheg:bb4l:FSREmission:vetoDipoleFrame", 0),
	m_scaleResonanceVeto("Powheg:bb4l:ScaleResonance:veto", 0.)
    {
      std::cout << "**********************************************************" << std::endl;
      std::cout << "*                                                        *" << std::endl;
      std::cout << "*           Applying Powheg BB4L UserHook!               *" << std::endl;
      std::cout << "*       Must run on dedicated Powheg LHE input file      *" << std::endl;
      std::cout << "*           (on your own responsibility)                 *" << std::endl;
      std::cout << "*                                                        *" << std::endl;
      std::cout << "**********************************************************" << std::endl;

    }


    ~PowhegBB4Ltms() override { std::cout << "Number of FSR vetoed in BB4l = " << nFSRvetoBB4l << std::endl; }


    //Initialization
    bool initAfterBeams() {
      // settings of the parent class
      PowhegHooks::initAfterBeams();
      // settings of this class
      return true;
    }


    //--- PROCESS LEVEL HOOK ---------------------------------------------------
    // called at the LHE level
    inline bool canVetoProcessLevel() { return true; }
    inline bool doVetoProcessLevel(Event &e) {
      /////////////
      // count myself final particles ....
      //int    count = 0;
      //int nFinal = settingsPtr->mode("POWHEG:nFinal");
      //double pT1 = 0., pTsum = 0.;
      //for (int i = e.size() - 1; i > 0; i--) {
      //	if (e[i].isFinal()) {
      //    count++;
      //    pT1    = e[i].pT();
      //    pTsum += e[i].pT();
      //  } else break;
      //}
      //// Extra check that we have the correct final state                                                                                                     
      //std::cout << "INFO: counted final particles is "<< count<< "; nFinal is "<<nFinal << std::endl;
      //    }
      
      ////////////    
      
      
      // extract the radtype from the event comment
      stringstream ss;
      ss << infoPtr->getEventComments();
      string temp;
      ss >> temp >> radtype_.radtype;
      assert (temp == "#rwgt");

      // find last top and the last anti-top in the record
      int i_top = -1, i_atop = -1;
      for (int i = 0; i < e.size(); i++) {
	if (e[i].id() == 6) i_top = i;
	if (e[i].id() == -6) i_atop = i;
      }
      if (i_top != -1) 
	m_topresscale = findresscale(i_top, e);
      else 
	m_topresscale = 1e30;
      if (i_top != -1) 
	m_atopresscale = findresscale(i_atop, e);
      else 
	m_atopresscale = 1e30;
      // initialize stuff
      doVetoFSRInit();
      // do not veto, ever
      return false;
    }


    //--- PARTON LEVEL HOOK ----------------------------------------------------
    // called after shower
    bool retryPartonLevel() { return m_vetoPartonLevel(settingsPtr) || m_vetoAtPL(settingsPtr); }
    inline bool canVetoPartonLevel() { return m_vetoPartonLevel(settingsPtr) || m_vetoAtPL(settingsPtr); }
    inline bool doVetoPartonLevel(const Event &e) {
      if(radtype_.radtype==2)
	return false;
      if(m_debug(settingsPtr)){
	if (m_dryRunFSR(settingsPtr) && m_wouldVetoFsr) {
	  double scale = getdechardness(m_vetoTopCharge, e);
	  std::cout << "FSRdecScale = " << m_vetoDecScale << ", PLdecScale = " << scale << ", ratio = " << m_vetoDecScale/scale << std::endl;
	}
      }
      if (m_vetoPartonLevel(settingsPtr)) {
	double topdecscale = getdechardness(1, e);
	double atopdecscale = getdechardness(-1, e);
	if ((topdecscale > m_topresscale) || (atopdecscale > m_atopresscale)) {
	  return true;
	}
	else 
	  return false;
      }
      if (m_vetoAtPL(settingsPtr)) {
	if (m_dryRunFSR(settingsPtr) && m_wouldVetoFsr) return true;
	else return false;
      }
      return false;
    }
    
    

    //--- Internal helper functions --------------------------------------------
    // Calculates the scale of the hardest emission from within the resonance system
    // translated by Markus Seidel modified by Tomas Jezo
    inline double findresscale( const int iRes, const Event& event) {
      double scale = 0.;
      
      int nDau = event[iRes].daughterList().size();
      
      if (nDau == 0) {
	// No resonance found, set scale to high value
	// Pythia will shower any MC generated resonance unrestricted
	scale = 1e30;
      }
      else if (nDau < 3) {
	// No radiating resonance found
	scale =	m_pTmin(settingsPtr);
      }
      else if (abs(event[iRes].id()) == 6) {
	// Find top daughters
	int idw = -1, idb = -1, idg = -1;
	
	for (int i = 0; i < nDau; i++) {
	  int iDau = event[iRes].daughterList()[i];
	  if (abs(event[iDau].id()) == 24) idw = iDau;
	  if (abs(event[iDau].id()) ==  5) idb = iDau;
	  if (abs(event[iDau].id()) == 21) idg = iDau;
	}
	
	// Get daughter 4-vectors in resonance frame
	Vec4 pw(event[idw].p());
	pw.bstback(event[iRes].p());
	
	Vec4 pb(event[idb].p());
	pb.bstback(event[iRes].p());
	
	Vec4 pg(event[idg].p());
	pg.bstback(event[iRes].p());
	
	// Calculate scale
	scale = sqrt(2*pg*pb*pg.e()/pb.e());
      }
      else {
	scale = 1e30;
      }
      
      return scale;
    }
    
    // The following routine will match daughters of particle `e[iparticle]` 
    // to an expected pattern specified via the list of expected particle PDG ID's `ids`, 
    // id wildcard is specified as 0 if match is obtained, the positions and the momenta
    // of these particles are returned in vectors `positions` and `momenta` respectively
    // if exitOnExtraLegs==true, it will exit if the decay has more particles than expected, but not less
    inline bool match_decay(int iparticle, const Event &e, const vector<int> &ids,
			    vector<int> &positions, vector<Vec4> &momenta, bool exitOnExtraLegs = true){
      // compare sizes
      if (e[iparticle].daughterList().size() != ids.size()) {
	if (exitOnExtraLegs && e[iparticle].daughterList().size() > ids.size()) {
	  std::cout << "extra leg" << std::endl;
	  exit(-1);
	}
	return false; 
      }
      
      // compare content
      for (unsigned int i = 0; i < e[iparticle].daughterList().size(); i++) {
	int di = e[iparticle].daughterList()[i];
	if (ids[i] != 0 && e[di].id() != ids[i]) 
	  return false;
      }
      // reset the positions and momenta vectors (because they may be reused)
      positions.clear();
      momenta.clear();
      // construct the array of momenta
      for (unsigned int i = 0; i < e[iparticle].daughterList().size(); i++) {
	int di = e[iparticle].daughterList()[i];
	positions.push_back(di);
	momenta.push_back(e[di].p());
      }
      return true;
    }
    
    inline double qSplittingScale(Vec4 pt, Vec4 p1, Vec4 p2){
      p1.bstback(pt);
      p2.bstback(pt);
      return sqrt( 2*p1*p2*p2.e()/p1.e() );
    }
    
    inline double gSplittingScale(Vec4 pt, Vec4 p1, Vec4 p2){
      p1.bstback(pt);
      p2.bstback(pt);
      return sqrt( 2*p1*p2*p1.e()*p2.e()/(pow(p1.e()+p2.e(),2)) );
    }
    // Routines to calculate the pT (according to pTdefMode) in a FS splitting:
    // i (radiator before) -> j (emitted after) k (radiator after)
    // For the Pythia pT definition, a recoiler (after) must be specified.
    // (INSPIRED BY pythia8F77_31.cc double pTpythia)
    inline double pTpythia(const Event &e, int RadAfterBranch, int EmtAfterBranch,
			   int RecAfterBranch)
    {
      
      // Convenient shorthands for later
      Vec4 radVec = e[RadAfterBranch].p();
      Vec4 emtVec = e[EmtAfterBranch].p();
      Vec4 recVec = e[RecAfterBranch].p();
      int  radID  = e[RadAfterBranch].id();
      
      // Calculate virtuality of splitting
      Vec4 Q(radVec + emtVec);
      double Qsq = Q.m2Calc();
      
      
      // Mass term of radiator
      double m2Rad = (abs(radID) >= 4 && abs(radID) < 7) ?
	pow2(particleDataPtr->m0(radID)) : 0.;
      
      // z values for FSR 
      double z, pTnow;
      // Construct 2 -> 3 variables
      Vec4 sum = radVec + recVec + emtVec;
      double m2Dip = sum.m2Calc();
      double x1 = 2. * (sum * radVec) / m2Dip;
      double x3 = 2. * (sum * emtVec) / m2Dip;
      z     = x1 / (x1 + x3);
      pTnow = z * (1. - z);
      
      // Virtuality
      pTnow *= (Qsq - m2Rad);
      
      if (pTnow < 0.) {
	infoPtr->errorMsg("Warning: pTpythia was negative");
	return -1.;
      }
      else
	return(sqrt(pTnow));
    }


    inline double getdechardness(int topcharge, const Event &e){
      int tid = 6*topcharge, wid = 24*topcharge, bid = 5*topcharge, gid = 21, wildcard = 0;
      // find last top in the record
      int i_top = -1;
      Vec4 p_top, p_b, p_g, p_g1, p_g2;
      for (int i = 0; i < e.size(); i++) 
	if (e[i].id() == tid) {
	  i_top = i;
	  p_top = e[i].p();
	}
      if (i_top == -1) return -1.0;
      
      // summary of cases
      // 1.) t > W b
      //   a.) b > 3     ... error
      //   b.) b > b g   ... h = sqrt(2*p_g*p_b*p_g.e()/p_b.e())
      //   c.) b > other ... h = -1
      //   return h
      // 2.) t > W b g
      //   a.)   b > 3     ... error
      //   b.)   b > b g   ... h1 = sqrt(2*p_g*p_b*p_g.e()/p_b.e())
      //   c.)   b > other ... h1 = -1
      //   i.)   g > 3     ... error
      //   ii.)  g > 2     ... h2 = sqrt(2*p_g1*p_g2*p_g1.e()*p_g2.e()/(pow(p_g1.e(),2)+pow(p_g2.e(),2))) );
      //   iii.) g > other ... h2 = -1
      //   return max(h1,h2)
      // 3.) else ... error
      
      vector<Vec4> momenta;
      vector<int> positions;
      
      // 1.) t > b W
      if ( match_decay(i_top, e, vector<int> {wid, bid}, positions, momenta, false) ) {
	double h;
	int i_b = positions[1];
	// a.+b.) b > 3 or b > b g 
	if ( match_decay(i_b, e, vector<int> {bid, gid}, positions, momenta) )
	  h = qSplittingScale(e[i_top].p(), momenta[0], momenta[1]);
	// c.) b > other
	else 
	  h = -1;
	return h;
      } 
      // 2.) t > b W g
      else if ( match_decay(i_top, e, vector<int> {wid, bid, gid}, positions, momenta, false) ) {
	double h1, h2;
	int i_b = positions[1], i_g = positions[2];
	// a.+b.) b > 3 or b > b g
	if ( match_decay(i_b, e, vector<int> {bid, gid}, positions, momenta) )
	  h1 = qSplittingScale(e[i_top].p(), momenta[0], momenta[1]);
	// c.) b > other
	else 
	  h1 = -1;
	// i.+ii.) g > 3 or g > 2
	if ( match_decay(i_g, e, vector<int> {wildcard, wildcard}, positions, momenta) )
	  h2 = gSplittingScale(e[i_top].p(), momenta[0], momenta[1]);
	// c.) b > other
	else 
	  h2 = -1;
	return max(h1, h2);
      }
      // 3.) else
      else { 
	std::cout << "getdechardness" << std::endl;
	std::cout << "top at position " << i_top << std::endl;
	std::cout << "with " << e[i_top].daughterList().size() << " daughters " << std::endl;
	for (unsigned int i = 0; i < e[i_top].daughterList().size(); i++) {
	  int di = e[i_top].daughterList()[i];
	  std::cout << "with daughter " << di << ": " << e[di].id() << std::endl;
	}
	exit(-1);
      }
    }



    //--- SCALE RESONANCE HOOK -------------------------------------------------
    // called before each resonance decay shower
    inline bool canSetResonanceScale() { return m_scaleResonanceVeto(settingsPtr); }
    // if the resonance is the (anti)top set the scale to:
    //  ---> (anti)top virtuality if radtype=2
    //  ---> (a)topresscale otherwise
    // if is not the top, set it to a big number
    inline double scaleResonance(int iRes, const Event &e) {
      if (e[iRes].id() == 6){
	if(radtype_.radtype == 2)
	  return sqrt(e[iRes].m2Calc());
	else
	  return m_topresscale;
      }
      else if (e[iRes].id() == -6){
	if(radtype_.radtype == 2)
	  return sqrt(e[iRes].m2Calc());
	else
	  return m_atopresscale;
      }
      else
	return pow(10.0,30.);
    }
    
    
    //--- FSR EMISSION LEVEL HOOK ----------------------------------------------
  
    // FSR veto: this should be true if we want PowhegHooksBB4l veto in decay
    //           OR PowhegHooks veto in production. (The virtual method
    //           PowhegHooks::canVetoFSREmission has been replaced by
    //           PowhegHooksBB4L::canVetoFSREmission, so FSR veto in production
    //           must be handled here. ISR and MPI veto are instead still
    //           handled by PowhegHooks.)
    virtual inline bool canVetoFSREmission() { return m_vetoFSREmission(settingsPtr) || m_vetoProduction(settingsPtr); }
    virtual inline bool doVetoFSREmission(int sizeOld, const Event &e, int iSys, bool inResonance) {
      //////////////////////////////
      //VETO INSIDE THE RESONANCE //
      //////////////////////////////
      if (inResonance && m_vetoFSREmission(settingsPtr)) {

	// get the participants of the splitting: the recoiler, the radiator and the emitted 
	int iRecAft = e.size() - 1;
	int iEmt = e.size() - 2;
	int iRadAft = e.size() - 3;
	int iRadBef = e[iEmt].mother1();
	
	// find the top resonance the radiator originates from
	int iTop = e[iRadBef].mother1();
	int distance = 1;
	while (abs(e[iTop].id()) != 6 && iTop > 0) {
	  iTop = e[iTop].mother1();
	  distance ++;
	}
	if (iTop == 0) {
	  infoPtr->errorMsg("Warning in PowhegHooksBB4L::doVetoFSREmission: emission in resonance not from top quark, not vetoing");
	  return doVetoFSR(false,0,0);
	}
	int iTopCharge = (e[iTop].id()>0)?1:-1;
	
	// calculate the scale of the emission
	double scale;
	//using pythia pT definition ...
	if(m_pTpythiaVeto(settingsPtr))
	  scale = pTpythia(e, iRadAft, iEmt, iRecAft);
	else{	//.. or using POWHEG pT definition
	  Vec4 pr(e[iRadAft].p()), pe(e[iEmt].p()), pt(e[iTop].p()), prec(e[iRecAft].p()), psystem;
	  // The computation of the POWHEG pT can be done in the top rest frame or in the dipole one.
	  // pdipole = pemt +prec +prad (after the emission)
	  // For the first emission off the top resonance pdipole = pw +pb (before the emission) = ptop
	  if(m_vetoDipoleFrame(settingsPtr))
	    psystem = pr+pe+prec;
	  else
	    psystem = pt;
	  
	  // gluon splitting into two partons
	  if (e[iRadBef].id() == 21)
	    scale = gSplittingScale(psystem, pr, pe);
	  // quark emitting a gluon (or a photon)
	  else if (abs(e[iRadBef].id()) == 5 && ((e[iEmt].id() == 21) && ! m_vetoQED(settingsPtr)) )
	    scale = qSplittingScale(psystem, pr, pe);
	  // other stuff (which we should not veto)
	  else {
	    scale = 0;
	  }
	}
	
	if (iTopCharge > 0) {
	  if (m_onlyDistance1(settingsPtr)) {
	    if ( m_debug(settingsPtr) && (distance == 1) && scale > m_topresscale && ! m_wouldVetoFsr)
	      std::cout << e[iTop].id() << ": " << e[iRadBef].id() << " > " << e[iRadAft].id() << " + " << e[iEmt].id() << "; " << scale << std::endl;
	    return doVetoFSR((distance == 1) && scale > m_topresscale,scale,iTopCharge);
	  }
	  else {
	    if ( m_debug(settingsPtr) && scale > m_topresscale && ! m_wouldVetoFsr)
	      std::cout << e[iTop].id() << ": " << e[iRadBef].id() << " > " << e[iRadAft].id() << " + " << e[iEmt].id() << "; " << scale << std::endl;
	    return doVetoFSR(scale > m_topresscale,scale,iTopCharge);
	  }
	}
	else if (iTopCharge < 0){
	  if (m_onlyDistance1(settingsPtr)){
	    if ( m_debug(settingsPtr) && (distance == 1) && scale > m_atopresscale && ! m_wouldVetoFsr)
	      std::cout << e[iTop].id() << ": " << e[iRadBef].id() << " > " << e[iRadAft].id() << " + " << e[iEmt].id() << "; " << scale << std::endl;
	    return doVetoFSR((distance == 1) && scale > m_atopresscale,scale,iTopCharge);
	  }
	  else {
	    if ( m_debug(settingsPtr) && scale > m_topresscale && ! m_wouldVetoFsr)
	      std::cout << e[iTop].id() << ": " << e[iRadBef].id() << " > " << e[iRadAft].id() << " + " << e[iEmt].id() << "; " << scale << std::endl;
	    return doVetoFSR(scale > m_atopresscale,scale,iTopCharge);
	  }
	}
	else {
	  std::cout << "Bug in PohwgeHooksBB4l" << std::endl;
	}
      }

      /////////////////////////////////
      // VETO THE PRODUCTION PROCESS //
      /////////////////////////////////
      else if(!inResonance && m_vetoProduction(settingsPtr)){
	return PowhegHooks::doVetoFSREmission(sizeOld, e, iSys, inResonance);
      }
      return 0;
    }
    
    
    
    inline bool doVetoFSR(bool condition, double scale, int iTopCharge)  {
      if(radtype_.radtype==2)
	return false;
      if (condition) {
	if (!m_wouldVetoFsr) {
	  m_wouldVetoFsr = true;
	  m_vetoDecScale = scale;
	  m_vetoTopCharge = iTopCharge;
	}
	if (m_dryRunFSR(settingsPtr)) return false;
	else {
          nFSRvetoBB4l++;
	  return true;
	}
      }
      else return false;
    }
    
    inline void doVetoFSRInit() {
      m_wouldVetoFsr = false;
      m_vetoDecScale = -1;
      m_vetoTopCharge = 0;
    }
    
    
  private:


    Pythia8_UserHooks::UserSetting<bool> m_debug;
    Pythia8_UserHooks::UserSetting<bool> m_vetoFSREmission, m_dryRunFSR, m_onlyDistance1, m_vetoAtPL, m_vetoQED;
    Pythia8_UserHooks::UserSetting<int> m_vetoPartonLevel;
    bool m_wouldVetoFsr;
    double m_topresscale,  m_vetoDecScale, m_atopresscale;
    unsigned long int m_nISRveto, m_nFSRveto;
    unsigned long int nFSRvetoBB4l;

    Pythia8_UserHooks::UserSetting<double> m_pTmin;
    int m_vetoTopCharge;
    Pythia8_UserHooks::UserSetting<int> m_vetoProduction, m_pTpythiaVeto, m_vetoDipoleFrame;
    Pythia8_UserHooks::UserSetting<double>  m_scaleResonanceVeto;

  };
  
  //==========================================================================
  
} // end namespace Pythia8
