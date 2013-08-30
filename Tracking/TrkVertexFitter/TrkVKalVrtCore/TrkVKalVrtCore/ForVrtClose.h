/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TrkVKalVrtCore_ForVrtClose_H
#define _TrkVKalVrtCore_ForVrtClose_H

#include "TrkVKalVrtCore/CommonPars.h"

namespace Trk {

//
// Structure needed for "passing near other vertex" constraint
//  Vertex+cov.matrix+Charge themselves are also here 
//   (previously were in ForCFT structure!!!)
//
//  Constraint usage is controlled by "passNearVertex" and "passWithTrkCov" flags
//          in VKVertex object itself.
//--------------------------------------------------------------------------------

  struct ForVrtClose{
    long int Charge;          // Charge of summary track for propagation 
    double vrt[3], covvrt[6];
    double dcovf[21];         // XYZPxPyPz covariance matrix 
                              // for constraint calculation.
			      // Its dependance on vertex position is small,
			      // so it is calculated on previous iteration and
			      // not touched during PostFit
    double ywgt[3], rv0[2];
    double cvder[12];	              /* was [2][6] */
    double dcv[6*(NTrkM*3+3)];	      /* was [6][903] */

    ForVrtClose(){
      Charge=0;  vrt[0]=vrt[1]=vrt[2]=0.;
      for(int i=0; i<2*6; i++)cvder[i]=0.;
      for(int i=0; i<21;  i++)dcovf[i]=0.;
      dcovf[0]=dcovf[2]=dcovf[5]=dcovf[9]=dcovf[14]=dcovf[20]=10.;
    };
  };

}

#endif
