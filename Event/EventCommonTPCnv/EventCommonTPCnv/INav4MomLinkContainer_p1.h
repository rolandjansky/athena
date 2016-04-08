///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** INav4MomLinkContainer_p1.h
 *  @author: Karsten Koeneke (DESY)
 *  @date July 2009
 *  @brief this defines the persisten representation of a vector
 *         of ElementLinks to INavigable4MomentumCollection
 *****************************************************************/

#ifndef EVENTCOMMONTPCNV_INAV4MOMLINKCONTAINER_P1_H
#define EVENTCOMMONTPCNV_INAV4MOMLINKCONTAINER_P1_H

// DataModel includes
#include "DataModelAthenaPool/VectorElementLink_p1.h"

/** Definition of the persistent version of INav4MomLinkContainer */
// class INav4MomLinkContainer_p1 : public GeneratePersVectorELinkType_p1< INav4MomLinkContainer >::type
// MN: avoid transient EDM in ROOT6 dicts
class INav4MomLinkContainer_p1 : public VectorElementLinkInt_p1
{
};


#endif // EVENTCOMMONTPCNV_INAV4MOMLINKCONTAINER_P1_H
