/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArROD includes
#include "SuperCellVsCaloCellTestAlg.h"

#include "CaloEvent/CaloCellContainer.h"

#include "TFile.h"
#include "TH1D.h"

SuperCellVsCaloCellTestAlg::SuperCellVsCaloCellTestAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator )
 , m_sc2ccMappingTool("CaloSuperCellIDTool"), m_ccIdHelper(0)
{

   declareProperty("SuperCellContainer",m_scKey="SCell");
   declareProperty("CaloCellContainer",m_ccKey="AllCalo");

}


SuperCellVsCaloCellTestAlg::~SuperCellVsCaloCellTestAlg() {}


StatusCode SuperCellVsCaloCellTestAlg::initialize() {

   //get the mapping tool
   CHECK( m_sc2ccMappingTool.retrieve() );
   //and the id helper (used for making id hashes)
   CHECK( detStore()->retrieve (m_ccIdHelper, "CaloCell_ID") );

   for(int i=0;i<CaloCell_ID::NSUBCALO;i++) {
      m_eReso[i] = new TH1D(TString::Format("eReso%d",i),TString::Format("(SC_Energy - CC_Energy)/CC_Energy region %d",i),100,-10,10);
   }

   return StatusCode::SUCCESS;
}

StatusCode SuperCellVsCaloCellTestAlg::finalize() {

   TFile f1("hist.root","recreate");
   for(int i=0;i<CaloCell_ID::NSUBCALO;i++) m_eReso[i]->Write();
   f1.Close();

  return StatusCode::SUCCESS;
}

StatusCode SuperCellVsCaloCellTestAlg::execute() {  
   //get the supercells, calocells
   const CaloCellContainer* scells=0;
   CHECK( evtStore()->retrieve(scells, m_scKey) );
   const CaloCellContainer* ccells=0;
   CHECK( evtStore()->retrieve(ccells, m_ccKey) );

   //iterate over supercells, and build up a histogram of the resolution
   for(auto scell : *scells) {
      std::vector<Identifier> ccellIds = m_sc2ccMappingTool->superCellToOfflineID( scell->ID() );

      double cellE(0.);
      //use findCell function of CaloCellContainer, which takes an identifier hash 
      for(auto& ccellId : ccellIds) {
         const CaloCell* ccell = ccells->findCell(m_ccIdHelper->calo_cell_hash(ccellId));
         cellE += ccell->e();
      }

      m_eReso[m_ccIdHelper->sub_calo(scell->ID())]->Fill( (scell->e()-cellE)/cellE );
   }


  return StatusCode::SUCCESS;
}


