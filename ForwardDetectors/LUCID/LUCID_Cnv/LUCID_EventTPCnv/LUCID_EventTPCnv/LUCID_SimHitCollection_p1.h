/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCIDEVENTTPCNV_LUCID_SIMHITCOLLECTION_P1_H
#define LUCIDEVENTTPCNV_LUCID_SIMHITCOLLECTION_P1_H

#include <vector>
#include "LUCID_SimHit_p1.h"


class LUCID_SimHitCollection_p1
{
public:
  /// typedefs
  typedef std::vector<LUCID_SimHit_p1> HitVector;
  typedef HitVector::const_iterator const_iterator;
  typedef HitVector::iterator       iterator;


  /// Default constructor
  LUCID_SimHitCollection_p1 ();
  // Accessors
  const std::string&  name() const;
  const HitVector&    getVector() const;
  //private:
  std::vector<LUCID_SimHit_p1>   m_cont;
  std::string m_name;
};


// inlines

inline
LUCID_SimHitCollection_p1::LUCID_SimHitCollection_p1 () {}

inline
const std::string&
LUCID_SimHitCollection_p1::name() const
{return m_name;}

inline
const std::vector<LUCID_SimHit_p1>&
LUCID_SimHitCollection_p1::getVector() const
{return m_cont;}

#endif
