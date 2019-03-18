/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     T2CaloJetGridFromLvl1Ppr.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Kyle Cranmer <cranmer@cern.ch>
CREATED:  November, 2005

PURPOSE: Data preparation from Lvl1 Calo

********************************************************************/
#include "TrigT2CaloJet/T2CaloJetGridFromLvl1Ppr.h"
#include "TrigCaloEvent/TrigT2Jet.h"

#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/IToolSvc.h"

T2CaloJetGridFromLvl1Ppr::T2CaloJetGridFromLvl1Ppr(const std::string& type,
				     const std::string& name,
				     const IInterface* parent):
  T2CaloJetBaseTool(type, name, parent), m_gridElement(0)
{  
}

T2CaloJetGridFromLvl1Ppr::~T2CaloJetGridFromLvl1Ppr() 
{
}

StatusCode T2CaloJetGridFromLvl1Ppr::initialize() 
{
  if ( !m_gridElement ) m_gridElement = new Trig3Momentum();

  // from IAlgToolCalo
  m_geometryTool.disable();
  m_data.disable();

  return StatusCode::SUCCESS;
}

StatusCode T2CaloJetGridFromLvl1Ppr::execute()
{
  return StatusCode::SUCCESS;
}


//StatusCode T2CaloJetGridFromLvl1Ppr::execute(TrigT2Jet
//* jet,double etamin, double etamax, double phimin, double phimax)
StatusCode T2CaloJetGridFromLvl1Ppr::execute(TrigT2Jet* jet,
                                             double /*etamin*/,
                                             double /*etamax*/,
                                             double /*phimin*/, 
                                             double /*phimax*/)
{

  // this method should use region selector to get TriggerTowers from
  // the Lvl1 PreProcessor.  Will require some chages to Region Selector

  std::vector<Trig3Momentum>* grid = new std::vector<Trig3Momentum>();

  // now get TT's

  // possibly do some calibration

  // now fill grid elements like this
  Trig3Momentum* gridElement =  m_gridElement;
  gridElement->setE(10000.);
  gridElement->setEta(1.);
  gridElement->setPhi(0.);

  //push them back into the grid
  grid->push_back(*gridElement);


  //then set the grid in the jet.  do not delete anything.  Cleanup
  //happens at the end of jet finding.
  jet->setGrid(grid);

  return StatusCode::SUCCESS;
}


StatusCode T2CaloJetGridFromLvl1Ppr::finalize()
{
  if ( m_gridElement ) delete m_gridElement;
  m_gridElement = 0;
  return StatusCode::SUCCESS;
}
