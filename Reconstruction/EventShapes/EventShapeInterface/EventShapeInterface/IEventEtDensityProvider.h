// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPEINTERFACE_IEVENTETDENSITYPROVIDER_H
#define EVENTSHAPEINTERFACE_IEVENTETDENSITYPROVIDER_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"

#include "EventShapeEvent/EventEtDensity.h"

static const InterfaceID IID_IEventEtDensityProvider("IEventEtDensityProvider",
						     1,0);

class IEventEtDensityProvider : virtual public IAlgTool, 
				virtual public IIncidentListener
{
public:

  /*! @brief Baseclass destructor */
  virtual ~IEventEtDensityProvider() { }

  /*! @brief AlgTool identifier */ 
  static const InterfaceID& interfaceID()
 { return IID_IEventEtDensityProvider; }

  /*! @brief Initialization */
  virtual StatusCode initialize() = 0;

  /*! @brief Interface to density */
  virtual double rho()   = 0;
  virtual double sigma() = 0;
  virtual double area()  = 0;
  /*! @brief Interface to density at given eta */
  virtual double rho(double eta)   = 0;
  virtual double sigma(double eta) = 0;
  virtual double area(double eta)  = 0;
  /*! @brief Interface to density in given eta range */
  virtual double rho(double etaMin,double etaMax)   = 0;
  virtual double sigma(double etaMin,double etaMax) = 0;
  virtual double area(double etaMin,double etaMax)  = 0;
  /*! @brief Interface to all EventEtDensity data */
  virtual const EventEtDensity& eventEtDensity()                            = 0;
  virtual const EventEtDensity& eventEtDensity(double eta)                  = 0;
  virtual const EventEtDensity& eventEtDensity(double etaMin,double etaMax) = 0;
  
};
#endif
