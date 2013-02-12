/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Converter from DetDescrCondIdentifier_old to 64-bit Identifier 
// $Id: DetDescrCondIdentifier_old_cnv.cxx 485488 2012-02-28 13:14:06Z schaffer $

#define private public
#define protected public
#include "Identifier/Identifier.h"
#undef private
#undef protected
#include "DetDescrCondTPCnv/DetDescrCondIdentifier_old_cnv.h"

void DetDescrCondIdentifier_old_cnv::Convert (Identifier* newobj,
                                              const DetDescrCondIdentifier_old* oldobj) 
{
    Identifier id(oldobj->m_id);
    newobj->m_id = id.get_compact();
//     std::cout << "DetDescrCondIdentifier_old_cnv::Convert old/new " << std::hex 
//               << oldobj->m_id << "/" << newobj->m_id << std::endl;

}
