/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EMConvert.cxx
PACKAGE:  offline/Reconstruction/egammaEvent

AUTHORS:  David Joffe
CREATED:  Oct. 5, 2005

PURPOSE:  object containing information about conversions
UPDATED:  Jul 30, 2010 (JM) Add ambiguity result

********************************************************************/

// INCLUDE HEADER FILES:

#include <math.h>

#include "egammaEvent/EMConvert.h"

#include "GaudiKernel/GaudiException.h"
#include "SGTools/ClassName.h"

#include "CLHEP/Matrix/SymMatrix.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/ExtendedVxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/RecVertex.h"
#include "TrkParameters/TrackParameters.h"
#include "SGTools/BaseInfo.h"


using CLHEP::Hep3Vector;


//  END OF HEADER FILES INCLUDE

// ----------------------------
//  Constructor
// ----------------------------

// give default values
EMConvert::EMConvert()
  :	egDetail()
{ }

// destructor
EMConvert::~EMConvert()
{ }

bool EMConvert::hasBoolParameter(egammaParameters::ParamDef key) const {
  if (key == egammaParameters::convTrackMatch)          return true;
  if (key == egammaParameters::convAngleMatch)          return true;
  return false;		       
}    

bool EMConvert::hasIntParameter(egammaParameters::ParamDef key) const {
  if (key == egammaParameters::refittedVertex_ndf)            return true;
  if (key == egammaParameters::refittedVertex_numTracks)      return true;
  if (key == egammaParameters::refittedVertex_track1_author)  return true;
  if (key == egammaParameters::refittedVertex_track2_author)  return true;
  if (key == egammaParameters::ambiguityResult)               return true;
  if (key == egammaParameters::linkIndex)                     return true;
  return false;		       
}    

bool EMConvert::hasParameter(egammaParameters::ParamDef key) const {
  if(hasBoolParameter(key))                                      return true;
  if(hasIntParameter(key))                                       return true;
  if (key == egammaParameters::refittedVertex_x)                 return true;
  if (key == egammaParameters::refittedVertex_y)                 return true;
  if (key == egammaParameters::refittedVertex_z)                 return true;
  if (key == egammaParameters::refittedVertex_Covxx)             return true;
  if (key == egammaParameters::refittedVertex_Covxy)             return true;
  if (key == egammaParameters::refittedVertex_Covxz)             return true;
  if (key == egammaParameters::refittedVertex_Covyy)             return true;
  if (key == egammaParameters::refittedVertex_Covyz)             return true;
  if (key == egammaParameters::refittedVertex_Covzz)             return true;
  if (key == egammaParameters::refittedVertex_chi2)              return true;
  if (key == egammaParameters::refittedVertex_track1_phi)        return true;
  if (key == egammaParameters::refittedVertex_track1_theta)      return true;
  if (key == egammaParameters::refittedVertex_track1_qOverP)     return true;
  if (key == egammaParameters::refittedVertex_track1_chi2)       return true;
  if (key == egammaParameters::refittedVertex_track2_phi)        return true;
  if (key == egammaParameters::refittedVertex_track2_theta)      return true;
  if (key == egammaParameters::refittedVertex_track2_qOverP)     return true;
  if (key == egammaParameters::refittedVertex_track2_chi2)       return true;
  if (key == egammaParameters::refittedVertex_Covd01d01)         return true;
  if (key == egammaParameters::refittedVertex_Covd01z01)         return true;
  if (key == egammaParameters::refittedVertex_Covd01phi1)        return true;
  if (key == egammaParameters::refittedVertex_Covd01theta1)      return true;
  if (key == egammaParameters::refittedVertex_Covd01qOverP1)     return true;
  if (key == egammaParameters::refittedVertex_Covz01z01)         return true;
  if (key == egammaParameters::refittedVertex_Covz01phi1)        return true;
  if (key == egammaParameters::refittedVertex_Covz01theta1)      return true;
  if (key == egammaParameters::refittedVertex_Covz01qOverP1)     return true;
  if (key == egammaParameters::refittedVertex_Covphi1phi1)       return true;
  if (key == egammaParameters::refittedVertex_Covphi1theta1)     return true;
  if (key == egammaParameters::refittedVertex_Covphi1qOverP1)    return true;
  if (key == egammaParameters::refittedVertex_Covtheta1theta1)   return true;
  if (key == egammaParameters::refittedVertex_Covtheta1qOverP1)  return true;
  if (key == egammaParameters::refittedVertex_CovqOverP1qOverP1) return true;
  if (key == egammaParameters::refittedVertex_Covphi1phi2)       return true;
  if (key == egammaParameters::refittedVertex_Covphi1theta2)     return true;
  if (key == egammaParameters::refittedVertex_Covphi1qOverP2)    return true;
  if (key == egammaParameters::refittedVertex_Covtheta1phi2)     return true;
  if (key == egammaParameters::refittedVertex_Covtheta1theta2)   return true;
  if (key == egammaParameters::refittedVertex_Covtheta1qOverP2)  return true;
  if (key == egammaParameters::refittedVertex_CovqOverP1phi2)    return true;
  if (key == egammaParameters::refittedVertex_CovqOverP1theta2)  return true;
  if (key == egammaParameters::refittedVertex_CovqOverP1qOverP2) return true;
  if (key == egammaParameters::refittedVertex_Covphi1x)          return true;
  if (key == egammaParameters::refittedVertex_Covphi1y)          return true;
  if (key == egammaParameters::refittedVertex_Covphi1z)          return true;
  if (key == egammaParameters::refittedVertex_Covtheta1x)        return true;
  if (key == egammaParameters::refittedVertex_Covtheta1y)        return true;
  if (key == egammaParameters::refittedVertex_Covtheta1z)        return true;
  if (key == egammaParameters::refittedVertex_CovqOverP1x)       return true;
  if (key == egammaParameters::refittedVertex_CovqOverP1y)       return true;
  if (key == egammaParameters::refittedVertex_CovqOverP1z)       return true;
  if (key == egammaParameters::refittedVertex_Covd02d02)         return true;
  if (key == egammaParameters::refittedVertex_Covd02z02)         return true;
  if (key == egammaParameters::refittedVertex_Covd02phi2)        return true;
  if (key == egammaParameters::refittedVertex_Covd02theta2)      return true;
  if (key == egammaParameters::refittedVertex_Covd02qOverP2)     return true;
  if (key == egammaParameters::refittedVertex_Covz02z02)         return true;
  if (key == egammaParameters::refittedVertex_Covz02phi2)        return true;
  if (key == egammaParameters::refittedVertex_Covz02theta2)      return true;
  if (key == egammaParameters::refittedVertex_Covz02qOverP2)     return true;
  if (key == egammaParameters::refittedVertex_Covphi2phi2)       return true;
  if (key == egammaParameters::refittedVertex_Covphi2theta2)     return true;
  if (key == egammaParameters::refittedVertex_Covphi2qOverP2)    return true;
  if (key == egammaParameters::refittedVertex_Covtheta2theta2)   return true;
  if (key == egammaParameters::refittedVertex_Covtheta2qOverP2)  return true;
  if (key == egammaParameters::refittedVertex_CovqOverP2qOverP2) return true;
  if (key == egammaParameters::refittedVertex_Covphi2x)          return true;
  if (key == egammaParameters::refittedVertex_Covphi2y)          return true;
  if (key == egammaParameters::refittedVertex_Covphi2z)          return true;
  if (key == egammaParameters::refittedVertex_Covtheta2x)        return true;
  if (key == egammaParameters::refittedVertex_Covtheta2y)        return true;
  if (key == egammaParameters::refittedVertex_Covtheta2z)        return true;
  if (key == egammaParameters::refittedVertex_CovqOverP2x)       return true;
  if (key == egammaParameters::refittedVertex_CovqOverP2y)       return true;
  if (key == egammaParameters::refittedVertex_CovqOverP2z)       return true;
  if (key == egammaParameters::convMatchDeltaEta1)               return true;
  if (key == egammaParameters::convMatchDeltaPhi1)               return true;
  if (key == egammaParameters::convMatchDeltaEta2)               return true;
  if (key == egammaParameters::convMatchDeltaPhi2)               return true;
  if (key == egammaParameters::convMatchDeltaEta1_refit)         return true;
  if (key == egammaParameters::convMatchDeltaPhi1_refit)         return true;
  if (key == egammaParameters::convMatchDeltaEta2_refit)         return true;
  if (key == egammaParameters::convMatchDeltaPhi2_refit)         return true;
  return false;		       
}    

//interfaces
const std::string& EMConvert::className() const
{
  static const std::string s_name = ClassName<EMConvert>::name();
  return s_name;
}

double EMConvert::parameter(egammaParameters::ParamDef key) const {

  if ( !hasParameter(key) )      
    return egammaParameters::EgParamUndefined;

  if(hasBoolParameter(key)){
    if ( boolParameter(key)) return 1.;
    return 0;
  }

  if ( hasIntParameter(key) ) {
    return (double) intParameter(key);
  }

  double result = egammaParameters::EgParamUndefined;
  typedef std::pair<egammaParameters::ParamDef,double> elParams;
  std::vector<elParams>::const_iterator p = m_parameters.begin();
 
  for (;p !=m_parameters.end(); p++) {
    if ( (*p).first == key ) {
      result = (*p).second;
      break;
    }
  }
  return result;
}

