// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IdentifierSet_h
#define IdentifierSet_h

#include "Identifier/Identifier.h"

/**
 * @typedef IdentifierSet
 * @brief IdentifierSet is std::set of Identifier and used in SCT_RODVetoCondAlg
 * and SCT_RODVetoTool. CLASS_DEF is necessary to be recorded in StoreGate.
 **/

typedef std::set<Identifier> IdentifierSet;

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( IdentifierSet , 229132278 , 1 )

#endif // IdentifierSet_h
