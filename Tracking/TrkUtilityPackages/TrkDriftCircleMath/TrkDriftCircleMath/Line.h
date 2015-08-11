/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_LINE_H
#define DCMATH_LINE_H

#include <cmath>
#include <ostream>
#include <iostream>

#include "TrkDriftCircleMath/LocPos.h"
#include "TrkDriftCircleMath/LocDir.h"
#include "TrkDriftCircleMath/RotPhi.h"

namespace TrkDriftCircleMath {

  class Line{
  public:
    Line( LocPos pos, LocDir dir ) 
      : m_phi( atan2(dir.y(),dir.x()) ), m_pos(pos)
    { 
      RotPhi rot( m_phi );
      LocPos p = (rot*pos);
      m_x0 = p.x();
      m_y0 = p.y();
    }

    Line( LocPos pos, double phi) : m_phi(phi),m_pos(pos) {  
      RotPhi rot( m_phi );
      LocPos p = (rot*pos);
      m_x0 = p.x();
      m_y0 = p.y();
    }

      Line( double x0, double y0, double phi) : m_phi(phi), m_pos(RotPhi( -phi )*LocPos(x0,y0)), m_x0(x0),m_y0(y0) 
    {
    }

    ~Line() {}
    
    /** set methods to reduce copying of data */
    void set( const LocPos& pos, const LocDir& dir ){
      m_phi = atan2(dir.y(),dir.x());
      m_pos.set(pos);
      updateLocalParameters();
    }

    void set( const LocPos& pos, double phi ){
      m_phi = phi;
      m_pos.set(pos);
      updateLocalParameters();
    }

    void set( double x0, double y0, double phi ){
      m_phi = phi;
      m_x0 = x0;
      m_y0 = y0; 
      updateGlobalParameters();
    }

    void updateLocalParameters() {
      RotPhi rot( m_phi );
      LocPos p = (rot*m_pos);
      m_x0 = p.x();
      m_y0 = p.y();
    }

    void updateGlobalParameters() {
      m_pos.set(RotPhi( -m_phi )*LocPos(m_x0,m_y0));
    }

    double phi() const { return m_phi; }
    double x0() const { return m_x0; }
    double y0() const { return m_y0; }
    LocPos position() const { return m_pos; }
    LocDir direction() const { return LocDir( cos( phi() ), sin( phi() ) ); }

  private:
    double m_phi;
    LocPos m_pos;
    double m_x0;
    double m_y0;
  };

  std::ostream& operator<<( std::ostream& os, const TrkDriftCircleMath::Line& l );

}
 

#endif
