/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Converter from DetDescrCondIdentifier_old to 64-bit Identifier 
// $Id: DetDescrCondIdentifier_old_cnv.cxx 699569 2015-10-09 04:40:43Z ssnyder $

#include "Identifier/Identifier.h"
#include "DetDescrCondTPCnv/DetDescrCondIdentifier_old_cnv.h"

void DetDescrCondIdentifier_old_cnv::Convert (Identifier* newobj,
                                              const DetDescrCondIdentifier_old* oldobj) 
{
    Identifier id(oldobj->m_id);
    *newobj = id;
//     std::cout << "DetDescrCondIdentifier_old_cnv::Convert old/new " << std::hex 
//               << oldobj->m_id << "/" << newobj->m_id << std::endl;

}
