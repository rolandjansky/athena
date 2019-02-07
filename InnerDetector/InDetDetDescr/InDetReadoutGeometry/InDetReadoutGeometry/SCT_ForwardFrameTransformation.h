/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_ForwardFrameTransformation.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_SCT_FORWARDFRAMETRANSFORMATION_H
#define INDETREADOUTGEOMETRY_SCT_FORWARDFRAMETRANSFORMATION_H

namespace InDetDD {

class SCT_ForwardPolarPosition;
class SiLocalPosition;

   
  /** @class SCT_ForwardFrameTransformation

      Class that connect cartesian and pokar coordinates

      @author: Grant Gorfine
      - modified & maintained : Andreas Salzburger */

  class SCT_ForwardFrameTransformation {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
    public:

      /** Copy constructor: */
      SCT_ForwardFrameTransformation(const SCT_ForwardFrameTransformation &frame);

      /** Constructor with parameters:
          eta coordinate of module center
          phi coordinate of module center
          r from module center to beam */
      SCT_ForwardFrameTransformation(const double etaCenter,const double phiCenter,
                                 const double radius);

      /** Destructor: */
      virtual ~SCT_ForwardFrameTransformation() = default;

      /** Assignment operator: */
      SCT_ForwardFrameTransformation &operator=(const SCT_ForwardFrameTransformation &frame);

      /** Default move assignment operator: */
      SCT_ForwardFrameTransformation &operator=(SCT_ForwardFrameTransformation &&) = default;

      ///////////////////////////////////////////////////////////////////
      // Const methods:
      ///////////////////////////////////////////////////////////////////

      /** eta coordinate of module center: */
      double etaCenter() const;

      /** phi coordinate of modulecenter: */
      double phiCenter() const;

      /** r from module center to beam:*/
      double radius() const;

      /** create a SCT_ForwardPolarPosition from a SiLocalPosition */
      SCT_ForwardPolarPosition polarFromCartesian(const SiLocalPosition& cartesian) const;
  
      /** create a SiLocalPosition from a SCT_ForwardPolarPosition */
      SiLocalPosition cartesianFromPolar(const SCT_ForwardPolarPosition& polar) const;
 
      ///////////////////////////////////////////////////////////////////
      // Private methods:
      ///////////////////////////////////////////////////////////////////
   private:

      SCT_ForwardFrameTransformation();

      ///////////////////////////////////////////////////////////////////
      // Private data:
      ///////////////////////////////////////////////////////////////////
  private:
    double m_etaCenter; //!< eta coordinate of module center
    double m_phiCenter; //!< phi coordinate of module center
    double m_radius; //!< r from module center to beam
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline double SCT_ForwardFrameTransformation::etaCenter() const
{
  return m_etaCenter;
}

inline double SCT_ForwardFrameTransformation::phiCenter() const
{
  return m_phiCenter;
}

inline double SCT_ForwardFrameTransformation::radius() const
{
  return m_radius;
}

} // namespace InDetDD

#endif // INDETREADOUTGEOMETRY_SCT_FORWARDFRAMETRANSFORMATION_H