bool EMConvert::boolParameter(egammaParameters::ParamDef key) const {

  typedef std::pair<egammaParameters::ParamDef,bool> elParams;

  bool result = false;
  std::vector<elParams>::const_iterator p = m_parametersBool.begin();
 
  for (;p !=m_parametersBool.end(); p++) {
    if ( (*p).first == key ) {
      result = (*p).second;
      break;
    }
  }
  return result;
}

int EMConvert::intParameter(egammaParameters::ParamDef key) const {


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


void EMConvert::set_parameter(egammaParameters::ParamDef key, double value, bool overwrite) {

  if ( hasBoolParameter(key) ) {
    if ( value != 0. ) 
      set_parameterBool(key,true,overwrite);
    else
      set_parameterBool(key,false,overwrite);    
  }

  if ( hasIntParameter(key) ) {
    set_parameterInt(key,(int)value,overwrite);
  }


  typedef std::pair<egammaParameters::ParamDef,double> elParams;
  std::vector<elParams>::iterator p = m_parameters.begin();
 
  for (;p !=m_parameters.end(); p++) {
    if ( (*p).first == key ) break;
  }

  if ( p == m_parameters.end() ) {
    m_parameters.push_back(std::pair<egammaParameters::ParamDef,double>(key,value));
  }
  else {
    if ( overwrite ) {
      (*p).second = value;
    }
    else {      
      throw GaudiException("parameter not saved", "EMConvert::set_parameter(...)", StatusCode::FAILURE);
    }
  }
}


void EMConvert::set_parameterBool(egammaParameters::ParamDef key, bool value, bool overwrite) {

  typedef std::pair<egammaParameters::ParamDef,bool> elParams;

  std::vector<elParams>::iterator p = m_parametersBool.begin();
 
  for (;p !=m_parametersBool.end(); p++) {
    if ( (*p).first == key ) break;
  }

  if ( p == m_parametersBool.end() ) {
    m_parametersBool.push_back( elParams(key,value) );
  }
  else {
    if ( overwrite ) {
      (*p).second = value;
    }
    else {      
      throw GaudiException("parameter not saved", "EMConvert::set_parameter(...)", StatusCode::FAILURE);
    }
  }
}


void EMConvert::set_parameterInt(egammaParameters::ParamDef key, int value, bool overwrite) {

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
      throw GaudiException("parameter not saved", "EMConvert::set_parameter(...)", StatusCode::FAILURE);
    }
  }
}


// print track match quantities

void 
EMConvert::print() const
{
   std::cout << " EMConvert: convtrackmatch            "<< boolParameter(egammaParameters::convTrackMatch) << std::endl;
   std::cout << " EMConvert: convanglematch            "<< boolParameter(egammaParameters::convAngleMatch) << std::endl;
   std::cout << " EMConvert: convMatchDeltaEta1        "<< parameter(egammaParameters::convMatchDeltaEta1) << std::endl;
   std::cout << " EMConvert: convMatchDeltaPhi1        "<< parameter(egammaParameters::convMatchDeltaPhi1) << std::endl;
   std::cout << " EMConvert: convMatchDeltaEta2        "<< parameter(egammaParameters::convMatchDeltaEta2) << std::endl;
   std::cout << " EMConvert: convMatchDeltaPhi2        "<< parameter(egammaParameters::convMatchDeltaPhi2) << std::endl;
   std::cout << " EMConvert: convMatchDeltaEta1_refit  "<< parameter(egammaParameters::convMatchDeltaEta1_refit) << std::endl;
   std::cout << " EMConvert: convMatchDeltaPhi1_refit  "<< parameter(egammaParameters::convMatchDeltaPhi1_refit) << std::endl;
   std::cout << " EMConvert: convMatchDeltaEta2_refit  "<< parameter(egammaParameters::convMatchDeltaEta2_refit) << std::endl;
   std::cout << " EMConvert: convMatchDeltaPhi2_refit  "<< parameter(egammaParameters::convMatchDeltaPhi2_refit) << std::endl;
   std::cout << " EMConvert: vertex_x                  "<< parameter(egammaParameters::refittedVertex_x) << std::endl;
   std::cout << " EMConvert: vertex_y                  "<< parameter(egammaParameters::refittedVertex_y) << std::endl;
   std::cout << " EMConvert: vertex_z                  "<< parameter(egammaParameters::refittedVertex_z) << std::endl;
   std::cout << " EMConvert: vertex_Covxx              "<< parameter(egammaParameters::refittedVertex_Covxx) << std::endl;
   std::cout << " EMConvert: vertex_Covxy              "<< parameter(egammaParameters::refittedVertex_Covxy) << std::endl;
   std::cout << " EMConvert: vertex_Covxz              "<< parameter(egammaParameters::refittedVertex_Covxz) << std::endl;
   std::cout << " EMConvert: vertex_Covyy              "<< parameter(egammaParameters::refittedVertex_Covyy) << std::endl;
   std::cout << " EMConvert: vertex_Covyz              "<< parameter(egammaParameters::refittedVertex_Covyz) << std::endl;
   std::cout << " EMConvert: vertex_Covzz              "<< parameter(egammaParameters::refittedVertex_Covzz) << std::endl;
   std::cout << " EMConvert: vertex_chi2               "<< parameter(egammaParameters::refittedVertex_chi2) << std::endl;
   std::cout << " EMConvert: vertex_ndf                "<< intParameter(egammaParameters::refittedVertex_ndf) << std::endl;
   std::cout << " EMConvert: vertex_numTracks          "<< intParameter(egammaParameters::refittedVertex_numTracks) << std::endl;
   std::cout << " EMConvert: vertex_track1_phi         "<< parameter(egammaParameters::refittedVertex_track1_phi) << std::endl;
   std::cout << " EMConvert: vertex_track1_theta       "<< parameter(egammaParameters::refittedVertex_track1_theta) << std::endl;
   std::cout << " EMConvert: vertex_track1_qOverP      "<< parameter(egammaParameters::refittedVertex_track1_qOverP) << std::endl;
   std::cout << " EMConvert: vertex_track1_chi2        "<< parameter(egammaParameters::refittedVertex_track1_chi2) << std::endl;
   std::cout << " EMConvert: vertex_track1_author      "<< intParameter(egammaParameters::refittedVertex_track1_author) << std::endl;
   std::cout << " EMConvert: vertex_track2_phi         "<< parameter(egammaParameters::refittedVertex_track2_phi) << std::endl;
   std::cout << " EMConvert: vertex_track2_theta       "<< parameter(egammaParameters::refittedVertex_track2_theta) << std::endl;
   std::cout << " EMConvert: vertex_track2_qOverP      "<< parameter(egammaParameters::refittedVertex_track2_qOverP) << std::endl;
   std::cout << " EMConvert: vertex_track2_chi2        "<< parameter(egammaParameters::refittedVertex_track2_chi2) << std::endl;
   std::cout << " EMConvert: vertex_track2_author      "<< intParameter(egammaParameters::refittedVertex_track2_author) << std::endl;
   std::cout << " EMConvert: vertex_Covd01d01         "<< parameter(egammaParameters::refittedVertex_Covd01d01) << std::endl;
   std::cout << " EMConvert: vertex_Covd01z01         "<< parameter(egammaParameters::refittedVertex_Covd01z01) << std::endl;
   std::cout << " EMConvert: vertex_Covd01phi1        "<< parameter(egammaParameters::refittedVertex_Covd01phi1) << std::endl;
   std::cout << " EMConvert: vertex_Covd01theta1      "<< parameter(egammaParameters::refittedVertex_Covd01theta1) << std::endl;
   std::cout << " EMConvert: vertex_Covd01qOverP1     "<< parameter(egammaParameters::refittedVertex_Covd01qOverP1) << std::endl;
   std::cout << " EMConvert: vertex_Covz01z01         "<< parameter(egammaParameters::refittedVertex_Covz01z01) << std::endl;
   std::cout << " EMConvert: vertex_Covz01phi1        "<< parameter(egammaParameters::refittedVertex_Covz01phi1) << std::endl;
   std::cout << " EMConvert: vertex_Covz01theta1      "<< parameter(egammaParameters::refittedVertex_Covz01theta1) << std::endl;
   std::cout << " EMConvert: vertex_Covz01qOverP1     "<< parameter(egammaParameters::refittedVertex_Covz01qOverP1) << std::endl;
   std::cout << " EMConvert: vertex_Covphi1phi1       "<< parameter(egammaParameters::refittedVertex_Covphi1phi1) << std::endl;
   std::cout << " EMConvert: vertex_Covphi1theta1     "<< parameter(egammaParameters::refittedVertex_Covphi1theta1) << std::endl;
   std::cout << " EMConvert: vertex_Covphi1qOverP1    "<< parameter(egammaParameters::refittedVertex_Covphi1qOverP1) << std::endl;
   std::cout << " EMConvert: vertex_Covtheta1theta1   "<< parameter(egammaParameters::refittedVertex_Covtheta1theta1) << std::endl;
   std::cout << " EMConvert: vertex_Covtheta1qOverP1  "<< parameter(egammaParameters::refittedVertex_Covtheta1qOverP1) << std::endl;
   std::cout << " EMConvert: vertex_CovqOverP1qoverP1 "<< parameter(egammaParameters::refittedVertex_CovqOverP1qOverP1) << std::endl;
   std::cout << " EMConvert: vertex_Covphi1phi2       "<< parameter(egammaParameters::refittedVertex_Covphi1phi2) << std::endl;
   std::cout << " EMConvert: vertex_Covphi1heta2      "<< parameter(egammaParameters::refittedVertex_Covphi1theta2) << std::endl;
   std::cout << " EMConvert: vertex_Covphi1qOverP2    "<< parameter(egammaParameters::refittedVertex_Covphi1qOverP2) << std::endl;
   std::cout << " EMConvert: vertex_Covtheta1phi2     "<< parameter(egammaParameters::refittedVertex_Covtheta1phi2) << std::endl;
   std::cout << " EMConvert: vertex_Covtheta1theta2   "<< parameter(egammaParameters::refittedVertex_Covtheta1theta2) << std::endl;
   std::cout << " EMConvert: vertex_Covtheta1qOverP2  "<< parameter(egammaParameters::refittedVertex_Covtheta1qOverP2) << std::endl;
   std::cout << " EMConvert: vertex_CovqOverP1phi2    "<< parameter(egammaParameters::refittedVertex_CovqOverP1phi2) << std::endl;
   std::cout << " EMConvert: vertex_CovqOverP1theta2  "<< parameter(egammaParameters::refittedVertex_CovqOverP1theta2) << std::endl;
   std::cout << " EMConvert: vertex_CovqOverP1qOverP2 "<< parameter(egammaParameters::refittedVertex_CovqOverP1qOverP2) << std::endl;
   std::cout << " EMConvert: vertex_Covphi1x          "<< parameter(egammaParameters::refittedVertex_Covphi1x) << std::endl;
   std::cout << " EMConvert: vertex_Covphi1y          "<< parameter(egammaParameters::refittedVertex_Covphi1y) << std::endl;
   std::cout << " EMConvert: vertex_Covphi1z          "<< parameter(egammaParameters::refittedVertex_Covphi1z) << std::endl;
   std::cout << " EMConvert: vertex_Covtheta1x        "<< parameter(egammaParameters::refittedVertex_Covtheta1x) << std::endl;
   std::cout << " EMConvert: vertex_Covtheta1y        "<< parameter(egammaParameters::refittedVertex_Covtheta1y) << std::endl;
   std::cout << " EMConvert: vertex_Covtheta1z        "<< parameter(egammaParameters::refittedVertex_Covtheta1z) << std::endl;
   std::cout << " EMConvert: vertex_CovqOverP1x       "<< parameter(egammaParameters::refittedVertex_CovqOverP1x) << std::endl;
   std::cout << " EMConvert: vertex_CovqOverP1y       "<< parameter(egammaParameters::refittedVertex_CovqOverP1y) << std::endl;
   std::cout << " EMConvert: vertex_CovqOverP1z       "<< parameter(egammaParameters::refittedVertex_CovqOverP1z) << std::endl;
   std::cout << " EMConvert: vertex_Covd02d02         "<< parameter(egammaParameters::refittedVertex_Covd02d02) << std::endl;
   std::cout << " EMConvert: vertex_Covd02z02         "<< parameter(egammaParameters::refittedVertex_Covd02z02) << std::endl;
   std::cout << " EMConvert: vertex_Covd02phi2        "<< parameter(egammaParameters::refittedVertex_Covd02phi2) << std::endl;
   std::cout << " EMConvert: vertex_Covd02theta2      "<< parameter(egammaParameters::refittedVertex_Covd02theta2) << std::endl;
   std::cout << " EMConvert: vertex_Covd02qOverP2     "<< parameter(egammaParameters::refittedVertex_Covd02qOverP2) << std::endl;
   std::cout << " EMConvert: vertex_Covz02z02         "<< parameter(egammaParameters::refittedVertex_Covz02z02) << std::endl;
   std::cout << " EMConvert: vertex_Covz02phi2        "<< parameter(egammaParameters::refittedVertex_Covz02phi2) << std::endl;
   std::cout << " EMConvert: vertex_Covz02theta2      "<< parameter(egammaParameters::refittedVertex_Covz02theta2) << std::endl;
   std::cout << " EMConvert: vertex_Covz02qOverP2     "<< parameter(egammaParameters::refittedVertex_Covz02qOverP2) << std::endl;
   std::cout << " EMConvert: vertex_Covphi2phi2       "<< parameter(egammaParameters::refittedVertex_Covphi2phi2) << std::endl;
   std::cout << " EMConvert: vertex_Covphi2theta2     "<< parameter(egammaParameters::refittedVertex_Covphi2theta2) << std::endl;
   std::cout << " EMConvert: vertex_Covphi2qOverP2    "<< parameter(egammaParameters::refittedVertex_Covphi2qOverP2) << std::endl;
   std::cout << " EMConvert: vertex_Covtheta2theta2   "<< parameter(egammaParameters::refittedVertex_Covtheta2theta2) << std::endl;
   std::cout << " EMConvert: vertex_Covtheta2qOverP2  "<< parameter(egammaParameters::refittedVertex_Covtheta2qOverP2) << std::endl;
   std::cout << " EMConvert: vertex_CovqOverP2qOverP2 "<< parameter(egammaParameters::refittedVertex_CovqOverP2qOverP2) << std::endl;
   std::cout << " EMConvert: vertex_Covphi2x          "<< parameter(egammaParameters::refittedVertex_Covphi2x) << std::endl;
   std::cout << " EMConvert: vertex_Covphi2y          "<< parameter(egammaParameters::refittedVertex_Covphi2y) << std::endl;
   std::cout << " EMConvert: vertex_Covphi2z          "<< parameter(egammaParameters::refittedVertex_Covphi2z) << std::endl;
   std::cout << " EMConvert: vertex_Covtheta2x        "<< parameter(egammaParameters::refittedVertex_Covtheta2x) << std::endl;
   std::cout << " EMConvert: vertex_Covtheta2y        "<< parameter(egammaParameters::refittedVertex_Covtheta2y) << std::endl;
   std::cout << " EMConvert: vertex_Covtheta2z        "<< parameter(egammaParameters::refittedVertex_Covtheta2z) << std::endl;
   std::cout << " EMConvert: vertex_CovqOverP2x       "<< parameter(egammaParameters::refittedVertex_CovqOverP2x) << std::endl;
   std::cout << " EMConvert: vertex_CovqOverP2y       "<< parameter(egammaParameters::refittedVertex_CovqOverP2y) << std::endl;
   std::cout << " EMConvert: vertex_CovqOverP2z       "<< parameter(egammaParameters::refittedVertex_CovqOverP2z) << std::endl;
   std::cout << " EMConvert: ambiguityResult          "<< intParameter(egammaParameters::ambiguityResult) << std::endl;
   std::cout << " EMConvert: linkIndex                "<< intParameter(egammaParameters::linkIndex) << std::endl;
}



