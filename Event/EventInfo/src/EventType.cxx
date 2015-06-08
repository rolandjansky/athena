/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EventType.cxx
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Paolo Calafiura <pcalafiura@lbl.gov>
 *
 * $Id: EventType.cxx,v 1.15 2009-05-18 08:31:07 schaffer Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "EventInfo/EventType.h"
// #include "AtlasMcWeight.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

// List of EventTypeCodes
//   To add on a new code, simply extend at the end 
const EventType::EventTypeCode  EventType::IS_SIMULATION        = 0;
const EventType::EventTypeCode  EventType::IS_TESTBEAM          = 1;
const EventType::EventTypeCode  EventType::IS_CALIBRATION       = 2;


EventType::EventType()
        : m_mc_event_weights(1,1),
          m_mc_channel_number(0),
          m_mc_event_number(0)

{}


EventType::~EventType()
{}

/// Less than comparision needed to create e.g. set<EventType>
bool
EventType::operator<(const EventType& rhs) const
{
   if (this->m_bit_mask.size() != rhs.m_bit_mask.size()) {
      return(this->m_bit_mask.size() < rhs.m_bit_mask.size());
   }
   for (BitMaskIterator i = this->bit_mask_begin(), j = rhs.bit_mask_begin();
	   i != this->bit_mask_end(); i++, j++) {
      if (*i != *j) {
         return(*i < *j);
      }
   }
   return(this->m_user_type < rhs.m_user_type);
}

void 		
EventType::add_type (EventTypeCode type_code)
{
    if(m_bit_mask.size() <= type_code) m_bit_mask.resize(type_code + 1, false);
    m_bit_mask[type_code] = true;
}

void 		
EventType::set_user_type	(const std::string& user_type)
{
    m_user_type = user_type;
}

bool 		
EventType::test		(EventTypeCode type_code) const
{
    if (m_bit_mask.size() <= type_code) return false;
    return m_bit_mask[type_code];
}

std::string
EventType::typeToString    () const
{
    std::string result;

    result +=  "Event type: sim/data - ";
    if (test(EventType::IS_SIMULATION)) {
	result += " is sim ";
    }
    else {
	result += " is data ";
    }
	result += ", testbeam/atlas - ";
    if (test(EventType::IS_TESTBEAM)) {
	result += " is testbeam ";
    }
    else {
	result += " is atlas ";
    }
	result += ", calib/physics - ";
    if (test(EventType::IS_CALIBRATION)) {
	result += " is calib ";
    }
    else {
	result += " is physics ";
    }
    return (result);
}


const std::string&	
EventType::user_type	(void) const
{
    char sep = '#';
    std::string::size_type beg = m_user_type.find(sep);
    static std::string user_type;
    if (beg != std::string::npos) {
	user_type = m_user_type.substr(0, beg);
    }
    else {
	user_type = m_user_type;
    }
    return user_type;
}


void       
EventType::get_detdescr_tags(NameTagPairVec& pairs) const
{
    // We must extract from m_user_type the dd tags for the moment to
    // avoid schema evolution.

    char sep = '#';
    bool done = false;
    std::string::size_type beg = m_user_type.find(sep);
    do {
	if (beg != std::string::npos) {
	    std::string::size_type end1 = m_user_type.find(sep, beg+1);
	    if (end1 != std::string::npos) {
		std::string::size_type end2 = m_user_type.find(sep, end1+1);
		if (end2 != std::string::npos) {
		    // end2 is a new separator
		    std::string first  = m_user_type.substr(beg+1, end1-beg-1);
		    std::string second = m_user_type.substr(end1+1, end2-end1-1);
		    pairs.push_back(NameTagPair(first,second));

		    // continue with new beg
		    beg = end2;
		}
		else {
		    // end2 is the end of the string
		    std::string first  = m_user_type.substr(beg+1, end1-beg-1);
		    std::string second = m_user_type.substr(end1+1, m_user_type.size()-1);
		    pairs.push_back(NameTagPair(first,second));
		    done = true; // finished
		}
	    }
	    else {
		done = true; // finished
	    }
	}
	else {
	    done = true; // finished
	}
    } while (!done);
}

const std::string&  
EventType::get_detdescr_tags() const
{
    // Concatenate the dd tags to a single string
    NameTagPairVec pairs;
    get_detdescr_tags(pairs);
    static std::string result;
    result.clear();
    for (unsigned int i = 0; i < pairs.size(); ++i) {
        result += pairs[i].first + ' ' + pairs[i].second + ' ';
    }
    return (result);
}

EventType::number_type
EventType::mc_channel_number() const
{
    return m_mc_channel_number;
}


uint64_t
EventType::mc_event_number() const
{
    return m_mc_event_number;
}

    

float 
EventType::mc_event_weight  (unsigned int iweight) const
{
    if (iweight < m_mc_event_weights.size()) return m_mc_event_weights[iweight];
    return 0;
}


EventType::BitMaskIterator 	
EventType::bit_mask_begin	(void) const
{
    return m_bit_mask.begin();
}

EventType::BitMaskIterator 	
EventType::bit_mask_end		(void) const
{
    return m_bit_mask.end();
}


void
EventType::set_mc_channel_number(number_type chan)
{
    m_mc_channel_number =  chan;
}



void
EventType::set_mc_event_number(uint64_t evt)
{
    m_mc_event_number = evt;
}


void
EventType::set_mc_event_weight(float weight, unsigned int iweight, unsigned int nWeightsMax)
{
    if (nWeightsMax > m_mc_event_weights.capacity()) m_mc_event_weights.reserve(nWeightsMax);
    if (m_mc_event_weights.size() <= iweight) m_mc_event_weights.resize(iweight + 1);
    m_mc_event_weights[iweight] = weight;
}

unsigned int
EventType::n_mc_event_weights () const
{
    return (unsigned int)m_mc_event_weights.size();
}

void
EventType::reset_detdescr_tags()
{
    m_user_type = user_type();
}

