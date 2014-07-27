// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         IDScanSpacePoint.h  
 **
 **   Description:  A space point class to be used in standalone  
 **                 mode, since TrigSiSpacePoint has lots of things  
 **                 that are not needed, and should be in there.
 **
 **                 Really all the essentials should be in a base class
 **                 and the extra, athena dependent bits, should be in 
 **                 a derived class   
 ** 
 **                 The data members are self evident if you look at 
 **                 TrigSiSpacePoint, from which this class draws           
 **
 **
 **   Author:       M.Sutton  
 **
 **   Created:      Fri May 26 21:29:35 BST 2006
 **   Modified:     September 22 2011
 **                   
 **                   
 **
 **************************************************************************/ 


#ifndef __IDSCANSPACEPOINT_H
#define __IDSCANSPACEPOINT_H

#include <vector>
#include <ostream>
#include <cmath>
#include <sys/types.h>

using namespace std;

#include <TObject.h>




class IDScanSpacePoint : public TObject { 

 public:

  IDScanSpacePoint() { }
  IDScanSpacePoint(double r,  double phi,  double z, 
		   double dr, double dphi, double dz, 
		   long  layer, 
		   double locT, double locL);
 
  IDScanSpacePoint(double r,  double phi,  double z, 
		   double dr, double dphi, double dz, 
		   long  layer, 
		   double locT, double locL,
		   const unsigned int* elementList,
  		   const int* width, const int* strip,
		   int barCode);

  double r()    const {return m_r;}
  double phi()  const {return m_phi;}
  double z()    const {return m_z;}
  double dr()   const {return m_dr;} 
  double dphi() const {return m_dphi;}
  double dz()   const {return m_dz;}

  // Methods to calculate associated values
  double x()    const {return m_x;}
  double y()    const {return m_y;}
  double eta(double z0=0) const {
    double zr = (m_z-z0)/m_r; 
    return log(zr+sqrt(1.+zr*zr));
  }
  
  double locT() const {return m_locT;}
  double locL() const {return m_locL;}
  
  long layer()  const {return m_layer;}

  const unsigned int* DetElementList() const {return m_elementList; }

  // width in number of strips
  const int* width() const {return m_width;}
  // Number of first strip
  const int* stripNumber() const {return m_stripNumber;}

  int barCode() const {return m_barCode;}

 protected:

  long          m_layer;

  double	m_r;
  double	m_phi;
  double	m_z;
  double        m_x;
  double        m_y;
  
  double	m_dr;
  double	m_dphi;
  double	m_dz;

  double	m_locT;
  double	m_locL;

  unsigned int  m_elementList[2];
  int m_width[2];
  int m_stripNumber[2];
  int m_barCode;

  ClassDef(IDScanSpacePoint, 1)

};




ostream& operator<<(ostream& , const IDScanSpacePoint& );

#endif  /* __IDSCANSPACEPOINT_H */