// return values for the different variables in the EMConvert object

bool  EMConvert::convTrackMatch()   const { return boolParameter(egammaParameters::convTrackMatch)  ;}
bool  EMConvert::convAngleMatch()   const { return boolParameter(egammaParameters::convAngleMatch)  ;}
double  EMConvert::convMatchDeltaEta1()   const { return parameter(egammaParameters::convMatchDeltaEta1)  ;}
double  EMConvert::convMatchDeltaPhi1()   const { return parameter(egammaParameters::convMatchDeltaPhi1)  ;}
double  EMConvert::convMatchDeltaEta2()   const { return parameter(egammaParameters::convMatchDeltaEta2)  ;}
double  EMConvert::convMatchDeltaPhi2()   const { return parameter(egammaParameters::convMatchDeltaPhi2)  ;}
double  EMConvert::convMatchDeltaEta1_refit()   const { return parameter(egammaParameters::convMatchDeltaEta1_refit)  ;}
double  EMConvert::convMatchDeltaPhi1_refit()   const { return parameter(egammaParameters::convMatchDeltaPhi1_refit)  ;}
double  EMConvert::convMatchDeltaEta2_refit()   const { return parameter(egammaParameters::convMatchDeltaEta2_refit)  ;}
double  EMConvert::convMatchDeltaPhi2_refit()   const { return parameter(egammaParameters::convMatchDeltaPhi2_refit)  ;}
double  EMConvert::vertex_x()   const { return parameter(egammaParameters::refittedVertex_x);}
double  EMConvert::vertex_y()   const { return parameter(egammaParameters::refittedVertex_y);}
double  EMConvert::vertex_z()   const { return parameter(egammaParameters::refittedVertex_z);}
double  EMConvert::vertex_Covxx()   const { return parameter(egammaParameters::refittedVertex_Covxx);}
double  EMConvert::vertex_Covxy()   const { return parameter(egammaParameters::refittedVertex_Covxy);}
double  EMConvert::vertex_Covxz()   const { return parameter(egammaParameters::refittedVertex_Covxz);}
double  EMConvert::vertex_Covyy()   const { return parameter(egammaParameters::refittedVertex_Covyy);}
double  EMConvert::vertex_Covyz()   const { return parameter(egammaParameters::refittedVertex_Covyz);}
double  EMConvert::vertex_Covzz()   const { return parameter(egammaParameters::refittedVertex_Covzz);}
double  EMConvert::vertex_chi2()   const { return parameter(egammaParameters::refittedVertex_chi2);}
int  EMConvert::vertex_ndf()   const { return intParameter(egammaParameters::refittedVertex_ndf);}
int  EMConvert::vertex_numTracks()   const { return intParameter(egammaParameters::refittedVertex_numTracks);}
double  EMConvert::vertex_track1_phi()   const { return parameter(egammaParameters::refittedVertex_track1_phi);}
double  EMConvert::vertex_track1_theta()   const { return parameter(egammaParameters::refittedVertex_track1_theta);}
double  EMConvert::vertex_track1_qOverP()   const { return parameter(egammaParameters::refittedVertex_track1_qOverP);}
double  EMConvert::vertex_track1_chi2()   const { return parameter(egammaParameters::refittedVertex_track1_chi2);}
int  EMConvert::vertex_track1_author()   const { return intParameter(egammaParameters::refittedVertex_track1_author);}
double  EMConvert::vertex_track2_phi()   const { return parameter(egammaParameters::refittedVertex_track2_phi);}
double  EMConvert::vertex_track2_theta()   const { return parameter(egammaParameters::refittedVertex_track2_theta);}
double  EMConvert::vertex_track2_qOverP()   const { return parameter(egammaParameters::refittedVertex_track2_qOverP);}
double  EMConvert::vertex_track2_chi2()   const { return parameter(egammaParameters::refittedVertex_track2_chi2);}
int  EMConvert::vertex_track2_author()   const { return intParameter(egammaParameters::refittedVertex_track2_author);}
double EMConvert::vertex_Covd01d01() const { return parameter(egammaParameters::refittedVertex_Covd01d01);}
double EMConvert::vertex_Covd01z01() const { return parameter(egammaParameters::refittedVertex_Covd01z01);}
double EMConvert::vertex_Covd01phi1() const { return parameter(egammaParameters::refittedVertex_Covd01phi1);}
double EMConvert::vertex_Covd01theta1() const { return parameter(egammaParameters::refittedVertex_Covd01theta1);}
double EMConvert::vertex_Covd01qOverP1() const { return parameter(egammaParameters::refittedVertex_Covd01qOverP1);}
double EMConvert::vertex_Covz01z01() const { return parameter(egammaParameters::refittedVertex_Covz01z01);}
double EMConvert::vertex_Covz01phi1() const { return parameter(egammaParameters::refittedVertex_Covz01phi1);}
double EMConvert::vertex_Covz01theta1() const { return parameter(egammaParameters::refittedVertex_Covz01theta1);}
double EMConvert::vertex_Covz01qOverP1() const { return parameter(egammaParameters::refittedVertex_Covz01qOverP1);}
double EMConvert::vertex_Covphi1phi1() const { return parameter(egammaParameters::refittedVertex_Covphi1phi1);}
double EMConvert::vertex_Covphi1theta1() const { return parameter(egammaParameters::refittedVertex_Covphi1theta1);}
double EMConvert::vertex_Covphi1qOverP1() const { return parameter(egammaParameters::refittedVertex_Covphi1qOverP1);}
double EMConvert::vertex_Covtheta1theta1() const { return parameter(egammaParameters::refittedVertex_Covtheta1theta1);}
double EMConvert::vertex_Covtheta1qOverP1() const { return parameter(egammaParameters::refittedVertex_Covtheta1qOverP1);}
double EMConvert::vertex_CovqOverP1qOverP1() const { return parameter(egammaParameters::refittedVertex_CovqOverP1qOverP1);}
double EMConvert::vertex_Covphi1phi2() const { return parameter(egammaParameters::refittedVertex_Covphi1phi2);}
double EMConvert::vertex_Covphi1theta2() const { return parameter(egammaParameters::refittedVertex_Covphi1theta2);}
double EMConvert::vertex_Covphi1qOverP2() const { return parameter(egammaParameters::refittedVertex_Covphi1qOverP2);}
double EMConvert::vertex_Covtheta1phi2() const { return parameter(egammaParameters::refittedVertex_Covtheta1phi2);}
double EMConvert::vertex_Covtheta1theta2() const { return parameter(egammaParameters::refittedVertex_Covtheta1theta2);}
double EMConvert::vertex_Covtheta1qOverP2() const { return parameter(egammaParameters::refittedVertex_Covtheta1qOverP2);}
double EMConvert::vertex_CovqOverP1phi2() const { return parameter(egammaParameters::refittedVertex_CovqOverP1phi2);}
double EMConvert::vertex_CovqOverP1theta2() const { return parameter(egammaParameters::refittedVertex_CovqOverP1theta2);}
double EMConvert::vertex_CovqOverP1qOverP2() const { return parameter(egammaParameters::refittedVertex_CovqOverP1qOverP2);}
double EMConvert::vertex_Covphi1x() const { return parameter(egammaParameters::refittedVertex_Covphi1x);}
double EMConvert::vertex_Covphi1y() const { return parameter(egammaParameters::refittedVertex_Covphi1y);}
double EMConvert::vertex_Covphi1z() const { return parameter(egammaParameters::refittedVertex_Covphi1z);}
double EMConvert::vertex_Covtheta1x() const { return parameter(egammaParameters::refittedVertex_Covtheta1x);}
double EMConvert::vertex_Covtheta1y() const { return parameter(egammaParameters::refittedVertex_Covtheta1y);}
double EMConvert::vertex_Covtheta1z() const { return parameter(egammaParameters::refittedVertex_Covtheta1z);}
double EMConvert::vertex_CovqOverP1x() const { return parameter(egammaParameters::refittedVertex_CovqOverP1x);}
double EMConvert::vertex_CovqOverP1y() const { return parameter(egammaParameters::refittedVertex_CovqOverP1y);}
double EMConvert::vertex_CovqOverP1z() const { return parameter(egammaParameters::refittedVertex_CovqOverP1z);}
double EMConvert::vertex_Covd02d02() const { return parameter(egammaParameters::refittedVertex_Covd02d02);}
double EMConvert::vertex_Covd02z02() const { return parameter(egammaParameters::refittedVertex_Covd02z02);}
double EMConvert::vertex_Covd02phi2() const { return parameter(egammaParameters::refittedVertex_Covd02phi2);}
double EMConvert::vertex_Covd02theta2() const { return parameter(egammaParameters::refittedVertex_Covd02theta2);}
double EMConvert::vertex_Covd02qOverP2() const { return parameter(egammaParameters::refittedVertex_Covd02qOverP2);}
double EMConvert::vertex_Covz02z02() const { return parameter(egammaParameters::refittedVertex_Covz02z02);}
double EMConvert::vertex_Covz02phi2() const { return parameter(egammaParameters::refittedVertex_Covz02phi2);}
double EMConvert::vertex_Covz02theta2() const { return parameter(egammaParameters::refittedVertex_Covz02theta2);}
double EMConvert::vertex_Covz02qOverP2() const { return parameter(egammaParameters::refittedVertex_Covz02qOverP2);}
double EMConvert::vertex_Covphi2phi2() const { return parameter(egammaParameters::refittedVertex_Covphi2phi2);}
double EMConvert::vertex_Covphi2theta2() const { return parameter(egammaParameters::refittedVertex_Covphi2theta2);}
double EMConvert::vertex_Covphi2qOverP2() const { return parameter(egammaParameters::refittedVertex_Covphi2qOverP2);}
double EMConvert::vertex_Covtheta2theta2() const { return parameter(egammaParameters::refittedVertex_Covtheta2theta2);}
double EMConvert::vertex_Covtheta2qOverP2() const { return parameter(egammaParameters::refittedVertex_Covtheta2qOverP2);}
double EMConvert::vertex_CovqOverP2qOverP2() const { return parameter(egammaParameters::refittedVertex_CovqOverP2qOverP2);}
double EMConvert::vertex_Covphi2x() const { return parameter(egammaParameters::refittedVertex_Covphi2x);}
double EMConvert::vertex_Covphi2y() const { return parameter(egammaParameters::refittedVertex_Covphi2y);}
double EMConvert::vertex_Covphi2z() const { return parameter(egammaParameters::refittedVertex_Covphi2z);}
double EMConvert::vertex_Covtheta2x() const { return parameter(egammaParameters::refittedVertex_Covtheta2x);}
double EMConvert::vertex_Covtheta2y() const { return parameter(egammaParameters::refittedVertex_Covtheta2y);}
double EMConvert::vertex_Covtheta2z() const { return parameter(egammaParameters::refittedVertex_Covtheta2z);}
double EMConvert::vertex_CovqOverP2x() const { return parameter(egammaParameters::refittedVertex_CovqOverP2x);}
double EMConvert::vertex_CovqOverP2y() const { return parameter(egammaParameters::refittedVertex_CovqOverP2y);}
double EMConvert::vertex_CovqOverP2z() const { return parameter(egammaParameters::refittedVertex_CovqOverP2z);}
int  EMConvert::ambiguityResult()   const { return intParameter(egammaParameters::ambiguityResult);}
int  EMConvert::linkIndex()   const { return intParameter(egammaParameters::linkIndex);}

