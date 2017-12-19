/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMAQWETA1C_H
#define EGAMMACALOTOOLS_EGAMMAQWETA1C_H

/**
  @class egammaqweta1c
         correction for the eta width modulation in strips 
   @author H. Ma
   @author Frederic Derue
*/


#include "AthenaBaseComps/AthAlgTool.h"
#include "egammaInterfaces/Iegammaqweta1c.h"

// Why is this an alg-tool and not just a free-standing function?
// a header needs to be made public for trigger use in that case.

class egammaqweta1c : public AthAlgTool, virtual public Iegammaqweta1c {

 public:

  /** @brief Default constructor*/
  egammaqweta1c(const std::string& type,
		const std::string& name,
		const IInterface* parent);
  /** @brief Destructor*/
  virtual ~egammaqweta1c();  
  
  /** @brief initialize method*/
  virtual StatusCode initialize() override;
  /** @brief finalize method*/
  virtual StatusCode finalize() override;

  /** @brief returns corrected width at eta*/
  virtual float Correct(float eta, float etacell, float width) const override final;
  /** @brief returns relative position within the cell*/
  virtual double RelPosition(float eta, float etacell) const override final;
};

#endif
