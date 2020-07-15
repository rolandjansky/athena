/*
  Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration
*/

#include "UserHooksUtils.h"
#include "UserSetting.h"
#include "Pythia8_i/UserHooksFactory.h"
#include "boost/lexical_cast.hpp"
#include "boost/bind.hpp"
#include <boost/math/tools/roots.hpp>
#include <math.h>
#include <stdexcept>
#include <iostream>

namespace Pythia8{
  class DecayToSUEP;
}

Pythia8_UserHooks::UserHooksFactory::Creator<Pythia8::DecayToSUEP> DecayToSUEPCreator("DecayToSUEP");

#define SUEP_DEBUG 1

namespace Pythia8{

  //******************************
  //** Main UserHook derived class
  //******************************

  /** Pythia8 UserHook to decay a given scalar particle to SUEP. 
   *
   * Details on models available on arXiv:1612.00850.
   * This is an adaption from the available public code at: 
   * https://gitlab.com/simonknapen/suep_generator 
   * by Simon Knapen.
   *
   */
  class DecayToSUEP : public UserHooks{
    
  public:
    
    DecayToSUEP(): 
      m_pdgId("DecayToSUEP:PDGId", 25), 
      m_mass("DecayToSUEP:Mass", 125.0),
      m_darkMesonMass("DecayToSUEP:DarkMesonMass", 1.), 
      m_darkTemperature("DecayToSUEP:DarkTemperature", 1.) {
      
      std::cout<<"**********************************************************"<<std::endl;
      std::cout<<"*                                                        *"<<std::endl;
      std::cout<<"*             Enabled SUEP decay UserHook!               *"<<std::endl;
      std::cout<<"*                                                        *"<<std::endl;
      std::cout<<"**********************************************************"<<std::endl;
      
    }
    
    ~DecayToSUEP(){}

    // Enable the call to the user-hook
    virtual bool canVetoProcessLevel() {
      return true;
    }

    /* Actually implement the user hook.
     *
     * We slightly abuse this function in the sense that no veto is performed but instead
     * the hook is used to modify the event record in between the process-level and parton-level steps.
     * This modification is allowed in the Pythia8 manual, with warning of event consistency that
     * should pose no harm in this context.
     */
    virtual bool doVetoProcessLevel(Event& process);


  private:

    /** PDG Id of particle to be decayed to SUEP */
    Pythia8_UserHooks::UserSetting<int> m_pdgId;

    /** Mass of system decaying [GeV] */
    Pythia8_UserHooks::UserSetting<double> m_mass;

    /** Dark-meson mass parameter [GeV] */
    Pythia8_UserHooks::UserSetting<double> m_darkMesonMass;

    /** Temperature parameter [GeV] */
    Pythia8_UserHooks::UserSetting<double> m_darkTemperature;

  };

  //******************************
  //** Auxiliary class for SUEP generation
  //******************************
  class tolerance {
  public:
    tolerance(double eps) :
      _eps(eps) {
    }
    bool operator()(double a, double b) {
      return (fabs(b - a) <= _eps);
    }
  private:
    double _eps;
  };

  class Suep_shower 
  { 
  public: 
    
    // Data Members 
    double m;
    double Temp;
    double Etot; 
    
    // Constructor
    Suep_shower(double mass, double temperature, double energy);
    
    // methods
    double f(double p);
    double fp(double p);
    double test_fun(double p);
    std::vector<double> generate_fourvector(); 
    std::vector< std::vector <double> > generate_shower();
    double reballance_func(double a, std::vector< std::vector <double> > event);
    
  private:
    // private variables
    double A;
    double p_m;
    std::pair <double,double> p_plusminus;
    double p_plus, p_minus;
    double lambda_plus,lambda_minus;
    double q_plus, q_minus, q_m;
    
  }; 

  using namespace boost::math::tools;           // For bracket_and_solve_root.
  // constructor
  Suep_shower::Suep_shower(double mass, double temperature, double energy) {
    m = mass;
    Temp=temperature;
    Etot=energy;
    
    A=m/Temp;
    p_m=sqrt(2/(A*A)*(1+sqrt(1+A*A)));
    
    double pmax=sqrt(2+2*sqrt(1+A*A))/A; // compute the location of the maximum, to split the range 
        
    tolerance tol = 0.00001;
    //TODO: remove boost dependency if possible
    p_plus = (bisect(boost::bind(&Suep_shower::test_fun, this, _1),pmax,50.0, tol)).first; // first root
    p_minus = (bisect(boost::bind(&Suep_shower::test_fun, this, _1), 0.0,pmax, tol)).first; // second root
    lambda_plus = - f(p_plus)/fp(p_plus);
    lambda_minus = f(p_minus)/fp(p_minus);
    q_plus = lambda_plus / (p_plus - p_minus);
    q_minus = lambda_minus / (p_plus - p_minus);
    q_m = 1- (q_plus + q_minus);
    
  }

