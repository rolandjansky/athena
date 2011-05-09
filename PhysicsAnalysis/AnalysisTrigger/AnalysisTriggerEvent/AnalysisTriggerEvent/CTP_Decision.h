// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CTP_Decision.h,v 1.10 2008-05-20 16:59:50 efeld Exp $
#ifndef ANALYSISTRIGGEREVENT_CTP_DECISION_H
#define ANALYSISTRIGGEREVENT_CTP_DECISION_H

// Standard include(s):
#include <inttypes.h>

// STL include(s):
#include <string>
#include <vector>

// Gaudi/Athena include(s):
#include "CLIDSvc/CLASS_DEF.h"

/**
 *   @short Legacy data class representing the LVL1 CTP trigger decision
 *
 *          This class holds the LVL1 CTP trigger decision, 
 *          a vector of fired LVL1 items and trigger type word.
 *          The CTP trigger decision vector is 256 bits long, arranged 
 *          into 8 32 bit words, which can be accessed either independently
 *          or as a vector.
 *
 *          This class is now outdated. For analysis purposes one should
 *          rather use TrigDec::TrigDecisionTool to retrieve the LVL1
 *          decision for the event.
 *
 *  @author Tadashi Maeno <Tadashi.Maeno@cern.ch>
 *  @author Wolfgang Ehrenfeld <Wolfgang.Menges@desy.de>
 * @version \$Revision: 1.10 $
 *    @date \$Date: 2008-05-20 16:59:50 $
 */
class CTP_Decision {

public:
   /// Default (empty) constructor
   CTP_Decision() : m_items( 0 ), m_triggerTypeWord( 0 ) {
      m_CTPResultWord.reserve( 8 );
   }
   /// Destructor
   virtual ~CTP_Decision() {}

   /// Type storing the names of the passed LVL1 items
   typedef std::vector< std::string > items_type;

   /// Test if event was accepted at LVL1
   bool isAccepted() const { return m_items.size(); }

   /// Get a vector of fired LVL1 items
   const items_type& getItems() const { return m_items; }

   /// Get vector of CTP trigger words
   std::vector< uint32_t > getWords() const { return m_CTPResultWord; }
   /// Get one CTP trigger word
   uint32_t getWord( const unsigned int i ) const {
      return m_CTPResultWord.size() > i ? m_CTPResultWord[ i ] : 0;
   }

   uint32_t getWord0() const { return getWord( 0 ); } //!< get 1. CTP trigger word
   uint32_t getWord1() const { return getWord( 1 ); } //!< get 2. CTP trigger word
   uint32_t getWord2() const { return getWord( 2 ); } //!< get 3. CTP trigger word
   uint32_t getWord3() const { return getWord( 3 ); } //!< get 4. CTP trigger word
   uint32_t getWord4() const { return getWord( 4 ); } //!< get 5. CTP trigger word
   uint32_t getWord5() const { return getWord( 5 ); } //!< get 6. CTP trigger word
   uint32_t getWord6() const { return getWord( 6 ); } //!< get 7. CTP trigger word
   uint32_t getWord7() const { return getWord( 7 ); } //!< get 8. CTP trigger word

   /// Get trigger type word
   uint32_t getTriggerTypeWord() const { return m_triggerTypeWord; }

   /// Set vector of fired LVL1 items 
   void setItems( const items_type& items ) { m_items = items; }
   /// Set a fired LVL1 item
   void setItem( const items_type::value_type &item ) { m_items.push_back(item); }

   /// Set vector of CTP trigger words 
   void setWords( const std::vector< uint32_t > words ) { m_CTPResultWord = words; }
   /// Set one CTP trigger word 
   void setWord( const unsigned int i, const uint32_t word ) { 
      if( m_CTPResultWord.size() <= i ) m_CTPResultWord.resize( i + 1 );
      m_CTPResultWord[ i ] = word;
   }

   void setWord0( const uint32_t word ) { setWord( 0, word ); } //!< set 1. CTP trigger word 
   void setWord1( const uint32_t word ) { setWord( 1, word ); } //!< set 2. CTP trigger word
   void setWord2( const uint32_t word ) { setWord( 2, word ); } //!< set 3. CTP trigger word
   void setWord3( const uint32_t word ) { setWord( 3, word ); } //!< set 4. CTP trigger word
   void setWord4( const uint32_t word ) { setWord( 4, word ); } //!< set 5. CTP trigger word
   void setWord5( const uint32_t word ) { setWord( 5, word ); } //!< set 6. CTP trigger word
   void setWord6( const uint32_t word ) { setWord( 6, word ); } //!< set 7. CTP trigger word
   void setWord7( const uint32_t word ) { setWord( 7, word ); } //!< set 8. CTP trigger word

   /// Set trigger type word.
   void setTriggerTypeWord( uint32_t word ) { m_triggerTypeWord = word; }

   /// Dump raw content of object to string 
   const std::string dump() const;

private:
   items_type m_items;                      //!< vector of fired LVL1 items
   std::vector< uint32_t > m_CTPResultWord; //!< vector of CTP trigger words
   uint32_t m_triggerTypeWord;              //!< LVL1 trigger type word

}; // class CTP_Decision

CLASS_DEF( CTP_Decision, 123529088, 1 )

#endif // ANALYSISTRIGGEREVENT_CTP_DECISION_H
