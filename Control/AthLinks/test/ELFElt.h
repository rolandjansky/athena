// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/test/ELFElt.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2015
 * @brief Test for ElementLink forward declaration.
 */


#ifndef ATHLINKS_ELFELT_H
#define ATHLINKS_ELFELT_H


#include "AthLinks/ElementLink.h"


class ELFCont;
class ELFElt;


ELEMENTLINK_FWD(ELFCont, ELFElt);


class ELFElt
{
public:
  ELFElt(int x) : m_x(x) {}
  int x() const { return m_x; }
  void setLink (const std::string& key, int ndx);

  const ELFElt* ptr() const;

private:
  int m_x;
  ElementLink<ELFCont> m_link;
};


#endif // not ATHLINKS_ELFELT_H
