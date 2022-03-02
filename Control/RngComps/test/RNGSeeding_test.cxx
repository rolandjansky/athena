/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaKernel/test/RNGSeeding_test.cxx
 * @author Michael Duehrssen <michael.duehrssen@cern.ch>
 * @date Aug, 2021
 * @brief Unit test for RNG seeding quality.
 */


#undef NDEBUG
#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <stdlib.h> 
#include <math.h> 
#include "AthenaKernel/RNGWrapper.h"
#include "AtlasCLHEP_RandomGenerators/dSFMTEngine.h"
#include "CLHEP/Random/Ranlux64Engine.h"
#include "CLHEP/Random/RanecuEngine.h"
#include <boost/crc.hpp>

#define CHECK_BIT(var,pos) (((var)>>(pos)) & 1)

/// using crc32 for architecture independence in combining the seeds
inline uint32_t crc_combine(uint32_t seed, uint32_t v) {
  boost::crc_32_type crf;
  crf.process_bytes(&seed,sizeof(uint32_t));
  crf.process_bytes(&v,sizeof(uint32_t));
  return crf.checksum();
}

std::ostream& operator<<(std::ostream& os, const std::vector<unsigned long>& state)
{
  for(auto s : state) os << s << ", ";
  return os;
}

inline uint32_t gethash(const std::vector<unsigned long>& state)
{
  uint32_t hash=0;
  for(auto s : state) hash=crc_combine(hash, s);
  return hash;
}

inline uint32_t getmaskedhash(const std::vector<unsigned long>& state,uint32_t hashmask)
{
  return gethash(state) & hashmask;
}

void test1(ATHRNG::RNGWrapper* wrapper, const ATHRNG::RNGWrapper::SeedingOptionType option=ATHRNG::RNGWrapper::SeedingDefault, const uint64_t nseeds=1000000, uint64_t run=330000, std::string algName="TestAlg", const uint64_t maxevnr=1ULL<<34)
{
  const size_t slot=0;
  EventContext ctx;
  ctx.setSlot( slot );
  
  const size_t slot_evnr=0;
  wrapper->setSeedLegacy(algName, slot_evnr, 0, run, 0, option);
  auto evnr_engine=wrapper->getEngine(ctx);
  std::vector< bool > used_events(maxevnr);
  
  uint64_t nprint=nseeds/5;
  if(nprint>10000000) nprint=10000000;
  if(nprint<1000000) nprint=1000000;

  int hashbits=24;
  uint64_t vec_ev_size=1ULL<<hashbits;
  if(vec_ev_size>4294967296) vec_ev_size=4294967296;
  uint32_t hashmask=(uint32_t)(vec_ev_size-1);//mask a few bits from the hash, as it's more memory efficient to have a smaller vector with more events for each hash
  
  std::cout << "test1 with <<"<<wrapper->getEngine(ctx)->name()<<", "<<nseeds<<" seeds tested, ";
  switch(option) {
    case ATHRNG::RNGWrapper::MC16Seeding:  std::cout << "using setSeedMC16";
             break;
    case ATHRNG::RNGWrapper::MC21Seeding:  std::cout << "using setSeedMC21";
             break;
    default: std::cout << "using setSeedMC20";
             break;
  }  
  float occ=1.0*nseeds/vec_ev_size;
  size_t increment=(size_t)ceil(sqrt(occ));
  
  std::cout << ", hash vector size="<<vec_ev_size<<", expect occupancy of "<<occ<<" increment="<<increment<<std::endl;

  std::vector< std::vector< uint64_t > > vec_ev(vec_ev_size); //(almost) all hashes will be used. std::vector far more efficient than using a map.

  uint64_t nidentical=0;
  size_t maxvecsize=0;
  for(uint64_t iseed=0;iseed<nseeds;++iseed) {
    //Merge two 32bit random integers into one 64bit integer
    uint64_t ev=uint32_t( *evnr_engine );
    ev=ev<<32;
    ev+=uint32_t( *evnr_engine );
    //restruct range to [0,maxevnr)
    ev%=maxevnr;
    if(used_events[ev]) {
      //std::cout <<"calculating hash "<< iseed<< ", ev="<<ev<<" used before. Skipping\n";
      --iseed;
      continue;
    }
    used_events[ev]=true;
    
    if(iseed % nprint == 0 && nseeds>nprint) std::cout <<"calculating hashes:"<< iseed<< ", ev="<<ev<< ", max internal vector size="<<maxvecsize<<std::endl;
    
    wrapper->setSeedLegacy(algName, slot, ev, run, 0, option);     
    
    std::vector<unsigned long> state = wrapper->getEngine(ctx)->put();
    uint32_t maskedhash=getmaskedhash(state,hashmask);
    size_t vecsize=vec_ev[maskedhash].size();

    if(vecsize==vec_ev[maskedhash].capacity()) {
      if(vecsize>=maxvecsize) {
        maxvecsize=vecsize+1;
        //std::cout <<"new max size="<<maxvecsize<<", current capacity="<<vec_ev[maskedhash].capacity()<<"\n";
      }  
      vec_ev[maskedhash].reserve(vecsize+increment);
    }  
    vec_ev[maskedhash].push_back(ev);
  }  


  nprint=vec_ev_size/5;
  if(nprint>100000000) nprint=100000000;
  if(nprint<10000000) nprint=10000000;

  for(uint32_t maskedhash=0;maskedhash<vec_ev_size;++maskedhash) {
    if(maskedhash % nprint == 0 && maskedhash>nprint) std::cout <<"testing hashes:"<< maskedhash << ", so far total=" << nidentical << "/" << maskedhash <<std::endl;

    size_t vecsize=vec_ev[maskedhash].size();
    if(vecsize<2) continue;

    std::vector< std::vector<unsigned long> > states(vecsize);
    std::vector< uint32_t > hashes(vecsize);
    for(size_t iev=0;iev<vecsize;++iev) {
      uint64_t ev=vec_ev[maskedhash][iev];
      wrapper->setSeedLegacy(algName, slot, ev, run, 0, option);     
      states[iev] = wrapper->getEngine(ctx)->put();
      hashes[iev]=gethash(states[iev]);
      uint32_t maskedoldhash=hashes[iev] & hashmask;

      if(maskedhash!=maskedoldhash) {
        std::cout << "closure problem!";
        std::cout << "  new :"<< maskedhash ;
        std::cout << ", prev:"<< maskedoldhash << " state=" << states[iev]<<std::endl;
      }

    }

    for(size_t iev=0;iev<vecsize-1;++iev) {
      auto& state=states[iev];
      uint32_t hash=hashes[iev];
      for(size_t jev=iev+1;jev<vecsize;++jev) {
        if(hash!=hashes[jev]) continue;
        if(state==states[jev]) {
          //std::cout << "Found same hash " << hash << " for two states ; ";
          //std::cout << " full state identical, total=" << nidentical << "/" << maskedhash+1 << " identical \n";
          nidentical++;
          break;
        } else {
          //std::cout << "Found same hash " << hash << " for two states ; ";
          //std::cout << " full state different, total=" << nidentical << "/" << maskedhash+1 << " identical \n";
          //std::cout << "  new :"<< maskedhash    << ": " << state    << '\n';
          //std::cout << "  prev:"<< maskedoldhash << ": " << oldstate << "\n\n";
        }  
      }
    }
  }

  std::cout <<"Total=" << nidentical << "/" << nseeds << " identical"<<std::endl;
}

