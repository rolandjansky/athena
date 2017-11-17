/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOSTEDJETTAGGERS_BOOSTEDJETTAGGERSDICT_H
#define BOOSTEDJETTAGGERS_BOOSTEDJETTAGGERSDICT_H

#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__

// Includes for the dictionary generation:
#include "BoostedJetTaggers/SmoothedWZTagger.h"
#include "BoostedJetTaggers/SmoothedTopTagger.h"
#include "BoostedJetTaggers/JSSWTopTaggerBDT.h"
#include "BoostedJetTaggers/JSSWTopTaggerDNN.h"
#include "BoostedJetTaggers/TopoclusterTopTagger.h"
#include "BoostedJetTaggers/JetQGTagger.h"

#endif // BOOSTEDJETTAGGERS_BOOSTEDJETTAGGERSDICT_H
