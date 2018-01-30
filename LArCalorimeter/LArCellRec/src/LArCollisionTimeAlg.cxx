/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCellRec/LArCollisionTimeAlg.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"
#include "Identifier/Identifier.h"
#include "CaloIdentifier/CaloCell_ID.h"


//Constructor
LArCollisionTimeAlg:: LArCollisionTimeAlg(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_nevt(0),
    m_calo_id(NULL),m_noiseTool("CaloNoiseTool/calonoise")
  {
    declareProperty("NoiseTool", m_noiseTool);
    declareProperty("cellContainerName", m_cellsContName="AllCalo" );
    declareProperty("collisionTime", m_collTimeName="LArCollisionTime" );
  }
  
//__________________________________________________________________________
//Destructor
  LArCollisionTimeAlg::~LArCollisionTimeAlg()
  {
    ATH_MSG_DEBUG ("LArCollisionTimeAlg destructor called");
  }

//__________________________________________________________________________
StatusCode LArCollisionTimeAlg::initialize()
  {
    ATH_MSG_DEBUG ("LArCollisionTimeAlg initialize()");

    //retrieve ID helpers 
    ATH_CHECK( detStore()->retrieve( m_caloIdMgr ) );
    m_calo_id      = m_caloIdMgr->getCaloCell_ID();


    // get calonoise tool 
    if (m_noiseTool) {
      ATH_CHECK( m_noiseTool.retrieve() );
    }

    ATH_CHECK( m_cellsContName.initialize() );
    ATH_CHECK( m_collTimeName.initialize() );

    return StatusCode::SUCCESS; 

  }

//__________________________________________________________________________
StatusCode LArCollisionTimeAlg::finalize()
  {
    ATH_MSG_DEBUG ("LArCollisionTimeAlg finalize()");
    return StatusCode::SUCCESS; 
  }
  
//__________________________________________________________________________
StatusCode LArCollisionTimeAlg::execute()
  {
    //.............................................
    
    ATH_MSG_DEBUG ("LArCollisionTimeAlg execute()");

   m_nevt++;

  // Get the CaloCellContainer
  SG::ReadHandle<CaloCellContainer> cell_container (m_cellsContName);

  if(!cell_container.isValid()) {
      ATH_MSG_INFO (" Could not get pointer to Cell Container ");
      // Construct the output object
      SG::WriteHandle<LArCollisionTime> larTime (m_collTimeName);
      ATH_CHECK( larTime.record (std::make_unique<LArCollisionTime>()) );

      return StatusCode::SUCCESS;
  }

  // Loop over the CaloCellContainer
  int ncellA=0;
  int ncellC=0;
  float energyA=0.;
  float energyC=0.;
  float timeA=0.;
  float timeC=0.;

  CaloCellContainer::const_iterator first_cell = cell_container->begin();
  CaloCellContainer::const_iterator end_cell   = cell_container->end();
  ATH_MSG_DEBUG ("*** Start loop over CaloCells in LArCollisionTimeAlg");
  for (; first_cell != end_cell; ++first_cell)
  {
      Identifier cellID = (*first_cell)->ID();
      if (m_calo_id->is_tile(cellID)) continue;

      double eta   =  (*first_cell)->eta();
      if (std::fabs(eta)<1.5) continue;

      uint16_t provenance = (*first_cell)->provenance();
//
// check time correctly available
//   for Data:    offline Iteration   0x2000 time available, 0x0100 Iteration converted, not 0x0200 and not 0x0400 (bad cells), 0x00A5 (correctly calibrated)
//                DSP time  0x1000 : cell from DSP, 0x2000 time available, not 0x0200 and not 0x0400
//   for MC   check time available  0x2000, and not bad cel

      uint16_t mask1 = 0x3DFF;
      if (!m_iterCut) mask1 = 0x3CFF;
      uint16_t cut1 = 0x21A5;
      if (!m_iterCut) cut1 = 0x20A5;

      if ( (provenance & mask1) != cut1 && (provenance & 0x3C00) != 0x3000 && !m_isMC) continue;
      if ( (provenance & 0x2C00) != 0x2000 && m_isMC) continue;

      double energy=  (*first_cell)->energy();
      double noise = -1;
      if (!m_noiseTool.empty()) {
        noise = m_noiseTool->totalNoiseRMS((*first_cell));
      }
      double signif=9999.;
      if (noise>0.) signif = energy/noise;
      if (signif < 5.) continue;

      double ecut=-1;
      if (m_calo_id->is_lar_fcal(cellID) && m_calo_id->calo_sample(cellID)==CaloCell_ID::FCAL0) ecut=1200.;
      if (m_calo_id->is_em_endcap_inner(cellID) ) ecut=250.;

      if (ecut<0.) continue;
      if (energy<ecut) continue;

      double time = (*first_cell)->time();

      if (eta>0.) {
          ncellA += 1;
          energyA += energy;
          timeA += time;
      }
      else {
          ncellC += 1;
          energyC += energy;
          timeC += time;
      }
          
  }

  if (ncellA>0) timeA = timeA/((float)(ncellA));
  if (ncellC>0) timeC = timeC/((float)(ncellC));

  if (ncellA>m_minCells && ncellC > m_minCells && std::fabs(timeA-timeC)<m_timeCut) setFilterPassed(true);
  else                        setFilterPassed(false);

  //std::cout << " ncellA, ncellA, energyA, energyC, timeA, timeC  " << ncellA << " " << ncellC << " " << energyA << " " << energyC << " " << timeA << " " << timeC << std::endl;
  auto tmplarTime = std::make_unique<LArCollisionTime>(ncellA,ncellC,energyA,energyC,timeA,timeC);
  // Construct the output object
  SG::WriteHandle<LArCollisionTime> larTime (m_collTimeName);
  if (! larTime.put (std::move (tmplarTime))  )  {
     ATH_MSG_WARNING( "Could not record the LArCollisionTime object with key "<<m_collTimeName );

  }

  return StatusCode::SUCCESS;
}
