/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCOND_DETDESCRCONDIDENTIFIER_OLD_CNV_H 
#define DETDESCRCOND_DETDESCRCONDIDENTIFIER_OLD_CNV_H 
//
// DetDescrCondIdentifier_old_cnv.h - TVirtualConverter used to read
// in an Identifier object with an unsigned int and convert it to an
// Identifier with an unsigned long long
//
// RD Schaffer, started 23/02/12
//

#include "RootConversions/TVirtualConverter.h"
#include "Identifier/Identifier.h"
#include "DetDescrCondTPCnv/DetDescrCondIdentifier_old.h"

class DetDescrCondIdentifier_old_cnv 
  : public TVirtualConverter_T<Identifier, DetDescrCondIdentifier_old>
{
public:
  virtual void Convert (Identifier* newobj,
                        const DetDescrCondIdentifier_old* oldobj);
};

#endif // DETDESCRCOND_DETDESCRCONDIDENTIFIER_OLD_CNV_H 
