/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARRAWCONDITIONSDICTCOMMON_H
#define LARRAWCONDITIONSDICTCOMMON_H

#include "LArRawConditions/LArConditionsContainer.h"
#include "LArRawConditions/LArConditionsSubset.h"
#include "AthenaPoolUtilities/CondMultChanCollection.h"
#include "StoreGate/StoreGateSvc.h" // for PyGate

#define INSTAN_CC(T) \
  namespace LArRawConditionsDict { \
    LArConditionsContainer<T>::ConstCorrectionIt      ccit_##T; \
    LArConditionsContainer<T>::ConditionsMap          ccmap_##T; \
  } struct xyzzy
#define INSTAN_CS(T) \
  namespace LArRawConditionsDict { \
    LArConditionsSubset<T>                          cs_##T;   \
    LArConditionsSubset<T>::CorrectionPair          cscp_##T; \
    LArConditionsSubset<T>::ConstCorrectionVecIt    cscv_##T;  \
    LArConditionsSubset<T>::FebPair                 csfb_##T; \
    CondMultChanCollection<LArConditionsSubset<T> > cscmcc_##T; \
  } struct xyzzy
#define INSTAN_BOTH(T) \
  INSTAN_CC(T); INSTAN_CS(T)

/*
#define PYGATE(T) \
  namespace LArRawConditionsDict { \
    PyGate<T> pyGate_##T; \
  } struct xyzzy
*/

#endif // not LARRAWCONDITIONSDICTCOMMON_H
