/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     TriggerDecision.cxx
PACKAGE:  Trigger/TrigEvent/TrigSteeringEvent/

AUTHORS:  M. Wielers
CREATED:  Sep 2005

PURPOSE:  temporary class to store the trigger decision

********************************************************************/

// INCLUDE HEADER FILES:

#include "TrigSteeringEvent/TriggerDecision.h"
#include <iomanip>
#include <iostream>
#include <string>

//  END OF HEADER FILES INCLUDE

// ----------------------------
//  Constructors
// ----------------------------
TriggerDecision::TriggerDecision() :
  m_L1defined(false), m_L2defined(false), m_EFdefined(false)
{ }

TriggerDecision::TriggerDecision(bool defL1, bool defL2, bool defEF) :
  m_L1defined(defL1), m_L2defined(defL2), m_EFdefined(defEF)
{ }

// --------------------------
// Destructor
// --------------------------
TriggerDecision::~TriggerDecision()
{ }

// --------------------------
// print contents of TriggerDecision map
// ----------------------------
// private print method
void TriggerDecision::print(bool print_all) const
{
  // to use message service
  std::string thisName("TriggerDecision::print");
  MsgStream log(m_messageSvcProvider.msgSvc(), thisName);

  int NoMenuItem = m_TriggerMapL1.size() + m_TriggerMapL2.size()
    + m_TriggerMapEF.size() - m_ignore.size();

  log << MSG::INFO << "TriggerDecision: no of TriggerItems defined in this run "
	    << NoMenuItem<< endmsg;

  // L1
  log << MSG::INFO << "TriggerDecision: Level 1:" <<endmsg;
  std::map<std::string,bool>::const_iterator it1 = m_TriggerMapL1.begin();
  std::map<std::string,bool>::const_iterator it1E = m_TriggerMapL1.end();
  for (; it1!=it1E; ++it1) {

    if (print_all || it1->second) {
      log << MSG::INFO << "TriggerDecision: L1 Trigger Item " << it1->first
	  << " " << ((it1->second) ? "is fulfilled" : " did not pass") <<endmsg;
    }
  }
  // L2
  log << MSG::INFO << "TriggerDecision: Level 2:" <<endmsg;
  std::map<std::string,bool>::const_iterator it2 = m_TriggerMapL2.begin();
  std::map<std::string,bool>::const_iterator it2E = m_TriggerMapL2.end();
  for (; it2!=it2E; ++it2) {

    if (print_all || it2->second) {
      log << MSG::INFO << "TriggerDecision: L2 Trigger Item " << it2->first
	  << " " << ((it2->second) ? "is fulfilled" : " did not pass") <<endmsg;
    }
  }
  // EF
  log << MSG::INFO << "TriggerDecision: Event Filter:" <<endmsg;
  std::map<std::string,bool>::const_iterator it = m_TriggerMapEF.begin();
  std::map<std::string,bool>::const_iterator itE = m_TriggerMapEF.end();
  for (; it!=itE; ++it) {

    if (print_all || it->second) {
      log << MSG::INFO << "TriggerDecision: EF Trigger Item " << it->first
	  << " " << ((it->second) ? "is fulfilled" : " did not pass") <<endmsg;
    }
  }
  if (!print_all && ! isTriggerPassed()) {
    log << MSG::INFO << "TriggerDecision: No trigger item passed " << endmsg;
  }
}

// print all signatures: public method
void TriggerDecision::print() const {
  this->print(true);
}

// print accepted signatures: public method
void TriggerDecision::printAccepted() const {
  this->print(false);
}


// --------------------------
// Find if some signature is defined (at any level or at a certain level)
// ----------------------------
bool TriggerDecision::isDefined(std::string TriggerItem) const {
  return ( isDefined(TriggerItem, L1) ||
	   isDefined(TriggerItem, L2) ||
	   isDefined(TriggerItem, EF) );
}

