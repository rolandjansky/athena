/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "TrigInterfaces/IMonitoredAlgo.h"

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


  class MonitoredSplitVertex
  {
  public:

    // Registration
    void declare( IMonitoredAlgo* p, std::string prefix = "", std::string suffix = "" )
    {
      const IMonitoredAlgo::ContainerReset reset = IMonitoredAlgo::AutoClear;

      p->declareMonitoredStdContainer( prefix + "1NTrks"   + suffix, m_1NTrks, reset );
      p->declareMonitoredStdContainer( prefix + "1X"       + suffix, m_1X    , reset );
      p->declareMonitoredStdContainer( prefix + "1Y"       + suffix, m_1Y    , reset );
      p->declareMonitoredStdContainer( prefix + "1Z"       + suffix, m_1Z    , reset );
      p->declareMonitoredStdContainer( prefix + "1Xerr"    + suffix, m_1Xerr , reset );
      p->declareMonitoredStdContainer( prefix + "1Yerr"    + suffix, m_1Yerr , reset );
      p->declareMonitoredStdContainer( prefix + "1Zerr"    + suffix, m_1Zerr , reset );
      p->declareMonitoredStdContainer( prefix + "2NTrks"   + suffix, m_2NTrks, reset );
      p->declareMonitoredStdContainer( prefix + "2X"       + suffix, m_2X    , reset );
      p->declareMonitoredStdContainer( prefix + "2Y"       + suffix, m_2Y    , reset );
      p->declareMonitoredStdContainer( prefix + "2Z"       + suffix, m_2Z    , reset );
      p->declareMonitoredStdContainer( prefix + "2Xerr"    + suffix, m_2Xerr , reset );
      p->declareMonitoredStdContainer( prefix + "2Yerr"    + suffix, m_2Yerr , reset );
      p->declareMonitoredStdContainer( prefix + "2Zerr"    + suffix, m_2Zerr , reset );
      p->declareMonitoredStdContainer( prefix + "DNTrks"   + suffix, m_DNTrks, reset );
      p->declareMonitoredStdContainer( prefix + "DX"       + suffix, m_DX    , reset );
      p->declareMonitoredStdContainer( prefix + "DY"       + suffix, m_DY    , reset );
      p->declareMonitoredStdContainer( prefix + "DZ"       + suffix, m_DZ    , reset );
      p->declareMonitoredStdContainer( prefix + "DXerr"    + suffix, m_DXerr , reset );
      p->declareMonitoredStdContainer( prefix + "DYerr"    + suffix, m_DYerr , reset );
      p->declareMonitoredStdContainer( prefix + "DZerr"    + suffix, m_DZerr , reset );
      p->declareMonitoredStdContainer( prefix + "DXpull"   + suffix, m_DXpull, reset );
      p->declareMonitoredStdContainer( prefix + "DYpull"   + suffix, m_DYpull, reset );
      p->declareMonitoredStdContainer( prefix + "DZpull"   + suffix, m_DZpull, reset );
    }

    // Update method
    void push_back( const T2SplitVertex& split )
    {
      m_1NTrks.push_back( split.vertex1().NTrks() );
      m_1X    .push_back( split.vertex1().X    () );
      m_1Y    .push_back( split.vertex1().Y    () );
      m_1Z    .push_back( split.vertex1().Z    () );
      m_1Xerr .push_back( split.vertex1().Xerr () );
      m_1Yerr .push_back( split.vertex1().Yerr () );
      m_1Zerr .push_back( split.vertex1().Zerr () );
      m_2NTrks.push_back( split.vertex2().NTrks() );
      m_2X    .push_back( split.vertex2().X    () );
      m_2Y    .push_back( split.vertex2().Y    () );
      m_2Z    .push_back( split.vertex2().Z    () );
      m_2Xerr .push_back( split.vertex2().Xerr () );
      m_2Yerr .push_back( split.vertex2().Yerr () );
      m_2Zerr .push_back( split.vertex2().Zerr () );
      m_DNTrks.push_back( split.DNTrks  () );
      m_DX    .push_back( split.DX      () );
      m_DY    .push_back( split.DY      () );
      m_DZ    .push_back( split.DZ      () );
      m_DXerr .push_back( split.DXerr   () );
      m_DYerr .push_back( split.DYerr   () );
      m_DZerr .push_back( split.DZerr   () );
      m_DXpull.push_back( split.DXpull  () );
      m_DYpull.push_back( split.DYpull  () );
      m_DZpull.push_back( split.DZpull  () );
    }

  private:
    // Data members
    std::vector<unsigned> m_1NTrks;
    std::vector<double  > m_1X    ;
    std::vector<double  > m_1Y    ;
    std::vector<double  > m_1Z    ;
    std::vector<double  > m_1Xerr ;
    std::vector<double  > m_1Yerr ;
    std::vector<double  > m_1Zerr ;
    std::vector<unsigned> m_2NTrks;
    std::vector<double  > m_2X    ;
    std::vector<double  > m_2Y    ;
    std::vector<double  > m_2Z    ;
    std::vector<double  > m_2Xerr ;
    std::vector<double  > m_2Yerr ;
    std::vector<double  > m_2Zerr ;
    std::vector<double  > m_DNTrks;
    std::vector<double  > m_DX    ;
    std::vector<double  > m_DY    ;
    std::vector<double  > m_DZ    ;
    std::vector<double  > m_DXerr ;
    std::vector<double  > m_DYerr ;
    std::vector<double  > m_DZerr ;
    std::vector<double  > m_DXpull;
    std::vector<double  > m_DYpull;
    std::vector<double  > m_DZpull;
  };

} // end namespace

#endif
