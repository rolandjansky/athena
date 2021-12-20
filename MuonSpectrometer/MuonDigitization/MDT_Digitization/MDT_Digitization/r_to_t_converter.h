/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 02.03.2000, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

////////////////////////////
// CLASS R_to_t_converter //
////////////////////////////

///////////////////////////////////////////////////////////////////
// THIS CLASS CONVERTS RADII TO DRIFT TIMES ACCORDING TO A GIVEN //
//                         r-t RELATION.                         //
///////////////////////////////////////////////////////////////////

#ifndef R_to_t_converterH
#define R_to_t_converterH

#include "GaudiKernel/RndmGenerators.h"
#include "MDT_Digitization/rt_relation.h"

class R_to_t_converter {
private:
    // asymmetry parameter
    inline double p1(const double& r) const;

public:
    // default constructor
    R_to_t_converter(void) { return; }

    // method
    // random generation of the drift time corresponding to the radius r
    // according to the r-t relation rt; non-gaussian tails in the resolution
    // function are taken into account
    inline double convert(const double& r, const Rt_relation& rt) const;
};

#include "MDT_Digitization/r_to_t_converter.icc"

#endif
