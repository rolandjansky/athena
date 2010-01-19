/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************************

TauJetLinks.h is needed to be able to add ParticleLinks<TauJetContainer>
                    to store gate. A similar header is needed for every instantiation
                    of ParticleLinks

         S. Protopopescu  15-DEC-2008

*****************************************************************************************/

#ifndef TAUJETLINKS_H
#define TAUJETLINKS_H

#include "tauEvent/TauJetContainer.h"
#include "ParticleEvent/ParticleLinks.h"
#include "SGTools/CLASS_DEF.h"

typedef  ParticleLinks<Analysis::TauJetContainer> TauJetLinks ;
typedef  ElementLink<Analysis::TauJetContainer> TauJetLink ;

CLASS_DEF(TauJetLinks,149212465 , 1)


#endif // TAUJETLINKS_H
