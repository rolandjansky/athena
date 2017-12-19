/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LAROFITERRESULTCONTAINER_H
#define LAROFITERRESULTCONTAINER_H

#include <vector>
#include "LArRawEvent/LArOFIterResults.h"
#include "AthenaKernel/CLASS_DEF.h"

class LArOFIterResultsContainer : public std::vector<LArOFIterResults> {

};

CLASS_DEF(LArOFIterResultsContainer,1208025250,0)

#endif
