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


#include "QatPlotWidgets/LinearRangeDivider.h"
#include <cmath>
#include <sstream>
inline double ciel(double x) { 
  double fx = floor(x);
  if (x==fx) return x;
  else return x+1.0;
}
LinearRangeDivider::LinearRangeDivider():
  m_min(0.0),m_max(1.0)
{
}

LinearRangeDivider::~LinearRangeDivider() {
}



// Set the range:
void LinearRangeDivider::setRange(double min, double max){

  // This protection insures 
  if (min==max) return;
  m_min=min;
  m_max=max;
  recompute();




}

// Set the range:
void LinearRangeDivider::setMin(double min){
  setRange(min,m_max);
}

// Set the range:
void LinearRangeDivider::setMax(double max){
  setRange(m_min,max);
}


// Get the number of subdivisions:
int LinearRangeDivider::getNumSubdivisions() const{
  return m_subdivision.size();
}

// Get the location of each subdivision:
const RangeDivision & LinearRangeDivider::getSubdivision(int i) const{
  return m_subdivision[i];
}


void LinearRangeDivider::recompute() {
  // Clean out old subdivisions:
  m_subdivision.erase(m_subdivision.begin(),m_subdivision.end());

  double lower, multiplier;
  int nTicks;

  double exponent = floor(log10(m_max-m_min));
  multiplier = pow(10,exponent);
  int Ntyp = int(10*m_max/multiplier-10*m_min/multiplier); 
  // number from 10 to 100
  double omult;
  if (Ntyp<12) {
    omult = .1*multiplier;
    multiplier = .2* multiplier;
  }
  else if (Ntyp<16) {
    omult = .1*multiplier;
    multiplier = .3*multiplier;
  }
  else if (Ntyp<25) {
    omult = .2*multiplier;
    multiplier = .4*multiplier;
  }
  else if (Ntyp<30) {
    omult = .1*multiplier;
    multiplier = .5*multiplier;
  }
  else if (Ntyp<40) {
    omult = .2*multiplier;
    multiplier = .6*multiplier;
  }
  else if (Ntyp<50) {
    omult = .2*multiplier;
    multiplier = .8*multiplier;
  }
  else if (Ntyp<60) {
    omult = multiplier;
    multiplier = 1.0*multiplier;
  }
  else if (Ntyp<75) {
    omult = 0.4*multiplier;
    multiplier = 1.2*multiplier;
  }
  else if (Ntyp<80) {
    omult = 0.5*multiplier;
    multiplier = 1.5*multiplier;
  }
  else {
    omult =      0.5*multiplier;
    multiplier = 2.0*multiplier;
  }
  int tk=(int) floor(m_min/omult +0.5);

  lower  = tk*omult;
  nTicks=int(((m_max-lower)/multiplier) + 1.05);
  
  if (nTicks<50) {
    for (int i=0;i<nTicks;i++) {
      double sub = lower+i*multiplier;
      //if (fabs(xValue)/xMultiplier < 1.0E-6) xValue=0;
      if (sub>=m_min && sub<=m_max)   {
	m_subdivision.push_back(sub);
	
	std::ostringstream label;
	label << sub;
	QString qstr = label.str().c_str();
	m_subdivision.back().label()->setPlainText(qstr);
      }
      
    }
  }
  else {
  
    {
      m_subdivision.push_back(m_min);
      
      std::ostringstream label;
      label << m_min;
      QString qstr = label.str().c_str();
      m_subdivision.back().label()->setPlainText(qstr);
    }
    {
      m_subdivision.push_back(m_max);
      
      std::ostringstream label;
      label << m_max;
      QString qstr = label.str().c_str();
      m_subdivision.back().label()->setPlainText(qstr);
    }
  }
}

// Get the validity of each subdivision:
bool LinearRangeDivider::isValid(int i) const {
  const RangeDivision & rd=getSubdivision(i);
  return (rd.x()>=m_min && rd.x()<=m_max);
}

