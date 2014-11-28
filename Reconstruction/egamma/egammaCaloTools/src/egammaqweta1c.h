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

class egammaqweta1c : public AthAlgTool, virtual public Iegammaqweta1c {

 public:

  /** @brief Default constructor*/
  egammaqweta1c(const std::string& type,
		const std::string& name,
		const IInterface* parent);
  /** @brief Destructor*/
  ~egammaqweta1c();  
  
  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief execute method*/
  virtual StatusCode execute();
  /** @brief finalize method*/
  StatusCode finalize();

  /** @brief returns corrected width at eta*/
  float Correct(float eta, float etacell, float width);
  /** @brief returns relative position within the cell*/
  double RelPosition(float eta, float etacell);
 private:

};

#endif
