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
#include <cstring>
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
std::vector<const Genfun::AbsFunctional *>          *MinuitMinimizer::s_worldWideFunctionalList     =0;
std::vector<const Genfun::AbsFunction  *>          *MinuitMinimizer::s_worldWideFunctionList        =0;
std::vector<const ObjectiveFunction *>          *MinuitMinimizer::s_worldWideObjectiveList      =0;
std::vector<Genfun::Parameter *>                    *MinuitMinimizer::s_worldWideParameterList      =0;
bool MINUITERROR=false;
MinuitMinimizer::MinuitMinimizer(bool verbose)
  :m_fcnMin(0.0),
   m_status(0),
   m_verbose(verbose),
   m_minimizeCommand("MINIMIZE")
{
}

void MinuitMinimizer::addParameter(Genfun::Parameter * par) {

  // add the parameter to the list:
  m_parList.push_back(par);

  // resize the error matrix:
  m_errorList = CLHEP::HepSymMatrix(m_parList.size(),0);
  
  // create a new Vector, longer.
  CLHEP::HepVector tmp(m_parList.size());
  
  // Copy data there, adding one more datum:
  for (int i=0;i<m_valList.num_row();i++) tmp[i]=m_valList[i];
  tmp[m_valList.num_row()]=par->getValue();

  // And set the value list equal to that.
  m_valList=tmp;
  
}


MinuitMinimizer::~MinuitMinimizer() {
}
void MinuitMinimizer::minimize() {
  if ( ((m_functionList.size()==0) || (m_functionalList.size()==0) ) && m_objectiveList.size()==0 )return;
  unsigned int DIM=m_parList.size();
  //
  // Initialize:
  //
  int status;
  int fInputUnit=0,fOutputUnit=0,fSaveUnit=0;
  const char * title = "Embedded minimizer";
  mninit_(fInputUnit,fOutputUnit, fSaveUnit);
  mnseti_((char *) title, std::strlen(title));
  if (!m_verbose) {
    const char *printLevelCommand="SET PRINT -1";
    mncomd_(s_worldWideCallback,(char *) printLevelCommand, status, 0,12 );
  }
  // 
  // Declare all of the parameters:
  //
  for (unsigned int i=0; i<DIM;i++) {
    int number = i+1;
    double startingValue =m_parList[i]->getValue();
    double step          = (m_parList[i]->getUpperLimit()-m_parList[i]->getLowerLimit())/100.0;
    double bnd1          =m_parList[i]->getLowerLimit();
    double bnd2          =m_parList[i]->getUpperLimit();
    int error;
    mnparm_ ( number, 
	      m_parList[i]->getName().c_str(), 
	      startingValue,
	      step,
	      bnd1,
	      bnd2,
	      error,
	      m_parList[i]->getName().size());
  }
  //
  // Flush the world-wide-buffer and minimize this functional w.r.t its parameters.
  //
  s_worldWideFunctionalList    = &m_functionalList;
  s_worldWideFunctionList      = &m_functionList;
  s_worldWideObjectiveList     = &m_objectiveList;
  s_worldWideParameterList     = &m_parList;

  // Special case.  If there are no parameters make one FCN call and return;
  if (m_parList.size()==0) {
    int zeroLength=0;
    int iPar=4;
    (*s_worldWideCallback)(zeroLength,(double *) NULL,m_fcnMin, (double *) NULL, iPar, (void *) NULL);
    if (MINUITERROR) {
      MINUITERROR=false;
      throw std::runtime_error("MINUIT ERROR (INFINITE LIKELIHOOD?) ");
    }
    return;
  }
  const char *minimizeCommand=m_minimizeCommand.c_str();
  
  mncomd_(s_worldWideCallback,(char *) minimizeCommand, status, 0, std::strlen(minimizeCommand));
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
    m_valList[i]=value;
  }

  // Get function minimum:
  double estDistance, errdef;
  int npari, nparx;
  mnstat_(m_fcnMin, estDistance, errdef, npari, nparx, m_status);

  double *emat = new double [DIM*DIM];
  int IDIM=DIM;
  mnemat_(emat,IDIM);
  for (unsigned int i=0;i<DIM;i++) {
    for (unsigned int j=0;j<DIM;j++) {
      m_errorList[i][j]=emat[i*DIM+j];
    }
  }
  delete [] emat;

  // Do this so that nobody else can screw it up:
  s_worldWideFunctionalList    = 0;
  s_worldWideFunctionList      = 0;
  s_worldWideObjectiveList     = 0;
  s_worldWideParameterList     = 0;
}

