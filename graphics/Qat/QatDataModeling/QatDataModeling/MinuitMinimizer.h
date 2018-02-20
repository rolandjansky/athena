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


// Write this class now.  Abstract it later.
#ifndef QATDATAMODELING_MINUITMINIMIZER_H
#define QATDATAMODELING_MINUITMINIMIZER_H
#include <vector>
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"

namespace Genfun {
  class AbsFunctional;
  class Parameter;
  class AbsFunction;
}

class ObjectiveFunction;

class MinuitMinimizer {
  
public:
  
  // Constructor:
  MinuitMinimizer                 (bool verbose=false);

  // Destructor:
  virtual ~MinuitMinimizer                ();

  // Add a parameter.  Minuit minimizes the statistic w.r.t the parameters.
  virtual void addParameter       (Genfun::Parameter * par);

  // Add a statistic:  a function, and a functional (e.g. Chisqaured, Likelihood)  
  void addStatistic               (const Genfun::AbsFunctional * functional, const Genfun::AbsFunction * function);

  // Add a statistic:  a (Gaussian) constraint or similar function of parameters.
  void addStatistic               (const ObjectiveFunction *);

  // Tell minuit to minimize this:
  virtual void minimize           ();

  // Get the parameter value:
  double getValue(const Genfun::Parameter *) const;

  // Get the parameter error:
  double getError(const Genfun::Parameter *ipar, const Genfun::Parameter *jpar=NULL) const;

  // Get the functionValue
  double getFunctionValue() const;

  // Get the status
  int getStatus() const;

  // Get all of the values, as a vector:
  CLHEP::HepVector  getValues() const;
  
  // Get all of the errors, as a matrix:
  CLHEP::HepSymMatrix getErrorMatrix() const;

  // Get information on parameters:
  unsigned int getNumParameters() const;

  // Get Parameter:
  const Genfun::Parameter *getParameter( unsigned int i) const;

  // Get Parameter
  Genfun::Parameter * getParameter(unsigned int i);

  // Set the minimizer command (nominally "MINIMIZE").
  void setMinimizeCommand(const std::string & command);
  
private:

  std::vector<const Genfun::AbsFunctional *> m_functionalList;
  std::vector<const Genfun::AbsFunction   *> m_functionList;
  std::vector<const ObjectiveFunction     *> m_objectiveList;
  std::vector<Genfun::Parameter *>           m_parList;
  CLHEP::HepVector                           m_valList;
  CLHEP::HepSymMatrix                        m_errorList;
  double                                     m_fcnMin;
  int                                        m_status;
  bool                                       m_verbose;
  std::string                                m_minimizeCommand;

  static void                                      s_worldWideCallback(int &, double *, double &, double *, int &, void *);
  static std::vector<const Genfun::AbsFunctional *>       *s_worldWideFunctionalList;
  static std::vector<const Genfun::AbsFunction *>         *s_worldWideFunctionList;
  static std::vector<const ObjectiveFunction   *>         *s_worldWideObjectiveList;
  static std::vector<Genfun::Parameter *>                 *s_worldWideParameterList;
 
    
};

class HistogramManager;

struct MinuitMeasurement {
  double value;
  double error;
};

// This records the state of the minimizer in an output directory:
void record(HistogramManager *output, const MinuitMinimizer & minimizer);

// This retrieves a single measurement:
MinuitMeasurement getMeasurement(const HistogramManager *manager, const std::string & name);

#endif