  // maxwell-boltzman distribution, slightly massaged
  double  Suep_shower::f(double p){
    return p*p*exp(-A*p*p/(1+sqrt(1+p*p)));
  }

  // derivative of maxwell-boltzmann
  double  Suep_shower::fp(double p){
    return exp(-A*p*p/(1+sqrt(1+p*p)))*p*(2-A*p*p/sqrt(1+p*p));
  }

  // test function to be solved for p_plusminus
  double  Suep_shower::test_fun(double p){
    return log(f(p)/f(p_m))+1.0;
  }

  // generate one random 4 vector from the thermal distribution
  vector<double> Suep_shower::generate_fourvector(){
    
    vector<double> fourvec;
    double en, phi, theta, p;//kinematic variables of the 4 vector
    
    // first do momentum, following arxiv:1305.5226
    double U, V, X, Y, E;
    int i=0;      
    while(i<100){
      U = ((double) rand() / RAND_MAX);
      V = ((double) rand() / RAND_MAX);
        
      if(U < q_m){
	Y=U/q_m;
	X=( 1 - Y )*( p_minus + lambda_minus )+Y*( p_plus - lambda_plus );
	if(V < f(X) / f(p_m) && X>0){
	  break;
	}
      }
      else{if(U < q_m + q_plus){
	  E = -log((U-q_m)/q_plus);
	  X = p_plus - lambda_plus*(1-E);
	  if(V<exp(E)*f(X)/f(p_m) && X>0){
	    break;
	  }
	}
	else{
	  E = - log((U-(q_m+q_plus))/q_minus);
	  X = p_minus + lambda_minus * (1 - E);
	  if(V < exp(E)*f(X)/f(p_m) && X>0){
	    break;
	  }
	}
      }
    }
    p=X*(this->m); // X is the dimensionless momentum, p/m
    
    // now do the angles
    phi = 2.0*M_PI*((double) rand() / RAND_MAX);
    theta = acos(2.0*((double) rand() / RAND_MAX)-1.0);
    
    // compose the 4 vector
    en = sqrt(p*p+(this->m)*(this->m));
    fourvec.push_back(en);
    fourvec.push_back(p*cos(phi)*sin(theta));
    fourvec.push_back(p*sin(phi)*sin(theta));
    fourvec.push_back(p*cos(theta));
    
    return fourvec; 
  }

  // auxiliary function which computes the total energy difference as a function of the momentum vectors and a scale factor "a"
  // to ballance energy, we solve for "a" by demanding that this function vanishes
  // By rescaling the momentum rather than the energy, I avoid having to do these annoying rotations from the previous version 
  double Suep_shower::reballance_func(double a, vector< vector <double> > event){
    double result =0.0;
    double p2;
    for(unsigned n = 0; n<event.size();n++){
      p2 = event[n][1]*event[n][1] + event[n][2]*event[n][2] + event[n][3]*event[n][3];
      result += sqrt(a*a*p2 + (this->m)* (this->m));
    }
    return result - (this->Etot);
  }

  // generate a shower event, in the rest frame of the shower
  vector< vector <double> > Suep_shower::generate_shower(){
    
    vector<vector<double> > event;
    double sum_E = 0.0;
    
    // fill up event record
    while(sum_E<(this->Etot)){
      event.push_back(this->generate_fourvector());
      sum_E += (event.back()).at(0);     
    }
    
    // reballance momenta
    int len = event.size();
    double sum_p, correction;
    for(int i = 1;i<4;i++){ // loop over 3 carthesian directions
        
      sum_p = 0.0;
      for(int n=0;n<len;n++){
	sum_p+=event[n][i];
      }
      correction=-1.0*sum_p/len;
        
      for(int n=0;n<len;n++){
	event[n][i] += correction;
      } 
    }
    
    // finally, ballance the total energy, without destroying momentum conservation
    tolerance tol = 0.00001;
    double p_scale;
    p_scale = (bisect(boost::bind(&Suep_shower::reballance_func, this, _1, event),0.0,2.0, tol)).first;
    
    for(int n=0;n<len;n++){
      event[n][1] = p_scale*event[n][1];
      event[n][2] = p_scale*event[n][2];
      event[n][3] = p_scale*event[n][3];
      // force the energy with the on-shell condition
      event[n][0] = sqrt(event[n][1]*event[n][1] + event[n][2]*event[n][2] + event[n][3]*event[n][3] + (this->m)*(this->m));
    }
    
    return event;    
  }

