/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_LOCVEC2D_H
#define DCMATH_LOCVEC2D_H

/** local 2D vector */

#include <ostream>
#include <cmath>

namespace TrkDriftCircleMath {

  class LocVec2D{
  public:
    LocVec2D( double x, double y ) 
      : m_x(x),m_y(y) {}
  
    ~LocVec2D() {}

    double x() const { return m_x; }
    double y() const { return m_y; }

    void set( const LocVec2D& vec ) { m_x = vec.x(); m_y = vec.y(); }
    void setX( double x) { m_x = x; }
    void setY( double y) { m_y = y; }

    LocVec2D operator+( const LocVec2D& lv ) const {
      return LocVec2D( x()+lv.x(), y()+lv.y() ); }

    LocVec2D operator-( const LocVec2D& lv ) const {
      return LocVec2D( x()-lv.x(), y()-lv.y() ); }

    LocVec2D& operator+=( const LocVec2D& lv ) {
      m_x+=lv.x(); m_y+=lv.y();
      return *this; }

    LocVec2D& operator-=( const LocVec2D& lv ) {
      m_x-=lv.x(); m_y-=lv.y();
      return *this; }

    double operator*( const LocVec2D& lv ) const {
      return x()*lv.x() + y()*lv.y(); }
  
    LocVec2D operator*( double v ) const {
      LocVec2D n(*this); return n*=v; }

    LocVec2D& operator*=( double v ) {
      m_x*=v; m_y*=v; return *this; }

    LocVec2D& operator/=( double v ) {
      m_x/=v; m_y/=v; return *this; }

    double cross( const LocVec2D& lv ) const { 
      return y()*lv.x() - x()*lv.y(); }

    double mag() const { return sqrt( (*this)*(*this) ); }
  private:
    double m_x;
    double m_y;
  };


  std::ostream& operator<<( std::ostream& os, const TrkDriftCircleMath::LocVec2D& lv );
}


#endif
