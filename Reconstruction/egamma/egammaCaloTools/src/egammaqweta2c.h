/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMAQWETA2C_H
#define EGAMMACALOTOOLS_EGAMMAQWETA2C_H

/**
  @class egammaqweta2c
          correction for the eta width modulation in sampling 2
   @author H. Ma
   @author Frederic Derue

*/


#include "AthenaBaseComps/AthAlgTool.h"
#include "egammaInterfaces/Iegammaqweta2c.h"

class egammaqweta2c : public AthAlgTool, virtual public Iegammaqweta2c {

 public:
  
  /** @brief Default constructor*/
  egammaqweta2c(const std::string& type,
		const std::string& name,
		const IInterface* parent);
  /** @brief Destructor*/
  ~egammaqweta2c();  
  
  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief execute method*/
  virtual StatusCode execute();
  /** @brief finalize method*/
  StatusCode finalize();

  /** @brief returns corrected width at eta. */
  float Correct(float eta, float etacell, float width);
  /** @brief returns method within the cell*/
  double RelPosition(float eta, float etacell);

 private:

  static const float P0A[3];
  static const float P1A[3];
  static const float P2A[3];

  static const float P0B[3];
  static const float P1B[3];
  static const float P2B[3];

  static const float P0C[3];
  static const float P1C[3];
  static const float P2C[3];

};

#endif




