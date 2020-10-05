/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATOROBJECTSMCEVENTCOLLECTION_H
#define GENERATOROBJECTSMCEVENTCOLLECTION_H 1


#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenRanges.h"

#include <iostream>
/**
// @brief This defines the McEventCollection, which is really just an 
//   ObjectVector of McEvent objects
// @class McEventCollection
// File:  GeneratorObjects/McEventCollection.h
// Description:
//   This defines the McEventCollection, which is really juat an 
//   ObjectVector of McEvent objects.
//
// AuthorList:
//         M. Shapiro:  Initial Code March 2000
//         C. Leggett:  added maps, inherits from ObjectVector 4/25/01
//         fixed for gcc Oct 26 2001: Ian Hinchliffe
*/

class McEventCollection : public DataVector<HepMC::GenEvent>
{
  typedef DataVector<HepMC::GenEvent>::iterator EventIterator;
  typedef DataVector<HepMC::GenEvent>::const_iterator EventConstIterator;

 public:
  McEventCollection ();

  ~McEventCollection ();

  McEventCollection (const McEventCollection& in);


  McEventCollection& operator=(const McEventCollection&);
  
  const HepMC::GenEvent* find(int evtNumber) const;
};

inline
McEventCollection::McEventCollection (void)
  :     DataVector<HepMC::GenEvent>       ()
{}

inline McEventCollection::McEventCollection(const McEventCollection& in) : DataVector<HepMC::GenEvent>()
{
  McEventCollection::operator=(in);
}


inline McEventCollection& McEventCollection::operator=(const McEventCollection& in)
{
  // Force a deep copy on the DataVector
  //
  for (const HepMC::GenEvent* ev : in)
  {
    DataVector<HepMC::GenEvent>::push_back(new HepMC::GenEvent(*ev));
  }

  return *this;
}

inline
McEventCollection::~McEventCollection (void)
{}


CLASS_DEF(McEventCollection, 133273, 1)

#endif
