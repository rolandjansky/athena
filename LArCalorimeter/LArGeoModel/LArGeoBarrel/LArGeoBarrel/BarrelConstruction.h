/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// BarrelConstruction
// Insert the LAr Barrel into a pre-defined mother volume.
// Author: Gaston Parrour  translated to GeoModel by G.Unal

#ifndef LARGEOBARREL_BARRELCONSTRUCTION_H
#define LARGEOBARREL_BARRELCONSTRUCTION_H

#include "LArGeoCode/VDetectorParameters.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "CLHEP/GenericFunctions/FunctionNoop.hh"


namespace LArGeo {

  class BarrelConstruction
  {
  public:

    // Constructor;
    BarrelConstruction(bool fullGeo);

    // Destructor:
    virtual ~BarrelConstruction();

    // Get the envelope containing this detector.
    GeoFullPhysVol* GetPositiveEnvelope();
    GeoFullPhysVol* GetNegativeEnvelope();

    // Set parameters for the barrel.
    void setBarrelSagging(bool flag)        {m_A_SAGGING  = flag;}
    void setBarrelCellVisLimit(int maxCell) {m_NVISLIM    = maxCell;}

    void printParams();

  private: 

    void MakeEnvelope();

    // It is illegal to copy a BarrelConstruction:
    BarrelConstruction (const BarrelConstruction &);

    // It is illegal to assign a BarrelConstruction:
    BarrelConstruction & operator= (const BarrelConstruction &);

    // Three auxiliary functions:------------------------------------//
    //                                                               //
    Genfun::FunctionNoop Fx( double r,                               //
			       Genfun::GENFUNCTION G,                //
			       const double Cenx[],                  //
			       const double Ceny[] ) const;          //
      //                                                             //
      Genfun::FunctionNoop Fy( double r,                             //
			       Genfun::GENFUNCTION G,                //
			       const double Cenx[],                  //
			       const double Ceny[] ) const;          //
      //                                                             //
      Genfun::FunctionNoop Dely(Genfun::GENFUNCTION G ) const;       //
      Genfun::FunctionNoop Del1(Genfun::GENFUNCTION G ) const;       //
      Genfun::FunctionNoop Del2(Genfun::GENFUNCTION G ) const;       //

      //-------------------------------------------------------------//

      // Atan2 for Generic Functions..:------------------------------//
      Genfun::FunctionNoop ATan2(Genfun::GENFUNCTION y,              //
				 Genfun::GENFUNCTION x) const;       //
      //-------------------------------------------------------------//

      // Detector parameters ACCG, ACCA, ACMB, ACCO
      LArGeo::VDetectorParameters* m_parameters;

      bool                    m_A_SAGGING;
      int                     m_NVISLIM;

      // volumes that are private member variables:
      GeoFullPhysVol*  m_ecamPhysicalPos;
      GeoFullPhysVol*  m_ecamPhysicalNeg;

      bool             m_fullGeo;  // true->FULL, false->RECO
  };

 

}  // namespace LArGeo

#endif // LARGEOBARREL_BARRELCONSTRUCTION_H
