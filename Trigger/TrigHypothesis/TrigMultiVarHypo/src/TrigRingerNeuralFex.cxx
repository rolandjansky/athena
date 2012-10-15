/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigRingerNeuralFex.cxx
 * @author Danilo Enoque Ferreira de Lima 
 * $Author: dferreir $
 * $Date: 2009-05-07 21:01:28 $
 *
 * Implements the ringer-output neural processor
 */

#include <numeric>

#include "TrigMultiVarHypo/TrigRingerNeuralFex.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEvent/RingerRings.h"
#include "TrigMultiVarHypo/Neural.h"
#include "TrigCaloEvent/TrigRNNOutput.h"
#include "TrigCaloEvent/TrigRNNOutputContainer.h"

const double TrigRingerNeuralFex::ENERGY_THRESHOLD = 0.001;

TrigRingerNeuralFex::TrigRingerNeuralFex(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::FexAlgo(name, pSvcLocator), m_network(0)
{
  declareProperty("HltFeature", m_hlt_feature = "TrigRingerNeuralFex");
  declareProperty("Feature", m_feature = "TrigT2CaloEgamma");

  declareProperty("NodesVector", m_nodes);
  declareProperty("WeightVector", m_weight);
  declareProperty("BiasVector", m_bias);

  declareProperty("NRings", m_nRings);
  declareProperty("NormalisationRings", m_normRings);
  declareProperty("SectionRings", m_sectionRings);

  m_key="";
}

HLT::ErrorCode TrigRingerNeuralFex::hltInitialize() {

  // For each configured ring set, create an ordered RingSet, placing them into a vector.
  if (m_nRings.size() != m_normRings.size()) {
    msg() << MSG::FATAL << "Wrong configuration parameters for ring building." << endreq;
    return StatusCode::FAILURE;
  }

  try{
    m_network = new Neural(m_nodes, m_weight, m_bias);
  }
  catch(std::bad_alloc xa){
    msg() << MSG::ERROR << "Allocation Failure" << endreq;
    return HLT::ERROR;
  }
  catch(int i){
    if (i==BAD_WEIGHT_SIZE)
      msg() << MSG::ERROR << "Weight vector size is not compatible with nodes vector." << endreq;
    if (i==BAD_BIAS_SIZE)
      msg() << MSG::ERROR << "Bias vector size is not compatible with nodes vector." << endreq;
    return HLT::ERROR;
  }

  // For each configured ring set, create an ordered RingSet, placing them into a vector.
  for (unsigned int i = 0; i < m_nRings.size(); ++i) {
    m_ringsSet.push_back(RingSet(m_nRings[i], (RingSet::Normalisation) m_normRings[i], (RingSet::Section) m_sectionRings[i]));
  }
  m_maxRingsAccumulated = std::accumulate(m_nRings.begin(), m_nRings.end(), 0);


  if ( doTiming() ){
    m_storeTimer = addTimer("StoreOutput");
    m_decisionTimer = addTimer("Decision");
    m_normalizationTimer = addTimer("Normalization");
  }

  if ( msg().level() <= MSG::DEBUG ) msg() << MSG::DEBUG << "TrigRingerNeuralFex initialization completed successfully." << endreq;
  return HLT::OK;
}

HLT::ErrorCode TrigRingerNeuralFex::hltFinalize() {
  if (m_network != 0) delete m_network;
  return HLT::OK;
}

const TrigEMCluster* TrigRingerNeuralFex::get_cluster (const HLT::TriggerElement* ote) {
  const TrigEMCluster *pattern = 0;
  HLT::ErrorCode status = getFeature(ote, pattern, m_feature);
  return (status == HLT::OK) ? pattern : 0;
}

void TrigRingerNeuralFex::sequential(Pattern& rings, const Feature& stop_energy) {

  //if the ENERGY_THRESHOLD is greater than `stop', use stop instead.
  Feature stop = stop_energy;
  if (ENERGY_THRESHOLD > stop) {
    stop = ENERGY_THRESHOLD;
  }

  Pattern norm(rings.size(), 0);

  Feature sum = 0;
  for (size_t i = 0; i < rings.size(); ++i)
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
    Feature min = 999e30;
    for (size_t i = 0; i < rings.size(); ++i) {
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
    if (norm[0] < ENERGY_THRESHOLD) {
      return;
    }

    for (size_t i = 0; i < rings.size(); i++) {
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
  for (size_t i = 0; i < rings.size(); ++i) {
    rings[i] /= norm[i];
  }
}

void TrigRingerNeuralFex::normalize_rings(std::vector<RingSet>& rset) {
  //at this point, I have all ring sets, separated
  double emsection = 0; // energy at e.m. section
  double hadsection = 0; // energy at hadronic section

  //for each RingSet (first iteration) -- apply set dependent norms.
  for (std::vector<RingSet>::iterator jt=rset.begin(); jt!=rset.end(); ++jt) {
    //calculate the relevant energies
    double setenergy = 0;
    for (size_t i=0; i<jt->pattern().size(); ++i) setenergy += jt->pattern()[i];
    if (jt->section() == RingSet::EM) emsection += setenergy;
    else hadsection += setenergy;

    //what is the normalisation strategy here? Can I do something already?
    switch(jt->normalisation())
    {
      case RingSet::SET:
        if (setenergy > ENERGY_THRESHOLD) {
		for (size_t i = 0; i < jt->pattern().size(); ++i)
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
  for (std::vector<RingSet>::iterator jt=rset.begin(); jt!=rset.end(); ++jt)
  {
    //what is the normalisation strategy here? Do the rest of options
    switch(jt->normalisation())
    {
      case RingSet::EVENT:
        if (event > ENERGY_THRESHOLD) {
		for (size_t i = 0; i < jt->pattern().size(); ++i)
			jt->pattern()[i] /= fabs(event);
		}
      break;
      case RingSet::SECTION:
        if (jt->section() == RingSet::EM)
        {
	        if (emsection > ENERGY_THRESHOLD) {
			for (size_t i = 0; i < jt->pattern().size(); ++i)
				jt->pattern()[i] /= fabs(emsection);
		}
        }
        else
        {
	        if (hadsection > ENERGY_THRESHOLD) {
			for (size_t i = 0; i < jt->pattern().size(); ++i)
				jt->pattern()[i] /= fabs(hadsection);
		}
        }
      break;
      default: //do nothing
      break;
    }
  }
}


HLT::ErrorCode TrigRingerNeuralFex::hltExecute(const HLT::TriggerElement* te, HLT::TriggerElement* outputTE) {
  te = NULL; // just to avoid the compiler warning about unused variable.

  const TrigEMCluster *rtrig = get_cluster(outputTE);
  if (!rtrig) {
    msg() << MSG::ERROR << "Could not get TrigEMCluster from Navigation." << endreq;
    return HLT::MISSING_FEATURE;
  }

  const RingerRings* rings = rtrig->rings();
  if (!rings) {
    msg() << MSG::ERROR << "There is no RingerRings available in this TrigEMCluster. Perhaps the chain's FEX wasn't correctly configured!! Are you using RingerFex?!?!?!" << endreq;
    return HLT::BAD_ALGO_CONFIG;
  }

  if (m_nodes.at(0) != rings->rings().size()) {
    msg() << MSG::FATAL << "Number of rings calculated by RingerFex (" << rings->rings() << ") is different from input layer at this neural network (" << m_nodes.at(0) << ")" << endreq;
    return HLT::BAD_ALGO_CONFIG;
  }

  if (doTiming()) m_normalizationTimer->start();

  // RingerRings 2 RingSets
  unsigned ridx = 0;
  for (std::vector<RingSet>::iterator jt=m_ringsSet.begin(); jt!=m_ringsSet.end(); ++jt) {
    jt->reset(); //reset this ringset
    for (size_t j = 0; j < jt->max(); j++){
      jt->add( rings->rings().at(ridx) );
      ridx++;
    } //for each ring of the RingSet
  } //for each RingSet

  // Normalize
  normalize_rings(m_ringsSet);

  // RingSets 2 RingerRings
  std::vector<float> &ref_rings = m_rings.rings();
  ref_rings.clear();
  ref_rings.reserve( m_maxRingsAccumulated );
  for (std::vector<RingSet>::iterator jt=m_ringsSet.begin(); jt!=m_ringsSet.end(); ++jt) {
    ref_rings.insert(ref_rings.end(), jt->pattern().begin(), jt->pattern().end());
  }

  if (doTiming()) m_normalizationTimer->stop();
  
  if (doTiming()) m_decisionTimer->start();
  
  if ( msg().level() <= MSG::DEBUG ) msg() << MSG::DEBUG<<"RingerRings size is " << ref_rings.size() <<endreq;
  const std::vector<float> &theInput = ref_rings;
  const float decision = m_network->propagate(theInput);          // TODO: Extend propagate() to return output VECTORS and support NN with more than one output neuron!
  if (doTiming()) m_decisionTimer->stop();

  if ( msg().level() <= MSG::DEBUG ) msg() << MSG::DEBUG<<"Neural Network output is " << decision <<endreq;
  
  if (doTiming()) m_storeTimer->start();
  
  // save the decision at the StoreGate
  HLT::ErrorCode hltStatus;
  TrigRNNOutput *rnnOutput = new TrigRNNOutput;
  rnnOutput->output().push_back(decision);
  
  hltStatus = recordAndAttachFeature<TrigRNNOutput>(outputTE, rnnOutput, m_key, m_hlt_feature);
  if (hltStatus != HLT::OK) {
    msg() << MSG::ERROR << "Failed to record TrigRNNOutput to StoreGate." << endreq;
    if (doTiming()) m_storeTimer->stop();
    return HLT::ERROR;
  }
  
  // Link it to TrigEMCluster
  ElementLink<TrigEMClusterContainer> el_cluster;
  hltStatus = getFeatureLink<TrigEMClusterContainer, TrigEMCluster>(outputTE, el_cluster);
  if ( (hltStatus != HLT::OK) || (!el_cluster.isValid()) ) {
    msg() << MSG::ERROR << "Cannot get ElementLink to TrigEMCluster. I'll not link the TrigRNNOutput to the TrigEMCluster" << endreq;
  } else {
    rnnOutput->setCluster(*(el_cluster.getStorableObjectPointer()), (unsigned int) el_cluster.index());
  }
  
  if (doTiming()) m_storeTimer->stop();
  return HLT::OK;
}


TrigRingerNeuralFex::RingSet::RingSet (unsigned int max, const Normalisation &norm, const Section &section)
  : m_max(max),
    m_norm(norm),
    m_section(section),
    m_val(max)
{
}

TrigRingerNeuralFex::RingSet::~RingSet()
{
}
