/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TrigCaloClusterFillerTool.cxx
*/
#include "TrigCaloClusterFillerTool.h"
#include "AthenaKernel/errorcheck.h"

using namespace std;


namespace D3PD {

  TrigCaloClusterFillerTool::TrigCaloClusterFillerTool(const std::string& type, 
						       const std::string& name, 
						       const IInterface* parent)
    : BlockFillerTool<TrigCaloCluster>(type, name, parent)
  {
    // Avoid coverity warnings.
    mWriteBasics = true;
    mWriteHypoVars = true;
    mWriteDetails = true;
    book().ignore();

    declareProperty("WriteBasics", mWriteBasics=false, 
		    "Write basic information");
    declareProperty("WriteHypoVars", mWriteHypoVars=false, 
		    "Write variables used in hypo");
    declareProperty("WriteDetails", mWriteDetails=false, "Write detailed variables");
  }

  TrigCaloClusterFillerTool::~TrigCaloClusterFillerTool() {
  }

  StatusCode TrigCaloClusterFillerTool::book() {
    if (mWriteBasics) {
      CHECK(addVariable("quality", m_quality));
    }
    if (mWriteHypoVars) {
    }
    if  (mWriteDetails) {
      CHECK(addVariable("rawEnergy", m_rawEnergy));
      CHECK(addVariable("rawEta", m_rawEta));
      CHECK(addVariable("rawPhi", m_rawPhi));
      CHECK(addVariable("RoIWord", m_RoIword));
      CHECK(addVariable("nCells", m_nCells));
    }

    return StatusCode::SUCCESS;
  }

  StatusCode TrigCaloClusterFillerTool::fill(const TrigCaloCluster& x) {
    if (mWriteBasics) {
      *m_quality = x.quality();
    }
    if (mWriteHypoVars) {
    }
    if  (mWriteDetails) {
      *m_rawEnergy = x.rawEnergy();
      *m_rawEta = x.rawEta();
      *m_rawPhi = x.rawPhi();
      *m_RoIword = x.RoIword();
      *m_nCells = x.nCells();
    }

    return StatusCode::SUCCESS;
  }

}
