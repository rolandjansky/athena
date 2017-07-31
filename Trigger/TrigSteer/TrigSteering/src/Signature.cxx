/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : Signature
 *
 * @brief  one signature, for instance
 *         2 muons AND 4 electrons AND 1 jet ...
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: Signature.cxx,v 1.35 2009-02-19 10:53:44 tbold Exp $
 **********************************************************************************/

#include "TrigSteering/Signature.h"

#include "TrigSteering/Sequence.h"
#include "TrigSteering/ISequenceProvider.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigInterfaces/AlgoConfig.h"
#include "GaudiKernel/MsgStream.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigNavigation/Navigation.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include <iostream>

using namespace HLT;
using namespace std;


Signature::Signature(HLT::AlgoConfig* config)
  : m_config(config), m_configSig(0), m_foundOverlap(false)
{}

Signature::Signature( const TrigConf::HLTSignature* configSig,
                      HLT::ISequenceProvider* seqs,
                      HLT::AlgoConfig* config ) :
   m_config(config),
   m_configSig(configSig),
   m_foundOverlap(false)
{

   if ( !m_config ) {
      std::cerr << "AlgoConfig object not available in the Signature "<< configSig->label()<< " constructor " << std::endl;    
      return;
   }
   // Debug output
   if (m_config->getMsgLvl() <=MSG::DEBUG) {
      m_config->getMsgStream() << MSG::DEBUG << "Signature "
                               << configSig->label() << ": constructor" << endmsg;
   }

   // Copy the TE information over from the configSig; transform the "vector of TEs"
   // format to the "vector of (TE type, TE count, sequence for this type) used here


   // multiplicity of each output TE
   std::map<unsigned int, unsigned int> multiplicity;
   for ( TrigConf::HLTTriggerElement* te : configSig->outputTEs() ) multiplicity[ te->id() ]++;


   std::set<unsigned int> already_configured;


   for ( TrigConf::HLTTriggerElement* configTE : configSig->outputTEs() ) {

      const unsigned int id = configTE->id();
      if ( already_configured.find(id) != already_configured.end() )
         continue;
      already_configured.insert(id);

      Sequence* seq = seqs->findSeqForOutputTeType( id );
      if (!seq) {
         m_config->getMsgStream() << MSG::FATAL << "Signature: (TE = " << configTE->name()
                                  << ") : Could not find matching sequence for output TE."
                                  << endmsg;
         m_requiredTEs.clear();
         return;
      } else {
         addItem(multiplicity[id], seq);
         if (m_config->getMsgLvl() <=MSG::DEBUG) {
            m_config->getMsgStream() << MSG::DEBUG << "Made part of signature: " << configSig->label() << " TE = " << id
                                     << " (" << configTE->name() << ") x " << multiplicity[id] << endmsg;
         }
      }
   }

} // end constructor


void Signature::addItem(unsigned int mult, HLT::Sequence* seq ) {
  m_requiredTEs.push_back(RequiredTEs(mult, seq));
}


bool Signature::reset()
{
  // The signature doesn't have any transient info itself, so just reset the sequences
  for (unsigned int i = 0; i < m_requiredTEs.size(); ++i)
    if (!m_requiredTEs[i].sequence->reset()) return false;

  m_foundOverlap = false;

  return true;
}

bool Signature::resetROBRequestPreparation() {
  // The signature doesn't have any transient info itself, so just reset the sequences
  for (unsigned int i = 0; i < m_requiredTEs.size(); ++i)
    if (!m_requiredTEs[i].sequence->resetROBRequestPreparation()) return false;

  return true;
}


