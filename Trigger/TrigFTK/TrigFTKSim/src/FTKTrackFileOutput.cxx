/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKTrackFileOutput.h"

#include <TROOT.h>

FTKTrackFileOutput::FTKTrackFileOutput() : 
  FTKObjectOutput<FTKTrackStream>(), FTKTrackOutput()
{;}


FTKTrackFileOutput::~FTKTrackFileOutput()
{;}


/** add roads to a bank with a given ID */
void FTKTrackFileOutput::addTrack(int ibank, const FTKTrack &track)
{
  // create a new element into the clones array
  m_data[ibank]->addTrack(track);
}


/** add roads to a bank with a given ID */
void FTKTrackFileOutput::addTrackI(int ibank, const FTKTrack &track)
{
  // create a new element into the clones array
  m_data[ibank]->addTrackI(track);
}
