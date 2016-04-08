/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_ID_H
#define LUCID_ID_H

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Range.h"
#include "Identifier/IdHelper.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "CLIDSvc/CLASS_DEF.h"

#include <string>
#include <map>
#include <assert.h>
#include <vector>

class IdDictDictionary;

class LUCID_ID : public AtlasDetectorID
{
 public:


  typedef Identifier::size_type size_type; 
  typedef std::vector<Identifier>::const_iterator const_id_iterator;
  typedef MultiRange::const_identifier_factory const_expanded_id_iterator;

  LUCID_ID(void);
  ~LUCID_ID(void);

 private:



};

CLASS_DEF(LUCID_ID, 2718, 1)


#endif
