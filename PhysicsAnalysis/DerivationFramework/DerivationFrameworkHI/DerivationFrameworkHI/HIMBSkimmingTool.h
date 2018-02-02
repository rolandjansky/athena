/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef DERIVATIONFRAMEWORK_HIMBSKIMMINGTOOL_H
#define DERIVATIONFRAMEWORK_HIMBSKIMMINGTOOL_H

////////////////////////////////////////////////////////////////////////////////
///
/// \class HIJetMaxOverMeanTool
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Jan, 2016
///
/// \brief SkimmingTool that applies MB event selection to heavy ion events
/// All the work is done by the MBEventSelectionTool, implemented as an AsgTool
/// that may be reused in other contexts.
////////////////////////////////////////////////////////////////////////////////


#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"
#include "HIEventUtils/IHIEventSelectionTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace DerivationFramework {

  class HIMBSkimmingTool : public AthAlgTool, public ISkimmingTool {

  public: 
    HIMBSkimmingTool( const std::string& t, const std::string& n, const IInterface* p );

    ~HIMBSkimmingTool();

    // AthAlgTool interface
    StatusCode  initialize();
    StatusCode  finalize();

    // ISkimmingTool interface
    virtual bool eventPassesFilter() const;

  private:

    ToolHandle<HI::IHIEventSelectionTool> m_selection_tool;
    mutable unsigned long int m_total;
    mutable unsigned long int m_passed;
  }; 

}

#endif // DERIVATIONFRAMEWORK_HIMBSKIMMINGTOOL_H
