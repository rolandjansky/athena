/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DetDescrCondRootFix.h"
#include "RootConversions/TConverterRegistry.h"
#include "DetDescrCondTPCnv/DetDescrCondIdentifier_old_cnv.h"

DetDescrCondRootFix::DetDescrCondRootFix() 
{
#ifdef __IDENTIFIER_64BIT__
    static bool did_rootcnv = false;
    if (!did_rootcnv) {
        did_rootcnv = true;
        static DetDescrCondIdentifier_old_cnv cnv;
        TConverterRegistry::Instance()->AddConverter (&cnv);
    }
#endif /* __IDENTIFIER_64BIT__ */
}

