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
#ifndef _RemoteMinimizer_h_
#define _RemoteMinimizer_h_
#include <vector>
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"

namespace Genfun {
  class AbsFunctional;
  class Parameter;
  class AbsFunction;
}

class ObjectiveFunction;

template <class Q> class RemoteMinimizer {
  
public:
  
  // Constructor:
  RemoteMinimizer                 (Q *query, bool verbose=false);

  // Destructor:
  ~RemoteMinimizer                ();

  // Add a parameter.  Minuit minimizes the statistic w.r.t the parameters.
  void addParameter       (Genfun::Parameter * par, double * parLoc);

  // Add a socket, listening for Q
  void addSocket(ClientSocket *client);

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


  
private:

  class Response {
  public:
    int Status;
    double FCN;
  };


  
  Q                                          *query;
  Response                                    response;

  std::vector<const ObjectiveFunction     *> _objectiveList;
  std::vector<Genfun::Parameter *>           _parList;
  std::vector<double *>                      _parLocList;
  std::vector<ClientSocket *>                _socketList;
  CLHEP::HepVector                           _valList;
  CLHEP::HepSymMatrix                        _errorList;
  double                                     _fcnMin;
  int                                        _status;
  bool                                       _verbose;
 
  static void                                _worldWideCallback(int &, double *, double &, double *, int &, void *);
  static RemoteMinimizer                     *This;

};

#include "QatDataModeling/RemoteMinimizer.icc"

#endif
