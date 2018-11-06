/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_EMSHOWERMINIMALCONTAINER_H
#define EGAMMAEVENT_EMSHOWERMINIMALCONTAINER_H
/********************************************************************

NAME:     EMShowerMinimalContainer.h
PACKAGE:  offline/Reconstruction/egammaEvent

AUTHORS:  M.P. Casado
CREATED:  Sep 2003

********************************************************************/

// INCLUDE HEADER FILES:

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "egammaEvent/EMShowerMinimal.h"

class EMShowerMinimalContainer : public DataVector<EMShowerMinimal> 
{
 public:

  void print();

};

CLASS_DEF( EMShowerMinimalContainer , 1316949154 , 1 )

#endif

