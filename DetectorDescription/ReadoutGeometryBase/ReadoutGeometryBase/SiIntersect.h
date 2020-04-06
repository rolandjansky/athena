/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   SiIntersect.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_SIINTERSECT_H
#define INDETREADOUTGEOMETRY_SIINTERSECT_H

namespace InDetDD {

  /** @class SiIntersect
 
      class to run intersection tests

      @author Grant Gorfine
  */

  class SiIntersect {

    public:

      enum IntersectState {OUT = 0, BOUNDARY = 1, IN = 2};

      SiIntersect(IntersectState state = OUT);

      bool in() const; // Definitely in
      bool out() const; // Definitely out
      bool nearBoundary() const; // Near boundary within tolerences
      bool mayIntersect() const; // in() || nearBoundary()
      operator bool() const; // Equivalent to mayIntersect().

      void setIn();
      void setOut();
      void setNearBoundary();

    private:
      IntersectState m_state;

  };

inline SiIntersect::SiIntersect(IntersectState state) 
  : m_state(state)
{}

inline bool SiIntersect::in() const 
{
  return (m_state == IN);
}

inline bool SiIntersect::out() const 
{
  return (m_state == OUT);
}

inline bool SiIntersect::nearBoundary() const 
{
  return (m_state == BOUNDARY);
}


inline bool SiIntersect::mayIntersect() const 
{
  return (m_state == BOUNDARY || m_state == IN);
}

inline SiIntersect::operator bool() const 
{
  return mayIntersect();
}

inline void SiIntersect::setIn()
{
  m_state = IN;
}

inline void SiIntersect::setOut()
{
  m_state = OUT;
}

inline void SiIntersect::setNearBoundary()
{
  m_state = BOUNDARY;
}

} // namespace InDetDD

#endif  //INDETREADOUTGEOMETRY_SIINTERSECT_H

