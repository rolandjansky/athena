/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_ForwardPolarPosition.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_SCT_FORWARDPOLARPOSITION_H
#define INDETREADOUTGEOMETRY_SCT_FORWARDPOLARPOSITION_H

#include <string>

namespace InDetDD {

  /** @class SCT_ForwardPolarPosition 
  
      2D position in polar coordinates for the polar frame in
      the SCT endcaps.
  
      @author Grant Gorfine
    */
  class SCT_ForwardPolarPosition {
  
  public:
  
    // Implicit constructor
    SCT_ForwardPolarPosition();
  
    // Copy constructor:
    SCT_ForwardPolarPosition(const SCT_ForwardPolarPosition &polar);
  
    /** Constructor with parameters:
        r coordinate of point
       theta coordinate of point */
    SCT_ForwardPolarPosition(const double r,const double theta);
  
    // Destructor:
    virtual ~SCT_ForwardPolarPosition() = default;
  
    // Assignment operator:
    SCT_ForwardPolarPosition &operator=(const SCT_ForwardPolarPosition &polar);
  
    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////
  
    /** r coordinate of point: */
    double r() const;
  
    /** theta coordinate of point: */
    double theta() const;
  
    //Comparison operator
    virtual bool operator==(const SCT_ForwardPolarPosition &polar) const;
   
    //Print method:
    virtual std::string print() const;

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
  private:
    double m_r; // r coordinate of point
    double m_theta; // theta coordinate of point
  };
  
  ///////////////////////////////////////////////////////////////////
  // Inline methods:
  ///////////////////////////////////////////////////////////////////
  
  inline double SCT_ForwardPolarPosition::r() const
  {
    return m_r;
  }
  
  inline double SCT_ForwardPolarPosition::theta() const
  {
    return m_theta;
  }

} // namespace InDetDD

#endif // INDETREADOUTGEOMETRY_SCT_FORWARDPOLARPOSITION_H


