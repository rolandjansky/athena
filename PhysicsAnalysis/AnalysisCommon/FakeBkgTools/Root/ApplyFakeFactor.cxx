/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FakeBkgTools/ApplyFakeFactor.h"
#include "FakeBkgTools/FakeBkgInternals.h"

#include <cmath>

using namespace CP;

//=============================================================================
// Constructor
//=============================================================================
ApplyFakeFactor::ApplyFakeFactor(const std::string& name) :
  BaseLinearFakeBkgTool(name)
{
}

//=============================================================================
// Destructor
//=============================================================================
ApplyFakeFactor::~ApplyFakeFactor()
{
}

FakeBkgTools::Client ApplyFakeFactor::clientForDB()
{
    return FakeBkgTools::Client::FAKE_FACTOR;
}

StatusCode ApplyFakeFactor::initialize()
{
  return BaseLinearFakeBkgTool::initialize();
}

StatusCode ApplyFakeFactor::addEventCustom()
{
  m_cachedWeights.clear();

  return incrementTotalYield();
}

StatusCode ApplyFakeFactor::getEventWeightCustom(
    FakeBkgTools::Weight& outputEventWeight, const FakeBkgTools::FinalState& fs) {

  ATH_MSG_DEBUG("entering getEventWeight");

  // for the FF, we can handle the following (e.g. for a 2L final state):
  // N_LT, N_TL, N_LL
  // but not N_RR, N_FR, etc. (other than via the MC for N_RR)
  // and not N_TT, since this is part of what we want to estimate
  //
  // in general, we want to compute N_TT^{RF,FR,FF} = N_TT - N_TT^{RR}
  // which might need to be coded up as its own string
  //
  // maybe we want to only allow strProc == ">=1F"

  // reset initial values since they will be multiplied by
  // each lepton's FF and rel uncertainty 
  outputEventWeight.nominal = 1;
  outputEventWeight.uncertainties.clear();

  // check if the event satisfies the selection of interest
  // e.g. LT vs. TL, SS events, etc.
  unsigned long expectedTightLeptonsBitset = 0;
  if(!eventSatisfiesCriteria(fs, expectedTightLeptonsBitset)){

    ATH_MSG_DEBUG("event failed eventSatisfiesCriteria");

    // for an inappropriate event for our desired selection, 
    // return a weight of 0 and uncertainties of 0
    outputEventWeight.nominal = 0;

    // quit now, since the event does not satisfy our selection
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG("event passed eventSatisfiesCriteria!");

  // in principle m_particles may also include baseline, 
  // but we only want to have loose and tight at this stage!
  // User must *only* pass loose and tight leptons to the tool
  unsigned int nLooseLeps = 0;
  unsigned int ipart = 0;
  const FakeBkgTools::FSBitset newtights(expectedTightLeptonsBitset);

  for(FakeBkgTools::ParticleData& particle : m_particles){

    // only antiID (loose) leptons need an FF applied
    if(particle.tight) continue;
    if(!newtights[ipart++]) continue; // only multiply fake factors from loose leptons that are supposed to be tight in the wanted final state

    // FIXME when we load in the FFs, need to allow for e.g. pT, eta binning and arbitrary parameterizations
    float FF_value = particle.fake_factor.nominal;
    ATH_MSG_DEBUG("FF_value is " << FF_value);
    outputEventWeight.nominal *= FF_value;
    for(auto const& particleUncPair : particle.fake_factor.uncertainties){

      float FF_abs_err_up = particleUncPair.second.up;
      float FF_abs_err_down  = particleUncPair.second.down;

      float FF_rel_err_up = FF_abs_err_up / fabs(FF_value);
      float FF_rel_err_down = FF_abs_err_down / fabs(FF_value);

      // note that these are all relative uncertainties at this stage
      auto& unc =  outputEventWeight.uncertainties.emplace(particleUncPair.first, FakeBkgTools::Uncertainty{1.f, 1.f}).first->second;
      unc.up *= (1+FF_rel_err_up);
      unc.down *= (1-FF_rel_err_down);
    }
    ++nLooseLeps;
    // nLooseLeps will only count loose leptons "upgraded" to tight.
  }

  // Subtraction for double counting of events with two antiID leptons
  // which have had a FF applied, to "promote" them to a tight lepton.
  // It turns out this is true for all even multiplicities.
  if(nLooseLeps % 2 == 0){
    outputEventWeight.nominal *= -1;
  }
  

  for(auto& eventUncPair : outputEventWeight.uncertainties){
    eventUncPair.second.up = (eventUncPair.second.up - 1.f) * outputEventWeight.nominal;
    eventUncPair.second.down = (1.f - eventUncPair.second.down) * outputEventWeight.nominal;
  }

  ATH_MSG_DEBUG("outputEventWeight.nominal is " << outputEventWeight.nominal);

  return StatusCode::SUCCESS;
}

bool ApplyFakeFactor::eventSatisfiesCriteria(const FakeBkgTools::FinalState& fs, unsigned long& expectedTightLeptonsBitset){
  
  // fill bitset for tight leptons
  FakeBkgTools::FSBitset tights(0);
  unsigned int ipart = 0;
  for(FakeBkgTools::ParticleData& particle : m_particles){

    if(particle.tight){
      tights.set(ipart);
    }

    ++ipart;
  }
  
  // the logic is following: the event is accepted only if there's at least one "turn a loose lepton into tight" operation that gives the wanted final state
  FakeBkgTools::FSBitset charges = fs.retrieveCharges(m_particles); // could also use the code above instead, if it is need for other purposes
  unsigned int nValidCombinations = 0; // count the number of different possible+acceptable final states obtained by changing loose leptons into tight
  const unsigned long nc = (1<<m_particles.size());
  for(unsigned long c=0;c<nc;++c) // loop over all possible combinations for n leptons
  {
    auto newtights = FakeBkgTools::FSBitset(c);
    if(!fs.accept_selection(newtights, charges)){
      ATH_MSG_DEBUG("eventSatisfiesCriteria: failed selection");
      continue; // discard unwanted combinations (including charge requirements)
    }
    
    // if no loose lepton has been "upgraded", not a valid combination:
    if((newtights | tights) == tights){
      ATH_MSG_DEBUG("eventSatisfiesCriteria: newtights|tights == tights. newtights is " << newtights << " while tights is " << tights);
      continue;
    }
    // also, the combination can't have tight leptons "downgraded" to loose:
    if((newtights&tights) != tights){
      ATH_MSG_DEBUG("eventSatisfiesCriteria: newtights&tights != tights. newtights is " << newtights << " while tights is " << tights);
      continue;
    }
    ++nValidCombinations;
    expectedTightLeptonsBitset = c;
  }

  if(!nValidCombinations) return false;
  if(nValidCombinations > 1) // if > 1, would need to modify getEventWeight() to sum over all different combinations, I think
  {
    ATH_MSG_ERROR("this type of selection/process is currently not supported by the fake factor method");
    return false;
  }
  // process argument ignored for the moment. Should probably make sure that nothing exotic was requested. 

  return true;
}

