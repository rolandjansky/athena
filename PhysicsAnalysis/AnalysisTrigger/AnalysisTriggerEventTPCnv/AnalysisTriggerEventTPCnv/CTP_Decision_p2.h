/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTRIGGEREVENTTPCNV_CTP_DECISION_P2_H
#define ANALYSISTRIGGEREVENTTPCNV_CTP_DECISION_P2_H

// Needed includes:
#include <vector>
#include <string>
#include <Rtypes.h>

// Forward declaration of converter:
class CTP_DecisionCnv_p2;

/**
 *   @short Persistent representation of CTP_Decision
 *
 *          This is the second version of the persistent representation(s) of CTP_Decision.
 *
 * @version $Revision: 1.1 $
 *    @date $Date: 2007-07-10 17:11:13 $
 *  @author Wolfgang Ehrenfeld
 */
class CTP_Decision_p2 {

  friend class CTP_DecisionCnv_p2;

public:
  CTP_Decision_p2();

protected:
  std::vector<UInt_t> m_CTPResultWord;
  
  UInt_t m_triggerTypeWord;

  std::vector< std::string > m_items;

}; // class CTP_Decision_p2

inline CTP_Decision_p2::CTP_Decision_p2()
  : m_triggerTypeWord( 0 ), m_items( 0 ) {
  m_CTPResultWord.reserve(8);
}

#endif // ANALYSISTRIGGEREVENTTPCNV_CTP_DECISION_P2_H
