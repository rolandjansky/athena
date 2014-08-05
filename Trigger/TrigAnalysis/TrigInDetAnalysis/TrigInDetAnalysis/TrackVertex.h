// emacs: this is -*- c++ -*-
//
//   @file    TrackVertex.h        
//
//                   
//  
//   Copyright (C) 2010 M.Sutton (sutt@cern.ch)    
//
//   $Id: TrackVertex.h, v0.0   Thu 22 Apr 2010 15:32:23 BST sutt $


#ifndef __TRACKVERTEX_H
#define __TRACKVERTEX_H

#include <iostream>

#include "TObject.h"


class TrackVertex : public TObject {

public:

  TrackVertex(double  x=0, double  y=0, double  z=0,
	      double dx=0, double dy=0, double dz=0, 
	      int Ntracks=0, double c2=0, int dof=0 ) { 
    m_x[0] = x;
    m_x[1] = y;
    m_x[2] = z;

    m_dx[0] = dx;
    m_dx[1] = dy;
    m_dx[2] = dz;

    m_Ntracks=Ntracks;
    m_chi2 = c2;
    m_ndof = dof;
  } 

  TrackVertex(const TrackVertex& v) : TObject(v) { 
    for ( int i=3 ; i-- ; ) m_x[i]  = v.m_x[i];
    for ( int i=3 ; i-- ; ) m_dx[i] = v.m_dx[i];
    m_Ntracks=v.m_Ntracks;
    m_chi2=v.m_chi2;
    m_ndof=v.m_ndof;
  }


  virtual ~TrackVertex();

  const double* position() const { return m_x; }
  double*       position()       { return m_x; }

  const double* error() const { return m_dx; }
  double*       error()       { return m_dx; }

  int Ntracks() const  { return m_Ntracks; }

  double chi2()    const  { return m_chi2; }
  int    ndof()    const  { return m_ndof; }


private:

  double m_x[3];
  double m_dx[3];
  
  int    m_Ntracks;
  double m_chi2;
  int    m_ndof;

  ClassDef(TrackVertex,2)

};



inline std::ostream& operator<<( std::ostream& s, const TrackVertex& _t ) { 
  return s <<  "[ x=" << _t.position()[0]  << " +- " << _t.error()[0]
	   <<  "\ty=" << _t.position()[1]  << " +- " << _t.error()[1]
	   <<  "\tz=" << _t.position()[2]  << " +- " << _t.error()[2]
	   << ";\tchi2="   << _t.chi2()    << "/" << _t.ndof()
	   << "\tNtracks=" << _t.Ntracks() << "\t]"; 
}


#endif  // __TRACKVERTEX_H 










