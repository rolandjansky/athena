/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     EgammaAllFex.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloEgamma
// 
// AUTHOR:   D.O. Damazio
// 
//
// ********************************************************************

#include "CaloIdentifier/LArEM_ID.h"

//#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include "EgammaAllFex.h"
#include "TrigT2CaloCommon/Calo_Def.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

EgammaAllFex::EgammaAllFex(const std::string & type, const std::string & name, 
                   const IInterface* parent): IReAlgToolCalo(type, name, parent)
		   {
		   declareProperty("IncludeHad",  m_includeHad = false );
#ifndef NDEBUG
	// Create Geometry object
  // 0 -> CaloType EM, 2 -> Second Layer
  // m_geometry[0] = new T2Geometry(0,2);
#endif
}

EgammaAllFex::~EgammaAllFex(){
}

StatusCode EgammaAllFex::execute(xAOD::TrigEMCluster &rtrigEmCluster,
				 const IRoiDescriptor& roi,
				 const CaloDetDescrElement*& /*caloDDE*/,
                                 const EventContext& context) const { 
  
  // Time total AlgTool time 
  // reset error
  int ncells = 0;
  // LVL1 positions
  float etaL1=rtrigEmCluster.eta();
  float phiL1=rtrigEmCluster.phi();
  
  double totalEnergy = 0;
  double totalEnergyEM = 0;
  CaloSampling::CaloSample samp;

  ATH_MSG_DEBUG( "in execute(TrigEMCluster&)" );

	// Time to access RegionSelector

  for(DETID dd = TTEM; dd <= TTHEC; dd=(DETID)( ((int)dd)+1) ){
	  for(int sampling=0;sampling<4;sampling++) {
            LArTT_Selector<LArCellCont> sel;
	    ATH_CHECK( m_dataSvc->loadCollections(context, roi, dd, sampling, sel) );


      for(const LArCell* larcell : sel) {

        double energyCell = larcell->energy();
        totalEnergy += energyCell;
        if( dd==TTEM) totalEnergyEM += energyCell;
        //samp = CaloSampling::getSampling(*larcell);
        samp = larcell->caloDDE()->getSampling();
        rtrigEmCluster.setEnergy(samp,rtrigEmCluster.energy(samp) + energyCell);
        rtrigEmCluster.setRawEnergy(samp,rtrigEmCluster.rawEnergy(samp) + energyCell);
        ncells++;

      }  // end of loop over sampling
      } // end of loop over all samples   
    } // end of loop over DetID

    // TileCAL
          // Finished to access RegionSelector
          TileCellCollection seltile;
          ATH_CHECK(m_dataSvc->loadCollections(context, roi, seltile));


    for(const TileCell* tilecell : seltile) {

      double energyCell = tilecell->energy();
      totalEnergy += energyCell;
      //samp = CaloSampling::getSampling(*tilecell);
      samp = tilecell->caloDDE()->getSampling();
      rtrigEmCluster.setEnergy(samp,rtrigEmCluster.energy(samp) + energyCell);
      rtrigEmCluster.setRawEnergy(samp,rtrigEmCluster.rawEnergy(samp) + energyCell);
      ncells++;

    }  // end of loop over sampling

    
  // Update cluster Variables

  if ( m_includeHad )
  rtrigEmCluster.setEnergy(totalEnergy);
  else
  rtrigEmCluster.setEnergy(totalEnergyEM);
  rtrigEmCluster.setEta(etaL1);
  rtrigEmCluster.setPhi(phiL1);
  rtrigEmCluster.setNCells(ncells);
        
  // Finished save EMShowerMinimal time


  return StatusCode::SUCCESS;
}
