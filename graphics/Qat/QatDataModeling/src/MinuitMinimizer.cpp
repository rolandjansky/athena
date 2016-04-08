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


#include "CLHEP/GenericFunctions/AbsFunctional.hh"
#include "CLHEP/GenericFunctions/Parameter.hh"
#include "QatDataModeling/MinuitMinimizer.h"
#include "QatDataModeling/ObjectiveFunction.h"
#include "QatDataModeling/minuit.h"
#include <stdexcept>
typedef void   (*minuitCallback) (int    & npar,           // which par is changing?
				  double * gradient,       // my gradient calculation
				  double & functionValue,  // the function value
				  double * xValue,         // parameters
				  int    & iFlag,          // what to do flag.
				  void   * );              // unused pointer


extern "C" void mninit_ (int &, int &, int &);
extern "C" void mnseti_ (char * string, int size);
extern "C" void mnparm_ (int &, const char *, double &, double &, double &, double &, int &, int);
extern "C" void mncomd_ (minuitCallback, char *, int &, void *, int);
extern "C" void mnpout_ (int &, const char *, double &, double &, double &, double &, int &, int);
extern "C" void mnstat_ (double &, double &, double &, int &, int &, int &);
extern "C" void mnemat_ (double *, int &); 
std::vector<const Genfun::AbsFunctional *>          *MinuitMinimizer::_worldWideFunctionalList     =0;
std::vector<const Genfun::AbsFunction  *>          *MinuitMinimizer::_worldWideFunctionList        =0;
std::vector<const ObjectiveFunction *>          *MinuitMinimizer::_worldWideObjectiveList      =0;
std::vector<Genfun::Parameter *>                    *MinuitMinimizer::_worldWideParameterList      =0;
bool MINUITERROR=false;
MinuitMinimizer::MinuitMinimizer(bool verbose)
  :_fcnMin(0.0),
   _status(0),
   _verbose(verbose),
   _minimizeCommand("MINIMIZE")
{
}

void MinuitMinimizer::addParameter(Genfun::Parameter * par) {

  // add the parameter to the list:
  _parList.push_back(par);

  // resize the error matrix:
  _errorList = CLHEP::HepSymMatrix(_parList.size(),0);
  
  // create a new Vector, longer.
  CLHEP::HepVector tmp(_parList.size());
  
  // Copy data there, adding one more datum:
  for (int i=0;i<_valList.num_row();i++) tmp[i]=_valList[i];
  tmp[_valList.num_row()]=par->getValue();

  // And set the value list equal to that.
  _valList=tmp;
  
}


MinuitMinimizer::~MinuitMinimizer() {
}
void MinuitMinimizer::minimize() {
  if ( ((_functionList.size()==0) || (_functionalList.size()==0) ) && _objectiveList.size()==0 )return;
  unsigned int DIM=_parList.size();
  //
  // Initialize:
  //
  int status;
  int fInputUnit=0,fOutputUnit=0,fSaveUnit=0;
  const char * title = "Embedded minimizer";
  mninit_(fInputUnit,fOutputUnit, fSaveUnit);
  mnseti_((char *) title, sizeof(title));
  if (!_verbose) {
    const char *printLevelCommand="SET PRINT -1";
    mncomd_(_worldWideCallback,(char *) printLevelCommand, status, 0,12 );
  }
  // 
  // Declare all of the parameters:
  //
  for (unsigned int i=0; i<DIM;i++) {
    int number = i+1;
    double startingValue =_parList[i]->getValue();
    double step          = (_parList[i]->getUpperLimit()-_parList[i]->getLowerLimit())/100.0;
    double bnd1          =_parList[i]->getLowerLimit();
    double bnd2          =_parList[i]->getUpperLimit();
    int error;
    mnparm_ ( number, 
	      _parList[i]->getName().c_str(), 
	      startingValue,
	      step,
	      bnd1,
	      bnd2,
	      error,
	      _parList[i]->getName().size());
  }
  //
  // Flush the world-wide-buffer and minimize this functional w.r.t its parameters.
  //
  _worldWideFunctionalList    = &_functionalList;
  _worldWideFunctionList      = &_functionList;
  _worldWideObjectiveList     = &_objectiveList;
  _worldWideParameterList     = &_parList;

  // Special case.  If there are no parameters make one FCN call and return;
  if (_parList.size()==0) {
    int zeroLength=0;
    int iPar=4;
    (*_worldWideCallback)(zeroLength,(double *) NULL,_fcnMin, (double *) NULL, iPar, (void *) NULL);
    if (MINUITERROR) {
      MINUITERROR=false;
      throw std::runtime_error("MINUIT ERROR (INFINITE LIKELIHOOD?) ");
    }
    return;
  }
  const char *minimizeCommand=_minimizeCommand.c_str();
  
  mncomd_(_worldWideCallback,(char *) minimizeCommand, status, 0, sizeof(minimizeCommand));
  if (MINUITERROR) {
    MINUITERROR=false;
    throw std::runtime_error("MINUIT ERROR (INFINITE LIKELIHOOD?) ");
  }
  for (unsigned int i=0;i<DIM;i++) {
    char name[11];
    int I=i+1, iinternal=0;
    double value=0, error=0, bnd1=0, bnd2=0;
    mnpout_(I,name,value, error, bnd1, bnd2, iinternal, 10);
    name[10]=0;
    _valList[i]=value;
  }

  // Get function minimum:
  double estDistance, errdef;
  int npari, nparx;
  mnstat_(_fcnMin, estDistance, errdef, npari, nparx, _status);

  double *emat = new double [DIM*DIM];
  int IDIM=DIM;
  mnemat_(emat,IDIM);
  for (unsigned int i=0;i<DIM;i++) {
    for (unsigned int j=0;j<DIM;j++) {
      _errorList[i][j]=emat[i*DIM+j];
    }
  }
  delete [] emat;

  // Do this so that nobody else can screw it up:
  _worldWideFunctionalList    = 0;
  _worldWideFunctionList      = 0;
  _worldWideObjectiveList     = 0;
  _worldWideParameterList     = 0;
}

