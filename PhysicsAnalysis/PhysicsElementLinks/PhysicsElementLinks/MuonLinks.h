/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************************

MuonLinks.h is needed to be able to add ParticleLinks<MuonContainer>
                    to store gate. A similar header is needed for every instantiation
                    of ParticleLinks

         S. Protopopescu  15-DEC-2008


*****************************************************************************************/

#ifndef MUONLINKS_H
#define MUONLINKS_H

#include "muonEvent/MuonContainer.h"
#include "ParticleEvent/ParticleLinks.h"
#include "SGTools/CLASS_DEF.h"

typedef  ParticleLinks<Analysis::MuonContainer> MuonLinks ;
typedef  ElementLink<Analysis::MuonContainer> MuonLink ;


CLASS_DEF(MuonLinks,253440153 , 1)


#endif // MUONLINKS_H
