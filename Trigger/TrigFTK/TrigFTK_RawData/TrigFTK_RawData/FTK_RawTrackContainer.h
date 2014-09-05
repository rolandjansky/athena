/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_RAWTRACKCONTAINER_H
#define FTK_RAWTRACKCONTAINER_H

/**************************************************************

NAME:     FTK_RawTrackContainer.h
PACKAGE:  atlasoff/Trigger/TrigFTK/TrigFTKRaw

AUTHORS:  James Howarth
CREATED:  February 2014

PURPOSE:  A container for a collection of FTK_RawTrack objects.
          Based on code from Trigger/TrigEvent/TrigMuonEFContainer.h
**************************************************************/


#include "DataModel/DataVector.h"
#include "SGTools/BaseInfo.h"

#include "TrigFTK_RawData/FTK_RawTrack.h" // Change this once added to svn

typedef DataVector<FTK_RawTrack> FTK_RawTrackContainer;
CLASS_DEF( FTK_RawTrackContainer , 1294480537 , 1 )

//SG_BASE(FTK_RawTrackContainer, DataVector<FTK_RawTrack>);

#endif // FTK_RAWTRACKCONTAINER
