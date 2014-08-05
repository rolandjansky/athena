/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : DecisionAnalysis
 *
 * @brief access to information about the trigger decision
 *
 * @author Michael Begel  <michael.begel@cern.ch> - Brookhaven National Laboratory
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - LAPP Annecy
 *
 ***********************************************************************************/

#include <vector>
#include <exception>
#include "AthenaKernel/getMessageSvc.h"

#include "TrigDecisionTool/DecisionAccess.h"


#include "TrigNavigation/AccessProxy.h"

#include "TrigSteeringEvent/Chain.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "TrigDecisionTool/TDTUtilities.h"
#include "TrigDecisionTool/CacheGlobalMemory.h"


Trig::DecisionAccess::DecisionAccess() 
  : Logger("DecisionAccess") {}

Trig::DecisionAccess::~DecisionAccess() {}


StatusCode Trig::DecisionAccess::unpackDecision(const TrigDec::TrigDecision* dec) {
  cgm()->unpackDecision(dec);
  return cgm()->unpackNavigation(dec);
}

bool Trig::DecisionAccess::isPassed(const Trig::ChainGroup* chainGroup, 
				    unsigned int condition) const
{
  return chainGroup->isPassed(condition);
}

bool Trig::DecisionAccess::isPassed(const std::string& chain, 
				    unsigned int condition ) const

{
  const Trig::ChainGroup *g = cgm()->createChainGroup(Trig::convertStringToVector(chain));
  return isPassed(g,condition);
}

bool Trig::DecisionAccess::isPassed(const std::string& chain) const {
  return isPassed(chain, TrigDefs::Physics);
}

unsigned int Trig::DecisionAccess::isPassedBits(const Trig::ChainGroup* chainGroup) const
{
  return chainGroup->isPassedBits();
}

unsigned int Trig::DecisionAccess::isPassedBits(const std::string& chain) const

{
  const Trig::ChainGroup *g = cgm()->createChainGroup(Trig::convertStringToVector(chain));
  return isPassedBits(g);
}


const Trig::FeatureContainer
Trig::DecisionAccess::features(const Trig::ChainGroup* chain, unsigned int condition) const {
  return chain->features(condition);
}

const Trig::FeatureContainer
Trig::DecisionAccess::features(const std::string& chain, unsigned int condition) const {
  const Trig::ChainGroup *g = cgm()->createChainGroup(Trig::convertStringToVector(chain));
  return features(g,condition);
}

char
Trig::DecisionAccess::getBGCode() const {
  if( ! const_cast< Trig::CacheGlobalMemory* >( cgm() )->assert_decision() ) {
    log() << MSG::ERROR << "Something went wrong in decoding the trigger decision" << endreq;
    return 0;
  }
  return cgm()->m_bgCode;
}

/*
unsigned int Trig::DecisionAccess::initRoIWord(const HLT::TriggerElement* te) const {
    Feature<TrigRoiDescriptor> roi = ancestor<TrigRoiDescriptor>(te,"initialRoI");
    if ( roi.empty() )
      return 0;
    return roi.cptr()->roiWord();
}


  template<class T> 
  const T* Trig::DecisionAccess::getL1RoI(const std::vector<T>& rois, unsigned int id) const {
    typename std::vector<T>::const_iterator it;
    for ( it = rois.begin(); it != rois.end(); ++it ) {
      if ( it->getROIWord() == id ) {
	return &*it;
	break;
      }
    }    
    return 0;
  }


template<> 
const Trig::Feature<Muon_ROI> Trig::DecisionAccess::ancestor<Muon_ROI>(const HLT::TriggerElement* te, std::string  ) const {
  Trig::Feature<Muon_ROI> feature;
  const LVL1_ROI* lvl1;
  if ( cgm()->navigation()->getAccessProxy()->retrieve<LVL1_ROI>(lvl1, "LVL1_ROI").isFailure() ) {
    return feature;
    }
  feature = Trig::Feature<Muon_ROI>( getL1RoI(lvl1->getMuonROIs(), initRoIWord(te)), te, "" );
  return feature;
}

template<> 
const Trig::Feature<Jet_ROI> Trig::DecisionAccess::ancestor<Jet_ROI>(const HLT::TriggerElement* te, std::string  ) const {
  Trig::Feature<Jet_ROI> feature;
  const LVL1_ROI* lvl1;
  if ( cgm()->navigation()->getAccessProxy()->retrieve<LVL1_ROI>(lvl1, "LVL1_ROI").isFailure() ) {
      return feature;
  }
  feature = Trig::Feature<Jet_ROI>( getL1RoI(lvl1->getJetROIs(), initRoIWord(te)), te, "" );
  return feature;
} 

template<> 
const Trig::Feature<EmTau_ROI> Trig::DecisionAccess::ancestor<EmTau_ROI>(const HLT::TriggerElement* te, std::string  ) const {
  Trig::Feature<EmTau_ROI> feature;
  const LVL1_ROI* lvl1;
  if ( cgm()->navigation()->getAccessProxy()->retrieve<LVL1_ROI>(lvl1, "LVL1_ROI").isFailure() ) {
      return feature;
  }
  feature = Trig::Feature<EmTau_ROI>( getL1RoI(lvl1->getEmTauROIs(), initRoIWord(te)), te, "" );
  return feature;
}

*/
