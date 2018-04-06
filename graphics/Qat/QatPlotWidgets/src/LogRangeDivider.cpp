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


#include "QatPlotWidgets/LogRangeDivider.h"
#include <cmath>
#include <sstream>
#include <algorithm>
inline double ciel(double x) { 
  double fx = floor(x);
  if (x==fx) return x;
  else return x+1.0;
}
LogRangeDivider::LogRangeDivider():
  m_min(0.0),m_max(1.0)
{
}

LogRangeDivider::~LogRangeDivider() {
}



// Set the range:
void LogRangeDivider::setRange(double min, double max){

  // This protection insures 
  if (min==max) return;
  m_min=std::min(min,max);
  m_max=std::max(min,max);

  if (m_max<0) {
    m_max=1;
    m_min=0.1;
  }
  if (m_min<0) {
    m_min=m_max/10.0;
  }
  
  recompute();




}

// Set the range:
void LogRangeDivider::setMin(double min){
  setRange(min,m_max);
}

// Set the range:
void LogRangeDivider::setMax(double max){
  setRange(m_min,max);
}


// Get the number of subdivisions:
int LogRangeDivider::getNumSubdivisions() const{
  return m_subdivision.size();
}

// Get the location of each subdivision:
const RangeDivision & LogRangeDivider::getSubdivision(int i) const{
  return m_subdivision[i];
}

void LogRangeDivider::recompute() {
  // Clean out old subdivisions:
  m_subdivision.erase(m_subdivision.begin(),m_subdivision.end());

  
  double exponentMin = floor(log10(m_min));
  double multMin = pow(10,exponentMin);
  unsigned int NMin = (unsigned int) (m_min/multMin+1);
  if (NMin==10) {
    NMin=1;
    multMin*=10;
  }

  double nDecades = log10(m_max/m_min);
  static unsigned int valueSet3[]={1};
  static unsigned int valueSet2[]={1,5};
  static unsigned int valueSet1[]={1,2,5};
  static unsigned int valueSet0[]={1,2,3,4,5,6,7,8,9};
  static unsigned int *valueSet=NULL;
  unsigned int valueSetSize=0;
  unsigned int nJump = (unsigned int) (nDecades/8 + 1);
  if     (nDecades<1) {
    valueSet=valueSet0;
    valueSetSize=sizeof(valueSet0)/sizeof(int);
  }
  else if (nDecades<2) {
    valueSet=valueSet1;
    valueSetSize=sizeof(valueSet1)/sizeof(int);
  }
  else if (nDecades<3) {
    valueSet=valueSet2;
    valueSetSize=sizeof(valueSet2)/sizeof(int);
  }
  else {
    valueSet=valueSet3;
    valueSetSize=sizeof(valueSet3)/sizeof(int);
  }

  while (1) {

    if (std::find(valueSet,valueSet+valueSetSize,NMin)!=valueSet+valueSetSize) {
      m_subdivision.push_back(NMin*multMin);
      std::ostringstream label;
      label << NMin*multMin;
      QString qstr = label.str().c_str();
      m_subdivision.back().label()->setPlainText(qstr);
    }

    NMin++;
    if (NMin==10) {
      NMin=1;
      multMin*=pow(10,nJump);
    }
    if (NMin*multMin > m_max) break;
  }

}

// Get the validity of each subdivision:
bool LogRangeDivider::isValid(int i) const {
  const RangeDivision & rd=getSubdivision(i);
  return (rd.x()>=m_min && rd.x()<=m_max);
}

//if (fabs(xValue)/xMultiplier < 1.0E-6) xValue=0;
