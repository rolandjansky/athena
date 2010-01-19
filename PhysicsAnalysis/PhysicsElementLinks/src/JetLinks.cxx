/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************************

JetLinks.h is needed to be able to add ParticleLinks<JetContainer>
                    to store gate. A similar header is needed for every instantiation
                    of ParticleLinks

         S. Protopopescu  15-DEC-2008

*****************************************************************************************/

#include "PhysicsElementLinks/JetLinks.h"

template class ParticleLinks<JetCollection>;
