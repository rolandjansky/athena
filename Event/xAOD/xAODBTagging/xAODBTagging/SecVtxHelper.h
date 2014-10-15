/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_SECVTXHelper_H
#define XAOD_SECVTXHelper_H

#include "xAODTracking/Vertex.h"

namespace xAOD {

    namespace SecVtxHelper{
    // Helper functions for managing dynamic decorated variables

      float VertexMass(const xAOD::Vertex* );

      void  setVertexMass(xAOD::Vertex * , float value); 

      float EnergyFraction(const xAOD::Vertex * );

      void  setEnergyFraction(xAOD::Vertex * , float value);

      int   VtxNtrk(const xAOD::Vertex* );

      void  setVtxNtrk(xAOD::Vertex * , int value);

      float Vtxpt(const xAOD::Vertex* );

      void  setVtxpt(xAOD::Vertex * , float value);

      float Vtxeta(const xAOD::Vertex* );

      void  setVtxeta(xAOD::Vertex * , float value);

      float Vtxphi(const xAOD::Vertex* );

      void  setVtxphi(xAOD::Vertex * , float value);

      float VtxnormDist(const xAOD::Vertex* );

      void  setVtxnormDist(xAOD::Vertex * , float value);

    }


}

#endif 


