/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArOFCP1.h"
#include "LArRawConditions/LArCompactSubset.h"     

LArOFCP1::LArOFCP1(float timeOffset, float timeBinWidth,
                   const std::vector<std::vector<float> >& vOFC_a,
                   const std::vector<std::vector<float> >& vOFC_b)
  : LAr2DWaveBase (timeOffset, timeBinWidth, vOFC_a, vOFC_b)
{
} 


LArOFCP1::LArOFCP1(float timeOffset,
                   float timeBinWidth,
                   unsigned int nPhases,
                   unsigned int nSamples,
                   const std::vector<float>& ofc_a,
                   const std::vector<float>& ofc_b,
                   unsigned int index)
  : LAr2DWaveBase (timeOffset, timeBinWidth,
                   nPhases, nSamples,
                   ofc_a, ofc_b, index)
{
}


LArOFCP1::LArOFCP1 (const LArCompactSubsetChannelProxy& other)
  : LAr2DWaveBase (other)
{
}