// set values for the different variables in the EMConvert object

void  EMConvert::set_convTrackMatch(bool x)  { set_parameterBool(egammaParameters::convTrackMatch, x, true)    ;}
void  EMConvert::set_convAngleMatch(bool x)  { set_parameterBool(egammaParameters::convAngleMatch, x, true)    ;}
void  EMConvert::set_convMatchDeltaEta1(double x)  { set_parameter(egammaParameters::convMatchDeltaEta1, x, true)    ;}
void  EMConvert::set_convMatchDeltaPhi1(double x)  { set_parameter(egammaParameters::convMatchDeltaPhi1, x, true)    ;}
void  EMConvert::set_convMatchDeltaEta2(double x)  { set_parameter(egammaParameters::convMatchDeltaEta2, x, true)    ;}
void  EMConvert::set_convMatchDeltaPhi2(double x)  { set_parameter(egammaParameters::convMatchDeltaPhi2, x, true)    ;}
void  EMConvert::set_convMatchDeltaEta1_refit(double x)  { set_parameter(egammaParameters::convMatchDeltaEta1_refit, x, true)    ;}
void  EMConvert::set_convMatchDeltaPhi1_refit(double x)  { set_parameter(egammaParameters::convMatchDeltaPhi1_refit, x, true)    ;}
void  EMConvert::set_convMatchDeltaEta2_refit(double x)  { set_parameter(egammaParameters::convMatchDeltaEta2_refit, x, true)    ;}
void  EMConvert::set_convMatchDeltaPhi2_refit(double x)  { set_parameter(egammaParameters::convMatchDeltaPhi2_refit, x, true)    ;}
void  EMConvert::set_vertex_x(double x)  { set_parameter(egammaParameters::refittedVertex_x, x, true);}
void  EMConvert::set_vertex_y(double x)  { set_parameter(egammaParameters::refittedVertex_y, x, true);}
void  EMConvert::set_vertex_z(double x)  { set_parameter(egammaParameters::refittedVertex_z, x, true);}
void  EMConvert::set_vertex_Covxx(double x)  { set_parameter(egammaParameters::refittedVertex_Covxx, x, true);}
void  EMConvert::set_vertex_Covxy(double x)  { set_parameter(egammaParameters::refittedVertex_Covxy, x, true);}
void  EMConvert::set_vertex_Covxz(double x)  { set_parameter(egammaParameters::refittedVertex_Covxz, x, true);}
void  EMConvert::set_vertex_Covyy(double x)  { set_parameter(egammaParameters::refittedVertex_Covyy, x, true);}
void  EMConvert::set_vertex_Covyz(double x)  { set_parameter(egammaParameters::refittedVertex_Covyz, x, true);}
void  EMConvert::set_vertex_Covzz(double x)  { set_parameter(egammaParameters::refittedVertex_Covzz, x, true);}
void  EMConvert::set_vertex_chi2(double x)  { set_parameter(egammaParameters::refittedVertex_chi2, x, true);}
void  EMConvert::set_vertex_ndf(int x)  { set_parameterInt(egammaParameters::refittedVertex_ndf, x, true);}
void  EMConvert::set_vertex_numTracks(int x)  { set_parameterInt(egammaParameters::refittedVertex_numTracks, x, true);}
void  EMConvert::set_vertex_track1_phi(double x)  { set_parameter(egammaParameters::refittedVertex_track1_phi, x, true);}
void  EMConvert::set_vertex_track1_theta(double x)  { set_parameter(egammaParameters::refittedVertex_track1_theta, x, true);}
void  EMConvert::set_vertex_track1_qOverP(double x)  { set_parameter(egammaParameters::refittedVertex_track1_qOverP, x, true);}
void  EMConvert::set_vertex_track1_chi2(double x)  { set_parameter(egammaParameters::refittedVertex_track1_chi2, x, true);}
void  EMConvert::set_vertex_track1_author(int x)  { set_parameterInt(egammaParameters::refittedVertex_track1_author, x, true);}
void  EMConvert::set_vertex_track2_phi(double x)  { set_parameter(egammaParameters::refittedVertex_track2_phi, x, true);}
void  EMConvert::set_vertex_track2_theta(double x)  { set_parameter(egammaParameters::refittedVertex_track2_theta, x, true);}
void  EMConvert::set_vertex_track2_qOverP(double x)  { set_parameter(egammaParameters::refittedVertex_track2_qOverP, x, true);}
void  EMConvert::set_vertex_track2_chi2(double x)  { set_parameter(egammaParameters::refittedVertex_track2_chi2, x, true);}
void  EMConvert::set_vertex_track2_author(int x)  { set_parameterInt(egammaParameters::refittedVertex_track2_author, x, true);}
void EMConvert::set_vertex_Covd01d01(double x) { set_parameter(egammaParameters::refittedVertex_Covd01d01, x, true);}
void EMConvert::set_vertex_Covd01z01(double x) { set_parameter(egammaParameters::refittedVertex_Covd01z01, x, true);}
void EMConvert::set_vertex_Covd01phi1(double x) { set_parameter(egammaParameters::refittedVertex_Covd01phi1, x, true);}
void EMConvert::set_vertex_Covd01theta1(double x) { set_parameter(egammaParameters::refittedVertex_Covd01theta1, x, true);}
void EMConvert::set_vertex_Covd01qOverP1(double x) { set_parameter(egammaParameters::refittedVertex_Covd01qOverP1, x, true);}
void EMConvert::set_vertex_Covz01z01(double x) { set_parameter(egammaParameters::refittedVertex_Covz01z01, x, true);}
void EMConvert::set_vertex_Covz01phi1(double x) { set_parameter(egammaParameters::refittedVertex_Covz01phi1, x, true);}
void EMConvert::set_vertex_Covz01theta1(double x) { set_parameter(egammaParameters::refittedVertex_Covz01theta1, x, true);}
void EMConvert::set_vertex_Covz01qOverP1(double x) { set_parameter(egammaParameters::refittedVertex_Covz01qOverP1, x, true);}
void EMConvert::set_vertex_Covphi1phi1(double x) { set_parameter(egammaParameters::refittedVertex_Covphi1phi1, x, true);}
void EMConvert::set_vertex_Covphi1theta1(double x) { set_parameter(egammaParameters::refittedVertex_Covphi1theta1, x, true);}
void EMConvert::set_vertex_Covphi1qOverP1(double x) { set_parameter(egammaParameters::refittedVertex_Covphi1qOverP1, x, true);}
void EMConvert::set_vertex_Covtheta1theta1(double x) { set_parameter(egammaParameters::refittedVertex_Covtheta1theta1, x, true);}
void EMConvert::set_vertex_Covtheta1qOverP1(double x) { set_parameter(egammaParameters::refittedVertex_Covtheta1qOverP1, x, true);}
void EMConvert::set_vertex_CovqOverP1qOverP1(double x) { set_parameter(egammaParameters::refittedVertex_CovqOverP1qOverP1, x, true);}
void EMConvert::set_vertex_Covphi1phi2(double x) { set_parameter(egammaParameters::refittedVertex_Covphi1phi2, x, true);}
void EMConvert::set_vertex_Covphi1theta2(double x) { set_parameter(egammaParameters::refittedVertex_Covphi1theta2, x, true);}
void EMConvert::set_vertex_Covphi1qOverP2(double x) { set_parameter(egammaParameters::refittedVertex_Covphi1qOverP2, x, true);}
void EMConvert::set_vertex_Covtheta1phi2(double x) { set_parameter(egammaParameters::refittedVertex_Covtheta1phi2, x, true);}
void EMConvert::set_vertex_Covtheta1theta2(double x) { set_parameter(egammaParameters::refittedVertex_Covtheta1theta2, x, true);}
void EMConvert::set_vertex_Covtheta1qOverP2(double x) { set_parameter(egammaParameters::refittedVertex_Covtheta1qOverP2, x, true);}
void EMConvert::set_vertex_CovqOverP1phi2(double x) { set_parameter(egammaParameters::refittedVertex_CovqOverP1phi2, x, true);}
void EMConvert::set_vertex_CovqOverP1theta2(double x) { set_parameter(egammaParameters::refittedVertex_CovqOverP1theta2, x, true);}
void EMConvert::set_vertex_CovqOverP1qOverP2(double x) { set_parameter(egammaParameters::refittedVertex_CovqOverP1qOverP2, x, true);}
void EMConvert::set_vertex_Covphi1x(double x) { set_parameter(egammaParameters::refittedVertex_Covphi1x, x, true);}
void EMConvert::set_vertex_Covphi1y(double x) { set_parameter(egammaParameters::refittedVertex_Covphi1y, x, true);}
void EMConvert::set_vertex_Covphi1z(double x) { set_parameter(egammaParameters::refittedVertex_Covphi1z, x, true);}
void EMConvert::set_vertex_Covtheta1x(double x) { set_parameter(egammaParameters::refittedVertex_Covtheta1x, x, true);}
void EMConvert::set_vertex_Covtheta1y(double x) { set_parameter(egammaParameters::refittedVertex_Covtheta1y, x, true);}
void EMConvert::set_vertex_Covtheta1z(double x) { set_parameter(egammaParameters::refittedVertex_Covtheta1z, x, true);}
void EMConvert::set_vertex_CovqOverP1x(double x) { set_parameter(egammaParameters::refittedVertex_CovqOverP1x, x, true);}
void EMConvert::set_vertex_CovqOverP1y(double x) { set_parameter(egammaParameters::refittedVertex_CovqOverP1y, x, true);}
void EMConvert::set_vertex_CovqOverP1z(double x) { set_parameter(egammaParameters::refittedVertex_CovqOverP1z, x, true);}
void EMConvert::set_vertex_Covd02d02(double x) { set_parameter(egammaParameters::refittedVertex_Covd02d02, x, true);}
void EMConvert::set_vertex_Covd02z02(double x) { set_parameter(egammaParameters::refittedVertex_Covd02z02, x, true);}
void EMConvert::set_vertex_Covd02phi2(double x) { set_parameter(egammaParameters::refittedVertex_Covd02phi2, x, true);}
void EMConvert::set_vertex_Covd02theta2(double x) { set_parameter(egammaParameters::refittedVertex_Covd02theta2, x, true);}
void EMConvert::set_vertex_Covd02qOverP2(double x) { set_parameter(egammaParameters::refittedVertex_Covd02qOverP2, x, true);}
void EMConvert::set_vertex_Covz02z02(double x) { set_parameter(egammaParameters::refittedVertex_Covz02z02, x, true);}
void EMConvert::set_vertex_Covz02phi2(double x) { set_parameter(egammaParameters::refittedVertex_Covz02phi2, x, true);}
void EMConvert::set_vertex_Covz02theta2(double x) { set_parameter(egammaParameters::refittedVertex_Covz02theta2, x, true);}
void EMConvert::set_vertex_Covz02qOverP2(double x) { set_parameter(egammaParameters::refittedVertex_Covz02qOverP2, x, true);}
void EMConvert::set_vertex_Covphi2phi2(double x) { set_parameter(egammaParameters::refittedVertex_Covphi2phi2, x, true);}
void EMConvert::set_vertex_Covphi2theta2(double x) { set_parameter(egammaParameters::refittedVertex_Covphi2theta2, x, true);}
void EMConvert::set_vertex_Covphi2qOverP2(double x) { set_parameter(egammaParameters::refittedVertex_Covphi2qOverP2, x, true);}
void EMConvert::set_vertex_Covtheta2theta2(double x) { set_parameter(egammaParameters::refittedVertex_Covtheta2theta2, x, true);}
void EMConvert::set_vertex_Covtheta2qOverP2(double x) { set_parameter(egammaParameters::refittedVertex_Covtheta2qOverP2, x, true);}
void EMConvert::set_vertex_CovqOverP2qOverP2(double x) { set_parameter(egammaParameters::refittedVertex_CovqOverP2qOverP2, x, true);}
void EMConvert::set_vertex_Covphi2x(double x) { set_parameter(egammaParameters::refittedVertex_Covphi2x, x, true);}
void EMConvert::set_vertex_Covphi2y(double x) { set_parameter(egammaParameters::refittedVertex_Covphi2y, x, true);}
void EMConvert::set_vertex_Covphi2z(double x) { set_parameter(egammaParameters::refittedVertex_Covphi2z, x, true);}
void EMConvert::set_vertex_Covtheta2x(double x) { set_parameter(egammaParameters::refittedVertex_Covtheta2x, x, true);}
void EMConvert::set_vertex_Covtheta2y(double x) { set_parameter(egammaParameters::refittedVertex_Covtheta2y, x, true);}
void EMConvert::set_vertex_Covtheta2z(double x) { set_parameter(egammaParameters::refittedVertex_Covtheta2z, x, true);}
void EMConvert::set_vertex_CovqOverP2x(double x) { set_parameter(egammaParameters::refittedVertex_CovqOverP2x, x, true);}
void EMConvert::set_vertex_CovqOverP2y(double x) { set_parameter(egammaParameters::refittedVertex_CovqOverP2y, x, true);}
void EMConvert::set_vertex_CovqOverP2z(double x) { set_parameter(egammaParameters::refittedVertex_CovqOverP2z, x, true);}
void  EMConvert::set_ambiguityResult(int x)  { set_parameterInt(egammaParameters::ambiguityResult, x, true);}
void  EMConvert::set_linkIndex(int x)  { set_parameterInt(egammaParameters::linkIndex, x, true);}


