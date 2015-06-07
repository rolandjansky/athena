/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * TrigEgammaMonTool:TrigEgammaPhysValMonTool
 * Author:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 * Date: May 2015
 * Description:
 *      Wrapper class for PhysValMon framework 
 **********************************************************************/
#include "TrigEgammaPhysValMonTool.h"

TrigEgammaPhysValMonTool::
TrigEgammaPhysValMonTool( const std::string & type, const std::string & name, const IInterface* parent)
    :TrigEgammaMonTool(type, name, parent)
{
}

TrigEgammaPhysValMonTool::~TrigEgammaPhysValMonTool(){}

StatusCode TrigEgammaPhysValMonTool::initialize(){
    StatusCode sc = TrigEgammaMonTool::initialize();
    return sc;
}

StatusCode TrigEgammaPhysValMonTool::bookHistograms(){
    StatusCode sc = TrigEgammaMonTool::book();
    return sc;
}

StatusCode TrigEgammaPhysValMonTool::fillHistograms(){
    StatusCode sc = TrigEgammaMonTool::fill();
    return sc;
}

StatusCode TrigEgammaPhysValMonTool::procHistograms(){
    StatusCode sc = TrigEgammaMonTool::proc();
    return sc;
}

