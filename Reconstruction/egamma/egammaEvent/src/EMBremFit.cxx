/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 /********************************************************************

NAME:     EMBremFit.cxx
PACKAGE:  offline/Reconstruction/egammaEvent

AUTHORS:  Anne-Catherine Le Bihan
CREATED:  March 2006

PURPOSE:  object containing information about Bremstrahlung fit

********************************************************************/

// INCLUDE HEADER FILES:

#include <math.h>
#include <iomanip>
#include <iostream>
#include "egammaEvent/EMBremFit.h"
#include "GaudiKernel/GaudiException.h"
#include "SGTools/ClassName.h"
#include "SGTools/BaseInfo.h"

//  END OF HEADER FILES INCLUDE

// ----------------------------
//  Constructor
// ----------------------------

// give default values
EMBremFit::EMBremFit()
  :	egDetail()
{ }	

// destructor
EMBremFit::~EMBremFit()
{ }

// interfaces
std::string EMBremFit::s_className = ClassName<EMBremFit>::name();
const std::string& EMBremFit::className() const
{
  return s_className;
}

double EMBremFit::parameter(egammaParameters::ParamDef key) const {

  if ( hasIntParameter(key) ) {
    return (double) intParameter(key);
  }

  typedef std::pair<egammaParameters::ParamDef,double> elParams;

  double result = egammaParameters::EgParamUndefined;
  std::vector<elParams>::const_iterator p = m_parameters.begin();
 
  for (;p !=m_parameters.end(); p++) {
    if ( (*p).first == key ) {
      result = (*p).second;
      break;
    }
  }

  return result;
}

void EMBremFit::set_parameter(egammaParameters::ParamDef key, double value, bool overwrite) {

  if ( hasIntParameter(key) ) {
    set_parameterInt(key,(int)value,overwrite);
  }

  typedef std::pair<egammaParameters::ParamDef,double> elParams;

  std::vector<elParams>::iterator p = m_parameters.begin();
 
  for (;p !=m_parameters.end(); p++) {
    if ( (*p).first == key ) break;
  }

  if ( p == m_parameters.end() ) {
    m_parameters.push_back( elParams(key,value) );
  }
  else {
    if ( overwrite ) {
      (*p).second = value;
    }
    else {      
      throw GaudiException("parameter not saved", "EMBremFit::set_parameter(...)", StatusCode::FAILURE);
    }
  }

}


int EMBremFit::intParameter(egammaParameters::ParamDef key) const {


  typedef std::pair<egammaParameters::ParamDef,int> elParams;

  int result = (int)egammaParameters::EgParamUndefined;
  std::vector<elParams>::const_iterator p = m_parametersInt.begin();
 
  for (;p !=m_parametersInt.end(); p++) {
    if ( (*p).first == key ){
      result = (*p).second;
      break;
    }
  }

  return result;
}

void EMBremFit::set_parameterInt(egammaParameters::ParamDef key, int value, bool overwrite) {

  typedef std::pair<egammaParameters::ParamDef,int> elParams;

  std::vector<elParams>::iterator p = m_parametersInt.begin();
 
  for (;p !=m_parametersInt.end(); p++) {
    if ( (*p).first == key ) break;
  }

  if ( p == m_parametersInt.end() ) {
    m_parametersInt.push_back( elParams(key,value) );
  }
  else {
    if ( overwrite ) {
      (*p).second = value;
    }
    else {      
      throw GaudiException("parameter not saved", "EMBremFit::set_parameter(...)", StatusCode::FAILURE);
    }
  }

}


// print track match quantities

void 
EMBremFit::print() const
{
   std::cout << " EMBremFit : "<< parameter(egammaParameters::bremD0)    << std::endl;
   std::cout << " EMBremFit : "<< parameter(egammaParameters::bremPhi0)  << std::endl;
   std::cout << " EMBremFit : "<< parameter(egammaParameters::bremInvpT) << std::endl;
   std::cout << " EMBremFit : "<< parameter(egammaParameters::bremZ0)    << std::endl;
   std::cout << " EMBremFit : "<< parameter(egammaParameters::bremDzDr)  << std::endl;
   std::cout << " EMBremFit : "<< parameter(egammaParameters::bremRadius)<< std::endl;
   std::cout << " EMBremFit : "<< parameter(egammaParameters::bremX)     << std::endl;
   std::cout << " " << std::endl;
       
   std::cout << " EMBremInput : fit status : "<< intParameter(egammaParameters::bremFitStatus) << std::endl;
  
}
// return the values of the different variables in the EMBremFit object

