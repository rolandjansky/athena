/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CLHEP/Random/defs.h"
#include "CLHEP/Random/DoubConv.hh"

#include "AtlasCLHEP_RandomGenerators/RandExpZiggurat.h"

#include <iostream>
#include <cmath>	// for log()

namespace CLHEP {

bool RandExpZiggurat::s_ziggurat_is_init=RandExpZiggurat::ziggurat_init();
unsigned long RandExpZiggurat::kn[128], RandExpZiggurat::ke[256];
float RandExpZiggurat::wn[128],RandExpZiggurat::fn[128],RandExpZiggurat::we[256],RandExpZiggurat::fe[256];

std::string RandExpZiggurat::name() const {return "RandExpZiggurat";}

HepRandomEngine & RandExpZiggurat::engine() {return *m_localEngine;}

RandExpZiggurat::~RandExpZiggurat() {
  if ( m_deleteEngine ) delete m_localEngine;
}

double RandExpZiggurat::operator()()
{
  return fire( m_defaultMean );
}

void RandExpZiggurat::shootArray( const int size, float* vect, float mean )
{
   for (int i=0; i<size; ++i) vect[i] = shoot(mean);
}

void RandExpZiggurat::shootArray( const int size, double* vect, double mean )
{
   for (int i=0; i<size; ++i) vect[i] = shoot(mean);
}

void RandExpZiggurat::shootArray(HepRandomEngine* anEngine, const int size, float* vect, float mean )
{
   for (int i=0; i<size; ++i) vect[i] = shoot(anEngine, mean);
}

void RandExpZiggurat::shootArray(HepRandomEngine* anEngine, const int size, double* vect, double mean )
{
   for (int i=0; i<size; ++i) vect[i] = shoot(anEngine, mean);
}

void RandExpZiggurat::fireArray( const int size, float* vect)
{
    for (int i=0; i<size; ++i) vect[i] = fire( m_defaultMean );
}

void RandExpZiggurat::fireArray( const int size, double* vect)
{
   for (int i=0; i<size; ++i) vect[i] = fire( m_defaultMean );
}

void RandExpZiggurat::fireArray( const int size, float* vect, float mean )
{
   for (int i=0; i<size; ++i) vect[i] = fire( mean );
}

void RandExpZiggurat::fireArray( const int size, double* vect, double mean )
{
   for (int i=0; i<size; ++i) vect[i] = fire( mean );
}

std::ostream & RandExpZiggurat::put ( std::ostream & os ) const {
  int pr=os.precision(20);
  std::vector<unsigned long> t(2);
  os << " " << name() << "\n";
  os << "Uvec" << "\n";
  t = DoubConv::dto2longs(m_defaultMean);
  os << m_defaultMean << " " << t[0] << " " << t[1] << "\n";
  os.precision(pr);
  return os;
#ifdef REMOVED
  int pr=os.precision(20);
  os << " " << name() << "\n";
  os << m_defaultMean << "\n";
  os.precision(pr);
  return os;
#endif
}

std::istream & RandExpZiggurat::get ( std::istream & is ) {
  std::string inName;
  is >> inName;
  if (inName != name()) {
    is.clear(std::ios::badbit | is.rdstate());
    std::cerr << "Mismatch when expecting to read state of a "
    	      << name() << " distribution\n"
	      << "Name found was " << inName
	      << "\nistream is left in the badbit state\n";
    return is;
  }
  if (possibleKeywordInput(is, "Uvec", m_defaultMean)) {
    std::vector<unsigned long> t(2);
    is >> m_defaultMean >> t[0] >> t[1]; m_defaultMean = DoubConv::longs2double(t); 
    return is;
  }
  // is >> m_defaultMean encompassed by possibleKeywordInput
  return is;
}


float RandExpZiggurat::ziggurat_efix(unsigned long jz,HepRandomEngine* anEngine)
{ 
  unsigned long iz=jz&255;
  
  float x;
  for(;;)
  {  
    if(iz==0) return (7.69711-log(ziggurat_UNI(anEngine)));          /* iz==0 */
    x=jz*we[iz]; 
    if( fe[iz]+ziggurat_UNI(anEngine)*(fe[iz-1]-fe[iz]) < exp(-x) ) return (x);

    /* initiate, try to exit for(;;) loop */
    jz=ziggurat_SHR3(anEngine);
    iz=(jz&255);
    if(jz<ke[iz]) return (jz*we[iz]);
  }
}

bool RandExpZiggurat::ziggurat_init()
{  
  const double m1 = 2147483648.0, m2 = 4294967296.;
  double dn=3.442619855899,tn=dn,vn=9.91256303526217e-3, q;
  double de=7.697117470131487, te=de, ve=3.949659822581572e-3;
  int i;

/* Set up tables for RNOR */
  q=vn/exp(-.5*dn*dn);
  kn[0]=(dn/q)*m1;
  kn[1]=0;

  wn[0]=q/m1;
  wn[127]=dn/m1;

  fn[0]=1.;
  fn[127]=exp(-.5*dn*dn);

  for(i=126;i>=1;i--) {
    dn=sqrt(-2.*log(vn/dn+exp(-.5*dn*dn)));
    kn[i+1]=(dn/tn)*m1;
    tn=dn;
    fn[i]=exp(-.5*dn*dn);
    wn[i]=dn/m1;
  }

/* Set up tables for REXP */
  q = ve/exp(-de);
  ke[0]=(de/q)*m2;
  ke[1]=0;

  we[0]=q/m2;
  we[255]=de/m2;

  fe[0]=1.;
  fe[255]=exp(-de);

  for(i=254;i>=1;i--) {
    de=-log(ve/de+exp(-de));
    ke[i+1]= (de/te)*m2;
    te=de;
    fe[i]=exp(-de);
    we[i]=de/m2;
  }
  s_ziggurat_is_init=true;
  return true;
}

}  // namespace CLHEP
