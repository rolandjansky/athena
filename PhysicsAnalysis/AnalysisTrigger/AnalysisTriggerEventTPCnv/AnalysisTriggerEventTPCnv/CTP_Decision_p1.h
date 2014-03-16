/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTRIGGEREVENTTPCNV_CTP_DECISION_P1_H
#define ANALYSISTRIGGEREVENTTPCNV_CTP_DECISION_P1_H

// Needed includes:
#include <vector>
#include <string>
#include <Rtypes.h>

// Forward declaration of converter:
class CTP_DecisionCnv_p1;

/**
 *   @short Persistent representation of CTP_Decision
 *
 *          This is the first version of the persistent representation(s) of CTP_Decision.
 *          It's basically the same as the original, without the accessor functions...
 *
 * @version $Revision: 1.1 $
 *    @date $Date: 2007-07-10 17:11:12 $
 *  @author Attila Krasznahorkay Jr.
 */
class CTP_Decision_p1 {

  friend class CTP_DecisionCnv_p1;

public:
  CTP_Decision_p1();

protected:
  UInt_t m_word0;
  UInt_t m_word1;
  UInt_t m_word2;
  UInt_t m_triggerTypeWord;

  std::vector< std::string > m_items;

}; // class CTP_Decision_p1

inline CTP_Decision_p1::CTP_Decision_p1()
  : m_word0( 0 ), m_word1( 0 ), m_word2( 0 ), m_triggerTypeWord( 0 ), m_items( 0 ) {

}

#endif // ANALYSISTRIGGEREVENTTPCNV_CTP_DECISION_P1_H
