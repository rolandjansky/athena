/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCombinedEventTPCnv
 * @Class  : TrigCompositeContainer_p1
 *
 * @brief persistent partner for TrigCompositeContainer
 *
 * @author Camille B.-Champagne  <camille.belanger-champagne@cern.ch>
 **********************************************************************************/

#ifndef TRIGCOMBINEDEVENTTPCNV_TRIGCOMPOSITECONTAINER_P1_H
#define TRIGCOMBINEDEVENTTPCNV_TRIGCOMPOSITECONTAINER_P1_H

#include "TrigCombinedEventTPCnv/TrigComposite_p1.h"

class TrigCompositeContainer_p1 : public std::vector<TrigComposite_p1> { };

#endif
