// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJetContainer_p1_old_cnv.h,v 1.1 2007-05-07 20:54:47 ssnyder Exp $

#ifndef tauEventTPCnv_TAUJETCONTAINER_P1_OLD_CNV_H
#define tauEventTPCnv_TAUJETCONTAINER_P1_OLD_CNV_H

#include "RootConversions/TVirtualConverter.h"
#include "tauEventTPCnv/TauJetContainer_p1.h"
#include "tauEventTPCnv/TauJetContainer_p1_old.h"

class TauJetContainer_p1_old_cnv
  : public TVirtualConverter_T<TauJetContainer_p1, TauJetContainer_p1_old>
{
public:
  virtual void Convert (TauJetContainer_p1* newobj,
                        const TauJetContainer_p1_old* oldobj);
};

#endif // not tauEventTPCnv_TAUJETCONTAINER_P1_OLD_CNV_H