void MinuitMinimizer::addStatistic(const Genfun::AbsFunctional * functional, const Genfun::AbsFunction * function) {
  m_functionalList.push_back(functional);
  m_functionList.push_back(function);
}

void MinuitMinimizer::addStatistic (const ObjectiveFunction *objective) {
  m_objectiveList.push_back(objective);
}


void MinuitMinimizer::s_worldWideCallback(int    & npar,            // which par is changing?
					   double * /*gradient*/,      // my gradient calculation
					   double & functionValue, // the function value
					   double * xValue,        // parameters
					   int    & iFlag,         // what to do flag.
					   void   * /*vp*/)
{
  switch (iFlag) {
    
  case 4: // value of 4:
    for (int i=0;i<npar;i++) {
      (*s_worldWideParameterList)[i]->setValue(xValue[i]);
    }
    functionValue=0;
    for (unsigned int j=0;j<s_worldWideFunctionalList->size();j++) {
      try {
	functionValue += (*(*s_worldWideFunctionalList)[j])[(*(*s_worldWideFunctionList)[j])];
      }
      catch (const std::exception & e) {
	std::cout << e.what() <<std::endl;
	for (unsigned int p=0;p<(*s_worldWideParameterList).size();p++) std::cerr << *(*s_worldWideParameterList)[p] << std::endl;
	MINUITERROR=true;  //THROW EXCEPTION.
      }
    }
    for (unsigned int j=0;j<s_worldWideObjectiveList->size();j++) {
      functionValue += (*(*s_worldWideObjectiveList)[j])();
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
  for (unsigned int i=0;i<m_parList.size();i++) {
    if (m_parList[i]==par) {
      return m_valList[i];
    }
  }
  std::cerr << "Error in MinuitMinimizer::getValue.  Parameter not found" << std::endl;
  return 0;
}

// Get the parameter error:
double MinuitMinimizer::getError(const Genfun::Parameter *ipar, const Genfun::Parameter *jpar) const {
  
  int iIndex=-1, jIndex=-1;
  
  for (unsigned int i=0;i<m_parList.size();i++) {
    if (m_parList[i]==ipar) {
      iIndex=i;
    }
  }
  
  if (iIndex!=-1) {
    if (jpar) {
      for (unsigned int j=0;j<m_parList.size();j++) {
	if (m_parList[j]==jpar) {
	  jIndex=j;
	}
      }
    }
    else {
      jIndex=iIndex;
    }
  }
  
  if (iIndex !=-1 && jIndex!=-1) return m_errorList[iIndex][jIndex];
  std::cerr << "Error in MinuitMinimizer::getError.  Parameter not found" << std::endl;
  return 0;
}


double MinuitMinimizer::getFunctionValue() const {
  return m_fcnMin;
}

int MinuitMinimizer::getStatus() const {
  return m_status;
}

CLHEP::HepVector MinuitMinimizer::getValues() const {
  return m_valList;
}

CLHEP::HepSymMatrix MinuitMinimizer::getErrorMatrix()  const {
  return m_errorList;
}


unsigned int MinuitMinimizer::getNumParameters() const {
  return m_parList.size();
}

Genfun::Parameter * MinuitMinimizer::getParameter(unsigned int i)  {
  return m_parList[i];
}

const Genfun::Parameter * MinuitMinimizer::getParameter(unsigned int i) const {
  return m_parList[i];
}
void MinuitMinimizer::setMinimizeCommand(const std::string & command) {
  m_minimizeCommand=command;
}
  
