/*
 * This file is taken from the available public code at: 
 * https://gitlab.com/simonknapen/suep_generator 
 * by Simon Knapen.
 */
#include "suep_shower.h"

using namespace boost::math::tools;           // For bracket_and_solve_root.
using namespace std;
using namespace Pythia8;

// constructor
Suep_shower::Suep_shower(double mass, double temperature, double energy, Pythia8::Rndm *rndm) {
  m = mass;
  Temp=temperature;
  Etot=energy;
  rndmEngine = rndm;
  
  A=m/Temp;
  p_m=std::sqrt(2/(A*A)*(1+std::sqrt(1+A*A)));
  
  double pmax=std::sqrt(2+2*std::sqrt(1+A*A))/A; // compute the location of the maximum, to split the range 
  
  tolerance tol = 0.00001;
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
  return p*p*exp(-A*p*p/(1+std::sqrt(1+p*p)));
}

// derivative of maxwell-boltzmann
double  Suep_shower::fp(double p){
  return exp(-A*p*p/(1+std::sqrt(1+p*p)))*p*(2-A*p*p/std::sqrt(1+p*p));
}

// test function to be solved for p_plus, p_minus
double  Suep_shower::test_fun(double p){
  return log(f(p)/f(p_m))+1.0;
}

// generate one random 4 vector from the thermal distribution
Vec4 Suep_shower::generateFourVector(){
  
  Vec4 fourvec;
  double en, phi, theta, p;//kinematic variables of the 4 vector
  
  // first do momentum, following arxiv:1305.5226
  double U(0.0), V(0.0), X(0.0), Y(0.0), E(0.0);
  int i=0;      
  while(i<100){
    if (rndmEngine) {
      U = rndmEngine->flat();
      V = rndmEngine->flat();
    } else {
      U = ((double) rand() / RAND_MAX);
      V = ((double) rand() / RAND_MAX);
    }
    
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
  if (rndmEngine) {
    phi = 2.0*M_PI*(rndmEngine->flat());
    theta = acos(2.0*rndmEngine->flat()-1.0);
  } else {
    phi = 2.0*M_PI*((double) rand() / RAND_MAX);
    theta = acos(2.0*((double) rand() / RAND_MAX)-1.0);
  }
  
  // compose the 4 vector
  en = std::sqrt(p*p+(this->m)*(this->m));
  fourvec.p(p*cos(phi)*sin(theta), p*sin(phi)*sin(theta), p*cos(theta), en);
  
  return fourvec; 
}

// auxiliary function which computes the total energy difference as a function of the momentum vectors and a scale factor "a"
// to ballance energy, we solve for "a" by demanding that this function vanishes
// By rescaling the momentum rather than the energy, I avoid having to do these annoying rotations from the previous version 
double Suep_shower::reballance_func(double a, const vector< Vec4 > &event){
  double result =0.0;
  double p2;
  for(unsigned n = 0; n<event.size();n++){
    p2 = event[n].px()*event[n].px() + event[n].py()*event[n].py() + event[n].pz()*event[n].pz();
    result += std::sqrt(a*a*p2 + (this->m)* (this->m));
  }
  return result - (this->Etot);
}

// generate a shower event, in the rest frame of the shower
vector< Vec4 > Suep_shower::generate_shower(){
  
  vector< Vec4 > event;
  double sum_E = 0.0;
  
  // fill up event record
  while(sum_E<(this->Etot)){
    event.push_back(this->generateFourVector());
    sum_E += (event.back()).e();
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

  //Shield against an exception in the calculation of "p_scale" further down. If it fails, abort the event.
  if(Suep_shower::reballance_func(2.0,event)<0.0){
    // failed to balance energy. 
    event.clear();
    return event;
  } 
  
  // finally, ballance the total energy, without destroying momentum conservation
  tolerance tol = 0.00001;
  double p_scale;
  try {
    p_scale = (bisect(boost::bind(&Suep_shower::reballance_func, this, _1, event),0.0,2.0, tol)).first;
  } catch (std::exception &e) {
    //in some rare circumstances, this balancing might fail
    std::cout << "[SUEP_SHOWER] WARNING: Failed to rebalance the following event; Printing for debug and throw exception" << std::endl;
    std::cout << e.what() << std::endl;
    std::cout << "N. Particle, px, py, pz, E" << std::endl;
    for (size_t jj=0; jj < event.size(); jj++) {
      //print out event
      std::cout << jj << ": " << event[jj].px() << ", " << event[jj].py() << ", " << event[jj].pz() << ", " << event[jj].e() << std::endl;
    }
    throw e;
  }
  
  for(int n=0;n<len;n++){
    event[n].px(p_scale*event[n].px());
    event[n].py(p_scale*event[n].py());
    event[n].pz(p_scale*event[n].pz());
    // force the energy with the on-shell condition
    event[n].e(std::sqrt(event[n].px()*event[n].px() + event[n].py()*event[n].py() + event[n].pz()*event[n].pz() + (this->m)*(this->m)));
  }
  
  return event;    
}