HLT::ErrorCode Signature::execute( bool& pass )
{
  // Debug output
  if (m_config->getMsgLvl() <= MSG::DEBUG) {
    m_config->getMsgStream() << MSG::DEBUG << "Signature "
                             << m_configSig->label()
                             << ": executing" << endmsg;
  }

  // collect operational information: create new TrigOperationalInfo
  if(m_config -> getSteeringOPILevel() > 0 && m_config -> getSteeringOPI()) {
    TrigOperationalInfo *steer_opi = m_config -> getSteeringOPI();
    steer_opi -> set("SIGNATURE:"+m_configSig->label(), m_requiredTEs.size());
  }

  HLT::ErrorCode retCode = HLT::OK;
  HLT::ErrorCode ec = HLT::OK;

  // loop over all Sequences
  for (unsigned int i = 0; i < m_requiredTEs.size(); ++i) {

    ec = m_requiredTEs[i].sequence->execute();

    if (ec.action() > HLT::Action::CONTINUE ) {
      return ec;
    }

    if (m_config->getMsgLvl() <= MSG::DEBUG) 
      m_config->getMsgStream() << MSG::DEBUG << "Signature " << m_configSig->label()
                               << " EC from sequence execution =  " << HLT::strErrorCode(ec) << endmsg;

    // Debug output
    if (m_config->getMsgLvl() <= MSG::DEBUG) {

      m_config->getMsgStream() << MSG::DEBUG << "Signature: found " << m_requiredTEs[i].sequence->activeOutputTEs()
                               << " active TEs, require " << m_requiredTEs[i].multiplicity;

      if (m_requiredTEs[i].sequence->activeOutputTEs() < static_cast<int>(m_requiredTEs[i].multiplicity))
        m_config->getMsgStream() << MSG::DEBUG << " -> failed!" << endmsg;
      else
        m_config->getMsgStream() << MSG::DEBUG << " -> satisfied!" << endmsg;
    }

    retCode = retCode > ec ? retCode : ec;

    // Check result of sequence agains requirements:
    if (m_requiredTEs[i].sequence->activeOutputTEs() < static_cast<int>(m_requiredTEs[i].multiplicity) ) {
      pass = false;
      return retCode;
    }


  }

  // Make sure all parts of this signature come from different RoIs.
  m_foundOverlap = checkRoIOverlap();
  pass = m_foundOverlap;

  // Debug output
  if (m_config->getMsgLvl() <= MSG::DEBUG) {
    if (pass)
      m_config->getMsgStream() << MSG::DEBUG << "Signature: done, satisfied" << endmsg;
    else
      m_config->getMsgStream() << MSG::DEBUG << "Signature: done, failed: TEs do not come from different RoIs!" << endmsg;

  }

  return retCode;
}



HLT::ErrorCode Signature::prepareRobRequests()
{
   // Debug output
   if (m_config->getMsgLvl() <= MSG::DEBUG) {
      m_config->getMsgStream() << MSG::DEBUG << "Signature "
                               << m_configSig->label()
                               << ": preparing ROB requests for first sequence of each final output TE" << endmsg;
   }

   HLT::ErrorCode retCode = HLT::OK; // largest error code
   HLT::ErrorCode ec = HLT::OK;

   // loop over all Sequences
   if (m_config->getMsgLvl() <= MSG::DEBUG) {
      m_config->getMsgStream() << MSG::DEBUG << "Signature "
                               << m_configSig->label()
                               << " has " << m_requiredTEs.size() << " sequence(s):" << endmsg;

      std::string telabel;
      for (unsigned int i = 0; i < m_requiredTEs.size(); ++i) {
         TrigConf::HLTTriggerElement::getLabel(m_requiredTEs[i].sequence->outputTEType(), telabel);
         m_config->getMsgStream() << MSG::DEBUG << "    " << telabel << endmsg;
      }
   }
   for (unsigned int i = 0; i < m_requiredTEs.size(); ++i) {
      ec = m_requiredTEs[i].sequence->prepareRobRequests();

      if (ec.action() > HLT::Action::CONTINUE )
         return ec;

      // Debug output
      if (m_config->getMsgLvl() <= MSG::DEBUG) 
         m_config->getMsgStream() << MSG::DEBUG << "In signature " << m_configSig->label()
                                  << ": EC from sequence prepareRobRequests =  " << HLT::strErrorCode(ec) << endmsg;

      retCode = retCode > ec ? retCode : ec;
   }

   return retCode;
}



bool Signature::isValid() const
{
  return (m_requiredTEs.size() > 0);
}


bool Signature::checkRoIOverlap() const
{
  return (!makeTECombination().empty());
}


