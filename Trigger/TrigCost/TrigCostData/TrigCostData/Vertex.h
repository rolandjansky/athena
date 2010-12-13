/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_VERTEX_H
#define TRIG_VERTEX_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : Vertex
 * @Author : Stephen Sekula, Rustem Ospanov
 *
 * @Brief  : Vertex information obtained for multiple interaction/collision study
 * 
 **********************************************************************************/

namespace Trig
{
  class Vertex {
  public:
    
    Vertex();
    ~Vertex() {}
        
  private:

    float fVertexNTracks;
    float fVertexChi2;
    float fVertexChi2Prob;
    float fVertexX;
    float fVertexY;
    float fVertexZ;
    float fVertexXerr;
    float fVertexYerr;
    float fVertexZerr;

  public :

    void SetVertexNTracks(float ntracks);
    void SetVertexQuality(float chi2, float chi2prob);
    void SetVertexPosition(float x, float y, float z);
    void SetVertexError(float errx, float erry, float errz);

    float GetVertexXerr() const { return fVertexXerr; }
    float GetVertexYerr() const { return fVertexYerr; }
    float GetVertexZerr() const { return fVertexZerr; }

    float GetVertexX() const { return fVertexX; }
    float GetVertexY() const { return fVertexY; }
    float GetVertexZ() const { return fVertexZ; }

    float GetVertexNTracks() const  { return fVertexNTracks; }
    float GetVertexChi2() const     { return fVertexChi2; }
    float GetVertexChi2Prob() const { return fVertexChi2Prob; }
  };
}

#endif
