/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiDiodesParameters.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_SIDIODESPARAMETERS_H
#define INDETREADOUTGEOMETRY_SIDIODESPARAMETERS_H

// Data members classes
#include "InDetReadoutGeometry/SiLocalPosition.h"

namespace InDetDD {
   
   /** @class SiDiodesParameters 
   Class to handle the position of the centre and the width of a
   diode or a cluster of diodes
   Version 1.1 15/08/2001 David Calvet
   */

  class SiDiodesParameters {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
    public:

      /** Implicit constructor: */
      SiDiodesParameters();

      /** Copy constructor: */
      SiDiodesParameters(const SiDiodesParameters &parameters);

      /** Constructor with parameters:
          position of the diodes centre
          width of the diodes */
      SiDiodesParameters(const SiLocalPosition &centre,
    		     const SiLocalPosition &width);

      /** Destructor: */
      ~SiDiodesParameters();

      /** Assignment operator: */
      SiDiodesParameters &operator=(const SiDiodesParameters &parameters);

      ///////////////////////////////////////////////////////////////////
      // Const methods:
      ///////////////////////////////////////////////////////////////////

      /** position of the diodes centre: */
      SiLocalPosition centre() const;

      /** width of the diodes: */
      SiLocalPosition width() const;
      SiLocalPosition halfWidth() const;

      /** boundaries of the diodes: */
      double xEtaMin() const;
      double xEtaMax() const;
      double xPhiMin() const;
      double xPhiMax() const;
      double xDepthMin() const;
      double xDepthMax() const;

      ///////////////////////////////////////////////////////////////////
      // Non-const methods:
      ///////////////////////////////////////////////////////////////////

      void centre(const SiLocalPosition &centre);

      void width(const SiLocalPosition &width);

      ///////////////////////////////////////////////////////////////////
      // Private data:
      ///////////////////////////////////////////////////////////////////
    private:
      SiLocalPosition m_centre; //!< position of the diodes centre
      SiLocalPosition m_width; //!< width of the diodes
  };

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline SiDiodesParameters::~SiDiodesParameters()
{}

inline SiLocalPosition SiDiodesParameters::centre() const
{
  return m_centre;
}

inline SiLocalPosition SiDiodesParameters::width() const
{
  return m_width;
}

inline SiLocalPosition SiDiodesParameters::halfWidth() const
{
  return m_width/2;
}

inline double SiDiodesParameters::xEtaMin() const
{
  return m_centre.xEta()-m_width.xEta()/2;
}

inline double SiDiodesParameters::xEtaMax() const
{
  return m_centre.xEta()+m_width.xEta()/2;
}

inline double SiDiodesParameters::xPhiMin() const
{
  return m_centre.xPhi()-m_width.xPhi()/2;
}

inline double SiDiodesParameters::xPhiMax() const
{
  return m_centre.xPhi()+m_width.xPhi()/2;
}

inline double SiDiodesParameters::xDepthMin() const
{
  return m_centre.xDepth()-m_width.xDepth()/2;
}

inline double SiDiodesParameters::xDepthMax() const
{
  return m_centre.xDepth()+m_width.xDepth()/2;
}

inline void SiDiodesParameters::centre(const SiLocalPosition &centre)
{
  m_centre=centre;
}

inline void SiDiodesParameters::width(const SiLocalPosition &width)
{
  m_width=width;
}

} // namespace InDetDD

#endif // INDETREADOUTGEOMETRY_SIDIODESPARAMETERS_H