  //******************************
  //** Implementation of DecayToSUEP UserHook
  //******************************
  bool DecayToSUEP::doVetoProcessLevel(Event& process)
  {
#ifdef SUEP_DEBUG
    std::cout << "[SUEP_DEBUG] " << "Start of user hook for this event." << std::endl;
#endif

    //First, find the particle to decay
    bool particleFound=false;

#ifdef SUEP_DEBUG
    for (int ii=0; ii < process.size(); ++ii) {
      std::cout << "[SUEP_DEBUG] " << ii << ": id=" << process[ii].id() << ", Final=" << process[ii].isFinal() << ", Status=" << process[ii].status() << ", daughter1=" << process[ii].daughter1() << ", daughter2=" << process[ii].daughter2() << std::endl;
    }
#endif

    for (int ii=0; ii < process.size(); ++ii) {
      //      if ( (process[ii].id() == m_pdgId(settingsPtr)) and (process[ii].isFinal()) ) {
      if ( (process[ii].id() == m_pdgId(settingsPtr)) and (process[ii].daughter1()!=process[ii].daughter2() && process[ii].daughter1()>0 && process[ii].daughter2()>0) ) {

	Vec4 higgs4mom, mesonmom;
	vector< vector <double> > suep_shower4momenta;	
	particleFound=true;

	//setup SUEP shower
	//TODO: could get m_mass later from event record if not defined as input, need to massage initialization function of Seup_shower class though
	static Suep_shower suep_shower(m_darkMesonMass(settingsPtr), m_darkTemperature(settingsPtr), m_mass(settingsPtr));

#ifdef SUEP_DEBUG
	std::cout << "[SUEP_DEBUG] " << "Particle (pdgId=" << m_pdgId(settingsPtr) << ", isFinal=True) found. Decaying to SUEP now." << std::endl;
#endif

	// First undo decay
	process[ii].undoDecay();

	int originalEventSize = process.size();

	// Generate the shower, output are 4 vectors in the rest frame of the shower
	higgs4mom=process[ii].p();
	suep_shower4momenta=suep_shower.generate_shower();

	// Loop over hidden sector mesons and append to the event	
	for (unsigned j = 0; j < suep_shower4momenta.size(); ++j){
	  //construct pythia 4vector
	  mesonmom.p(suep_shower4momenta[j][1],suep_shower4momenta[j][2],suep_shower4momenta[j][3],suep_shower4momenta[j][0]);
            
	  // boost to the lab frame
	  mesonmom.bst(higgs4mom.px()/higgs4mom.e(),higgs4mom.py()/higgs4mom.e(), higgs4mom.pz()/higgs4mom.e());
            
	  //append particle to the event. Hidden/dark meson pdg code is 999999.
	  process.append(999999, 91, ii, 0, 0, 0, 0, 0, mesonmom.px(), mesonmom.py(), mesonmom.pz(), mesonmom.e(), m_darkMesonMass(settingsPtr)); 

#ifdef SUEP_DEBUG
	  std::cout << "[SUEP_DEBUG] " << "Adding dark meson with px=" << mesonmom.px() << ", py=" << mesonmom.py() << ", pz=" << mesonmom.pz() << ", m=" << m_darkMesonMass(settingsPtr) << std::endl;
#endif
	}

	// Just to be sure, only modify Higgs status and daughters if a valid decay did happen
	if ( suep_shower4momenta.size() > 0 ) {
#ifdef SUEP_DEBUG
	  std::cout << "[SUEP_DEBUG] " << "Setting original particle status-code as non-Final particle. Adding daughters with indices: " << originalEventSize << " - " << process.size()-1 << std::endl;
#endif
	  // Change the status code of the Higgs to reflect that it has decayed.
	  process[ii].statusNeg();
          
	  //set daughters of the Higgs. Take advantage that we just appended them
	  process[ii].daughters(originalEventSize, process.size()-1); 
	}
	
	//no need to continue the loop
	break;

      } // if particle to decay found

    } // loop over particles in the event

#ifdef SUEP_DEBUG
    if (not particleFound) {
      std::cout << "[SUEP_DEBUG] " << "Particle " << m_pdgId(settingsPtr) << " not found. Nothing to decay to SUEP for this event." << std::endl;
    } else {
      std::cout << "[SEUP_DEBUG] " << "All Done for this event." << std::endl;
    }
#endif

#ifdef SUEP_DEBUG
    std::cout << "[SUEP_DEBUG] Printing event after adding SUEP:" << std::endl;
    for (int ii=0; ii < process.size(); ++ii) {
      std::cout << "[SUEP_DEBUG] " << ii << ": id=" << process[ii].id() << ", Final=" << process[ii].isFinal() << ", mayDecay=" << process[ii].mayDecay() << ", Status=" << process[ii].status() << ", daughter1=" << process[ii].daughter1() << ", daughter2=" << process[ii].daughter2() << std::endl;
    }
#endif

    //return false: let the event continue
    return false;
  }


} // namespace Pythia8
