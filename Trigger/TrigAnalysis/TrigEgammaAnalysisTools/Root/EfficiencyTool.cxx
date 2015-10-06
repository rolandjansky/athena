/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 **********************************************************************/
#include "TrigEgammaAnalysisTools/EfficiencyTool.h"
#include "string"
#include <algorithm>
#include "boost/algorithm/string.hpp"
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
//**********************************************************************

EfficiencyTool::
EfficiencyTool( const std::string& myname )
: TrigEgammaAnalysisBaseTool(myname) {
}

//**********************************************************************
StatusCode EfficiencyTool::childInitialize(){


    return StatusCode::SUCCESS;
}

StatusCode EfficiencyTool::childBook()
{

    return StatusCode::SUCCESS;
}
StatusCode EfficiencyTool::childExecute()
{

    return StatusCode::SUCCESS;
}

StatusCode EfficiencyTool::childFinalize(){
  return StatusCode::SUCCESS;
}

