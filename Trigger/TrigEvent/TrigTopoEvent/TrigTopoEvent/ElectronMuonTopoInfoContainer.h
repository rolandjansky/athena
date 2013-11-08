// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigEvent/TrigTopoEvent/ElectronMuonTopoInfoContainer.h
 **
 **   Description: 
 ** 	Container of ElectronMuonTopoInfo objects so that they can be 
 **		attached to TE				
 **   Author: Pavel Jez <pavel.jez@cern.ch>
 **
 **   Created:   Apr 11, 2011
 **
 **
 **************************************************************************/ 

#ifndef ELECTRONMUONTOPOINFO_CONTAINER
#define ELECTRONMUONTOPOINFO_CONTAINER

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

#include "ElectronMuonTopoInfo.h"
#include "SGTools/BaseInfo.h"

class ElectronMuonTopoInfoContainer : public DataVector<ElectronMuonTopoInfo> {}; 


CLASS_DEF( ElectronMuonTopoInfoContainer , 1271751257 , 1 )

SG_BASE(ElectronMuonTopoInfoContainer, DataVector<ElectronMuonTopoInfo>); 

#endif // ELECTRONMUONTOPOINFO_CONTAINER
