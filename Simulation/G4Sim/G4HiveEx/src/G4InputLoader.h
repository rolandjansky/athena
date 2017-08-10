/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////// -*- C++ -*- /////////////////////////////
// G4InputLoader.h 
// Header file for class G4InputLoader
// Author: C.Leggett
/////////////////////////////////////////////////////////////////// 
#ifndef G4HIVEEX_G4INPUTLOADER_H
#define G4HIVEEX_G4INPUTLOADER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"


class G4InputLoader
  : public ::AthAlgorithm
{ 
 public: 

  G4InputLoader( const std::string& name, ISvcLocator* pSvcLocator );

  //  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  //  virtual StatusCode  finalize();

 private: 

  std::string m_mcTruth;


}; 


#endif
