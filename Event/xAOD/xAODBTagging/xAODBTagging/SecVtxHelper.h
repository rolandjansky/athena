/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_SECVTXHelper_H
#define XAOD_SECVTXHelper_H

#include "xAODTracking/Vertex.h"

namespace xAOD {

    namespace SecVtxHelper{
    // Helper functions for managing dynamic decorated variables

      double VertexMass(const xAOD::Vertex* );

      void   setVertexMass(xAOD::Vertex * , double value); 

      double EnergyFraction(const xAOD::Vertex * );

      void   setEnergyFraction(xAOD::Vertex * , double value);


    }


}

#endif 