void EMConvert::fillDummy()
{
  set_vertex_x(-999.);
  set_vertex_y(-999.);
  set_vertex_z(-999.);
  set_vertex_Covxx(-999.);
  set_vertex_Covxy(-999.);
  set_vertex_Covxz(-999.);
  set_vertex_Covyy(-999.);
  set_vertex_Covyz(-999.);
  set_vertex_Covzz(-999.);
  set_vertex_chi2(-999.);
  set_vertex_ndf(-999);
  set_vertex_numTracks(-999);
  set_vertex_track1_phi(-999.);
  set_vertex_track1_theta(-999.);
  set_vertex_track1_qOverP(-999.);
  set_vertex_track1_chi2(-999.);
  set_vertex_track1_author(-999);
  set_vertex_track2_phi(-999.);
  set_vertex_track2_theta(-999.);
  set_vertex_track2_qOverP(-999.);
  set_vertex_track2_chi2(-999.);
  set_vertex_track2_author(-999);
  set_vertex_Covd01d01(-999.);
  set_vertex_Covd01z01(-999.);
  set_vertex_Covd01phi1(-999.);
  set_vertex_Covd01theta1(-999.);
  set_vertex_Covd01qOverP1(-999.);
  set_vertex_Covz01z01(-999.);
  set_vertex_Covz01phi1(-999.);
  set_vertex_Covz01theta1(-999.);
  set_vertex_Covz01qOverP1(-999.);
  set_vertex_Covphi1phi1(-999.);
  set_vertex_Covphi1theta1(-999.);
  set_vertex_Covphi1qOverP1(-999.);
  set_vertex_Covtheta1theta1(-999.);
  set_vertex_Covtheta1qOverP1(-999.);
  set_vertex_CovqOverP1qOverP1(-999.);
  set_vertex_Covphi1phi2(-999.);
  set_vertex_Covphi1theta2(-999.);
  set_vertex_Covphi1qOverP2(-999.);
  set_vertex_Covtheta1phi2(-999.);
  set_vertex_Covtheta1theta2(-999.);
  set_vertex_Covtheta1qOverP2(-999.);
  set_vertex_CovqOverP1phi2(-999.);
  set_vertex_CovqOverP1theta2(-999.);
  set_vertex_CovqOverP1qOverP2(-999.);
  set_vertex_Covphi1x(-999.);
  set_vertex_Covphi1y(-999.);
  set_vertex_Covphi1z(-999.);
  set_vertex_Covtheta1x(-999.);
  set_vertex_Covtheta1y(-999.);
  set_vertex_Covtheta1z(-999.);
  set_vertex_CovqOverP1x(-999.);
  set_vertex_CovqOverP1y(-999.);
  set_vertex_CovqOverP1z(-999.);
  set_vertex_Covd02d02(-999.);
  set_vertex_Covd02z02(-999.);
  set_vertex_Covd02phi2(-999.);
  set_vertex_Covd02theta2(-999.);
  set_vertex_Covd02qOverP2(-999.);
  set_vertex_Covz02z02(-999.);
  set_vertex_Covz02phi2(-999.);
  set_vertex_Covz02theta2(-999.);
  set_vertex_Covz02qOverP2(-999.);
  set_vertex_Covphi2phi2(-999.);
  set_vertex_Covphi2theta2(-999.);
  set_vertex_Covphi2qOverP2(-999.);
  set_vertex_Covtheta2theta2(-999.);
  set_vertex_Covtheta2qOverP2(-999.);
  set_vertex_CovqOverP2qOverP2(-999.);
  set_vertex_Covphi2x(-999.);
  set_vertex_Covphi2y(-999.);
  set_vertex_Covphi2z(-999.);
  set_vertex_Covtheta2x(-999.);
  set_vertex_Covtheta2y(-999.);
  set_vertex_Covtheta2z(-999.);
  set_vertex_CovqOverP2x(-999.);
  set_vertex_CovqOverP2y(-999.);
  set_vertex_CovqOverP2z(-999.);
  set_ambiguityResult(-999);
  set_linkIndex(-999);
  set_convMatchDeltaEta1(-999.);
  set_convMatchDeltaPhi1(-999.);
  set_convMatchDeltaEta2(-999.);
  set_convMatchDeltaPhi2(-999.);
  set_convMatchDeltaEta1_refit(-999.);
  set_convMatchDeltaPhi1_refit(-999.);
  set_convMatchDeltaEta2_refit(-999.);
  set_convMatchDeltaPhi2_refit(-999.);
}

