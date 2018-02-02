/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_DIMUON_CONTAINER
#define TRIG_DIMUON_CONTAINER

#include "AthenaKernel/CLASS_DEF.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"

typedef std::vector<std::pair<CombinedMuonFeature, CombinedMuonFeature> > TrigCombDiMuonContainer;

CLASS_DEF( TrigCombDiMuonContainer, 1123834557, 1 )

CLASS_DEF( DataVector<TrigCombDiMuonContainer> , 1250430296 , 1 )


#endif // TRIG_DIMUON_CONTAINER
