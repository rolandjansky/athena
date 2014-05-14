/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Material.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_MATERIAL_H
#define TRKGEOMETRY_MATERIAL_H

#include <vector>

namespace Trk {

  /** @struct MaterialComposition */
  struct MaterialComposition {
      std::vector<unsigned char> elements;
      std::vector<unsigned char> fractions;
      
      MaterialComposition() :
        elements(),
        fractions()
      {}

      ~MaterialComposition(){}
      
      MaterialComposition(std::vector<unsigned char> iel, std::vector<unsigned char> ifrac) :
        elements(iel),
        fractions(ifrac)
      {}
      
      MaterialComposition(const MaterialComposition& mc) :
        elements(mc.elements),
        fractions(mc.fractions)
      {}

      MaterialComposition& operator=(const MaterialComposition& mc) {
          if ( this != &mc){
              elements = mc.elements;
              fractions = mc.fractions;
          }
          return (*this);
      }
      
  };


   /** @class Material 
   
    A common object to be contained by
    - MaterialStep ( for mapping)
    - MaterialProperties ( for reconstruction )
    - Is optimizes for T/P split
    
    @author Andreas.Salzburger@cern.ch 
  */    
  class Material {
    public :  
      // standard x0, l0, A, Z, rho description
      mutable float  X0;
      mutable float  L0;
      mutable float  A;
      mutable float  Z;
      mutable float  rho;
      mutable float  dEdX;
      mutable float  zOaTr;
      mutable MaterialComposition* composition;

      /** Default Constructor needed for POOL */
      Material() :
        X0(10.e10),
        L0(10.e10),
        A(0.),
        Z(0.),
        rho(0.),
        dEdX(0.),
        zOaTr(0.),
        composition(0)
      {}    

      /** Constructor with arguments */
      Material(float iX0, 
               float iL0,
               float iA,
               float iZ,
               float iRho,
               float idEdX = 0., 
               MaterialComposition* mc = 0) :
        X0(iX0),
        L0(iL0),
        A(iA),
        Z(iZ),
        rho(iRho),
	dEdX(idEdX),
	zOaTr( iA>0? iZ/iA*iRho : 0.),
        composition(mc)
      {}    
      

      /** Copy Constructor */
      Material(const Material& amc) :
        X0(amc.X0),
        L0(amc.L0),
        A(amc.A),
        Z(amc.Z),
        rho(amc.rho),
        dEdX(amc.dEdX),
        zOaTr(amc.zOaTr),
        composition( amc.composition ? new MaterialComposition(*amc.composition) : 0 )
      {}

     /** Desctructor - delete the composition if there */
	~Material() { if (composition) delete composition;  }

      /** Assignment operator */
      Material& operator=(const Material& amc) {
          if (this != &amc){
              X0          = amc.X0;
              L0          = amc.L0;
              A           = amc.A;
              Z           = amc.Z;
              rho         = amc.rho;  
              dEdX        = amc.dEdX;  
              zOaTr       = amc.zOaTr;  
              composition =  amc.composition ? new MaterialComposition(*amc.composition) : 0;
          }
          return (*this);
      }

      /** scaling method */
      Material* scale( float sf) const;

      /** access to members */
      float zOverAtimesRho() const { return (*this).zOaTr; }  
      float x0()             const { return (*this).X0; }  
      float averageZ()       const { return (*this).Z; }  

  };

  inline Material* Material::scale( float sf) const {

    return new Material( X0/sf, L0/sf, sf*A, sf*Z, sf*rho);

  }

}

#endif
