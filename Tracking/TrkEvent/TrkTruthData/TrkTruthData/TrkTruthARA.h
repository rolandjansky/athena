/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file
 *
 * It's easier for  athena ROOT access  to work with DataVector derived containers.
 * Tracking Truth classes are not DataVectors, so we define DataVector-based collections of them.
 *
 */

#ifndef TRKTRUTHARA_H
#define TRKTRUTHARA_H


#include "DataModel/DataVector.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTrack/Track.h"

typedef DataVector<PRD_MultiTruthCollection>  PRD_MultiTruthCollectionContainer;
typedef DataVector<DetailedTrackTruthCollection> DetailedTrackTruthCollectionContainer;
typedef DataVector<TrackTruthCollection> TrackTruthCollectionContainer;

//CLASS_DEF( DataVector<PRD_MultiTruthCollection> , 1233310086 , 1 )
//CLASS_DEF( DataVector<DetailedTrackTruthCollection> , 1222236003 , 1 )
//CLASS_DEF( DataVector<TrackTruthCollection> , 1289094683 , 1 )

CLASS_DEF( PRD_MultiTruthCollectionContainer , 1205563645 , 1 )
CLASS_DEF( DetailedTrackTruthCollectionContainer , 1169163308 , 1 )
CLASS_DEF( TrackTruthCollectionContainer , 1314918444 , 1 )


#endif/*TRKTRUTHARA_H*/
