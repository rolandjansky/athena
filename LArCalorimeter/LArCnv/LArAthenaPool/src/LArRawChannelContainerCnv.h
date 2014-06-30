/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArRawChannelContainerCnv_H
#define LArRawChannelContainerCnv_H

#include "LArRawEventContainerCnv.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArTPCnv/LArRawChannelContainer_p1.h"
#include "LArTPCnv/LArRawChannelContainer_p2.h"
#include "LArTPCnv/LArRawChannelContainer_p3.h"
#include "LArTPCnv/LArRawChannelContainer_p4.h"
#include "LArTPCnv/LArRawChannelContainerCnv_p2.h"
#include "LArTPCnv/LArRawChannelContainerCnv_p3.h"
#include "LArTPCnv/LArRawChannelContainerCnv_p4.h"

// LArRawChannelContainerCnv_p3, used for T/P separation
// author R.Seuster <seuster@cern.ch>
// add _p4   G.Unal

typedef LArRawEventContainerCnv<LArRawChannelContainer,LArRawChannelContainer_p4,LArRawChannelContainerCnv_p4> LArRawChannelContainerCnv;

// for your convenience, latest persistent representation
typedef LArRawChannelContainer_p4 LArRawChannelContainer_PERSISTENT;

#endif
