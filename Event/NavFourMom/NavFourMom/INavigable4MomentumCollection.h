///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAVFOURMOM_INAVIGABLE4MOMENTUMCOLLECTION_H
#define NAVFOURMOM_INAVIGABLE4MOMENTUMCOLLECTION_H
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "CLIDSvc/CLASS_DEF.h"

#include "DataModel/DataVector.h"

#include "EventKernel/INavigable4Momentum.h"
//#include "NavFourMom/I4MomentumContainer.h"

//DATAVECTOR_VIRTBASES1 (INavigable4Momentum, I4Momentum);

typedef DataVector<INavigable4Momentum>  INavigable4MomentumCollection;

// re-use the 'old' way of declaring classes to the ClassIDSvc
CLASS_DEF( DataVector<INavigable4Momentum>, 1324108263 , 1 )

#endif // NAVFOURMOM_INAVIGABLE4MOMENTUMCOLLECTION_H