void test2(ATHRNG::RNGWrapper* wrapper, const ATHRNG::RNGWrapper::SeedingOptionType option=ATHRNG::RNGWrapper::SeedingDefault, const uint8_t increment_mask=2, const uint64_t ntest=200000)
{
  uint64_t base_ev=0;
  uint64_t base_run=330000;
  std::string base_algName="TestAlg";
  uint32_t base_offset=0;
  
  const size_t slot=0;
  EventContext ctx;
  ctx.setSlot( slot );

  std::cout << "test2 with <<"<<wrapper->getEngine(ctx)->name()<<", "<<ntest<<" seeds tested, ";
  switch(option) {
    case ATHRNG::RNGWrapper::MC16Seeding:  std::cout << "using setSeedMC16";
             break;
    case ATHRNG::RNGWrapper::MC21Seeding:  std::cout << "using setSeedMC21";
             break;
    default: std::cout << "using setSeedMC20";
             break;
  }  

  if(CHECK_BIT(increment_mask,0)) std::cout << ", incrementing event";
  if(CHECK_BIT(increment_mask,1)) std::cout << ", incrementing run";
  if(CHECK_BIT(increment_mask,2)) std::cout << ", incrementing algName";
  if(CHECK_BIT(increment_mask,3)) std::cout << ", incrementing offset";
  std::cout<<std::endl;

  uint64_t nprint=ntest/10;
  if(nprint>1000000) nprint=1000000;
  if(nprint<10) nprint=10;

  uint64_t nidentical=0;
  std::multimap< uint32_t , uint64_t > states;
  for(uint64_t itest=0;itest<ntest;++itest) {
    //if(itest % nprint == 0) std::cout <<"test:"<< itest << ", so far total=" << nidentical << "/" << itest << " identical \n";

    uint64_t ev=base_ev+CHECK_BIT(increment_mask,0)*itest;
    uint64_t run=base_run+CHECK_BIT(increment_mask,1)*itest;
    std::string algName=base_algName+std::to_string( CHECK_BIT(increment_mask,2)*itest );
    uint32_t offset=base_offset+CHECK_BIT(increment_mask,3)*itest;
    
    //std::cout<<"ev="<<ev<<" run="<<run<<" algName="<<algName<<" offset="<<offset<<"\n";

    wrapper->setSeedLegacy(algName, slot, ev, run, offset, option);     
    
    std::vector<unsigned long> state = wrapper->getEngine(ctx)->put();
    uint32_t hash=0;
    for(auto s : state) hash=crc_combine(hash, s);
    if(states.count(hash)>0) {
      auto range = states.equal_range(hash);
      for (auto i = range.first; i != range.second; ++i) {
        uint64_t iold=i->second;
        ev=base_ev+CHECK_BIT(increment_mask,0)*iold;
        run=base_run+CHECK_BIT(increment_mask,1)*iold;
        algName=base_algName+std::to_string( CHECK_BIT(increment_mask,2)*iold );
        offset=base_offset+CHECK_BIT(increment_mask,3)*iold;
        wrapper->setSeedLegacy(algName, slot, ev, run, offset, option);     
        std::vector<unsigned long> oldstate = wrapper->getEngine(ctx)->put();
        uint32_t oldhash=0;
        for(auto s : oldstate) oldhash=crc_combine(oldhash, s);
        if(hash!=oldhash) {
          std::cout << "closure problem!";
          std::cout << "  new :"<< hash << ": " << state << '\n';
          std::cout << "  prev:"<< i->first << ": " << oldstate<< "\n"<<std::endl;
        }

        if(state==oldstate) {
          //std::cout << "Found same hash " << hash << " for "<<states.count(hash)+1<<" states ; ";
          //std::cout <<" full state identical, total=" << nidentical << "/" << itest+1 << " identical \n";
          nidentical++;
          break;
        } else {
          //std::cout << "Found same hash " << hash << " for "<<states.count(hash)+1<<" states ; ";
          //std::cout << " full state different, total=" << nidentical << "/" << itest+1 << " identical \n";
          //std::cout << "  new :"<< hash << ": " << state << '\n';
          //std::cout <<"  prev:"<< i->first << ": " << oldstate<< "\n\n";
        }  
      }
      //std::cout << "=======================================\n"; 
      //std::cout << '\n'; 
    }
    states.insert(std::make_pair(hash,itest));
    
  }  
  std::cout <<"Total=" << nidentical << "/" << ntest << " identical"<<std::endl;
}


