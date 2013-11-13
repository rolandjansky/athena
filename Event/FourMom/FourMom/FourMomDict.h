/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOM_FOURMOMDICT_H
# define FOURMOM_FOURMOMDICT_H

#include "FourMom/P4EEtaPhiM.h"
#include "FourMom/P4IPtCotThPhiM.h"
#include "FourMom/P4PxPyPzE.h"
#include "FourMom/P4PtEtaPhiM.h"

// new implementations
#include "FourMom/P4Impl.h"
#include "FourMom/Lib/P4BaseEEtaPhiM.h"
#include "FourMom/Lib/P4ImplEEtaPhiM.h"

#include "FourMom/Lib/P4BaseIPtCotThPhiM.h"
#include "FourMom/Lib/P4ImplIPtCotThPhiM.h"

#include "FourMom/Lib/P4BasePtEtaPhiM.h"
#include "FourMom/Lib/P4ImplPtEtaPhiM.h"

#include "FourMom/Lib/P4BasePxPyPzE.h"
#include "FourMom/Lib/P4ImplPxPyPzE.h"

namespace FourMomDict {
  
  struct tmp {

    P4Impl<P4ImplEEtaPhiM>     m_eneEtaPhiMass;
    P4Impl<P4ImplIPtCotThPhiM> m_iPtCotThPhiMass;
    P4Impl<P4ImplPtEtaPhiM>    m_ptEtaPhiMass;
    P4Impl<P4ImplPxPyPzE>      m_pxPyPzEne;

  };
}

#endif 
