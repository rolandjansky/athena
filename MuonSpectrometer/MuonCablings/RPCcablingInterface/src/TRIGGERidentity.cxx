/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>
#include "RPCcablingInterface/TRIGGERidentity.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

const char TRIGGERidentity::s_type_tag [NTYPE][8]          = { {"no"},
                                                               {"low Pt"},
                                                               {"high Pt"} };
const char TRIGGERidentity::s_threshold_tag[NTHRESHOLD][7] = { {"first"},
                                                               {"second"},
                                                               {"third"} };
const float TRIGGERidentity::s_pt_value[NTYPE][NTHRESHOLD] = {{
                                                               0.,0.,0.
                                                              },
                                                              {
                                                               6.,8.,10.
                                                              },
							      {
                                                               11.,20.,40. 
                                                              }};

///////////////////////////////////////////////////////////////////////////////

TRIGGERidentity::TRIGGERidentity() :
    m_type(None),m_threshold(FirstTh),m_overlap(false) {}

TRIGGERidentity::TRIGGERidentity(TrigType type,Threshold threshold,bool flag) :
    m_type(type),m_threshold(threshold),m_overlap(flag) {}

TRIGGERidentity::TRIGGERidentity(const TRIGGERidentity& id) :
    m_type(id.type()),m_threshold(id.threshold()),m_overlap(id.overlap()) {}

TRIGGERidentity&
TRIGGERidentity::operator=(const TRIGGERidentity& id)
{
    if (this!=&id) { 
      m_type = id.type();
      m_threshold = id.threshold();
      m_overlap = id.overlap();
    }
    return *this;
}

bool
TRIGGERidentity::operator==(const TRIGGERidentity& id) const
{
    if(m_type      == id.type()      && 
       m_threshold == id.threshold() &&  
       m_overlap   == id.overlap() ) return true;
    else return false;
}

bool
TRIGGERidentity::operator!=(const TRIGGERidentity& id) const
{
    return !(*this == id);
}

bool
TRIGGERidentity::operator< (const TRIGGERidentity& id) const
{
    if(m_type <  id.type() ) return true;
    if(m_type == id.type())
    {
        if(m_threshold <  id.threshold()) return true;
	if(m_threshold == id.threshold())
	{
	    if (m_overlap < id.overlap()) return true;
	}
    }
    return false;
}

TRIGGERidentity::operator bool()
{
    if(m_type == None) return false;
    else               return true;
}

bool
TRIGGERidentity::operator !()
{
    return ! (bool)*this;
}


bool
TRIGGERidentity::highest(TrigType type) const
{
    if(type == High)
    {
        if(m_type == High && m_threshold == SecondTh ) return true;
    }
    if(type == Low)
    {
        if(m_type == Low && m_threshold == ThirdTh ) return true;
    }
    return false;
}

bool
TRIGGERidentity::searchForHigh() const
{
    TRIGGERidentity maxLow(Low,ThirdTh,false);
    if ( *this < maxLow ) return false;
    else return true;
}


void TRIGGERidentity::Print(ostream& stream,bool detail) const
{
    detail = true;
    if(detail)
    {
        stream << s_type_tag[type()] << " trigger, " 
               << s_threshold_tag[threshold()]
	       << " threshold fired (" << setw(4) 
               << s_pt_value[type()][threshold()]
	       << " GeV), overlap flag ";
        if(overlap()) stream << "ON";
        else          stream << "OFF";
    }
}
