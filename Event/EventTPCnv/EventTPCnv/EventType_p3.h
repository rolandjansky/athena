/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTPCNV_EVENTTYPE_P3_H
#define EVENTTPCNV_EVENTTYPE_P3_H
/*
 * @file EventType_p3.h
 *
 * @brief This class provides persistent representation for EventType class
 * @author Marcin.Nowak@cern.ch
 */

#include <vector>
#include <string>

class EventType_p3 {
public:

    typedef     unsigned int                        number_type;
    typedef	std::vector<bool>                   BitMask;  
    typedef     BitMask::const_iterator             BitMaskIterator;  

    EventType_p3();
    bool                operator<       (const EventType_p3& rhs) const;

    BitMask 		m_bit_mask; 
    std::string 	m_user_type;
    std::vector<float>  m_mc_event_weights;
    number_type         m_mc_channel_number;
    uint64_t            m_mc_event_number;
};

/// inlines 
inline
EventType_p3::EventType_p3()
        :
             m_mc_channel_number(0),
             m_mc_event_number(0)
{}

inline
bool EventType_p3::operator<(const EventType_p3& rhs) const
{
   if (this->m_bit_mask.size() != rhs.m_bit_mask.size()) {
      return(this->m_bit_mask.size() < rhs.m_bit_mask.size());
   }
   for (BitMaskIterator i = this->m_bit_mask.begin(), j = rhs.m_bit_mask.begin();
           i != this->m_bit_mask.end(); i++, j++) {
      if (*i != *j) {
         return(*i < *j);
      }
   }
   return(this->m_user_type < rhs.m_user_type);
}
#endif
