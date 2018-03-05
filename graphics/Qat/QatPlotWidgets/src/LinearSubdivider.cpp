//---------------------------------------------------------------------------//
//                                                                           //
// This file is part of the Pittsburgh Visualization System (PVS)            //
//                                                                           //
// Copyright (C) 2004 Joe Boudreau, University of Pittsburgh                 //
//                                                                           //
//  This program is free software; you can redistribute it and/or modify     //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation; either version 2 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  This program is distributed in the hope that it will be useful,          //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with this program; if not, write to the Free Software              //
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307     //
// --------------------------------------------------------------------------//


#include "QatPlotWidgets/LinearSubdivider.h"
#include <cmath>
#include <sstream>
#include <iostream>
inline double ciel(double x) { 
  double fx = floor(x);
  if (x==fx) return x;
  else return x+1.0;
}
LinearSubdivider::LinearSubdivider():
  m_min(0.0),m_max(1.0)
{
}

LinearSubdivider::~LinearSubdivider() {
}



// Set the range:
void LinearSubdivider::setRange(double min, double max){

  // This protection insures 
  if (min==max) return;
  m_min=min;
  m_max=max;
  recompute();




}

// Set the range:
void LinearSubdivider::setMin(double min){
  setRange(min,m_max);
}

// Set the range:
void LinearSubdivider::setMax(double max){
  setRange(m_min,max);
}


// Get the number of subdivisions:
int LinearSubdivider::getNumSubdivisions() const{
  return m_subdivision.size();
}

// Get the location of each subdivision:
const RangeDivision & LinearSubdivider::getSubdivision(int i) const{
  return m_subdivision[i];
}


void LinearSubdivider::recompute() {
  // Clean out old subdivisions:
  m_subdivision.erase(m_subdivision.begin(),m_subdivision.end());


  double exponent = floor(log10(m_max-m_min));
  double multiplier = pow(10,exponent);
  int Ntyp = int (10*m_max/multiplier-10*m_min/multiplier+0.5); 
  // number from 10 to 100
  int intervals=1;
  if (Ntyp==12) {
    intervals=4;
  }
  else if (Ntyp==20) {
    intervals=5;
  }
  else if (Ntyp==30) {
    intervals=5;
  }
  else if (Ntyp==40) {
    intervals=4;
  }
  else if (Ntyp==50) {
    intervals=5;
  }
  else if (Ntyp==60) {
    intervals=5;
  }
  else if (Ntyp==70) {
    intervals=7;
  }
  else if (Ntyp==80) {
    intervals=4;
  }
  else if (Ntyp==90) {
    intervals=3;
  }
  else {
    intervals=5;
  }
  multiplier *= (Ntyp/10.0/intervals);

  int nTicks=int(((m_max-m_min)/multiplier) + 1.05);
  
  if (nTicks<50) {
    for (int i=0;i<nTicks;i++) {
      double sub = m_min+i*multiplier;
      //if (fabs(xValue)/xMultiplier < 1.0E-6) xValue=0;
      if (sub>=m_min && sub<=m_max)   {
	m_subdivision.push_back(sub);
	
      }
      
    }
  }
  else {
  
    {
      m_subdivision.push_back(m_min);
    }
    {
      m_subdivision.push_back(m_max);
    }
  }
}

// Get the validity of each subdivision:
bool LinearSubdivider::isValid(int i) const {
  const RangeDivision & rd=getSubdivision(i);
  return (rd.x()>=m_min && rd.x()<=m_max);
}

