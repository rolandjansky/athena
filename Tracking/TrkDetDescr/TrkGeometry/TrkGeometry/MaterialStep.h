/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialStep.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_MATERIALSTEP_H
#define TRKGEOMETRY_MATERIALSTEP_H

#include "TrkGeometry/Material.h"
#include <iostream>
#include <vector>

class MsgStream;

class MaterialStepCnv_p1;

namespace Trk {

   /** @class MaterialStep 
   
    is needed for the recording of MaterialProperties
    from Geant4 and read them in with the mapping algorithm

    The class is very much simplified for the ease of persistency
    issues.
    

    @author Andreas.Salzburger@cern.ch 
  */    
  class MaterialStep {
    public:
      /** Default Constructor needed for POOL */
      MaterialStep();

      /** Constructor with arguments */
      MaterialStep(float x, float y, float z,
                   float t,
                   float matX0,
                   float matL0,
                   float matA,
                   float matZ,
                   float matRho,
                   float matdEdX=0.);

      /** Constructor with arguments - used for compound material maps */
      MaterialStep(float x, float y, float z,
                   float t,
                   float matX0,
                   float matL0,
                   float matA,
                   float matZ,
                   float matRho,
                   const std::vector<unsigned char>& elements,
                   const std::vector<unsigned char>& fractions,
                   float matdEdX=0.);

      /** Copy Constructor */
      MaterialStep(const MaterialStep& mstep);

      /** Destructor */
      virtual ~MaterialStep();

      /** Assignment operator */
      MaterialStep& operator=(const MaterialStep& mstep);

      /** Output Method for MsgStream, to be overloaded by child classes */
      virtual MsgStream& dump(MsgStream& sl) const;
      
      /** Output Method for std::ostream, to be overloaded by child classes */
      virtual std::ostream& dump(std::ostream& sl) const;

      /** Access method : steplength */
      double steplength() const;

      /** Access method : steplength */
      double steplengthInX0() const;

      /** Access method : steplength */
      double steplengthInL0() const;

      /** Access method : hitX,Y,Z,R */
      double hitX() const;
      double hitY() const;
      double hitZ() const;
      double hitR() const;

      /** Access method : material X0/A/Z/rho */
      double x0() const;
      double l0() const;
      double A() const;
      double Z() const;
      double rho() const;

      const Material& fullMaterial() const;


    protected:
      friend class ::MaterialStepCnv_p1;
      float                         m_steplength;
                                    
      float                         m_hitX;
      float                         m_hitY;
      float                         m_hitZ;
      float                         m_hitR;
                                    
      Material                      m_material;
      

  };

  inline double MaterialStep::steplength() const { return m_steplength; }

  inline double MaterialStep::steplengthInX0() const { return (m_steplength/m_material.X0); }

  inline double MaterialStep::steplengthInL0() const { return (m_steplength/m_material.L0); }

  inline double MaterialStep::hitX() const { return m_hitX; }

  inline double MaterialStep::hitY() const { return m_hitY; }

  inline double MaterialStep::hitZ() const { return m_hitZ; }

  inline double MaterialStep::hitR() const { return m_hitR; }

  inline double MaterialStep::x0() const  { return m_material.X0; }

  inline double MaterialStep::l0() const { return m_material.L0; }

  inline double MaterialStep::A() const { return m_material.A; }

  inline double MaterialStep::Z() const { return m_material.Z; }

  inline double MaterialStep::rho() const { return m_material.rho; }

  inline const Material& MaterialStep::fullMaterial() const { return m_material; }

  /**Overload of << operator for both, MsgStream and std::ostream for debug output*/
  MsgStream& operator << ( MsgStream& sl, const MaterialStep& mstep);
  std::ostream& operator << ( std::ostream& sl, const MaterialStep& mstep);

}

#endif