bool TriggerDecision::isDefined(std::string TriggerItem, unsigned int level) const {

  // to use message service
  std::string thisName("TriggerDecision::isDefined");
  MsgStream log(m_messageSvcProvider.msgSvc(), thisName);

  std::map<std::string,bool>::const_iterator it;

  if (level == L1 && m_L1defined) {
    it = m_TriggerMapL1.find(TriggerItem);
    if (it != m_TriggerMapL1.end()) {
      return true;
    } else {
      log << MSG::VERBOSE << "Trigger Item " << TriggerItem
	  << " not defined!!!" << endmsg;
    }
  } else if (level == L2 && m_L2defined) {
    it = m_TriggerMapL2.find(TriggerItem);
    if (it != m_TriggerMapL2.end()) {
      return true;
    } else {
      log << MSG::VERBOSE << "Trigger Item " << TriggerItem
	  << " not defined!!!" << endmsg;
    }
  } else if (level == EF && m_EFdefined) {
    it = m_TriggerMapEF.find(TriggerItem);
    if (it != m_TriggerMapEF.end()) {
      return true;
    } else {
      log << MSG::VERBOSE << "Trigger Item " << TriggerItem
	  << " not defined!!!" << endmsg;
    }
  } else {
    log << MSG::VERBOSE << "Trigger level enum TrigLevel["
	<< level << "] not defined!!!" << endmsg;
  }
  return false;
}


// --------------------------
// Find if some signature was passed (at some trigger level)
// ----------------------------
bool TriggerDecision::isTriggered(std::string TriggerItem) const {
  // to use message service
  std::string thisName("TriggerDecision::isTriggered");
  MsgStream log(m_messageSvcProvider.msgSvc(), thisName);

  // look in all levels
  if ( isDefined(TriggerItem,L1) ) {
    return ( isTriggered(TriggerItem,L1) );
  } else if ( isDefined(TriggerItem,L2) ) {
    return ( isTriggered(TriggerItem,L2) );
  } else if ( isDefined(TriggerItem,EF) ) {
    return ( isTriggered(TriggerItem,EF) );
  } else {
    log << MSG::VERBOSE << "Trigger Item " << TriggerItem
	<< " not defined!!!" << endmsg;
  }
  return false;
}

bool TriggerDecision::isTriggered(std::string TriggerItem, unsigned int level) const {

  // to use message service
  std::string thisName("TriggerDecision::isTriggered");
  MsgStream log(m_messageSvcProvider.msgSvc(), thisName);

  std::map<std::string,bool>::const_iterator it;

  if (level == L1 && m_L1defined) {
    it = m_TriggerMapL1.find(TriggerItem);
    if (it != m_TriggerMapL1.end()) {
      return (it->second);
    } else {
      log << MSG::VERBOSE << "Trigger Item " << TriggerItem
	  << " not defined!!!" << endmsg;
      return false;
    }
  } else if (level == L2 && m_L2defined) {
    it = m_TriggerMapL2.find(TriggerItem);
    if (it != m_TriggerMapL2.end()) {
      return (it->second);
    } else {
      log << MSG::VERBOSE << "Trigger Item " << TriggerItem
	  << " not defined!!!" << endmsg;
      return false;
    }
  } else if (level == EF && m_EFdefined) {
    it = m_TriggerMapEF.find(TriggerItem);
    if (it != m_TriggerMapEF.end()) {
      return (it->second);
    } else {
      log << MSG::VERBOSE << "Trigger Item " << TriggerItem
	  << " not defined!!!" << endmsg;
      return false;
    }
  } else {
    log << MSG::VERBOSE << "Trigger level enum TrigLevel["
	<< level << "] not defined!!!" << endmsg;
    return false;
  }
  return false;
}

// --------------------------
// Find if event passed trigger selection
// --------------------------
// at the highest defined trigger level
bool TriggerDecision::isTriggerPassed() const
{
  // to use message service
  std::string thisName("TriggerDecision::isTriggerPassed");
  MsgStream log(m_messageSvcProvider.msgSvc(), thisName);

  // returns true if EF defined and some EF signature passed
  // if EF not defined, try next level down, and so on
  bool triggered = false;

  if (m_EFdefined) {
    triggered = isPassedEF();
  } else if (m_L2defined) {
    triggered = isPassedL2();
  } else if (m_L1defined) {
    triggered = isPassedL1();
  } else {
    log << MSG::VERBOSE << "No Trigger Levels defined!!!" << endmsg;
  }

  return (triggered);
}

// at level 1
bool TriggerDecision::isPassedL1() const {

  bool triggered = false;
  std::map<std::string,bool>::const_iterator it = m_TriggerMapL1.begin();
  std::map<std::string,bool>::const_iterator itE = m_TriggerMapL1.end();
  for (; it!=itE; ++it) {
    if (it->second==true) triggered=true;
  }
  return triggered;
}

