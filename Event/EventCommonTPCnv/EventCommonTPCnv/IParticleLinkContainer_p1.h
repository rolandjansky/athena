///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** IParticleLinkContainer_p1.h
 *  @author: Karsten Koeneke (DESY)
 *  @date July 2009
 *  @brief this defines the persisten representation of a vector
 *         of ElementLinks to INavigable4MomentumCollection
 *****************************************************************/

#ifndef EVENTCOMMONTPCNV_IPARTICLELINKCONTAINER_P1_H
#define EVENTCOMMONTPCNV_IPARTICLELINKCONTAINER_P1_H

// STL includes
#include <vector>

// DataModel includes
#include "DataModelAthenaPool/VectorElementLinkCnv_p1.h"
#include "DataModelAthenaPool/VectorElementLink_p1.h"
#include "NavFourMom/IParticleLinkContainer.h"

/** Definition of the persistent version of IParticleLinkContainer */
class IParticleLinkContainer_p1 : public GeneratePersVectorELinkType_p1< IParticleLinkContainer >::type
{
  /** Declare the converter as a friend class */
  friend class IParticleLinkContainerCnv_p1;

};


#endif // EVENTCOMMONTPCNV_IPARTICLELINKCONTAINER_P1_H

