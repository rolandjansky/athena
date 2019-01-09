/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///Local include(s)
#include "TrigMultiVarHypo/preproc/TrigRingerPreprocessor.h"
#include "TrigMultiVarHypo/tools/TrigRingerHelper.h"
///std library(s)
#include <numeric>
#include <cmath>

#include <iostream>
using namespace std;

using TrigCaloRingsHelper::RingSet;

//!======================================================================================
TrigRingerPreprocessor::TrigRingerPreprocessor(std::vector<unsigned int> &nRings, 
                                               std::vector<unsigned int>  &normRings,
                                               std::vector<unsigned int>  &sectionRings)
{
  // For each configured ring set, create an ordered RingSet, placing them into a vector.
  for (unsigned int i = 0; i < nRings.size(); ++i) {
    m_ringsSet.push_back(new RingSet(nRings[i], (RingSet::Normalisation) normRings[i], (RingSet::Section) sectionRings[i]));
  } 

  m_energyThreshold = -9999; 
  m_maxRingsAccumulated = std::accumulate(nRings.begin(), nRings.end(), 0);
}
//!======================================================================================
TrigRingerPreprocessor::~TrigRingerPreprocessor(){
  for(unsigned rs=0; rs<m_ringsSet.size(); ++rs){
    if(m_ringsSet.at(rs)) delete m_ringsSet.at(rs);
  }
}
//!======================================================================================
void TrigRingerPreprocessor::sequential(Pattern& rings, const Feature& stop_energy) {

  //if the energyThreshold is greater than `stop', use stop instead.
  Feature stop = stop_energy;
  if (m_energyThreshold > stop) {
    stop = m_energyThreshold;
  }

  Pattern norm(rings.size(), 0);
  Feature sum = 0;

  for (unsigned i = 0; i < rings.size(); ++i)
    sum += rings[i];

  norm[0] = std::fabs(sum);

  //if the sum is less than stop, apply layer normalisation to all rings
  if (norm[0] < stop) {
    //if the normalization factor is smaller than stop, I have to verify if it
    //is still too small. A small value will make things go bad... So, I
    //verify if the sum is smaller than the layer maxima, if that is the case,
    //I have to reconsider the normalization factor to a more reasonable
    //value, e.g. the layer absolute maximum
    Feature max = -999e30;
    Feature min =  999e30;
    for (unsigned i = 0; i < rings.size(); ++i) {
      if (rings[i] < min)
        min = rings[i];
      if (rings[i] > max)
        max = rings[i];
    }
    if (norm[0] < max) {
      Feature new_norm = std::fabs(max);
      if (std::fabs(min) > new_norm) new_norm = std::fabs(min);
      norm[0] = new_norm;
    }

    //if the sum is even less than the threshold, do not apply any
    //normalisation at all here!
    if (norm[0] < m_energyThreshold) {
      return;
    }

    for (unsigned i = 0; i < rings.size(); i++) {
      rings[i] /= norm[0];
    }
    return;
  }

  //I'm ok to proceed with the calculations
  bool fixed = false; //shall I stop to apply a variant normalisation factor?
  for (unsigned int i=1; i<rings.size(); ++i) {
    norm[i] = std::fabs(norm[i-1] - rings[i-1]);
    //if the normalization factor is less than `stop', just apply a fixed,
    //known to be great enough, value instead of that. Otherwise, the noise
    //will get too amplified.
    if (fixed || norm[i] < stop) {
      norm[i] = norm[0];
      if (!fixed) {
	      fixed = true;
      }
    }
  }
  for (unsigned i = 0; i < rings.size(); ++i) {
    rings[i] /= norm[i];
  }
}
//!======================================================================================
void TrigRingerPreprocessor::normalize_rings(std::vector<RingSet*> &rset) {

  //at this point, I have all ring sets, separated
  double emsection = 0; // energy at e.m. section
  double hadsection = 0; // energy at hadronic section

  //for each RingSet (first iteration) -- apply set dependent norms.
  for (auto &jt : rset) 
  {
    //calculate the relevant energies
    double setenergy = 0;
    for (unsigned i=0; i<jt->pattern().size(); ++i) setenergy += jt->pattern()[i];
    if (jt->section() == RingSet::EM) emsection += setenergy;
    else hadsection += setenergy;

    //what is the normalisation strategy here? Can I do something already?
    switch(jt->normalisation())
    {
      case RingSet::SET:
        if (setenergy > m_energyThreshold) {
		      for (unsigned i = 0; i < jt->pattern().size(); ++i)
			      jt->pattern()[i] /= fabs(setenergy);
	      }
        break;
      case RingSet::SEQUENTIAL:
        sequential(jt->pattern());
        break;
      default: //do nothing
        break;
    }
  } //for each RingSet (first iteration)

  double event = emsection + hadsection; // event energy
  //for each RingSet (third iteration) -- now accumulate and store
  for (auto &jt : rset) 
  {
    //what is the normalisation strategy here? Do the rest of options
    switch(jt->normalisation())
    {
      case RingSet::EVENT:
        if (event > m_energyThreshold) {
          for (unsigned i = 0; i < jt->pattern().size(); ++i)
            jt->pattern()[i] /= fabs(event);
        }
        break;
      case RingSet::SECTION:
        if (jt->section() == RingSet::EM)
        {
	        if (emsection > m_energyThreshold) {
            for (unsigned i = 0; i < jt->pattern().size(); ++i)
               jt->pattern()[i] /= fabs(emsection);
          }
        }else
        {
          if (hadsection > m_energyThreshold) {
            for (unsigned i = 0; i < jt->pattern().size(); ++i)
              jt->pattern()[i] /= fabs(hadsection);
          }
        }
        break;

      default: //do nothing
      break;
    }
  }
}
//!======================================================================================
bool TrigRingerPreprocessor::ppExecute( Pattern& rings) {
  // vector rings 2 RingSets
  unsigned ridx = 0;
  for (auto& jt : m_ringsSet){
    jt->reset(); //reset this ringset
    for (unsigned j = 0; j < jt->max(); j++){
      jt->add( rings.at(ridx) );
      ridx++;
    } //for each ring of the RingSet
  } //for each RingSet

  // Normalize
  normalize_rings(m_ringsSet);

  // RingSets 2 vector rings
  rings.clear();
  rings.reserve( m_maxRingsAccumulated );
  for (auto& jt : m_ringsSet) rings.insert(rings.end(), jt->pattern().begin(), jt->pattern().end());
  return true;
}
//!======================================================================================
float TrigRingerPreprocessor::normalize_eta( float eta, float etamin, float etamax ){
  int s = eta >= 0 ? 1 : -1;
  return ((std::abs(eta)-etamin) * s)/(etamax-etamin);
}
//!======================================================================================
float TrigRingerPreprocessor::normalize_mu(float mu, float mumax ){
  if(mu>mumax)
    mu=mumax;
  return (mu/mumax);
}











