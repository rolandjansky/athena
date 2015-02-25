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

// C++ STL includes
#include <vector>

// external packages declarations
#include "Minuit2/FCNBase.h"
#include "CLHEP/Vector/ThreeVector.h"

namespace ROOT {
    namespace Minuit2 {

/**
  * 
* @class TileMuonTrackDistance
* @brief Auxiliary to TileMuonFitter. Chi-square function for Minuit.
*
  * This class is auxiliary to TileMuonFitter. Provides sum 
  * of squared orthogonal distances from cells to track. Implemented as 
  * a FCNBase in order to be called directly by Minuit. 
  */  

class TileMuonTrackDistance : public FCNBase 
{

public:

    /** Constructor */
    TileMuonTrackDistance(
        const std::vector<double>& X,
        const std::vector<double>& Y,
        const std::vector<double>& Z,
        const std::vector<double>& E)
        : theX(X),theY(Y),theZ(Z),theE(E),doWeighted(false),
          theMeanX(0.0),theMeanY(0.0),theMeanZ(0.0) {}
    
    virtual ~TileMuonTrackDistance() {}
        
    /** Provides Chi-square in function of parameter vector. */
    virtual double operator()(const std::vector<double>&) const;
    virtual double Up() const {return 1.;}
    
    /** Calculates means (weighted or not) of cell positions. */
    void Means();
    /** Returns squared distance from point to track defined by par. */
    double Distance2SinglePoint(double x, double y, double z, const std::vector<double>& par) const;
    /** Returns point in track defined by par closest to dataPoint. */
    CLHEP::Hep3Vector ClosestPoint(CLHEP::Hep3Vector *dataPoint, const std::vector<double>& par) const;
    
    /** X mean getter. */
	double GetMeanX(){return theMeanX;}    
    /** Y mean getter. */
    double GetMeanY(){return theMeanY;}    
    /** Z mean getter. */
    double GetMeanZ(){return theMeanZ;} 
	
    /** Set weight/no-weight flag. */
    void SetWeighted(bool weight) {doWeighted = weight;}   
    /** Get weight/no-weight flag. */
    bool GetWeighted()            {return doWeighted;}   
	
private:
    /** Vector of cell center X coordinates. */
    std::vector<double> theX;
    /** Vector of cell center Y coordinates. */
    std::vector<double> theY;
    /** Vector of cell center Z coordinates. */
    std::vector<double> theZ;
    /** Vector of cell energies. */
    std::vector<double> theE;
	
    /** Weight/no-weight flag. */
    bool doWeighted;
    
    double theMeanX;
    double theMeanY;
    double theMeanZ;

};

    }
}