void EMConvert::fillVxDetails(const Trk::VxCandidate* vxCand, int trkAuthor)
{
  if(vxCand){
    fillVxTracks(vxCand, trkAuthor);
    fillVertex(vxCand);
  } else {
    throw GaudiException("Parameters not saved, no VxCandidate", "EMConvert::fillDetails(...)", StatusCode::FAILURE);
  }
}

void EMConvert::fillVertex(const Trk::VxCandidate* vxCand)
{
  set_vertex_x(vxCand->recVertex().position().x());
  set_vertex_y(vxCand->recVertex().position().y());
  set_vertex_z(vxCand->recVertex().position().z());
  set_vertex_Covxx(vxCand->recVertex().covariancePosition()(Trk::x, Trk::x));
  set_vertex_Covxy(vxCand->recVertex().covariancePosition()(Trk::x, Trk::y));
  set_vertex_Covxz(vxCand->recVertex().covariancePosition()(Trk::x, Trk::z));
  set_vertex_Covyy(vxCand->recVertex().covariancePosition()(Trk::y, Trk::y));
  set_vertex_Covyz(vxCand->recVertex().covariancePosition()(Trk::y, Trk::z));
  set_vertex_Covzz(vxCand->recVertex().covariancePosition()(Trk::z, Trk::z));
  set_vertex_chi2(vxCand->recVertex().fitQuality().chiSquared());
  set_vertex_ndf(vxCand->recVertex().fitQuality().numberDoF());
  std::vector<Trk::VxTrackAtVertex*> trks(*(vxCand->vxTrackAtVertex()));
  int numTracksAtVertex = trks.size();
  set_vertex_numTracks(numTracksAtVertex);
  const Trk::ExtendedVxCandidate* evxCand = dynamic_cast<const Trk::ExtendedVxCandidate*>(vxCand);
  if(evxCand){
    const Amg::MatrixX *errMat = evxCand->fullCovariance();
    set_vertex_Covphi1phi1((*errMat)(3,3));
    set_vertex_Covphi1theta1((*errMat)(3,4));
    set_vertex_Covphi1qOverP1((*errMat)(3,5));
    set_vertex_Covtheta1theta1((*errMat)(4,4));
    set_vertex_Covtheta1qOverP1((*errMat)(4,5));
    set_vertex_CovqOverP1qOverP1((*errMat)(5,5));
    set_vertex_Covphi1x((*errMat)(3,0));
    set_vertex_Covphi1y((*errMat)(3,1));
    set_vertex_Covphi1z((*errMat)(3,2));
    set_vertex_Covtheta1x((*errMat)(4,0));
    set_vertex_Covtheta1y((*errMat)(4,1));
    set_vertex_Covtheta1z((*errMat)(4,2));
    set_vertex_CovqOverP1x((*errMat)(5,0));
    set_vertex_CovqOverP1y((*errMat)(5,1));
    set_vertex_CovqOverP1z((*errMat)(5,2));
    if(numTracksAtVertex>1){
      set_vertex_Covphi1phi2((*errMat)(6,6));
      set_vertex_Covphi1theta2((*errMat)(6,7));
      set_vertex_Covphi1qOverP2((*errMat)(6,8));
      set_vertex_Covtheta1phi2((*errMat)(4,6));
      set_vertex_Covtheta1theta2((*errMat)(4,7));
      set_vertex_Covtheta1qOverP2((*errMat)(4,8));
      set_vertex_CovqOverP1phi2((*errMat)(5,6));
      set_vertex_CovqOverP1theta2((*errMat)(5,7));
      set_vertex_CovqOverP1qOverP2((*errMat)(5,8));
      set_vertex_Covphi2phi2((*errMat)(6,6));
      set_vertex_Covphi2theta2((*errMat)(6,7));
      set_vertex_Covphi2qOverP2((*errMat)(6,8));
      set_vertex_Covtheta2theta2((*errMat)(7,7));
      set_vertex_Covtheta2qOverP2((*errMat)(7,8));
      set_vertex_CovqOverP2qOverP2((*errMat)(8,8));
      set_vertex_Covphi2x((*errMat)(6,0));
      set_vertex_Covphi2y((*errMat)(6,1));
      set_vertex_Covphi2z((*errMat)(6,2));
      set_vertex_Covtheta2x((*errMat)(7,0));
      set_vertex_Covtheta2y((*errMat)(7,1));
      set_vertex_Covtheta2z((*errMat)(7,2));
      set_vertex_CovqOverP2x((*errMat)(8,0));
      set_vertex_CovqOverP2y((*errMat)(8,1));
      set_vertex_CovqOverP2z((*errMat)(8,2));
    }
  }
}