int main(int argc, char *argv[])
{
  std::cout << "RNGSeeding_test"<<std::endl;
  
  int sel_opt=-1;
  
  if(argc>=2) sel_opt=atol(argv[1]);
  int iarg=2;
  
  const size_t nSlots = 3;
  std::vector< ATHRNG::RNGWrapper* > wrappers;
  std::vector< uint64_t > nev;
  
  std::function< CLHEP::HepRandomEngine*(void) > dSFMTFactory = [](void)->CLHEP::HepRandomEngine* { 
    return new CLHEP::dSFMTEngine(); 
  };
  wrappers.push_back(new ATHRNG::RNGWrapper( dSFMTFactory , nSlots ));
  if(argc>=iarg+1) nev.push_back(atol(argv[iarg]));
   else nev.push_back(100000);
  ++iarg; 
  
  std::function< CLHEP::HepRandomEngine*(void) > RanluxFactory = [](void)->CLHEP::HepRandomEngine*{
    return new CLHEP::Ranlux64Engine();
  };
  wrappers.push_back(new ATHRNG::RNGWrapper( RanluxFactory , nSlots ));
  if(argc>=iarg+1) nev.push_back(atol(argv[iarg]));
   else nev.push_back(0);
  ++iarg; 

  std::function< CLHEP::HepRandomEngine*(void) > RanecuFactory = [](void)->CLHEP::HepRandomEngine*{
    return new CLHEP::RanecuEngine();
  };
  wrappers.push_back(new ATHRNG::RNGWrapper( RanecuFactory , nSlots ));
  if(argc>=iarg+1) nev.push_back(atol(argv[iarg]));
   else nev.push_back(1000000);
  ++iarg; 
  
  for(int iwrapper=0;iwrapper<(int)wrappers.size();++iwrapper) {
    if(nev[iwrapper]<=0) continue;
    for(auto option : ATHRNG::RNGWrapper::all_SeedingOptions) {
      if(sel_opt>=0) if(sel_opt!=option) continue;
      test1(wrappers[iwrapper],option,nev[iwrapper]);
      test2(wrappers[iwrapper],option,1,nev[iwrapper]/5);
      test2(wrappers[iwrapper],option,2,nev[iwrapper]/5);
      test2(wrappers[iwrapper],option,4,nev[iwrapper]/5);
      test2(wrappers[iwrapper],option,8,nev[iwrapper]/5);
      std::cout << std::endl;
    }  
  }  
  
  return 0;
}
