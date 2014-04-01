/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2JetContainer_p3
 *
 * @brief persistent partner for TrigT2JetContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigT2JetContainer_p3.h 362053 2011-04-28 12:02:40Z salvator $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGT2JETCONTAINER_P3_H
#define TRIGCALOEVENTTPCNV_TRIGT2JETCONTAINER_P3_H

#include "TrigCaloEventTPCnv/TrigT2Jet_p3.h"
  
class TrigT2JetContainer_p3 : public std::vector<TrigT2Jet_p3> { };
 
#endif
