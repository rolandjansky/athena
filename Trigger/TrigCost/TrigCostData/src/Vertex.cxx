/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local
#include "TrigCostData/Vertex.h"

using namespace std;

//----------------------------------------------------------------------------------
Trig::Vertex::Vertex() :
  fVertexNTracks(0),
  fVertexChi2(0),
  fVertexChi2Prob(0),
  fVertexX(0),
  fVertexY(0),
  fVertexZ(0),
  fVertexXerr(0),
  fVertexYerr(0),
  fVertexZerr(0)
{
}


//----------------------------------------------------------------------------------
void Trig::Vertex::SetVertexNTracks(float ntracks)
{
  fVertexNTracks = ntracks ;
}

//----------------------------------------------------------------------------------
void Trig::Vertex::SetVertexQuality(float chi2, float chi2prob)
{
  fVertexChi2       = chi2;
  fVertexChi2Prob   = chi2prob;
}  


//----------------------------------------------------------------------------------
void Trig::Vertex::SetVertexPosition(float x, float y, float z)
{
  fVertexX = x;
  fVertexY = y;
  fVertexZ = z;
}

//----------------------------------------------------------------------------------
void Trig::Vertex::SetVertexError(float errx, float erry, float errz)
{
  fVertexXerr = errx;
  fVertexYerr = erry;
  fVertexZerr = errz;
}
