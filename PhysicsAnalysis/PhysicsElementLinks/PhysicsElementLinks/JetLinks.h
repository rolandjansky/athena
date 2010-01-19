/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************************

JetLinks.h is needed to be able to add ParticleLinks<JetContainer>
                    to store gate. A similar header is needed for every instantiation
                    of ParticleLinks

         S. Protopopescu  15-DEC-2008

*****************************************************************************************/

#ifndef JETLINKS_H
#define JETLINKS_H

#include "JetEvent/JetCollection.h"
#include "ParticleEvent/ParticleLinks.h"
#include "SGTools/CLASS_DEF.h"

typedef  ParticleLinks<JetCollection> JetLinks ;
typedef  ElementLink<JetCollection> JetLink ;

CLASS_DEF(JetLinks, 53032824, 1)


#endif // JETLINKS_H
