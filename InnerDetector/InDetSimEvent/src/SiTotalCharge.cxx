/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiTotalCharge.cxx
//   Implementation file for class SiTotalCharge
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.5 08/06/2001 David Calvet
///////////////////////////////////////////////////////////////////

#include "InDetSimEvent/SiTotalCharge.h"
#include <iterator>

// Implicit constructor:
SiTotalCharge::SiTotalCharge(const alloc_t& alloc) :
  m_charge(0),
  m_chargeComposition(alloc),
  m_emptyLink()
{}

// Copy constructor:
SiTotalCharge::SiTotalCharge(const SiTotalCharge &totalCharge) :
  m_charge(totalCharge.m_charge),
  m_chargeComposition(totalCharge.m_chargeComposition),
  m_emptyLink()
{}

// Assignment operator:
SiTotalCharge &SiTotalCharge::operator=(const SiTotalCharge &totalCharge)
{
  if (this!=&totalCharge) {
    m_charge=totalCharge.m_charge;
    m_chargeComposition=totalCharge.m_chargeComposition;
    m_emptyLink=totalCharge.m_emptyLink;
  } else {}
  return *this;
}

// give main single process charge:
const SiCharge& SiTotalCharge::mainCharge() const
{
  // start with the first charge
  list_t::const_iterator p_charge=m_chargeComposition.begin();

  // main charge to be determined, initialized to the first one
  list_t::const_iterator main_charge = p_charge;
  ++p_charge;
  // look for the biggest amount of charge in the remaining charges
  for( ; p_charge!=m_chargeComposition.end() ; ++p_charge) {
    if (p_charge->charge()>main_charge->charge()) main_charge=p_charge;
  }
  return *main_charge;
}

bool SiTotalCharge::fromTrack() const 
{
  if(m_chargeComposition.size()==0)
    {
      return false;
    }
  SiCharge::Process process=mainCharge().processType();
  return (process==SiCharge::track || 
	  process==SiCharge::cut_track || 
	  process==SiCharge::diodeX_Talk);
}

// add another charge:
void SiTotalCharge::add(const SiCharge &charge)
{
  // increase the total deposited charge
  m_charge+=charge.charge();

  // add the SiCharge to the list of charges
  addSiCharge(charge);
}

// add another total charge:
void SiTotalCharge::add(const SiTotalCharge &totalCharge)
{
  // increase the total deposited charge
  m_charge+=totalCharge.charge();

  // add the new list of charges to the present list of charges
  for(list_t::const_iterator p_charge=
	totalCharge.chargeComposition().begin() ;
      p_charge!=totalCharge.chargeComposition().end() ; ++p_charge) {
    addSiCharge(*p_charge);
  }
}

// remove time information of the SiCharge objects:
void SiTotalCharge::removeTimeInformation()
{
  // save the old charge composition
  list_t oldComposition;
  m_chargeComposition.swap(oldComposition);

  // loop on all old charges
  for(list_t::const_iterator p_charge=oldComposition.begin() ;
      p_charge!=oldComposition.end() ; ++p_charge) {
    // add the old charge (without time) to the list
    addSiCharge(SiCharge(p_charge->charge(),0,
			 p_charge->processType(),p_charge->particleLink()));
  }
}

// remove small SiCharge objects:
void SiTotalCharge::removeSmallCharges(const double minimumCharge)
{
  // loop on all charges
  for(list_t::iterator p_charge=m_chargeComposition.begin() ;
      p_charge!=m_chargeComposition.end() ; ) { 
    // !!! p_charge is changed in the loop !!!

    // if the charge is too small remove it from list
    if (p_charge->charge()>-minimumCharge && 
	p_charge->charge()<minimumCharge) {
      p_charge=m_chargeComposition.erase(p_charge);
    } else {
      ++p_charge;
    }
  }
}

// add another SiCharge to the charge composition (not the total value):
void SiTotalCharge::addSiCharge(const SiCharge &charge)
{
  // try to merge the SiCharge in the list of existing charges
  for(list_t::iterator p_charge=m_chargeComposition.begin() ;
      p_charge!=m_chargeComposition.end() ; ++p_charge) {
    if (p_charge->add(charge)) return;
  }
  // add the charge to the list if not merged in existing one
  m_chargeComposition.push_back(charge);
}

///////////////////////////////////////////////////////////////////
// Input/Output stream functions:
///////////////////////////////////////////////////////////////////
std::ostream &operator<<(std::ostream &out,const SiTotalCharge &totalCharge)
{
  out << "Total charge=" << totalCharge.charge() 
      << " Composition:" << std::endl;
  copy(totalCharge.chargeComposition().begin(),
       totalCharge.chargeComposition().end(),
       std::ostream_iterator<SiCharge>(out,"\n"));
  return out;
}