void MinuitMinimizer::addStatistic(const Genfun::AbsFunctional * functional, const Genfun::AbsFunction * function) {
  _functionalList.push_back(functional);
  _functionList.push_back(function);
}

void MinuitMinimizer::addStatistic (const ObjectiveFunction *objective) {
  _objectiveList.push_back(objective);
}


void MinuitMinimizer::_worldWideCallback(int    & npar,            // which par is changing?
					   double * /*gradient*/,      // my gradient calculation
					   double & functionValue, // the function value
					   double * xValue,        // parameters
					   int    & iFlag,         // what to do flag.
					   void   * /*vp*/)
{
  switch (iFlag) {
    
  case 4: // value of 4:
    for (int i=0;i<npar;i++) {
      (*_worldWideParameterList)[i]->setValue(xValue[i]);
    }
    functionValue=0;
    for (unsigned int j=0;j<_worldWideFunctionalList->size();j++) {
      try {
	functionValue += (*(*_worldWideFunctionalList)[j])[(*(*_worldWideFunctionList)[j])];
      }
      catch (const std::exception & e) {
	std::cout << e.what() <<std::endl;
	for (unsigned int p=0;p<(*_worldWideParameterList).size();p++) std::cerr << *(*_worldWideParameterList)[p] << std::endl;
	MINUITERROR=true;  //THROW EXCEPTION.
      }
    }
    for (unsigned int j=0;j<_worldWideObjectiveList->size();j++) {
      functionValue += (*(*_worldWideObjectiveList)[j])();
    }
    // for debug:
    break;
  default:
    std::cerr << "Warning.  _worldWideCallback called back with flag of " << iFlag << std::endl;
    break;
  }
}


// Get the parameter value:
double MinuitMinimizer::getValue(const Genfun::Parameter *par) const {
  for (unsigned int i=0;i<_parList.size();i++) {
    if (_parList[i]==par) {
      return _valList[i];
    }
  }
  std::cerr << "Error in MinuitMinimizer::getValue.  Parameter not found" << std::endl;
  return 0;
}

// Get the parameter error:
double MinuitMinimizer::getError(const Genfun::Parameter *ipar, const Genfun::Parameter *jpar) const {
  
  int iIndex=-1, jIndex=-1;
  
  for (unsigned int i=0;i<_parList.size();i++) {
    if (_parList[i]==ipar) {
      iIndex=i;
    }
  }
  
  if (iIndex!=-1) {
    if (jpar) {
      for (unsigned int j=0;j<_parList.size();j++) {
	if (_parList[j]==jpar) {
	  jIndex=j;
	}
      }
    }
    else {
      jIndex=iIndex;
    }
  }
  
  if (iIndex !=-1 && jIndex!=-1) return _errorList[iIndex][jIndex];
  std::cerr << "Error in MinuitMinimizer::getError.  Parameter not found" << std::endl;
  return 0;
}


double MinuitMinimizer::getFunctionValue() const {
  return _fcnMin;
}

int MinuitMinimizer::getStatus() const {
  return _status;
}

CLHEP::HepVector MinuitMinimizer::getValues() const {
  return _valList;
}

CLHEP::HepSymMatrix MinuitMinimizer::getErrorMatrix()  const {
  return _errorList;
}


unsigned int MinuitMinimizer::getNumParameters() const {
  return _parList.size();
}

Genfun::Parameter * MinuitMinimizer::getParameter(unsigned int i)  {
  return _parList[i];
}

const Genfun::Parameter * MinuitMinimizer::getParameter(unsigned int i) const {
  return _parList[i];
}
void MinuitMinimizer::setMinimizeCommand(const std::string & command) {
  _minimizeCommand=command;
}
  
