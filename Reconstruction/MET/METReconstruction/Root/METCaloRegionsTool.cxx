///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METCaloRegionsTool.cxx 
// Implementation file for class METCaloRegionsTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METCaloRegionsTool.h"

// MET EDM
#include "xAODMissingET/MissingETComposition.h"

namespace met {

  using std::vector;
  //
  using xAOD::IParticle;
  //
  using xAOD::CaloCluster;
  using xAOD::CaloClusterContainer;
  //
  using xAOD::MissingET;
  using xAOD::MissingETComposition;
  using xAOD::MissingETContainer;

  // Initialize CaloRegionNames
  const std::string METCaloRegionsTool::CaloRegionNames[METCaloRegionsTool::REGIONS_TOTAL] = 
  {
    "EMB",
    "EME",
    "FCAL",
    "HEC",
    "PEMB",
    "PEME",
    "TILE"
  };

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 

  // Constructors
  ////////////////
  METCaloRegionsTool::METCaloRegionsTool(const std::string& name) : 
    AsgTool(name)
  {
    declareProperty( "InputCollection", m_input_data_key       );
    declareProperty( "UseCells"       , m_calo_useCells = true );
  }

  // Destructor
  ///////////////
  METCaloRegionsTool::~METCaloRegionsTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METCaloRegionsTool::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  StatusCode METCaloRegionsTool::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 

  StatusCode METCaloRegionsTool::execute(xAOD::MissingET* metTerm_EMB, xAOD::MissingETComponentMap* /*metMap*/) {

    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    // Create the container and push back the new MET terms 
    MissingETBase::Types::bitmask_t source = MissingETBase::Source::Calo | MissingETBase::Source::clusterEM();
    MissingETContainer* metCont = dynamic_cast<MissingETContainer*>( metTerm_EMB->container() );
    for( int i=0; i < REGIONS_TOTAL; ++i) {
      // Create the new terms 
      if( i > 0 ) { 
        metCont->push_back( new MissingET(0.,0.,0.) ); 
      }
      // Set Name and Source
      metCont->at(i)->setName( CaloRegionNames[i] );      
      metCont->at(i)->setSource( source );      
    }
      
    // Either Cells or Clusters
    if(m_calo_useCells) {
      // Retrieve the cell container
      const CaloCellContainer*   caloCellCont = 0;
      #ifndef XAOD_STANDALONE
      if( evtStore()->retrieve(caloCellCont, m_input_data_key).isFailure() ) {
        ATH_MSG_WARNING("Unable to retrieve input cell cluster container");
        return StatusCode::SUCCESS;
      }
      #endif
      // Fill MET
      return fillCellMet(metCont,caloCellCont);
    } else {
      // Retrieve the calo container
      const CaloClusterContainer*   caloClusCont = 0;
      if( evtStore()->retrieve(caloClusCont, m_input_data_key).isFailure() ) {
        ATH_MSG_WARNING("Unable to retrieve input calo cluster container");
        return StatusCode::SUCCESS;
      }
      // Fill MET
      return fillClusterMet(metCont,caloClusCont);
    } // end if use clusters if/else

    // Debug information
    for( int i=0; i < REGIONS_TOTAL; ++i) {
      MissingET* currentTerm = (MissingET*) metCont->at(i);
      if(!currentTerm) continue;
      ATH_MSG_DEBUG ( "MET Calo term name " << currentTerm->name()   << 
                      ", source "           << currentTerm->source() <<
                      ", mpx "              << currentTerm->mpx()    <<
                      ", mpy "              << currentTerm->mpy()    <<
                      ", met "              << currentTerm->met()    <<
                      ", sumet "            << currentTerm->sumet() 
                    );      
    } // end debug information

    return StatusCode::SUCCESS;
  }