// at level 2
bool TriggerDecision::isPassedL2() const {

  bool triggered = false;
  std::map<std::string,bool>::const_iterator it = m_TriggerMapL2.begin();
  std::map<std::string,bool>::const_iterator itE = m_TriggerMapL2.end();
  for (; it!=itE; ++it) {
    if (it->second==true) triggered=true;
  }
  return triggered;
}

// at event filter
bool TriggerDecision::isPassedEF() const {

  bool triggered = false;
  std::map<std::string,bool>::const_iterator it = m_TriggerMapEF.begin();
  std::map<std::string,bool>::const_iterator itE = m_TriggerMapEF.end();
  for (; it!=itE; ++it) {
    if (it->second==true) triggered=true;
  }
  return triggered;
}


// ----------------------------------------------------------------
// Accessors to return list of signatures and copy of internal maps
// ----------------------------------------------------------------

// return vector with all signature labels for all configured trigger levels
std::vector<std::string> TriggerDecision::signatures() const
{
  std::vector<std::string> v_labels;

  if (m_L1defined) {
    std::vector<std::string> t_vec = signatures(L1);
    for (unsigned int i=0; i<t_vec.size(); ++i) v_labels.push_back(t_vec[i]);
  }
  if (m_L2defined) {
    std::vector<std::string> t_vec = signatures(L1);
    for (unsigned int i=0; i<t_vec.size(); ++i) v_labels.push_back(t_vec[i]);
  }
  if (m_EFdefined) {
    std::vector<std::string> t_vec = signatures(L1);
    for (unsigned int i=0; i<t_vec.size(); ++i) v_labels.push_back(t_vec[i]);
  }
  return v_labels;
}


// return vector with labels for all signatures defined at a certain level
std::vector<std::string> TriggerDecision::signatures(unsigned int level) const
{
  std::map<std::string, bool>::const_iterator it;
  std::map<std::string, bool>::const_iterator end;
  std::vector<std::string> v_labels;

  // find which level map should be read
  if (level == L1 && m_L1defined) {
    it  = m_TriggerMapL1.begin();
    end = m_TriggerMapL1.end();
  } else if (level == L2 && m_L2defined) {
    it  = m_TriggerMapL2.begin();
    end = m_TriggerMapL2.end();
  } else if (level == EF && m_EFdefined) {
    it  = m_TriggerMapEF.begin();
    end = m_TriggerMapEF.end();
  } else {
    return v_labels; // no level selected, return empty vector
  }

  // loop over map for selected level and fill output vector
  for (; it != end; ++it) {
    // push label into vector
    v_labels.push_back( it->first );
  }

  return v_labels;

}

// return copies of the internal maps
std::map<std::string,bool> TriggerDecision::L1map() const {
  return m_TriggerMapL1;
}

std::map<std::string,bool> TriggerDecision::L2map() const {
  return m_TriggerMapL2;
}

std::map<std::string,bool> TriggerDecision::EFmap() const {
  return m_TriggerMapEF;
}

std::vector<std::string> TriggerDecision::ignoredSigs() const {
  return m_ignore;
}


// --------------------------
// Methods to fill map
// --------------------------
// insert trigger items in map
void TriggerDecision::set_TriggerItem(std::string TriggerItem, bool triggered, unsigned int level)
{
  if (level == L1 && m_L1defined) {
    m_TriggerMapL1.insert(std::map<std::string,bool>::value_type(TriggerItem, triggered));
    return;
  } else if (level == L2 && m_L2defined) {
    m_TriggerMapL2.insert(std::map<std::string,bool>::value_type(TriggerItem, triggered));
    return;
  } else if (level == EF && m_EFdefined) {
    m_TriggerMapEF.insert(std::map<std::string,bool>::value_type(TriggerItem, triggered));
    return;
  }
}

// set ignorable signatures
void TriggerDecision::set_ignore_TriggerItem(std::string TriggerItem)
{
  m_ignore.push_back(TriggerItem);
}

// check if trigger item is ignorable // this is unused anyway
bool TriggerDecision::ignore_TriggerItem(std::string TriggerItem)
{
  bool ignore=false;
  for (size_t i=0; i<m_ignore.size();++i) {
    if (m_ignore[i]==TriggerItem) {
      ignore=true;
      break;
    }
  }
  return (ignore);
}
