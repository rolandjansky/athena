/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IdDict package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: IdDictFieldImplementation.cxx,v 1.4 2003-09-04 08:55:25 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "IdDict/IdDictFieldImplementation.h"
#include "IdDict/IdDictDefs.h"
#include <iostream>
#include <sstream>

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

static void tabify (int ntot_spaces, std::stringstream& str) 
{ 
    for (int i = 0; i < ntot_spaces; ++i) { 
	str << " "; 
    } 
} 

//-----------------------------------------------------------------
const IdDictRange*  IdDictFieldImplementation::range() const
//-----------------------------------------------------------------
{ return (m_range); } 

//-----------------------------------------------------------------
void  IdDictFieldImplementation::set_range        (const IdDictRange* range)
//-----------------------------------------------------------------
{
    m_range = range;
}

//-----------------------------------------------------------------
void IdDictFieldImplementation::show             (void) const
//-----------------------------------------------------------------
{
    std::cout << show_to_string() << std::endl;
}

//-----------------------------------------------------------------
std::string IdDictFieldImplementation::show_to_string   (void) const
{
    std::stringstream str;

    unsigned int pos;
    unsigned int nchar;
    str	<< "decode " << m_decode_index;
    str << " vals ";
    pos = str.tellp();
    // values
    str << (std::string)m_ored_field;
    nchar = (unsigned int)str.tellp() - pos;
    if (nchar < 15) tabify(15 - nchar, str);
    str << " mask/zero mask/shift/bits/offset ";
    pos = str.tellp();
    // mask
    str << std::hex << m_mask << " ";
    nchar = (unsigned int)str.tellp() - pos;
    if (nchar < 4) tabify(4 - nchar, str);
    // zeroing mask
    str << m_zeroing_mask << " " << std::dec;
    pos = str.tellp();
    // shift
    str << m_shift << " ";
    nchar = (unsigned int)str.tellp() - pos;
    if (nchar < 3) tabify(3 - nchar, str);
    pos = str.tellp();
    // bits
    str << m_bits << " ";
    nchar = (unsigned int)str.tellp() - pos;
    if (nchar < 3) tabify(3 - nchar, str);
    pos = str.tellp();
    // offset
    str <<m_bits_offset << " ";
    nchar = (unsigned int)str.tellp() - pos;
    if (nchar < 3) tabify(3 - nchar, str);
    str << "indexes ";
    pos = str.tellp();
    // indexes
    for (size_type i = 0; i < m_ored_field.get_indexes().size(); ++i) {
	str << m_ored_field.get_indexes()[i] << " ";
    }
    nchar = (unsigned int)str.tellp() - pos;
    if (nchar < 20) tabify(20 - nchar, str);
    str << "mode  ";
    switch (m_ored_field.get_mode()) { 
    case Range::field::both_bounded: 
	str << "both_bounded  ";
	break; 
    case Range::field::enumerated: 
	str << "enumerated  ";
	break; 
    default:
	str << "unknown  ";
	break; 
    } 

    return (str.str());
}