// this is not for online running (where we stop after searching for TEs after
// satisfying the signature) !
// this method simply returns a vector of combinations of TEs where each combination
// satisfies the Signature!
// NOTE this is time consuming !!!
const std::vector<std::vector< const TriggerElement*> >& Signature::makeAllTECombinations() const
{

  //std::cout << "makeAllTECombinatiosn() ... " << std::endl;

  m_allCombinations.clear();

  // Preparation: fill frame to find out combinations:
  // -------------------------------------------------
  std::vector< std::vector< const TriggerElement* > > remainingTEs;
  std::vector< unsigned int > remainingTEsFactor;
  unsigned int maxComb = 1;
  // loop over sequences => types that are produced
  for (unsigned int i = 0; i < m_requiredTEs.size(); ++i) {

    // loop over multiplicity
    for (unsigned int j = 0; j < m_requiredTEs[i].multiplicity; ++j) {

      //std::cout << "+-- adding column with TEs: ";
      remainingTEs.push_back(std::vector< const TriggerElement* >() );

      for (std::vector<TriggerElement*>::const_iterator iterTEs = m_requiredTEs[i].sequence->outputTEs().begin();
	   iterTEs !=  m_requiredTEs[i].sequence->outputTEs().end(); ++iterTEs) {
	if ( (*iterTEs)->getActiveState() ) {
	  remainingTEs.back().push_back( *iterTEs );
	  //std::cout << (*iterTEs)->getId() << " (at 0x" << *iterTEs << "), ";
	}
      }
      //std::cout << std::endl;
      remainingTEsFactor.push_back(maxComb);
      //std::cout << "factor = " << maxComb << std::endl;
      maxComb *= remainingTEs.back().size();


    }
  }

  // start method to find all combinations
  // -------------------------------------------------
  //  std::cout << "start the REAL THING now" << std::endl;
  unsigned int numAttempts = 0;
  while ( numAttempts < maxComb ) {
    //std::cout << "trying combination #" << numAttempts << std::endl;
    std::vector< const TriggerElement* > oneComb;
    for ( unsigned int node = 0; node < remainingTEs.size(); ++node) {
      //std::cout << "i am in row=" << node << std::endl;
      unsigned int fac = remainingTEsFactor[node];
      unsigned int mod = remainingTEs[node].size();
      unsigned int row = (numAttempts / fac) % mod;
      //std::cout << "fac=" << fac << " mod="<<mod<<" row="<<row<<std::endl;
      addIfNoOverlap(remainingTEs[node][row], oneComb);
    }
    // save if complete:
    if (oneComb.size() == remainingTEs.size()) {
      m_allCombinations.push_back( oneComb );
      //std::cout << "saved" << std::endl;
    }

    numAttempts++;
  }

  return m_allCombinations;
}

bool myTESort(const TriggerElement* left, const TriggerElement* right) {
  return left < right;
}

const std::vector< const TriggerElement*>& Signature::makeAllTEsFlat() const
{

  makeAllTECombinations();
  m_allFlatCombinations.clear();

  // make a unique list of these TEs:

  for ( unsigned int i = 0; i < m_allCombinations.size(); ++i) {
    for (unsigned int j=0; j < m_allCombinations[i].size(); ++j) {
      m_allFlatCombinations.push_back( m_allCombinations[i][j] );
    }
  }

  /*std::cout << "before unique, size="<< m_allFlatCombinations.size() << std::endl;
  for (unsigned int j=0; j < m_allFlatCombinations.size(); ++j) {
    std::cout << "TE=" << m_allFlatCombinations[j] << ", ";
  }
  std::cout << std::endl;*/

  // sort, make unique
  std::sort(m_allFlatCombinations.begin(), m_allFlatCombinations.end(), myTESort );

  /*std::cout << "after sort .. " << std::endl;
  for (unsigned int j=0; j < m_allFlatCombinations.size(); ++j) {
    std::cout << "TE=" << m_allFlatCombinations[j] << ", ";
  }
  std::cout << std::endl;*/

  std::vector< const HLT::TriggerElement* >::iterator l = unique(m_allFlatCombinations.begin(), m_allFlatCombinations.end());
  m_allFlatCombinations.erase(l, m_allFlatCombinations.end());

  //std::cout << "after unique, size="<< m_allFlatCombinations.size() << std::endl;

  return m_allFlatCombinations;
}

const std::vector<const TriggerElement*>& Signature::makeTECombination() const
{
  m_comb.clear();

  // Trivial case N_Seq == 1:
  if (m_requiredTEs.size() == 1) {

    for (std::vector<TriggerElement*>::const_iterator iterTEs = m_requiredTEs[0].sequence->outputTEs().begin();
         iterTEs !=  m_requiredTEs[0].sequence->outputTEs().end(); ++iterTEs)
      if ( (*iterTEs)->getActiveState() ) {
        addIfNoOverlap(*iterTEs, m_comb);
        if ( m_comb.size() == m_requiredTEs[0].multiplicity ) return m_comb;
      }

    // We didn't reach the expected size...
    m_comb.clear();
    return m_comb;
  }

  std::vector< std::vector< const TriggerElement* > > remainingTEs;
  std::vector< unsigned int > numMissingTEs;

  for (unsigned int i = 0; i < m_requiredTEs.size(); ++i) {

    remainingTEs.push_back(std::vector< const TriggerElement* >() );

    for (std::vector<TriggerElement*>::const_iterator iterTEs = m_requiredTEs[i].sequence->outputTEs().begin();
         iterTEs !=  m_requiredTEs[i].sequence->outputTEs().end(); ++iterTEs)
      if ( (*iterTEs)->getActiveState() ) remainingTEs.back().push_back( *iterTEs );

    numMissingTEs.push_back(m_requiredTEs[i].multiplicity);
  }

  if ( !recurseRoICombs( m_comb, remainingTEs, numMissingTEs ) ) m_comb.clear();

  return m_comb;
}


