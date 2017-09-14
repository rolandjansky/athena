/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ISCT_Amp.h
 * Header file for abstract base class ISCT_Amp
 * (c) ATLAS Detector software
 * Interface for the SiChargedDiode processor classes
 * 23/08/2007 Kondo.Gnanvo@cern.ch, and others   
 */

#ifndef SIDIGITIZATION_ISCT_AMP_H
#define SIDIGITIZATION_ISCT_AMP_H

#include "GaudiKernel/IAlgTool.h"
#include "InDetSimEvent/SiTotalCharge.h"

#include <vector>

static const InterfaceID IID_ISCT_Amp("ISCT_Amp", 1, 0);

class ISCT_Amp : virtual public IAlgTool {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:
  typedef SiTotalCharge::list_t list_t;

  //Retrieve interface ID
  static const InterfaceID & interfaceID() {return IID_ISCT_Amp; }

  // Destructor:
  virtual ~ISCT_Amp() {}

  ///////////////////////////////////////////////////////////////////
  // Pure virtual methods:
  ///////////////////////////////////////////////////////////////////

  // process the collection of charged diodes
  /** main purpose: CR-RC^3 response to a list of charges with times */
  virtual float response(const list_t &Charges, const float timeOverThreshold) const =0;
  virtual void response(const list_t &Charges, const float timeOverThreshold, std::vector<float> &resp) const =0;

  /** Neighbour strip cross talk response strip to a list of charges with times */
  virtual float crosstalk(const list_t &Charges, const float timeOverThreshold) const =0;
  virtual void crosstalk(const list_t &Charges, const float timeOverThreshold, std::vector<float> &resp) const =0;
};

#endif // SIDIGITIZATION_ISCT_AMP_H
