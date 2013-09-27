/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiTotalCharge.h
//   Header file for class SiTotalCharge
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class which contains the sum and the composition of all bare charges
// corresponding to a single element
///////////////////////////////////////////////////////////////////
// Version 1.5 08/06/2001 David Calvet
///////////////////////////////////////////////////////////////////

#ifndef SITRACKEREVENT_SITOTALCHARGE_H
#define SITRACKEREVENT_SITOTALCHARGE_H

#include <list>
#include "InDetSimEvent/SiCharge.h"
#include "DataModel/tools/ArenaSharedHeapSTLAllocator.h"

class SiTotalCharge {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  typedef SG::ArenaSharedHeapSTLAllocator<SiCharge> alloc_t;
  typedef std::list<SiCharge, alloc_t> list_t;

  // Implicit constructor:
  SiTotalCharge(const alloc_t& alloc);

  // Copy constructor:
  SiTotalCharge(const SiTotalCharge &totalCharge);

  // Destructor:
  ~SiTotalCharge();

  // Assignment operator:
  SiTotalCharge &operator=(const SiTotalCharge &totalCharge);

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  // total deposited charge in this element:
  double charge() const;

  // list of individual charges:
  const list_t &chargeComposition() const;

  // return true if the charge composition contains more than one charge:
  bool complexCharge() const;

  // return true if the main charge comes from a track (track/diodeX_Talk):
  bool fromTrack() const;

  // return true if the main charge is extraNoise:
  bool extraNoise() const;

  // return the barcode of the main charge:
  int trackBarcode() const;

  // returns the HepMcParticleLink of the main charge
  const HepMcParticleLink& particleLink() const;

  // return the time of the main charge
  double time() const;
  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////

  // add another SiCharge:
  void add(const SiCharge &charge);

  // add another SiTotalCharge:
  void add(const SiTotalCharge &totalCharge);

  // remove time information of the SiCharge objects:
  void removeTimeInformation();

  // remove small SiCharge objects:
  //   minimum charge allowed to keep a SiCharge in the composition list
  void removeSmallCharges(const double minimumCharge);

  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:

  // add another SiCharge to the charge composition (not the total value):
  void addSiCharge(const SiCharge &charge);

  // return the SiCharge corresponding to the process which deposited
  // the biggest amount of charge (returns dummy charge if list is empty)
  // (used to be a public method, but this caused problems if the list was empty):
  const SiCharge& mainCharge() const;

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  double m_charge; // total deposited charge in this element
  list_t m_chargeComposition; // list of individual charges
  HepMcParticleLink m_emptyLink;
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline SiTotalCharge::~SiTotalCharge()
{
}

inline double SiTotalCharge::charge() const 
{
  return m_charge;
}

inline const SiTotalCharge::list_t &SiTotalCharge::chargeComposition() const 
{
  return m_chargeComposition;
}

inline bool SiTotalCharge::complexCharge() const 
{
  return (m_chargeComposition.size()>1);
}

inline bool SiTotalCharge::extraNoise() const 
{
  if(m_chargeComposition.empty())
    {
      return false;
    }
  return (mainCharge().processType()==SiCharge::extraNoise);
}

inline int SiTotalCharge::trackBarcode() const 
{
  if(m_chargeComposition.empty())
    {
      return 0;
    }
  return mainCharge().trackBarcode();
}

inline double SiTotalCharge::time() const 
{
  if(m_chargeComposition.empty())
    {
      return 0.0;
    }
  return mainCharge().time();
}

inline const HepMcParticleLink& SiTotalCharge::particleLink() const
{
  if(m_chargeComposition.empty())
    {
      return m_emptyLink;
    }
  return mainCharge().particleLink();
}


///////////////////////////////////////////////////////////////////
// Input/Output stream functions:
///////////////////////////////////////////////////////////////////
std::ostream &operator<<(std::ostream &out,const SiTotalCharge &totalCharge);

#endif // SITRACKEREVENT_SITOTALCHARGE_H