float EMBremFit::bremD0    ()  const { return  parameter(egammaParameters::bremD0)                ;}
float EMBremFit::bremPhi0  ()  const { return  parameter(egammaParameters::bremPhi0)              ;}
float EMBremFit::bremInvpT ()  const { return  parameter(egammaParameters::bremInvpT)             ;}
float EMBremFit::bremZ0    ()  const { return  parameter(egammaParameters::bremZ0)	           ;}
float EMBremFit::bremDzDr  ()  const { return  parameter(egammaParameters::bremDzDr)              ;}
float EMBremFit::bremRadius()  const { return  parameter(egammaParameters::bremRadius)            ;}
float EMBremFit::bremX     ()  const { return  parameter(egammaParameters::bremX)	           ;}

float EMBremFit::bremInvpTerr() const { return  parameter(egammaParameters::bremInvpTerr)         ;}

int EMBremFit::bremNTRT()  const {return intParameter(egammaParameters::bremNTRT)  ;}
int EMBremFit::bremNR  ()  const {return intParameter(egammaParameters::bremNR)    ;}
int EMBremFit::bremNZ  ()  const {return intParameter(egammaParameters::bremNZ)    ;}

float EMBremFit::bremClusterRadius()  const {return parameter(egammaParameters::bremClusterRadius) ;}

int EMBremFit::bremFitStatus   ()  const { return  intParameter(egammaParameters::bremFitStatus)    ;}
int EMBremFit::bremErrorFlag   ()  const { return  intParameter(egammaParameters::bremErrorFlag)    ;}
float EMBremFit::bremFitChi2     ()  const { return  parameter(egammaParameters::bremFitChi2)      ;} 
int EMBremFit::linkIndex   ()  const { return  intParameter(egammaParameters::linkIndex)    ;} 
  
// set values for the different variables in the EMBremFit object

void EMBremFit::BremD0    (float x)  { set_parameter(egammaParameters::bremD0, x, true) ;}
void EMBremFit::BremPhi0  (float x)  { set_parameter(egammaParameters::bremPhi0, x, true)  ;}
void EMBremFit::BremInvpT (float x)  { set_parameter(egammaParameters::bremInvpT, x, true)  ;}
void EMBremFit::BremZ0    (float x)  { set_parameter(egammaParameters::bremZ0, x, true)  ;}
void EMBremFit::BremDzDr  (float x)  { set_parameter(egammaParameters::bremDzDr, x, true)  ;}
void EMBremFit::BremRadius(float x)  { set_parameter(egammaParameters::bremRadius, x, true)  ;}
void EMBremFit::BremX     (float x)  { set_parameter(egammaParameters::bremX, x, true)  ;}

void EMBremFit::BremInvpTerr(float x){ set_parameter(egammaParameters::bremInvpTerr, x, true) ;}

void EMBremFit::BremNTRT(int x)  { set_parameterInt(egammaParameters::bremNTRT, x, true) ;}
void EMBremFit::BremNR  (int x)  { set_parameterInt(egammaParameters::bremNR, x, true) ;}
void EMBremFit::BremNZ  (int x)  { set_parameterInt(egammaParameters::bremNZ, x, true) ;}

void EMBremFit::BremClusterRadius (float x)  { set_parameter(egammaParameters::bremClusterRadius, x, true)  ;}

void EMBremFit::BremFitStatus   (int x)	 { set_parameterInt(egammaParameters::bremFitStatus, x, true)  ;}
void EMBremFit::BremErrorFlag   (int x)	 { set_parameterInt(egammaParameters::bremErrorFlag, x, true)  ;}
void EMBremFit::BremFitChi2     (float x)   { set_parameter(egammaParameters::bremFitChi2, x, true)  ;}
void EMBremFit::set_linkIndex   (int x)	 { set_parameterInt(egammaParameters::linkIndex, x, true)  ;}
   
bool EMBremFit::hasParameter(egammaParameters::ParamDef key) const {
  if ( hasIntParameter(key) ) return true;
  if (key == egammaParameters::bremD0)	      return true;
  if (key == egammaParameters::bremPhi0)      return true;
  if (key == egammaParameters::bremInvpT)     return true;
  if (key == egammaParameters::bremZ0)	      return true;    
  if (key == egammaParameters::bremDzDr)      return true;
  if (key == egammaParameters::bremRadius)    return true;
  if (key == egammaParameters::bremX)	      return true;
  if (key == egammaParameters::bremInvpTerr)  return true;
  if (key == egammaParameters::bremClusterRadius) return true;
  if (key == egammaParameters::bremFitChi2)       return true;
  return false;		       
}    

bool EMBremFit::hasIntParameter(egammaParameters::ParamDef key) const {
  if (key == egammaParameters::bremNR)	      return true;
  if (key == egammaParameters::bremNZ)	      return true;
  if (key == egammaParameters::bremNTRT)      return true;
  if (key == egammaParameters::bremFitStatus)     return true;    
  if (key == egammaParameters::bremErrorFlag)     return true;
  if (key == egammaParameters::linkIndex)         return true;
  return false;
}


SG_ADD_BASE (EMBremFit,     egDetail);
