/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  Author :  Saclay Muon Software Group SaMuSoG
//  Date   :  18/05/00
//
// MagFieldAccess is a monostate class providing an static access
// to a AbstractMagneticField
//
#ifndef MAGFIELDACCESS
#define MAGFIELDACCESS

#include "BFieldCore/config.h"

#include "BFieldCore/MagFieldMisc.h"

#include "BFieldCore/AbstractMagneticField.h"

class MagFieldAccess{
public:
   MagFieldAccess();
  ~MagFieldAccess();

public:
///////////////////////////////////

// Get
   static AbstractMagneticField* GetAbstractMagneticField();

// Set
   static void SetAbstractMagneticField(AbstractMagneticField* pAbstractMagneticField);

// Reset
   static void ResetAbstractMagneticField();

private:
///////////////////////////////////
// Data
   static AbstractMagneticField* p_AbstractMagneticField ;
};
#endif

