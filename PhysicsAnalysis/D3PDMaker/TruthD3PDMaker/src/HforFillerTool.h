// -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/// @author George Lewis
/// @date Nov, 2010
/// @brief Fill in Heavy Flavor type for MC from HforTool

#ifndef TOPINPUTSD3PDMAKER_HFORFILLERTOOL_H
#define TOPINPUTSD3PDMAKER_HFORFILLERTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "GenInterfaces/IHforTool.h"

namespace D3PD {


  class HforFillerTool : public BlockFillerTool<void> {
  public:

    HforFillerTool (const std::string& type, const std::string& name, const IInterface* parent);

    StatusCode initialize();

    virtual StatusCode book();

    virtual StatusCode fill();


  private:

    ToolHandle<IHforTool> m_hfor_tool;

    int* m_hfor_type;
    std::vector<float>* hfor_Quarks_px;
    std::vector<float>* hfor_Quarks_py;
    std::vector<float>* hfor_Quarks_pz;
    std::vector<float>* hfor_Quarks_e;
    std::vector<int>* hfor_event_flavour;

  }; // class HforFillerTool

} // namespace D3PD

#endif
