/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * IAmplifier.h
 * Header file for abstract base class IAmplier 
 */

#ifndef SIDIGITIZATION_IAMPLIFIER_H
#define SIDIGITIZATION_IAMPLIFIER_H

#include "GaudiKernel/IAlgTool.h"
#include "InDetSimEvent/SiTotalCharge.h"

#include <vector>

static const InterfaceID IID_IAmplifier("IAmplifier", 1, 0);

class IAmplifier : virtual public IAlgTool {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:
  typedef SiTotalCharge::list_t list_t;

  //Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IAmplifier; }

  // Destructor:
  virtual ~IAmplifier() {}

  ///////////////////////////////////////////////////////////////////
  // Pure virtual methods:
  ///////////////////////////////////////////////////////////////////

  // process the collection of charged diodes
  /** main purpose: CR-RC^3 response to a list of charges with times */
  virtual float response(const list_t& Charges, const float timeOverThreshold) const =0;
  virtual void response(const list_t& Charges, const float timeOverThreshold, std::vector<float>& resp) const =0;

  /** Neighbour strip cross talk response strip to a list of charges with times */
  virtual float crosstalk(const list_t& Charges, const float timeOverThreshold) const =0;
  virtual void crosstalk(const list_t& Charges, const float timeOverThreshold, std::vector<float> &resp) const =0;
};

#endif // SIDIGITIZATION_IAMPLIFIER_H
