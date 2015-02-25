/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TileMuonTrackDistance
 * PACKAGE:  offline/TileCalorimeter/TileCosmicAlgs
 *
 * AUTHOR :  J. Maneira
 * CREATED:  10-Jul-2006
 *
 * PURPOSE:  Minuit fit function for TileMuonFitter
 *           Supplies summed distance from cells to track
 *
 *  Input: Cells
 *  Output: Track parameters
 *   
 ********************************************************************/
//
#include "TileCosmicAlgs/TileMuonTrackDistance.h"

using CLHEP::Hep3Vector;

namespace ROOT {
    namespace Minuit2 {

void TileMuonTrackDistance::Means()
{
    theMeanX = theMeanY = theMeanZ = 0.;
    double eSum = 0.;
    int ncells = theX.size();
    
    if (doWeighted){
        for (int i = 0; i < ncells; i++){
            theMeanX += theE[i]*theX[i];    	
            theMeanY += theE[i]*theY[i];    	
            theMeanZ += theE[i]*theZ[i];
            eSum  += theE[i];    	
        }
        if (eSum > 0) {
            theMeanX /= eSum;
            theMeanY /= eSum;
            theMeanZ /= eSum;
        } else {
            theMeanX = 0;
            theMeanY = 0;
            theMeanZ = 0;
	}
    } else {
        for (int i = 0; i < ncells; i++){
            theMeanX += theX[i];    	
            theMeanY += theY[i];    	
            theMeanZ += theZ[i];    	
    	}
    	theMeanX /= (double)(ncells);
    	theMeanY /= (double)(ncells);
    	theMeanZ /= (double)(ncells);
    }

}
/*==========================================================================*/
double TileMuonTrackDistance::operator()(const std::vector<double>& par) const
{
    double distSum2 = 0;
    std::vector<double> fourPar;
    
    fourPar.push_back(theMeanY - par[0]*theMeanX);
    fourPar.push_back(par[0]);
    fourPar.push_back(theMeanZ - par[1]*theMeanX);
    fourPar.push_back(par[1]);
   
    for (int i = 0; i < (int)(theX.size()); i++){
        if (doWeighted) {
            distSum2 += theE[i]*Distance2SinglePoint(theX[i],theY[i],theZ[i],fourPar);
        } else {
            distSum2 += Distance2SinglePoint(theX[i],theY[i],theZ[i],fourPar);
	}
    }
   
    return distSum2;

}
/*==========================================================================*/
double TileMuonTrackDistance::Distance2SinglePoint(double x, double y, double z, const std::vector<double>& par) const 
{
    
   Hep3Vector dataP(x,y,z);
   Hep3Vector lineP(ClosestPoint(&dataP,par));
	
   return (dataP - lineP).mag2();
}
/*==========================================================================*/
Hep3Vector TileMuonTrackDistance::ClosestPoint(Hep3Vector *dataPoint, const std::vector<double>& par) const 
{
    
    double aa,bb,cc,dd;
    double xort = 0;
    Hep3Vector linePoint;
	
    aa = par[0];
    bb = par[1];
    cc = par[2];
    dd = par[3];
   
    xort = dataPoint->getX() + bb*(dataPoint->getY() - aa) + dd*(dataPoint->getZ() - cc);
    xort /= (1+bb*bb+dd*dd);
    linePoint.setX(xort);
    linePoint.setY(aa + bb*xort);
    linePoint.setZ(cc + dd*xort);
	
    return linePoint;
}

    } //namespace Minuit2

}   //namespace ROOT



