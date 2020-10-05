/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_EMBREMFIT_H
#define EGAMMAEVENT_EMBREMFIT_H

/**
  @class EMBremFit
  EMBremFit is made by EMBremsstrahlungBuilder. 
  It is responsible for bremsstrahlung recovery using electromagnetic calorimeter. It re-calculates track pT accounting for energy of the photons emmitted in a hard bremsstrahlung. 
   @author Anne-Catherine Le Bihan 
*/

/********************************************************************
NAME:     EMBremFit.h
PACKAGE:  offline/Reconstruction/egammaEvent
Created March 2006 by Anne-Catherine Le Bihan 
UPDATED:
          Jun 14, 2007 (FD) Doxygen documentation
********************************************************************/

#include <vector>
#include "egammaEvent/egDetail.h"

class EMBremFit : public egDetail
{
 public:

  /** @brief Default constructor*/
  EMBremFit();

  /** @brief Destructor*/
  virtual ~EMBremFit();

  void print() const; 
  virtual const std::string& className() const;
  
  /** @brief */
  virtual int    intParameter(egammaParameters::ParamDef) const;
  /** @brief */
  virtual double parameter(egammaParameters::ParamDef) const ;
  /** @brief */
  virtual bool hasParameter(egammaParameters::ParamDef) const ;
  /** @brief checks if the parameter is integer */
  virtual bool hasIntParameter(egammaParameters::ParamDef) const ;
  /** @brief general set method for parameters */
  void set_parameter(egammaParameters::ParamDef, double, bool overwrite=false) ;
  
  
  /** @brief transverse impact parameter (distance of closest approach) after EMBremFit */
  float bremD0    ()  const;
  /** @brief azimuth angle of the momentum at the point of closest approach after EMBremFit */
  float bremPhi0  ()  const;
  /** @brief 1/pT estimate according to EMBremFit */
  float bremInvpT ()  const;
  /** @brief the z value at the point of closest approach after EMBremFit */
  float bremZ0    ()  const;
  /** @brief 1/tan(theta) of the track after EMBremFit */
  float bremDzDr  ()  const;
  /** @brief estimated brem radius from EMBremFit (mm) [x-y plane] */
  float bremRadius()  const;
  /** @brief EMBremFit track impact in 2nd. sampling - cluster distance (mm): 
	Delta phi(EMBremFit track impact in 2nd. sampling, cluster)*bremClusterRadius */
  float bremX     ()  const;
  
  /** @brief error associated to 1/pT by EMBremFit */
  float bremInvpTerr() const;
  
  /** @brief number of TRT hits included in the EMBremFit */
  int bremNTRT()  const;
  /** @brief number of R hits included in the EMBremFit */
  int bremNR  ()  const;
  /** @brief number of Z hits included in the EMBremFit */
  int bremNZ  ()  const;
  
  /** @brief cluster radius (mm) [x-y plane] from EMBremFit (#depth) */
  float bremClusterRadius()  const;
  
  /** @brief Minuit EMBremFit fit status */
  int bremFitStatus   ()  const;
  /** @brief Flag to tell why the EMBremFit was not called */
  int bremErrorFlag   ()  const;
  /** @brief chi2 of EMBremFit */
  float bremFitChi2     ()  const;
  /** @brief Index of track this EMBremFit belongs to */
  int linkIndex   ()  const;
  
  // set values for the different variables in the EMBremFit object
  
  /** @brief */
  void BremD0    (float);
  /** @brief */
  void BremPhi0  (float);
  /** @brief */
  void BremInvpT (float);
  /** @brief */
  void BremZ0    (float);
  /** @brief */
  void BremDzDr  (float);
  /** @brief */
  void BremRadius(float);
  /** @brief */
  void BremX     (float);
  
  /** @brief */
  void BremInvpTerr(float);
  
  /** @brief */
  void BremNTRT(int);
  /** @brief */
  void BremNR  (int);
  /** @brief */
  void BremNZ  (int);
  
  /** @brief */
  void BremClusterRadius (float);
  
  /** @brief */
  void BremFitStatus   (int);
  /** @brief */
  void BremErrorFlag   (int);
  /** @brief */
  void BremFitChi2     (float);
  /** @brief */
  void set_linkIndex   (int);
  

private: 
 
 void set_parameterInt(egammaParameters::ParamDef, int, bool overwrite=false) ; 

 std::vector< std::pair<egammaParameters::ParamDef,double> > m_parameters; 
 std::vector< std::pair<egammaParameters::ParamDef,int> > m_parametersInt; 
 static const std::string s_className;
   
};

#endif 


