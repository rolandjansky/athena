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


#include "QatDataModeling/HistLikelihoodFunctional.h"
#include "QatDataAnalysis/Hist1D.h"
#include "CLHEP/GenericFunctions/DefiniteIntegral.hh"
#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <math.h>

HistLikelihoodFunctional::HistLikelihoodFunctional(const Hist1D * histogram,
				       double minValue,
				       double maxValue,
				       bool integrate):
  m_histogram(histogram),
  m_minValue(minValue),
  m_maxValue(maxValue),
  m_integrate(integrate),
  m_nBins(m_histogram->nBins()),
  m_min(m_histogram->min()),
  m_max(m_histogram->max()),
  m_width(m_histogram->binWidth())
{
  m_delta=(m_max-m_min)/m_nBins;
}

HistLikelihoodFunctional::~HistLikelihoodFunctional() {
}

HistLikelihoodFunctional::HistLikelihoodFunctional (const HistLikelihoodFunctional & right):
  Genfun::AbsFunctional(),
  m_histogram(right.m_histogram),
  m_minValue(right.m_minValue),
  m_maxValue(right.m_maxValue),
  m_integrate(right.m_integrate),
  m_nBins(right.m_nBins),
  m_min(right.m_min),
  m_max(right.m_max),
  m_delta(right.m_delta),
  m_width(right.m_width)
{
}


double HistLikelihoodFunctional::operator[] (const Genfun::AbsFunction & function) const{
  double logLik=0;
  for (int i = 0; i<m_nBins;i++) {
    double a = m_min +i*m_delta;
    double b = a+m_delta;
    double y = m_histogram->bin(i);
    double e = m_histogram->binError(i);
    if (e!=0) {
      if (b>m_minValue && a < m_maxValue) {
        double y0;
        if (m_integrate) {
          Genfun::DefiniteIntegral integral(a,b);
          y0=integral[function];
	  if (!finite(y0)) {
	    std::ostringstream message;
	    message << "HistLikelihoodFunctional, Function is infinite at " << (a+b)/2.0 << std::endl;
	    throw std::runtime_error(message.str());
	  }

        }
        else {
          y0 = function ((a+b)/2.0)*m_histogram->binWidth();
	  if (!finite(y0)) {
	    std::ostringstream message;
	    message << "HistLikelihoodFunctional, Function is infinite at " << (a+b)/2.0 << std::endl;
	    throw std::runtime_error(message.str());
	  }
        }
        logLik += -2.0*(-y0 + y*log(y0));
	if (!finite(logLik)) {
	  std::ostringstream message;
	  message << "HistLikelihoodFunctional, Likelihood is infinite at " << (a+b)/2.0 << std::endl;
	  message << "Value of point is " << y0 << std::endl;
	  throw std::runtime_error(message.str());
	}
      }
    } 
  }
  return logLik;
}

