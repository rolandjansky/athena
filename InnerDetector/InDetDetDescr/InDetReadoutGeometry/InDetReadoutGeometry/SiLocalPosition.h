/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiLocalPosition.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_SILOCALPOSITION_H
#define INDETREADOUTGEOMETRY_SILOCALPOSITION_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include <cmath>

namespace Trk {
  enum ExtraLocalPosParam {distDepth = 2}; // These will be defined in Trk soon.
}

namespace InDetDD {

    /** @class SiLocalPosition
    Class to represent a position in the natural frame of a silicon sensor, for Pixel and SCT
    For Pixel: eta=column, phi=row

    Version 2.1 01/08/2001 David Calvet
    */

  class SiLocalPosition {


    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    /** Default constructor: */
    SiLocalPosition();
    
    /** Copy constructor: */
    SiLocalPosition(const SiLocalPosition &position) =  default;

    /** This allows one to pass a Amg::Vector2D  to a SiLocalPosition */
    SiLocalPosition(const Amg::Vector2D &position);

    /** Constructor with parameters:
        position along eta direction
        position along phi direction
        position along depth direction (default is 0) */
    SiLocalPosition(const double xEta,const double xPhi,
  		  const double xDepth=0);

    /** Destructor: */
    ~SiLocalPosition()=default;

    /** Assignment operator: */
    SiLocalPosition &operator=(const SiLocalPosition &) = default;
    
    /** Move assignment **/
    SiLocalPosition &operator=(SiLocalPosition &&) = default;

    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////

    /** position along eta direction:*/
    double xEta() const;

    /** position along phi direction:*/
    double xPhi() const;

    /** Cylindrical coordinate r:*/
    double r() const;

    /** Cylindrical coordinate phi:*/
    double phi() const;

    /** position along depth direction: */
    double xDepth() const;

    /** positions for Pixel: */
    double xColumn() const;
    double xRow() const;

    ///////////////////////////////////////////////////////////////////
    // Non-const methods:
    ///////////////////////////////////////////////////////////////////

    void xEta(const double xEta);

    void xPhi(const double xPhi);

    void xDepth(const double xDepth);

    // for Pixel:
    void xColumn(const double xColumn);
    void xRow(const double xRow);

    // addition of positions:
    SiLocalPosition &operator+=(const SiLocalPosition &position);
  
    // so we can go from SiLocalPosition to Trk::LocalPosition
    operator Amg::Vector2D(void) const;

    // scaling:
    SiLocalPosition &operator*=(const double factor);
    SiLocalPosition &operator/=(const double factor);

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////

  private:
    double m_xEta; //!< position along eta direction
    double m_xPhi; //!< position along phi direction
    double m_xDepth; //!< position along depth direction
  };

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline double SiLocalPosition::xEta() const
{
  return m_xEta;
}

inline double SiLocalPosition::xPhi() const
{
  return m_xPhi;
}

inline double SiLocalPosition::xDepth() const
{
  return m_xDepth;
}

inline double SiLocalPosition::r() const
{
  return std::sqrt(m_xEta * m_xEta + m_xPhi * m_xPhi);
}

inline double SiLocalPosition::phi() const
{
  return std::atan2(m_xPhi, m_xEta);
}

inline double SiLocalPosition::xColumn() const
{
  return m_xEta;
}

inline double SiLocalPosition::xRow() const
{
  return m_xPhi;
}

inline void SiLocalPosition::xEta(const double xEta)
{
  m_xEta=xEta;
}

inline void SiLocalPosition::xPhi(const double xPhi)
{
  m_xPhi=xPhi;
}

inline void SiLocalPosition::xDepth(const double xDepth)
{
  m_xDepth=xDepth;
}

inline void SiLocalPosition::xColumn(const double xColumn)
{
  m_xEta=xColumn;
}

inline void SiLocalPosition::xRow(const double xRow)
{
  m_xPhi=xRow;
}

///////////////////////////////////////////////////////////////////
// Binary operators:
///////////////////////////////////////////////////////////////////
SiLocalPosition operator+(const SiLocalPosition &position1,
			  const SiLocalPosition &position2);

SiLocalPosition operator*(const SiLocalPosition &position,const double factor);

inline SiLocalPosition operator*(const double factor,const SiLocalPosition &position)
{
  return position*factor;
}

SiLocalPosition operator/(const SiLocalPosition &position,const double factor);

} // namespace InDetDD

#endif // INDETREADOUTGEOMETRY_SILOCALPOSITION_H