  // Find MetTerm name for a given sampling
  MissingET* METCaloRegionsTool::findMetTerm(MissingETContainer* metContainer, CaloSampling::CaloSample sample) 
  { 
    switch(sample) {
      case CaloSampling::EMB1:
      case CaloSampling::EMB2:
      case CaloSampling::EMB3:
        return metContainer->at(EMB); 
        //
      case CaloSampling::EME1:
      case CaloSampling::EME2:
      case CaloSampling::EME3:
        return metContainer->at(EME); 
        //
      case CaloSampling::FCAL0:
      case CaloSampling::FCAL1:
      case CaloSampling::FCAL2:
        return metContainer->at(FCAL); 
        //
      case CaloSampling::HEC0:
      case CaloSampling::HEC1:
      case CaloSampling::HEC2:
      case CaloSampling::HEC3:
        return metContainer->at(HEC); 
        //
      case CaloSampling::PreSamplerB:
        return metContainer->at(PEMB); 
        //
      case CaloSampling::PreSamplerE:
        return metContainer->at(PEME); 
        //
      case CaloSampling::TileBar0:
      case CaloSampling::TileBar1:
      case CaloSampling::TileBar2:
      case CaloSampling::TileExt0:
      case CaloSampling::TileExt1:
      case CaloSampling::TileExt2:
      case CaloSampling::TileGap1:
      case CaloSampling::TileGap2:
      case CaloSampling::TileGap3:
        return metContainer->at(TILE); 
        //
      default: return 0x0;
    }
  }

  // Fill Cell MET
  StatusCode METCaloRegionsTool::fillCellMet(xAOD::MissingETContainer* metContainer,
                                             const CaloCellContainer* caloCellContainer) {
    #ifndef XAOD_STANDALONE
    // Loop over all cells
    for( CaloCellContainer::const_iterator iCell=caloCellContainer->begin();
       iCell!=caloCellContainer->end(); ++iCell ) {

      // Retrieve the sampling 
      CaloSampling::CaloSample sample = (CaloSampling::CaloSample) (*iCell)->caloDDE()->getSampling();

      // Eta is only defined if Energy != 0
      if(fabs((*iCell)->energy())>0) {
       
        // Calculate Et/phi
        double et_cell  = (*iCell)->energy()/cosh((*iCell)->eta());
        double phi_cell = (*iCell)->phi();
     
        // Find the associated MET 
        MissingET* metTerm  = findMetTerm(metContainer, sample); 
        if(!metTerm) {
          ATH_MSG_WARNING("Invalid calo sample MET pointer");
          continue;
        }

        // Add to MET
        metTerm->add(et_cell*cos(phi_cell),
                     et_cell*sin(phi_cell),
                     et_cell);
      } // end if energy>0 if
    } // end of loop overall cells
    #else
    ATH_MSG_WARNING("Cell information is only available in athena framework");
    #endif
    return StatusCode::SUCCESS;
  } // end of fillCellMet

  // Fill Cluster MET
  StatusCode METCaloRegionsTool::fillClusterMet(xAOD::MissingETContainer* metContainer, 
                                                const CaloClusterContainer* caloClusContainer) {

    // Loop over all clusters
    for( CaloClusterContainer::const_iterator iClus=caloClusContainer->begin();
                                              iClus!=caloClusContainer->end(); ++iClus ) {
      // Loop over all samplings
      for(int iSample=CaloSampling::PreSamplerB;
              iSample<=CaloSampling::FCAL2; ++iSample) {

        CaloSampling::CaloSample sample = (CaloSampling::CaloSample) iSample;

        // Eta is only defined if Energy != 0
        if(fabs((*iClus)->eSample(sample))>0) {

          // Calculate Et/phi
          double et_sample  = (*iClus)->eSample(sample)/cosh((*iClus)->etaSample(sample));
          double phi_sample = (*iClus)->phiSample(sample);

          // Find the associated MET 
          MissingET* metTerm  = findMetTerm(metContainer, sample); 
          if(!metTerm) {
            ATH_MSG_WARNING("Invalid calo sample MET pointer");
            continue;
          }

          // Add to MET
          metTerm->add(et_sample*cos(phi_sample),
                       et_sample*sin(phi_sample),
                       et_sample);
        } // end of energy>0 if
      } // end of loop over samplings
    } // end of loop over clusters
    return StatusCode::SUCCESS;
  } // end of fillClusterMet

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

}
