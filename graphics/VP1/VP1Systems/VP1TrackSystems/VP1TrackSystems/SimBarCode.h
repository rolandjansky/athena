/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class SimBarCode                          //
//                                                            //
//  Description: Identifier for truth particle                //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef SIMBARCODE_H
#define SIMBARCODE_H

#include "GeneratorObjects/HepMcParticleLink.h"

class SimBarCode {
public:

  static const int unknownPDG = -999;//Fixme: Better choice? 0?

  SimBarCode(int barCode, HepMcParticleLink::index_type evtIndex, int pdgCode);
  SimBarCode(const HepMcParticleLink&, int pdgCode);
  ~SimBarCode(){}

  inline int barCode() const;//For non-unique secondaries, this will return a negative number, which should not be presented to the user.
  inline int actualBarCode() const;//The original present in the simhit
  inline HepMcParticleLink::index_type evtIndex() const;
  inline int pdgCode() const;
  inline bool unknownPdgCode() const { return m_pdgCode==unknownPDG; }

  inline bool isNonUniqueSecondary() const;

  //For sorting (Does consider pdgCodes, even when one is unknown):
  inline bool operator<(const SimBarCode&) const;

  typedef std::pair<int,HepMcParticleLink::index_type> ExtBarCode;
  ExtBarCode extBarCode() const { return ExtBarCode(barCode(),evtIndex()); }
//   //Comparison operators do not consider pdgCodes if one of them is unknown!
//   inline bool operator==(const SimBarCode&) const;
//   inline bool operator!=(const SimBarCode&) const;

private:
  int m_barCode;
  HepMcParticleLink::index_type m_evtIndex;
  int m_pdgCode;
//   inline bool pdgCodesCompatible(const SimBarCode&) const;
};


#include "VP1TrackSystems/SimBarCode.icc"

#endif
