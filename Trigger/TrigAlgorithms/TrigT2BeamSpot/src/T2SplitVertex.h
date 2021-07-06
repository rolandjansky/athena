/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @version: $Id: T2SplitVertex.h 361213 2011-04-23 18:26:44Z bartoldu $
 *
 * @project: HLT, PESA algorithms
 * @package: TrigT2BeamSpot
 * @class  : T2SplitVertex
 *
 * @brief Helper class for a pair of vertices and their separation (delta)
 *        with some functionality used by the beam spot algorithm
 *
 * @author Rainer Bartoldus, SLAC, <bartoldu@slac.stanford.edu>
 * @author Emanuel Strauss, SLAC <estrauss@slac.stanford.edu>
 *
 **********************************************************************************/

#ifndef TRIGT2BEAMSPOT_T2SPLITVERTEX_H
#define TRIGT2BEAMSPOT_T2SPLITVERTEX_H

/// Local tools
#include "T2Vertex.h"

/// External classes
#include "TrigInDetEvent/TrigVertex.h"

#include <string>
#include <vector>
#include <cmath>

namespace PESA
{

  class T2SplitVertex
  {
  public:

    // Helpers
    double rss( double x, double y )
      {
        return sqrt( x*x+y*y );
      }

    double ratio( double x, double y )
    {
      return ( y > 0. ) ? x / y : 0.;
    }

    // Constructor

    T2SplitVertex( const TrigVertex& vertex1, const TrigVertex& vertex2 )
      : m_vertex1 ( vertex1 )
      , m_vertex2 ( vertex2 )
      , m_DNTrks  ( ( m_vertex1.NTrks() + m_vertex2.NTrks() ) / 2. )
      , m_DX      ( m_vertex1.X() - m_vertex2.X() )
      , m_DY      ( m_vertex1.Y() - m_vertex2.Y() )
      , m_DZ      ( m_vertex1.Z() - m_vertex2.Z() )
      , m_DXerr   ( rss( m_vertex1.Xerr(), m_vertex2.Xerr() ) )
      , m_DYerr   ( rss( m_vertex1.Yerr(), m_vertex2.Yerr() ) )
      , m_DZerr   ( rss( m_vertex1.Zerr(), m_vertex2.Zerr() ) )
      , m_DXpull  ( ratio( m_DX, m_DXerr ) )
      , m_DYpull  ( ratio( m_DY, m_DYerr ) )
      , m_DZpull  ( ratio( m_DZ, m_DZerr ) )
      {
      }

    // Accessors
    const T2SimpleVertex& vertex1 () const { return  m_vertex1; }
    const T2SimpleVertex& vertex2 () const { return  m_vertex2; }
    double                DNTrks  () const { return  m_DNTrks ; }
    double                DX      () const { return  m_DX     ; }
    double                DY      () const { return  m_DY     ; }
    double                DZ      () const { return  m_DZ     ; }
    double                DXerr   () const { return  m_DXerr  ; }
    double                DYerr   () const { return  m_DYerr  ; }
    double                DZerr   () const { return  m_DZerr  ; }
    double                DXpull  () const { return  m_DXpull ; }
    double                DYpull  () const { return  m_DYpull ; }
    double                DZpull  () const { return  m_DZpull ; }

  private:

    // Data members
    T2SimpleVertex m_vertex1;
    T2SimpleVertex m_vertex2;
    double         m_DNTrks ;
    double         m_DX     ;
    double         m_DY     ;
    double         m_DZ     ;
    double         m_DXerr  ;
    double         m_DYerr  ;
    double         m_DZerr  ;
    double         m_DXpull ;
    double         m_DYpull ;
    double         m_DZpull ;
  };

} // end namespace

#endif
