/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ADDNOISE_CELLBUILDERTOOL_H
#define ADDNOISE_CELLBUILDERTOOL_H
// 
// CellBuilderTool.cxx
//     Building Cells objects from Atlfast
//
// Michael Duehrssen

#include "FastCaloSim/BasicCellBuilderTool.h"
//#include "CaloUtils/ICaloNoiseTool.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include <string>

class TRandom;

namespace CLHEP {
  class HepRandomEngine;
}

class AddNoiseCellBuilderTool: public BasicCellBuilderTool
{
public:    
  AddNoiseCellBuilderTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent);
  ~AddNoiseCellBuilderTool();


  virtual StatusCode initialize();

  // update theCellContainer
  virtual StatusCode process( CaloCellContainer * theCellContainer) ;
private:
  std::string m_noiseToolName; 
  //NoiseTool
  ICaloNoiseTool* m_noiseTool;
  
  //TRandom* m_rand;

  ServiceHandle<IAtRndmGenSvc>   m_rndmSvc;
  CLHEP::HepRandomEngine*        m_randomEngine;
  std::string                    m_randomEngineName;         //!< Name of the random number stream
  
  bool m_donoise;
};

#endif





