/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */


#include "TrigEgammaEmulationTool/TrigEgammaEmulationTool.h"


using namespace Trig;



TrigEgammaEmulationTool::TrigEgammaEmulationTool( const std::string& myname )
    : AsgTool(myname)

{;}


StatusCode TrigEgammaEmulationTool::initialize() {
    ATH_MSG_INFO("Initialising accept...");
    return StatusCode::SUCCESS;
}





