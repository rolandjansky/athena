#ifndef PYTHIA8_USERHOOKS_USERHOOKSUTILS_H
#define PYTHIA8_USERHOOKS_USERHOOKSUTILS_H
#include "Pythia8/Event.h"
#include <stdexcept>

/**
 *  Some common functions for determining pTs and navigating event records for the PoWHEG + Pythia user hooks
 */

namespace Pythia8_UserHooks{
  
  /**
   * \return the dot product of \param leg and \param comparison
   */
  inline double pTProj(const Pythia8::Particle &leg, const Pythia8::Particle &comparison){
    return leg.px()*comparison.px() + leg.py()*comparison.py() + leg.pz()*comparison.pz();
  }
  
  /**
   * \return the dot product of the legs in the \param evt with indices 
   * \param legIndex and \param comparedIndex
   */
  inline double pTProj(size_t legIndex, size_t comparedIndex, const Pythia8::Event &evt){
    return pTProj(evt[legIndex], evt[comparedIndex]);
  }
  
  /**
   * \return the pT squared of \param leg relative to \param comparison
   */
  inline double pT2Leg(const Pythia8::Particle &leg, const Pythia8::Particle &comparison){
    double proj = pTProj(leg, comparison);
    return leg.pAbs2() - (proj*proj) / comparison.pAbs2();
  }
  
  /**
   * \return the pT squared of the leg with index \param legIndex compared to the leg with index
   * \param comparedIndex in Event \param evt
   */
  inline double pT2Leg(size_t legIndex, size_t comparedIndex, const Pythia8::Event &evt){
    return pT2Leg(evt[legIndex], evt[comparedIndex]);
  }
  
  /**
   * \return the pT of \param leg relative to \param comparison
   */
  inline double pTLeg(const Pythia8::Particle &leg, const Pythia8::Particle &comparison){
    return sqrt(pT2Leg(leg, comparison));
  }
  
  /**
   * \return the pT of the leg with index \param legIndex compared to the leg with index
   * \param comparedIndex in Event \param evt
   */
  inline double pTLeg(size_t legIndex, size_t comparedIndex, const Pythia8::Event &evt){
    return sqrt(pT2Leg(legIndex, comparedIndex, evt));
  }
  
  /**
   *  Return the index of the most recent emission in a \param evt with a given \param status
   */
  inline size_t findLastEmission(const Pythia8::Event &evt, int status){
    size_t emission = evt.size() - 1;
    
    while(emission != 0){
      if (evt[emission].isFinal() && evt[emission].status() == status) return emission;
      
      --emission;
    }
    
    return 0;
  }
  
  /**
   * \return the index of the most recent ISR emission in an \param evt
   */
  inline size_t findLastISREmission(const Pythia8::Event &evt){
    size_t emission = findLastEmission(evt, 43);
    if(emission == 0) throw std::runtime_error("findLastISREmission:: Could not find ISR emission");
    return emission;
  }
  
  /**
   * \return the index of the most recent FSR emission in an \param evt
   */
  inline size_t findLastFSREmission(const Pythia8::Event &evt){
    size_t emission = findLastEmission(evt, 51);
    if(emission == 0) throw std::runtime_error("findLastFSREmission:: Could not find FSR emission");
    return emission;
  }
  
  /**
   * \return the index of the most recent particle to radiate an ISR emission in an \param evt
   */
  
  inline size_t findLastISRRadiator(const Pythia8::Event &evt){
    size_t radiator = findLastEmission(evt, -41);
    if(radiator == 0) throw std::runtime_error("findLastISRRadiator:: Could not find ISR radiator");
    return radiator;
  }
  
  /**
   * \return the index of the most recent particle to radiate a FSR emission in an \param evt
   */
  
  inline size_t findLastFSRRadiator(const Pythia8::Event &evt){
    size_t emitted = findLastFSREmission(evt);
    return evt[emitted].mother1();
  }
  
}

#endif
