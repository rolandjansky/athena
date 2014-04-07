// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_EGDETAIL_H
#define EGAMMAEVENT_EGDETAIL_H 1
/**
  @class egDetail
          Base class for detailed egamma information
   @author K. Cranmer, S. Rajagopalan, D. Rousseau, D. Zerwas
*/
/********************************************************************

NAME:     egDetail.h
PACKAGE:  offline/Reconstruction/egammaEvent
CREATED:  Sept 15. 2006

UPDATED:
          Jun 14, 2007 (FD) Doxygen documentation
********************************************************************/

#include "egammaEvent/egammaParamDefs.h"
#include <string>

class egDetail

{
 public:

  /** @brief Default constructor*/
  egDetail();
  /** @brief Destructor*/
  virtual ~egDetail();
  /** @brief */
  virtual double parameter(egammaParameters::ParamDef) const; 
  /** @brief */
  virtual int    intParameter(egammaParameters::ParamDef) const;
  /** @brief */
  virtual bool   boolParameter(egammaParameters::ParamDef) const; 
  /** @brief */
  virtual bool   hasParameter(egammaParameters::ParamDef) const;
  /** @brief */
  virtual bool   hasIntParameter(egammaParameters::ParamDef) const;
  /** @brief */
  virtual bool   hasBoolParameter(egammaParameters::ParamDef) const;
  /** @brief */
  virtual const std::string& className() const;

  virtual int linkIndex() const;
  virtual void set_linkIndex(int);

};

#endif
