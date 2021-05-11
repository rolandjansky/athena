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
SiTotalCharge::SiTotalCharge() :
  m_charge(0),
  m_chargeComposition(),
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
  auto max_element = std::max_element(m_chargeComposition.cbegin(), m_chargeComposition.cend(),
		                      [](SiCharge const & lhs, SiCharge const & rhs)
                                      { return lhs.charge() < rhs.charge(); }
				     );
  return *max_element;
}

bool SiTotalCharge::fromTrack() const 
{
  if(m_chargeComposition.empty())
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

  std::for_each(totalCharge.chargeComposition().begin(),
                totalCharge.chargeComposition().end(),
                [this](const SiCharge & c) { addSiCharge(c); }
               );
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
  //erase-remove idiom to remove small charges
  auto new_end = std::remove_if(m_chargeComposition.begin(), m_chargeComposition.end(),
		  [&minimumCharge](const SiCharge& c){ return (c.charge() > -minimumCharge) && 
		                                              (c.charge() < minimumCharge); });
  m_chargeComposition.erase(new_end, m_chargeComposition.end());
}

// add another SiCharge to the charge composition (not the total value):
void SiTotalCharge::addSiCharge(const SiCharge &charge)
{
  auto element = std::find_if(m_chargeComposition.begin(), m_chargeComposition.end(),
		     [&charge](const SiCharge & other) {
                       return (charge.time() == other.time()) &&
		              (charge.processType() == other.processType()) &&
			      (charge.particleLink() == other.particleLink());
		     } );
  if(element != m_chargeComposition.end()) element->add(charge);
  // add the charge to the list if not merged in existing one
  else m_chargeComposition.push_back(charge);
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

