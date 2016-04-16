/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
#ifndef ANALYSISTRIGGEREVENT_TRIGGERDECISION_H
#define ANALYSISTRIGGEREVENT_TRIGGERDECISION_H

/**
 *  Prototype to provide Trigger Decision to end-user
 *
 *  @author M. Wielers
 */
#include <map>
#include <string>
#include <vector>

#include "CLIDSvc/CLASS_DEF.h"

// for message service provider
#include "TrigSteeringEvent/MessageSvcProvider.h"

/**
 * @brief Sumarises the decision of the trigger in user friendly form.
 * @warning this class is obsolete and is kept only for purpose of data reading done in rel 12 or before 
 */
class TriggerDecision
{
public:

  enum TrigLevel {L1=1,L2,EF};

  TriggerDecision ();
  /**
   * @brief custom c'tor 
   * @param l1 enabled LVL1
   * @param l2 enabled LVL2
   * @param ef enabled EF
   */
  TriggerDecision (bool l1, bool l2, bool ef);

  ~TriggerDecision ();

  // methods

  void print() const;
  void printAccepted() const;

  bool    isDefinedL1() const { return m_L1defined; }
  bool    isDefinedL2() const { return m_L2defined; }
  bool    isDefinedEF() const { return m_EFdefined; }
  bool    isDefined(std::string) const;
  bool    isDefined(std::string, unsigned int) const;

  bool    isTriggerPassed() const;  //!< global decision
  bool    isPassedL1() const;
  bool    isPassedL2() const;
  bool    isPassedEF() const;
  /**
   * @brief checks if given signature is satisfied
   * @param name signature name
   */
  bool    isTriggered(std::string name) const;         

  /**
   * @brief checks if given signature is satisfied
   * @param name signature name
   * @param levle further details in which level, @see TrigLevel
   */
  bool    isTriggered(std::string name, unsigned int level) const;  


  
  std::vector<std::string> signatures() const;          //!< list of all defined signatures   
  std::vector<std::string> signatures(unsigned int level) const; //!< list of defined signatures in level @see TrigLevel 

  /**
   * @brief all signatures to satisfied/unsatisfied map for Lvl1
   * This map contains mapping from signature name to the boolean which is true if given signature was active false otherwise. 
   */
  std::map<std::string,bool> L1map() const;              
  /**
   * @brief  all signatures to satisfied/unsatisfied map for Lvl2
   * @see  L1map()
   */
  std::map<std::string,bool> L2map() const;

  /**
   * @brief  all signatures to satisfied/unsatisfied map for Lvl2
   * @see  L1map()
   */
  std::map<std::string,bool> EFmap() const;

  /**
   * @brief list of signatures which were ignored during TriggerDecision object creation (do not enter to the map)
   */
  std::vector<std::string> ignoredSigs() const;

  
  void    set_TriggerItem(std::string, bool, unsigned int); //!< sets the active/inactive state for a signature
  void    set_ignore_TriggerItem(std::string);              //!< adds signature to the list of this to be ignored
  bool    ignore_TriggerItem(std::string);                  //!< checki if signature is ignored

private:

  void print(bool print_all) const;             //!< preety print


  bool m_L1defined, m_L2defined, m_EFdefined;   //!< flags for trigger levels


  std::map<std::string, bool> m_TriggerMapL1;   //!< map of activity for Lvl1
  std::map<std::string, bool> m_TriggerMapL2;   //!< map of activity for Lvl2
  std::map<std::string, bool> m_TriggerMapEF;   //!< map of activity for EF

  std::vector<std::string>  m_ignore;           //!< signatures to ignore

  MessageSvcProvider m_messageSvcProvider;      //!< needed for printing out messages using the message service

};

CLASS_DEF( TriggerDecision , 28494668, 1 )

#endif