void EMConvert::fillVxTracks(const Trk::VxCandidate* vxCand, int trkAuthor)
{
  std::vector<Trk::VxTrackAtVertex*> trks(*(vxCand->vxTrackAtVertex()));
  int numTracksAtVertex = trks.size();
  for(int i=0; i<numTracksAtVertex; i++){
    if(i==0){
      Trk::VxTrackAtVertex* trk = trks[i];
      const Trk::TrackParameters* trkParams = trk->perigeeAtVertex();
      set_vertex_track1_chi2(trk->trackQuality().chiSquared());
      set_vertex_track1_author(trkAuthor);
      if(trkParams){
        set_vertex_track1_phi(trkParams->parameters()[Trk::phi]);
        set_vertex_track1_theta(trkParams->parameters()[Trk::theta]);
        set_vertex_track1_qOverP(trkParams->parameters()[Trk::qOverP]);      
	const AmgSymMatrix(5)* errMat= trkParams->covariance();
	
	if(errMat){
	  set_vertex_Covd01d01((*errMat)(Trk::d0,Trk::d0));
	  set_vertex_Covd01z01((*errMat)(Trk::d0,Trk::z0));
	  set_vertex_Covd01phi1((*errMat)(Trk::d0,Trk::phi));
	  set_vertex_Covd01theta1((*errMat)(Trk::d0,Trk::theta));
	  set_vertex_Covd01qOverP1((*errMat)(Trk::d0,Trk::qOverP));
	  set_vertex_Covz01z01((*errMat)(Trk::z0,Trk::z0));
	  set_vertex_Covz01phi1((*errMat)(Trk::z0,Trk::phi));
	  set_vertex_Covz01theta1((*errMat)(Trk::z0,Trk::theta));
	  set_vertex_Covz01qOverP1((*errMat)(Trk::z0,Trk::qOverP));
	  if(numTracksAtVertex<2){
	    set_vertex_Covphi1phi1((*errMat)(Trk::phi,Trk::phi));
	    set_vertex_Covphi1theta1((*errMat)(Trk::phi,Trk::theta));
	    set_vertex_Covphi1qOverP1((*errMat)(Trk::phi,Trk::qOverP));
	    set_vertex_Covtheta1theta1((*errMat)(Trk::theta,Trk::theta));
	    set_vertex_Covtheta1qOverP1((*errMat)(Trk::theta,Trk::qOverP));
	    set_vertex_CovqOverP1qOverP1((*errMat)(Trk::qOverP,Trk::qOverP));
	  }
	}
      }
    }else if(i==1){
      Trk::VxTrackAtVertex* trk = trks[i];
      const Trk::TrackParameters* trkParams = trk->perigeeAtVertex();
      set_vertex_track2_chi2(trk->trackQuality().chiSquared());
      set_vertex_track2_author(trkAuthor);
      if(trkParams){
        set_vertex_track2_phi(trkParams->parameters()[Trk::phi]);
        set_vertex_track2_theta(trkParams->parameters()[Trk::theta]);
        set_vertex_track2_qOverP(trkParams->parameters()[Trk::qOverP]);      
	const AmgSymMatrix(5)* errMat= trkParams->covariance();
	if(errMat){
	  set_vertex_Covd02d02((*errMat)(Trk::d0,Trk::d0));
	  set_vertex_Covd02z02((*errMat)(Trk::d0,Trk::z0));
	  set_vertex_Covd02phi2((*errMat)(Trk::d0,Trk::phi));
	  set_vertex_Covd02theta2((*errMat)(Trk::d0,Trk::theta));
	  set_vertex_Covd02qOverP2((*errMat)(Trk::d0,Trk::qOverP));
	  set_vertex_Covz02z02((*errMat)(Trk::z0,Trk::z0));
	  set_vertex_Covz02phi2((*errMat)(Trk::z0,Trk::phi));
	  set_vertex_Covz02theta2((*errMat)(Trk::z0,Trk::theta));
	  set_vertex_Covz02qOverP2((*errMat)(Trk::z0,Trk::qOverP));
	}
      }
    }
  }
}

AmgSymMatrix(3)* EMConvert::getVxPosErrorMatrix() const
{
  // Make a 3x3 matrix that is filled with zero's alternatively we could fill it with the identity matrix;
  // Chosen the identity matrix option
  AmgSymMatrix(3)* hepSymMatrix =  new AmgSymMatrix(3);
  // Fill the matrix
  hepSymMatrix->fillSymmetric(0,0, vertex_Covxx());
  hepSymMatrix->fillSymmetric(0,1, vertex_Covxy());
  hepSymMatrix->fillSymmetric(0,2, vertex_Covxz());
  hepSymMatrix->fillSymmetric(1,1, vertex_Covyy());
  hepSymMatrix->fillSymmetric(1,2, vertex_Covyz());
  hepSymMatrix->fillSymmetric(2,2, vertex_Covzz());

  return hepSymMatrix;

}

AmgSymMatrix(5)* EMConvert::getTrk2ErrorMatrix() const
{
  AmgSymMatrix(5)* hepSymMatrix =  new AmgSymMatrix(5);
  // Fill the matrix
  hepSymMatrix->fillSymmetric(0,0, vertex_Covd02d02());
  hepSymMatrix->fillSymmetric(0,1, vertex_Covd02z02());
  hepSymMatrix->fillSymmetric(0,2, vertex_Covd02phi2());
  hepSymMatrix->fillSymmetric(0,3, vertex_Covd02theta2());
  hepSymMatrix->fillSymmetric(0,4, vertex_Covd02qOverP2());
  hepSymMatrix->fillSymmetric(1,1, vertex_Covz02z02());
  hepSymMatrix->fillSymmetric(1,2, vertex_Covz02phi2());
  hepSymMatrix->fillSymmetric(1,3, vertex_Covz02theta2());
  hepSymMatrix->fillSymmetric(1,4, vertex_Covz02qOverP2());
  hepSymMatrix->fillSymmetric(2,2, vertex_Covphi2phi2());
  hepSymMatrix->fillSymmetric(2,3, vertex_Covphi2theta2());
  hepSymMatrix->fillSymmetric(2,4, vertex_Covphi2qOverP2());
  hepSymMatrix->fillSymmetric(3,3, vertex_Covtheta2theta2());
  hepSymMatrix->fillSymmetric(3,4, vertex_Covtheta2qOverP2());
  hepSymMatrix->fillSymmetric(4,4, vertex_CovqOverP2qOverP2());

  return hepSymMatrix;
}

