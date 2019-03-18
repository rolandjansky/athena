/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     T2L1CaloJetFullScanBaseTool.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Matthew Tamsett <tamsett@cern.ch>
CREATED:  July, 2011

PURPOSE:  Base Tool for all T2L1CaloJetFullScan tools that require
          a vector of T2CaloJet objects as its input.  The derived
          tool must implement an execute(DataVector<T2CaloJet*>) method.

********************************************************************/
#include "TrigT2CaloJet/T2L1CaloJetFullScanBaseTool.h"
#include "TrigCaloEvent/TrigT2Jet.h"

#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/IToolSvc.h"

T2L1CaloJetFullScanBaseTool::T2L1CaloJetFullScanBaseTool(const std::string& type,
				     const std::string& name,
				     const IInterface* parent):
  IAlgToolCalo(type, name, parent)
{
    declareInterface<T2L1CaloJetFullScanBaseTool>( this );
}


T2L1CaloJetFullScanBaseTool::~T2L1CaloJetFullScanBaseTool()
{
}

StatusCode T2L1CaloJetFullScanBaseTool::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode T2L1CaloJetFullScanBaseTool::execute()
{
  return StatusCode::SUCCESS;
}

StatusCode T2L1CaloJetFullScanBaseTool::execute(DataVector<TrigT2Jet>* /*output jets*/,int& /*n_towers*/,float& /*L1 unpack time*/, float& /*all unpack time*/,float& /*fastjet time*/,float& /*all jet finding time*/)
{
  return StatusCode::SUCCESS;
}


StatusCode T2L1CaloJetFullScanBaseTool::finalize()
{
  return StatusCode::SUCCESS;
}
