/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CLHEP/Random/defs.h"
#include "CLHEP/Random/Random.h"
#include "AtlasCLHEP_RandomGenerators/dSFMTEngine.h"
#include "CLHEP/Random/engineIDulong.h"
#include "boost/io/ios_state.hpp"
#include "CxxUtils/checker_macros.h"
#include <string.h>
#include <sstream>
#include <cmath>	// for ldexp()
#include <stdlib.h>	// for abs(int)

extern "C" {
#include "dSFMT.h"
}

// Let the thread-safety checker know that dsfmt functions are ok.
namespace {
int wrap_init_gen_rand ATLAS_NOT_THREAD_SAFE (dsfmt_t* dsfmt, long seed)
{
  dsfmt_init_gen_rand (dsfmt, seed);
  return 0;
}
int wrap_init_by_array ATLAS_NOT_THREAD_SAFE (dsfmt_t* dsfmt,
                                              const uint32_t* seeds,
                                              int key_length)
{
  uint32_t* seeds_nc ATLAS_THREAD_SAFE = const_cast<uint32_t*> (seeds);
  dsfmt_init_by_array (dsfmt, seeds_nc, key_length);
  return 0;
}
}

using namespace std;

namespace CLHEP {

static const int MarkerLen = 64; // Enough room to hold a begin or end marker. 

std::string dSFMTEngine::name() const {return "dSFMTEngine";}

std::atomic<int> dSFMTEngine::numEngines = 0;

const unsigned int VECTOR_STATE_SIZE = 2 + (DSFMT_N + 1)*4;

dSFMTEngine::dSFMTEngine():m_dsfmt(0) 
{
  init_dsfmt();

  int engineNum = numEngines++;
  int cycle = abs(int(engineNum/maxIndex));
  int curIndex = abs(int(engineNum%maxIndex));
  long mask = ((cycle & 0x007fffff) << 8);
  long seedlist[2];
  HepRandom::getTheTableSeeds( seedlist, curIndex );
  seedlist[0] = (seedlist[0])^mask;
  seedlist[1] = 0;
  setSeeds( seedlist, engineNum );
  for( int i=0; i < 2000; ++i ) dSFMTEngine::flat();      // Warm up just a bit
}

dSFMTEngine::dSFMTEngine(long seed):m_dsfmt(0)  
{
  init_dsfmt();
  
  dSFMTEngine::setSeed( seed , 0);
  for( int i=0; i < 2000; ++i ) dSFMTEngine::flat();      // Warm up just a bit
}

dSFMTEngine::dSFMTEngine(const long * seeds):m_dsfmt(0) 
{
  init_dsfmt();
  
  dSFMTEngine::setSeeds( seeds, 0 );
  for( int i=0; i < 2000; ++i ) dSFMTEngine::flat();      // Warm up just a bit
}

void dSFMTEngine::init_dsfmt()
{
  int err=posix_memalign((void**)&m_dsfmt,16,sizeof(dsfmt_t));
  if(err!=0) {
    m_dsfmt=0;
    std::stringstream errstring;
    errstring << "dSFMTEngine::init_dsfmt() : could not allocate memory for dsfmt data structure, error=" << err;
    throw std::runtime_error(errstring.str());
  }
  //cout<<"dSFMTEngine::init_dsfmt() ptr="<<m_dsfmt<<endl;
}

dSFMTEngine::~dSFMTEngine()
{
  if(m_dsfmt) {
    free(m_dsfmt);
    m_dsfmt=0;
  }
}

dSFMTEngine::dSFMTEngine( const dSFMTEngine & p )  
{
  init_dsfmt();

  *this = p;
}

dSFMTEngine & dSFMTEngine::operator=( const dSFMTEngine & p ) {
  if( this != &p ) {
    *m_dsfmt=*p.m_dsfmt;
  }
  return *this;
}

double dSFMTEngine::flat() {
  double ret ATLAS_THREAD_SAFE = dsfmt_genrand_close_open(m_dsfmt);
  return ret;
}

void dSFMTEngine::flatArray( const int size, double *vect ) {
  for( int i=0; i < size; ++i) vect[i] = flat();
}

void dSFMTEngine::setSeed(long seed, int) {
  //std::cout<<"dSFMTEngine::setSeed seed="<<seed<<endl;
  int dum ATLAS_THREAD_SAFE [[maybe_unused]] = wrap_init_gen_rand(m_dsfmt,seed);
}

void dSFMTEngine::setSeeds(const uint32_t *seeds, int) {
  //std::cout<<"dSFMTEngine::setSeeds"<<endl;
  if (*seeds) {
    int i = 0;
    const int numBuff=DSFMT_N;
    while (i < numBuff && seeds[i]) {
      //cout<<"  seed "<<i<<" = "<<seeds[i]<<endl;
      ++i;
    }
    int dum ATLAS_THREAD_SAFE [[maybe_unused]] = wrap_init_by_array(m_dsfmt,seeds,i);
  } else {
    setSeed(1234567);
  }
}

void dSFMTEngine::setSeeds(const long *seeds, int) {
  //std::cout<<"dSFMTEngine::setSeeds"<<endl;
  if (*seeds) {
    int i = 0;
    const int numBuff=DSFMT_N;
    uint32_t buf[numBuff];
    while (i < numBuff && seeds[i]) {
      buf[i]=seeds[i];
      //cout<<"  seed "<<i<<" = "<<buf[i]<<endl;
      ++i;
    }
    int dum ATLAS_THREAD_SAFE [[maybe_unused]] = wrap_init_by_array(m_dsfmt,buf,i);
  } else {
    setSeed(1234567);
  }
}

void dSFMTEngine::saveStatus( const char filename[] ) const
{
   std::ofstream outFile( filename, std::ios::out ) ;
   if (!outFile.bad()) {
     outFile << m_dsfmt->idx << std::endl;
     for (int i=0; i<DSFMT_N + 1; ++i) outFile <<std::setprecision(20) 
                                               << m_dsfmt->status[i].u32[0] << " " << m_dsfmt->status[i].u32[1] << " " 
                                               << m_dsfmt->status[i].u32[2] << " " << m_dsfmt->status[i].u32[3] << " ";
     outFile << std::endl;
   }
}

void dSFMTEngine::restoreStatus( const char filename[] )
{
   std::ifstream inFile( filename, std::ios::in);
   if (!checkFile ( inFile, filename, engineName(), "restoreStatus" )) {
     std::cerr << "  -- Engine state remains unchanged\n";
     return;
   }

   if (!inFile.bad() && !inFile.eof()) {
     inFile >> m_dsfmt->idx;
     for (int i=0; i<DSFMT_N + 1; ++i) inFile >> m_dsfmt->status[i].u32[0] >> m_dsfmt->status[i].u32[1] 
                                              >> m_dsfmt->status[i].u32[2] >> m_dsfmt->status[i].u32[3];
   }
}

void dSFMTEngine::showStatus() const
{
   boost::io::ios_all_saver saver (std::cout);
   std::cout << std::endl;
   std::cout << "--------- dSFMT engine status ---------" << std::endl;
   std::cout << std::setprecision(20);
   std::cout << " Current index     = " << m_dsfmt->idx << std::endl;
   std::cout << " Array status[] = " << std::endl;
   for (int i=0; i<DSFMT_N + 1; ++i) {
     std::cout << m_dsfmt->status[i].u32[0] << " " << m_dsfmt->status[i].u32[1] << " " 
               << m_dsfmt->status[i].u32[2] << " " << m_dsfmt->status[i].u32[3] << std::endl;
   }
   std::cout << "----------------------------------------" << std::endl;
}

dSFMTEngine::operator float() {
  return (float)flat();
}

dSFMTEngine::operator unsigned int() {
  return dsfmt_genrand_uint32(m_dsfmt);
}

std::ostream & dSFMTEngine::put ( std::ostream& os ) const
{
   char beginMarker[] = "dSFMTEngine-begin";
   char endMarker[]   = "dSFMTEngine-end";

   int pr = os.precision(20);
   os << " " << beginMarker << " ";
   os << m_dsfmt->idx << " ";
   for (int i=0; i<DSFMT_N + 1; ++i) {
     os << m_dsfmt->status[i].u32[0] << "\n";
     os << m_dsfmt->status[i].u32[1] << "\n";
     os << m_dsfmt->status[i].u32[2] << "\n";
     os << m_dsfmt->status[i].u32[3] << "\n";
   }
   os << endMarker << "\n";
   os.precision(pr);
   return os;
}

std::vector<unsigned long> dSFMTEngine::put () const {
  std::vector<unsigned long> v;
  v.push_back (engineIDulong<dSFMTEngine>());
  v.push_back(m_dsfmt->idx); 
  for (int i=0; i<DSFMT_N + 1; ++i) {
     v.push_back(static_cast<unsigned long>(m_dsfmt->status[i].u32[0]));
     v.push_back(static_cast<unsigned long>(m_dsfmt->status[i].u32[1]));
     v.push_back(static_cast<unsigned long>(m_dsfmt->status[i].u32[2]));
     v.push_back(static_cast<unsigned long>(m_dsfmt->status[i].u32[3]));
  }
  return v;
}

std::istream &  dSFMTEngine::get ( std::istream& is )
{
  char beginMarker [MarkerLen];
  is >> std::ws;
  is.width(MarkerLen);  // causes the next read to the char* to be <=
			// that many bytes, INCLUDING A TERMINATION \0 
			// (Stroustrup, section 21.3.2)
  is >> beginMarker;
  if (strcmp(beginMarker,"dSFMTEngine-begin")) {
     is.clear(std::ios::badbit | is.rdstate());
     std::cerr << "\nInput stream mispositioned or"
	       << "\ndSFMTEngine state description missing or"
	       << "\nwrong engine type found." << std::endl;
     return is;
   }
  return getState(is);
}

std::string dSFMTEngine::beginTag ( )  { 
  return "dSFMTEngine-begin"; 
}

std::istream &  dSFMTEngine::getState ( std::istream& is )
{
  char endMarker   [MarkerLen];
  is >> m_dsfmt->idx;
  
  for (int i=0; i<DSFMT_N + 1; ++i) {
    is >> m_dsfmt->status[i].u32[0] ;
    is >> m_dsfmt->status[i].u32[1] ;
    is >> m_dsfmt->status[i].u32[2] ;
    is >> m_dsfmt->status[i].u32[3] ;
  }

  is >> std::ws;
  is.width(MarkerLen);  
  is >> endMarker;
  if (strcmp(endMarker,"dSFMTEngine-end")) {
     is.clear(std::ios::badbit | is.rdstate());
     std::cerr << "\ndSFMTEngine state description incomplete."
	       << "\nInput stream is probably mispositioned now." << std::endl;
     return is;
  }
  return is;
}

bool dSFMTEngine::get (const std::vector<unsigned long> & v) {
  if ((v[0] & 0xffffffffUL) != engineIDulong<dSFMTEngine>()) {
    std::cerr << 
    	"\ndSFMTEngine get:state vector has wrong ID word - state unchanged\n";
    return false;
  }
  return getState(v);
}

bool dSFMTEngine::getState (const std::vector<unsigned long> & v) {
  if (v.size() != VECTOR_STATE_SIZE ) {
    std::cerr << 
    	"\ndSFMTEngine get:state vector has wrong length - state unchanged\n";
    return false;
  }
  
  m_dsfmt->idx=v[1]; 
  for (int i=0; i<DSFMT_N + 1; ++i) {
     m_dsfmt->status[i].u32[0]=v[2+i*4+0];
     m_dsfmt->status[i].u32[1]=v[2+i*4+1];
     m_dsfmt->status[i].u32[2]=v[2+i*4+2];
     m_dsfmt->status[i].u32[3]=v[2+i*4+3];
  }

  return true;
}

}  // namespace CLHEP

