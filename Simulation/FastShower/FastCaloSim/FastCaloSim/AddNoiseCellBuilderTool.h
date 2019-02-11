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
#include "CaloInterface/ICaloNoiseTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include <string>

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
  virtual ~AddNoiseCellBuilderTool();


  virtual StatusCode initialize() override;

  // update theCellContainer
  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override;
private:

  ToolHandle<ICaloNoiseTool> m_noiseTool;   //NoiseTool - public
  ServiceHandle<IAtRndmGenSvc>   m_rndmSvc;
  CLHEP::HepRandomEngine*        m_randomEngine{};
  std::string                    m_randomEngineName{"FastCaloSimNoiseRnd"};         //!< Name of the random number stream
  bool m_donoise{true};
};

#endif
