/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_ISTANDALONEJETBTAGGERTOOL_HH
#define BTAGGING_ISTANDALONEJETBTAGGERTOOL_HH
///////////////////////////////////////////
///
/// \class IStandAloneJetBTaggerTool
/// StandAlone tool to run and add btagging information.
////////////////////////////////////////////

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace Analysis{

static const InterfaceID IID_IStandAloneJetBTaggerTool("IStandAloneJetBTaggerTool", 1, 0);

class  IStandAloneJetBTaggerTool : virtual public IAlgTool 
{  
  public:
  
    /** Virtual destructor */
    virtual ~IStandAloneJetBTaggerTool() {};
    
    /** AlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IStandAloneJetBTaggerTool; };

    virtual StatusCode initialize() = 0;
    virtual int modify() = 0;

};

}

#endif
