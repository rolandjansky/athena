/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ICLEANERTOOL_H
#define TRIGHLTJETHYPO_ICLEANERTOOL_H
/********************************************************************
 *
 * NAME:     ICleanerTool.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 14, 2019
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include "GaudiKernel/IAlgTool.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <string>

class ICleanerTool: virtual public ::IAlgTool{
 public:
  DeclareInterfaceID(ICleanerTool, 1, 0);

  // Abstract interface used by a factory function
  virtual ~ICleanerTool(){}
  virtual bool select(const pHypoJet&) const = 0;
  virtual std::string toString() const noexcept = 0;
  virtual std::string getName() const noexcept= 0;
};

#endif
