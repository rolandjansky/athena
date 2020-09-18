///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#if defined(XAOD_STANDALONE) || defined(XAOD_ANALYSIS)
#else
#include "CaloEvent/CaloCellContainer.h"
#include "CaloConditions/CaloNoise.h"
#endif

namespace met {

  using std::vector;
  //
  using xAOD::IParticle;
  //
  using xAOD::CaloCluster;
  using xAOD::CaloClusterContainer;
  //
  using xAOD::MissingET;
  using xAOD::MissingETContainer;

  // Initialize s_CaloRegionNames
  const std::string METCaloRegionsTool::s_CaloRegionNames[METCaloRegionsTool::REGIONS_TOTAL] = 
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
    declareProperty( "InputCollection", m_input_data_key            );
    declareProperty( "UseCells"       , m_calo_useCells      = true );
    declareProperty( "DoTriggerMET"   , m_calo_doTriggerMet  = true );
  }

  // Destructor
  ///////////////
  METCaloRegionsTool::~METCaloRegionsTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METCaloRegionsTool::initialize()
  {
    ATH_MSG_DEBUG("Initializing " << name() << "...");

    #if defined(XAOD_STANDALONE) || defined(XAOD_ANALYSIS)
    #else
    ATH_CHECK( m_noiseCDOKey.initialize() );
    #endif
    // Either Cells or Clusters
    if(m_calo_useCells) {
      ATH_CHECK( m_caloCellKey.assign(m_input_data_key));
    } else {
      ATH_CHECK( m_caloClusterKey.assign(m_input_data_key));
    } // end if use clusters if/else
    ATH_CHECK( m_caloCellKey.initialize(m_calo_useCells) );
    ATH_CHECK( m_caloClusterKey.initialize(!m_calo_useCells) );

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

  StatusCode METCaloRegionsTool::execute(xAOD::MissingET* metTerm_EMB, xAOD::MissingETComponentMap* /*metMap*/) const
  {

    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    // Create the container and push back the new MET terms 
    MissingETBase::Types::bitmask_t source = MissingETBase::Source::Calo | MissingETBase::Source::clusterEM();
    MissingETContainer* metCont = static_cast<MissingETContainer*>( metTerm_EMB->container() );

    // Push region terms to the container
    for( int i=0; i < REGIONS_TOTAL; ++i) {
      // Create the new terms 
      if( i > 0 ) { 
        metCont->push_back( new MissingET(0.,0.,0.) ); 
      }
      // Set Name and Source
      metCont->at(i)->setName( s_CaloRegionNames[i] );      
      metCont->at(i)->setSource( source );      
    }
    // The last term is Trigger MET if asked for by the user
    if( m_calo_useCells && m_calo_doTriggerMet ) {
      std::string termName = "";
      for( int i=0; i<3; ++i) {
        switch(i) {
          case 0: termName = "AllCells"; break;
          case 1: termName = "Cells_Abs2S"; break;
          case 2: termName = "Cells_Abs2S_m5S"; break;
        }
        metCont->push_back( new MissingET(0.,0.,0.) ); 
        metCont->at(REGIONS_TOTAL+i)->setName( termName );      
        metCont->at(REGIONS_TOTAL+i)->setSource( MissingETBase::Source::Calo );      
      }
    }
      
    StatusCode sc = StatusCode::SUCCESS;

    // Either Cells or Clusters
    if(m_calo_useCells) {
      // Retrieve the cell container
      SG::ReadHandle<CaloCellContainer> caloCellCont(m_caloCellKey);

      #if defined(XAOD_STANDALONE) || defined(XAOD_ANALYSIS)
      #else
      if (!caloCellCont.isValid()) {
        ATH_MSG_WARNING("Couldn't set up ReadHandle for Calo Cell Container "<<m_input_data_key);
        ATH_MSG_WARNING("Unable to retrieve input cell cluster container");
          return StatusCode::SUCCESS;

      }
      #endif
      // Fill MET
      sc = fillCellMet(metCont,caloCellCont.cptr());
    } else {
      // Retrieve the calo container
      SG::ReadHandle<CaloClusterContainer> caloClusCont(m_caloClusterKey);
      if (!caloClusCont.isValid()) {
        ATH_MSG_WARNING("Unable to retrieve input calo cluster container");
          return StatusCode::SUCCESS;

      }
      // Fill MET
      sc=fillClusterMet(metCont,caloClusCont.cptr());
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

    if(sc.isFailure()) { 
      ATH_MSG_WARNING("Unable to fill cell/cluster MET"); 
    } 
    
    return StatusCode::SUCCESS;
  }

  // Find MetTerm name for a given sampling
  MissingET* METCaloRegionsTool::findMetTerm(MissingETContainer* metContainer, CaloSampling::CaloSample sample) const 
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
                                             const CaloCellContainer* caloCellContainer) const
  {
    #if defined (XAOD_STANDALONE) || defined(XAOD_ANALYSIS)
    ATH_MSG_WARNING("Cell information is only available in athena framework");
    #else
    SG::ReadCondHandle<CaloNoise> noiseHdl{m_noiseCDOKey};
    const CaloNoise* noiseCDO=*noiseHdl;
    // Loop over all cells
    for (const CaloCell* cell : *caloCellContainer) {

      // Retrieve the sampling 
      CaloSampling::CaloSample sample = (CaloSampling::CaloSample) cell->caloDDE()->getSampling();

      // Calculate Et/phi
      double e_cell   = cell->energy();
      double et_cell  = e_cell/cosh(cell->eta());
      double phi_cell = cell->phi();
     
      // Find the associated MET 
      MissingET* metTerm  = findMetTerm(metContainer, sample); 
      if(!metTerm) {
        ATH_MSG_WARNING("Invalid calo sample MET pointer");
        continue;
      }

      // Add to MET for the Calo regions
      metTerm->add(et_cell*cos(phi_cell),
                   et_cell*sin(phi_cell),
                   et_cell);

      // Trigger MET
      if(m_calo_doTriggerMet) {
        #if defined(XAOD_STANDALONE) || defined(XAOD_ANALYSIS)
        double noise_cell = 0;
        #else
        double noise_cell = noiseCDO->getNoise(cell->ID(),cell->gain());
        #endif
        // All cells
        metContainer->at(REGIONS_TOTAL)->add(et_cell*cos(phi_cell),
                                             et_cell*sin(phi_cell),
                                             et_cell);
        // |E| > 2*sigma
        if( fabs(e_cell) <  2.0*noise_cell ) continue;
        metContainer->at(REGIONS_TOTAL+1)->add(et_cell*cos(phi_cell),
                                               et_cell*sin(phi_cell),
                                               et_cell);
        // E > -5*sigma
        if( e_cell       < -5.0*noise_cell ) continue;
        metContainer->at(REGIONS_TOTAL+2)->add(et_cell*cos(phi_cell),
                                               et_cell*sin(phi_cell),
                                               et_cell);
      }
    } // end of loop overall cells
    #endif
    return StatusCode::SUCCESS;
  } // end of fillCellMet

  // Fill Cluster MET
  StatusCode METCaloRegionsTool::fillClusterMet(xAOD::MissingETContainer* metContainer, 
                                                const CaloClusterContainer* caloClusContainer) const
  {

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
