// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/test/ELFCont.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2015
 * @brief Test for ElementLink forward declaration.
 */


#ifndef ATHLINKS_ELFCONT_H
#define ATHLINKS_ELFCONT_H


#include "ELFElt.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <vector>


class ELFCont
  : public std::vector<ELFElt*>
{
};


CLASS_DEF( ELFCont , 171212016 , 1 )


#endif // not ATHLINKS_ELFCONT_H
