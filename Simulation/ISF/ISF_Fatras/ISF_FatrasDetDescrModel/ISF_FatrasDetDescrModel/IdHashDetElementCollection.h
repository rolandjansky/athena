/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef IDHASHDETELEMENTCOLLECTION_H
#define IDHASHDETELEMENTCOLLECTION_H

#include <map>

#include "Identifier/IdentifierHash.h"
#include "ISF_FatrasDetDescrModel/PlanarDetElement.h"

#include "CLIDSvc/CLASS_DEF.h"

namespace iFatras {
  typedef std::multimap<IdentifierHash, iFatras::PlanarDetElement*> IdHashDetElementCollection;
}

CLASS_DEF(iFatras::IdHashDetElementCollection, 1106670437, 1)

#endif /*IDHASHDETELEMENTCOLLECTION_H*/
