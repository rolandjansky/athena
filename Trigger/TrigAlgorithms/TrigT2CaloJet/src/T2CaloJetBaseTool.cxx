/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     T2CaloJetBaseTool.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Kyle Cranmer <cranmer@cern.ch>
CREATED:  November, 2005

PURPOSE:  Base Tool for all T2CaloJet tools that require
          an T2CaloJet object as its input.  The derived
          tool must implement an execute(T2CaloJet*) method.

********************************************************************/
#include "TrigT2CaloJet/T2CaloJetBaseTool.h"
#include "TrigCaloEvent/TrigT2Jet.h"

#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/IToolSvc.h"

T2CaloJetBaseTool::T2CaloJetBaseTool(const std::string& type,
				     const std::string& name,
				     const IInterface* parent):
  IAlgToolCalo(type, name, parent)
{
    declareInterface<T2CaloJetBaseTool>( this );
}


T2CaloJetBaseTool::~T2CaloJetBaseTool()
{
}

StatusCode T2CaloJetBaseTool::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode T2CaloJetBaseTool::execute()
{
  return StatusCode::SUCCESS;
}


//StatusCode T2CaloJetBaseTool::execute(TrigT2Jet* jet,double etamin, double etamax, double phimin, double phimax)
// StatusCode T2CaloJetBaseTool::execute(TrigT2Jet* /*jet*/,double /*etamin*/, double /*etamax*/, double /*phimin*/, double /*phimax*/)
StatusCode T2CaloJetBaseTool::execute(TrigT2Jet* /* jet */, const IRoiDescriptor& /* roi */,
			       const CaloDetDescrElement*& /*caloDDE*/ )
{
  return StatusCode::SUCCESS;
}


StatusCode T2CaloJetBaseTool::finalize()
{
  return StatusCode::SUCCESS;
}