AmgSymMatrix(5)* EMConvert::getTrk1ErrorMatrix() const
{
  AmgSymMatrix(5)* hepSymMatrix =  new AmgSymMatrix(5);
  // Fill the matrix
  hepSymMatrix->fillSymmetric(0,0, vertex_Covd01d01());
  hepSymMatrix->fillSymmetric(0,1, vertex_Covd01z01());
  hepSymMatrix->fillSymmetric(0,2, vertex_Covd01phi1());
  hepSymMatrix->fillSymmetric(0,3, vertex_Covd01theta1());
  hepSymMatrix->fillSymmetric(0,4, vertex_Covd01qOverP1());
  hepSymMatrix->fillSymmetric(1,1, vertex_Covz01z01());
  hepSymMatrix->fillSymmetric(1,2, vertex_Covz01phi1());
  hepSymMatrix->fillSymmetric(1,3, vertex_Covz01theta1());
  hepSymMatrix->fillSymmetric(1,4, vertex_Covz01qOverP1());
  hepSymMatrix->fillSymmetric(2,2, vertex_Covphi1phi1());
  hepSymMatrix->fillSymmetric(2,3, vertex_Covphi1theta1());
  hepSymMatrix->fillSymmetric(2,4, vertex_Covphi1qOverP1());
  hepSymMatrix->fillSymmetric(3,3, vertex_Covtheta1theta1());
  hepSymMatrix->fillSymmetric(3,4, vertex_Covtheta1qOverP1());
  hepSymMatrix->fillSymmetric(4,4, vertex_CovqOverP1qOverP1());

  return hepSymMatrix;
}


Amg::MatrixX* EMConvert::getVxErrorMatrix() const
{
  if(vertex_numTracks()>0){
  
    Amg::MatrixX* hepSymMatrix =  new Amg::MatrixX(9,9);

    // Fill the matrix
    hepSymMatrix->fillSymmetric(0,0, vertex_Covxx());
    hepSymMatrix->fillSymmetric(0,1, vertex_Covxy());
    hepSymMatrix->fillSymmetric(0,2, vertex_Covxz());
    hepSymMatrix->fillSymmetric(0,3, vertex_Covphi1x());
    hepSymMatrix->fillSymmetric(0,4, vertex_Covtheta1x());
    hepSymMatrix->fillSymmetric(0,5, vertex_CovqOverP1x());
    hepSymMatrix->fillSymmetric(0,6, vertex_Covphi2x());
    hepSymMatrix->fillSymmetric(0,7, vertex_Covtheta2x());
    hepSymMatrix->fillSymmetric(0,8, vertex_CovqOverP2x());
    hepSymMatrix->fillSymmetric(1,1, vertex_Covyy());
    hepSymMatrix->fillSymmetric(1,2, vertex_Covyz());
    hepSymMatrix->fillSymmetric(1,3, vertex_Covphi1y());
    hepSymMatrix->fillSymmetric(1,4, vertex_Covtheta1y());
    hepSymMatrix->fillSymmetric(1,5, vertex_CovqOverP1y());
    hepSymMatrix->fillSymmetric(1,6, vertex_Covphi2y());
    hepSymMatrix->fillSymmetric(1,7, vertex_Covtheta2y());
    hepSymMatrix->fillSymmetric(1,8, vertex_CovqOverP2y());
    hepSymMatrix->fillSymmetric(2,2, vertex_Covzz());
    hepSymMatrix->fillSymmetric(2,3, vertex_Covphi1z());
    hepSymMatrix->fillSymmetric(2,4, vertex_Covtheta1z());
    hepSymMatrix->fillSymmetric(2,5, vertex_CovqOverP1z());
    hepSymMatrix->fillSymmetric(2,6, vertex_Covphi2z());
    hepSymMatrix->fillSymmetric(2,7, vertex_Covtheta2z());
    hepSymMatrix->fillSymmetric(2,8, vertex_CovqOverP2z());
    hepSymMatrix->fillSymmetric(3,3, vertex_Covphi1phi1());
    hepSymMatrix->fillSymmetric(3,4, vertex_Covphi1theta1());
    hepSymMatrix->fillSymmetric(3,5, vertex_Covphi1qOverP1());
    hepSymMatrix->fillSymmetric(3,6, vertex_Covphi1phi2());
    hepSymMatrix->fillSymmetric(3,7, vertex_Covphi1theta2());
    hepSymMatrix->fillSymmetric(3,8, vertex_Covphi1qOverP2());
    hepSymMatrix->fillSymmetric(4,4, vertex_Covtheta1theta1());
    hepSymMatrix->fillSymmetric(4,5, vertex_Covtheta1qOverP1());
    hepSymMatrix->fillSymmetric(4,6, vertex_Covtheta1phi2());
    hepSymMatrix->fillSymmetric(4,7, vertex_Covtheta1theta2());
    hepSymMatrix->fillSymmetric(4,8, vertex_Covtheta1qOverP2());
    hepSymMatrix->fillSymmetric(5,5, vertex_CovqOverP1qOverP1());
    hepSymMatrix->fillSymmetric(5,6, vertex_CovqOverP1phi2());
    hepSymMatrix->fillSymmetric(5,7, vertex_CovqOverP1theta2());
    hepSymMatrix->fillSymmetric(5,8, vertex_CovqOverP1qOverP2());
    hepSymMatrix->fillSymmetric(6,6, vertex_Covphi2phi2());
    hepSymMatrix->fillSymmetric(6,7, vertex_Covphi2theta2());
    hepSymMatrix->fillSymmetric(6,8, vertex_Covphi2qOverP2());
    hepSymMatrix->fillSymmetric(7,7, vertex_Covtheta2theta2());
    hepSymMatrix->fillSymmetric(7,8, vertex_Covtheta2qOverP2());
    hepSymMatrix->fillSymmetric(8,8, vertex_CovqOverP2qOverP2());
   
   return hepSymMatrix;

  }
  //Convert the CovarianceMatrix into a ErrorMatrix and create a new new object to pass;
  return 0;
}


Trk::VxCandidate* EMConvert::getVxCandidate() const
{

  Trk::ExtendedVxCandidate* evxCand = 0;
  Trk::VxCandidate* vxCand = 0;

  AmgSymMatrix(3)* posmat = getVxPosErrorMatrix();
  const Trk::RecVertex* vx = new Trk::RecVertex(Amg::Vector3D(vertex_x(),vertex_y(),vertex_z()), *posmat, (double)vertex_ndf(), (double)vertex_chi2());
  delete posmat;

  //perigee at vertex
  Amg::Vector3D globPos(vertex_x(), vertex_y(), vertex_z());
  AmgSymMatrix(5) *trk1EM = getTrk1ErrorMatrix();
    
  const Trk::PerigeeSurface   surface(globPos);
  
  Trk::Perigee* trkParams = const_cast<Trk::Perigee*>(surface.createParameters<5,Trk::Charged>( 
                                                            0,
                                                            0,
                                                            vertex_track1_phi(),
                                                            vertex_track1_theta(),
                                                            vertex_track1_qOverP(),
                                                            trk1EM ));
   
  Trk::VxTrackAtVertex* vxtrk1 = new Trk::VxTrackAtVertex(vertex_track1_chi2(), trkParams);




  std::vector<Trk::VxTrackAtVertex*> vxTrkAtVx;
  vxTrkAtVx.push_back(vxtrk1);

  Trk::VxTrackAtVertex* vxtrk2 = 0;
  if(vertex_numTracks()>1){
    globPos = Amg::Vector3D(vertex_x(), vertex_y(), vertex_z());
    AmgSymMatrix(5) *trk2EM = getTrk2ErrorMatrix();
   
    const Trk::PerigeeSurface   surface2(globPos);
  
    Trk::Perigee* trkParams2 = const_cast<Trk::Perigee*>(surface2.createParameters<5,Trk::Charged>( 
                                                            0,
                                                            0,
                                                            vertex_track1_phi(),
                                                            vertex_track1_theta(),
                                                            vertex_track1_qOverP(),
                                                            trk2EM ));
 
   
    vxtrk2 = new Trk::VxTrackAtVertex(vertex_track2_chi2(), trkParams2);
    vxTrkAtVx.push_back(vxtrk2);
    evxCand = new Trk::ExtendedVxCandidate(*vx, vxTrkAtVx, getVxErrorMatrix());
    delete vx;
    return evxCand;
  }else{//single track conversion -> VxCandidate
    vxCand = new Trk::VxCandidate(*vx, vxTrkAtVx);
    delete vx;
    return vxCand;
  }
}


SG_ADD_BASE (EMConvert,     egDetail);
