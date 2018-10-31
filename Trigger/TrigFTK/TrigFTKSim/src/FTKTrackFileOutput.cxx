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
void FTKTrackFileOutput::addTrack_pre_hw(int ibank, const FTKTrack &track)
{
  // create a new element into the clones array
  m_data[ibank]->addTrack_pre_hw(track);
}

/** add roads to a bank with a given ID */
void FTKTrackFileOutput::addTrackI(int ibank, const FTKTrack &track)
{
  // create a new element into the clones array
  m_data[ibank]->addTrackI(track);
}

/** add roads before Hit Warrior to a bank with a given ID */
void FTKTrackFileOutput::addTrackI_pre_hw(int ibank, const FTKTrack &track)
{
  // create a new element into the clones array
  m_data[ibank]->addTrackI_pre_hw(track);
}

/** add roads with patterns to a bank with a given ID */
void FTKTrackFileOutput::addTrack_pattern(int ibank, const FTKTrack &track)
{
  // create a new element into the clones array
  m_data[ibank]->addTrack_pattern(track);
}

/** add roads passing hits requirements to a bank with a given ID */
void FTKTrackFileOutput::addTrack_hits(int ibank, const FTKTrack &track)
{
  // create a new element into the clones array
  m_data[ibank]->addTrack_hits(track);
}