bool Signature::recurseRoICombs(std::vector< const TriggerElement* >& theComb,
                                std::vector< std::vector< const TriggerElement* > >& remainingTEs,
                                std::vector< unsigned int >& numMissingTEs) const
{
  // First find out which TE type we're missing
  int seq = -1;

  for (unsigned int i = 0; i < m_requiredTEs.size(); ++i)
    if (numMissingTEs[i] != 0) {
      seq = i;
      break;
    }

  //  if (seq == -1) std::cout << "Dunnit!" << std::endl;

  // If all sequences provided for -- we're done!
  if (seq == -1) return true;

  //  std::cout << "Current Comb: ";
  //  for (unsigned int j = 0; j < theComb.size(); j++) {
  //    std::cout << " " <<  theComb[j];
  //  }
  //  std::cout << std::endl;
  //  std::cout << "Remain " << std::endl;

  //  for (unsigned int i = 0; i < m_sequences.size(); i++) {
  //    std::cout << m_requiredTEs[i].first << " :";
  //
  //    for (unsigned int j = 0; j < remainingTEs[i].size(); j++) {
  //      std::cout << " " <<  remainingTEs[i][j];
  //    }
  //
  //    std::cout << " (" << numMissingTEs[i] << " req'd)" << std::endl;
  //  }


  // If not, try to add TEs of the required type

  std::vector< const TriggerElement* > removedForNow;

  while (remainingTEs[seq].size() > 0) {

    // Pop the last TE in the vector of available ones and save it
    const TriggerElement* te = remainingTEs[seq].back();
    remainingTEs[seq].pop_back();
    removedForNow.push_back(te);

    // If it doesn't overlap with the existing comb, add it and recurse.
    if ( addIfNoOverlap(te, theComb) ) {
      numMissingTEs[seq]--;

      //    std::cout << "adding " << te << " " << te->getId() << std::endl;

      if ( recurseRoICombs(theComb, remainingTEs, numMissingTEs) ) return true;
      theComb.pop_back();
      numMissingTEs[seq]++;
    }

    // Otherwise keep trying
  }

  //std::cout << "This branch failed!" << std::endl;

  // Put everything back in case we need it after we pop up a recursion level.
  std::copy(removedForNow.begin(), removedForNow.end(),
            std::back_insert_iterator< std::vector< const TriggerElement* > >( remainingTEs[seq] ) );

  return false;
}


bool Signature::addIfNoOverlap(const TriggerElement* te,
                               std::vector<const TriggerElement*>& theComb) const
{
  for (std::vector<const TriggerElement*>::const_iterator iterTEs = theComb.begin();
       iterTEs != theComb.end(); ++iterTEs) {
    bool commonRoI = m_config->getNavigation()->haveCommonRoI(*iterTEs, te);
    if (commonRoI) {
      // some debug output:
      if (m_config->getMsgLvl() <= MSG::DEBUG) {
        m_config->getMsgStream() << MSG::DEBUG << "Signature "
                                 << m_configSig->label()
                                 << ": found overlap between TEs: "
                                 << (*iterTEs) << " and " << te
                                 << endmsg;
      }
      return false;
    }
  }
  // everything's fine, there is no overlap so far. So, we can add this TE te to the vector theComb:
  theComb.push_back(te);
  return true;
}


/** Overload of << operator for both, MsgStream and std::ostream */
std::ostream& HLT::operator << (std::ostream& os, const HLT::Signature& c)
{
  c.print(os);
  return os;
}
MsgStream& HLT::operator << (MsgStream& msg, const HLT::Signature& c)
{
  c.print(msg);
  return msg;
}


//const std::vector< const std::pair<unsigned int, unsigned int> > Signature::getRequirements() const
//{
//  std::vector< const std::pair<unsigned int, unsigned int> > constVec;

//   for (std::vector< std::pair<unsigned int, unsigned int> >::const_iterator it = m_requiredTEs.begin();
//        it != m_requiredTEs.end(); ++it) {
//     const std::pair<unsigned int, unsigned int> myMap(*it);
//     constVec.push_back(myMap);
//   }
//  return constVec;
//}

const std::vector< const HLT::Sequence* > Signature::getSequences() const
{
  std::vector<const HLT::Sequence*> constVec;

  for (std::vector<RequiredTEs>::const_iterator it = m_requiredTEs.begin();
       it != m_requiredTEs.end(); ++it) {
    constVec.push_back(it->sequence);
  }
  return constVec;
}
