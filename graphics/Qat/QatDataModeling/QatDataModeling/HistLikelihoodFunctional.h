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


//-------------------------------------------------------------//
//                                                             //
// This functional returns the -2.0*log(L)  of a function      //
// with respect to some histogram.                             //
//                                                             //
//-------------------------------------------------------------//
#ifndef _HistLikelihoodFunctional_h_
#define _HistLikelihoodFunctional_h_
#include "CLHEP/GenericFunctions/AbsFunctional.hh"
class Hist1D;
class HistLikelihoodFunctional:public Genfun::AbsFunctional {

public:
  
  // Constructor:
  HistLikelihoodFunctional(const Hist1D * histogram, 
		     double lower=-1E100, 
		     double upper= 1E100, 
		     bool integrate=false);

  // Destructor:
  ~HistLikelihoodFunctional();

  // Copy constructor
  HistLikelihoodFunctional(const HistLikelihoodFunctional &);

  // Evaluate ChiSquared of a function w.r.t the histogram
  virtual double operator [] (const Genfun::AbsFunction & function) const;

private:
  
  const Hist1D       *_histogram;  // does not own this histogram.
  double              _minValue;   // min bin value for fit.
  double              _maxValue;   // max bin value for fit.
  bool                _integrate;  // integration flag.

  // These are caches for Histogram-like quantities. They are 
  // held here because they are expensive to extract from the
  // histogram:

  int   _nBins;
  float _min;
  float _max;
  float _delta;
  float _width;
  
  
  // Assignment is illegal:
  const HistLikelihoodFunctional & operator= (const HistLikelihoodFunctional &);


};
#endif


