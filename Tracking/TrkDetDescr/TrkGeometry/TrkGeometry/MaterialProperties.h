/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialProperties.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_MATERIALPROPERTIES_H
#define TRKGEOMETRY_MATERIALPROPERTIES_H

#include "TrkGeometry/Material.h"
#include "GaudiKernel/MsgStream.h"
//STD
#include <iosfwd>

class MaterialPropertiesCnv_p1;

namespace Trk {

  /** 
   @class MaterialProperties

   Material with information about thickness of material
   
   the units are :
    - thickness [mm] ( only used for layer description)
    - X0  [mm]
    - L0  [mm]
    - A   [g/mole]
    - Z 
    - rho [g/mm3]

   The Trk::MaterialProperties describe material associated with a Trk::Layer
   

   @author Andreas.Salzburger@cern.ch 
   */

  class MaterialProperties {
          
    public:
      /** Default Constructor */
      MaterialProperties() = default;
      /** Copy Constructor */
      MaterialProperties(const MaterialProperties& mprop) = default;
      /** Assignment Operator */
      MaterialProperties& operator=(const MaterialProperties& mprop) = default;
      /** Move Constructor */
      MaterialProperties(MaterialProperties&& mprop) = default;
      /** Move Assignment Operator */
      MaterialProperties& operator=(MaterialProperties&& mprop) = default;
      /** Destructor */
      ~MaterialProperties() = default;

      /** Constructor - for averaged material */
      MaterialProperties(float path,
                         float Xo,
                         float Lo,
                         float averageA,
                         float averageZ,
                         float averageRho,
                         float dEdX = 0.);

      /** Constructor - for full Material class */
      MaterialProperties(const Material& material, float path);

      /** Pseudo-Constructor clone() */
      MaterialProperties* clone() const;
       
     
      /** Scale operator - scales the material thickness */
      MaterialProperties& operator *= ( float scale);

      /** Return the stored Material */
      const Material& material() const;
 
      /** Return the stored Material */
      Material& material() ;
      
      /** Return the radiation length */
      float x0() const;

      /** Return the nuclear interaction length */
      float l0() const;

      /** Return the thickness in mm */
      float thickness() const;

      /** Return the radiationlength fraction */
      float thicknessInX0() const;

      /** Return the nuclear interaction length fraction */
      float thicknessInL0() const;
                 
      /** Returns the average Z of the material */
      float averageZ() const;
                  
      /** Return the average A of the material [gram/mole] */
      float averageA() const;
            
      /** Return the average density of the material
         - in [g/mm^3]
        */
      float averageRho() const;
      
      /** Return the @f$ Z/A * rho @f$ */ 
      float zOverAtimesRho() const;
      
      /** Return the @f$ d* Z/A * rho @f$ */ 
      float zOverAtimesRhoTimesD() const;
      
      /** Return method for @f$ dE/dX @f$*/
      float dEdX() const;

      /** Material averaging */
      void addMaterial(const Material& mp, float dInX0); 
      
      /** Set Material */
      void setMaterial(const Material& mp, float thickness=1.);
                                                                                  
    protected:
      friend class ::MaterialPropertiesCnv_p1;

      /** Set dEdX       - important for material calibarion */
      void setDeDx(float dEdX);

      Material m_material;
      float m_dInX0 = 0; //!< thickness in units of radiation length
      float m_dInL0 = 0; //!< thickness in units of nuclear interaction length
      float m_zOaTrTd = 0; //!< @f$ \frac{Z}{A}\cdot\rho\cdot d @f$ - in ATLAS units
  };
  /**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
  MsgStream& operator<<( MsgStream& sl, const MaterialProperties& mprop);
  std::ostream& operator<<( std::ostream& sl, const MaterialProperties& mprop);

  /** Useful typedefs */
  typedef std::vector< const MaterialProperties*>   MaterialPropertiesVector;
  typedef std::vector<MaterialPropertiesVector>     MaterialPropertiesMatrix;

       
} // end of namespace

#include "TrkGeometry/MaterialProperties.icc"
#endif // TRKGEOMETRY_MATERIALPROPERTIES_H

