/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterCnvTool.cxx 785752 2016-11-22 15:06:34Z ssnyder $

// EDM include(s):
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloClusterCellLinkContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

// Local include(s):
#include "CaloClusterCnvTool.h"


namespace {


typedef bool (xAOD::CaloCluster::*setterFunc) (const xAOD::CaloCluster::CaloSample,
                                               const float val);
                                               

void copyClusterVariables (const CaloCluster* aod,
                           xAOD::CaloCluster* xaod,
                           CaloCluster::variable_type varType,
                           setterFunc setter)
{
  std::vector<double> vals;
  if (! aod->getVariable (varType, vals, true))
    vals.resize (CaloSampling::Unknown, -999);

  for (unsigned iSamp=0;iSamp<CaloSampling::Unknown;++iSamp) {
    xAOD::CaloCluster_v1::CaloSample sNew=(xAOD::CaloCluster_v1::CaloSample)iSamp;
    CaloSampling:: CaloSample sOld=( CaloSampling:: CaloSample)iSamp;
    if (aod->hasSampling(sOld))
      ((*xaod).*setter) (sNew, vals[iSamp]);
  }
}


} // anonymous namespace


namespace xAODMaker {

   CaloClusterCnvTool::CaloClusterCnvTool( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) that the tool implements:
      declareInterface< ICaloClusterCnvTool >( this );
   }

   StatusCode CaloClusterCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode CaloClusterCnvTool::
   convert( const CaloCluster* aod,
            xAOD::CaloCluster* xaod,
            CaloClusterCellLinkContainer* ccclc ) const
   {

      //
      // Deal with the cell association:
      //
      if( aod->isCellLinkValid() && ccclc ) {

         ATH_MSG_DEBUG( "Found cluster with valid cell link" );

         CaloCluster::cell_iterator cit   = aod->begin();
         CaloCluster::cell_iterator cit_e = aod->end();
         if( cit != cit_e ) { //Protect against empty-cluster case

            //Get underlying cell container
            const CaloCellContainer* ccc = aod->getContainer( cit );

            if( ccc && ( !ccc->empty() ) ) { 

               CaloClusterCellLink* cccl = new CaloClusterCellLink( ccc );
               xaod->addCellLink( cccl );
               for( ; cit != cit_e; ++cit ) {
                  xaod->addCell( cit.getElement().index(), cit.getParameter() );
               }//end loop over cells of a cluster

               // Move cell links to external cell-link container:
               xaod->setLink( ccclc );

            } //end cell container ptr not NULL
            else {
               ATH_MSG_INFO( "Got NULL ptr to CaloCellContainer" );
            }

         }//end check cluster not-empty
         else {
            ATH_MSG_DEBUG( "Found cluster with empty list of constituents" );
         }
      }//end if isCellLink valid
      else {
         ATH_MSG_DEBUG( "Found cluster without valid cell link" );
      }

      // Set basic quantities:
      xaod->setSamplingPattern( aod->samplingPattern() );
      const xAOD::CaloCluster::ClusterSize clSize =
         static_cast< xAOD::CaloCluster::ClusterSize >( aod->getClusterSize() );
      xaod->setClusterSize( clSize );
      xaod->setEta0( ( double ) aod->eta0() );
      xaod->setPhi0( ( double ) aod->phi0() );
      xaod->setTime( aod->getTime() );

      xaod->setCalE(   aod->e   (P4SignalState::CALIBRATED) );
      xaod->setCalEta( aod->eta (P4SignalState::CALIBRATED) );
      xaod->setCalPhi( aod->phi (P4SignalState::CALIBRATED) );
      xaod->setCalM(   aod->m   (P4SignalState::CALIBRATED) );

      xaod->setRawE(   aod->e   (P4SignalState::UNCALIBRATED) );
      xaod->setRawEta( aod->eta (P4SignalState::UNCALIBRATED) );
      xaod->setRawPhi( aod->phi (P4SignalState::UNCALIBRATED) );
      xaod->setRawM(   aod->m   (P4SignalState::UNCALIBRATED) );

      xaod->setAltE(   aod->e   (P4SignalState::ALTCALIBRATED) );
      xaod->setAltEta( aod->eta (P4SignalState::ALTCALIBRATED) );
      xaod->setAltPhi( aod->phi (P4SignalState::ALTCALIBRATED) );
      xaod->setAltM(   aod->m   (P4SignalState::ALTCALIBRATED) );

      //
      // Copy the energy depositions per sampling:
      copyClusterVariables (aod, xaod,
                            CaloVariableType::ETA,
                            &xAOD::CaloCluster::setEta);
      copyClusterVariables (aod, xaod,
                            CaloVariableType::PHI,
                            &xAOD::CaloCluster::setPhi);
      copyClusterVariables (aod, xaod,
                            CaloVariableType::ENERGY,
                            &xAOD::CaloCluster::setEnergy);

      //
      // Copy bad-channel data:
      //
      xAOD::CaloClusterBadChannelList badChanList;
      const std::vector< CaloClusterBadChannelData >* bcvec =
         aod->getBadChannel();
      for( const CaloClusterBadChannelData& bcd : *bcvec ) {
         badChanList.emplace_back( bcd.getEta(), bcd.getPhi(),
                                   bcd.getLayer(), bcd.getFlag().packedData() );
      }
      xaod->setBadChannelList( badChanList );

      //
      // Copy the moments:
      //
      CaloCluster::moment_iterator mom_itr = aod->beginMoment();
      CaloCluster::moment_iterator mom_end = aod->endMoment();
      for( ; mom_itr != mom_end; ++mom_itr ) {

         // Forcibly convert between the two types:
	 const CaloCluster::moment_type aod_type =
           static_cast<CaloCluster::moment_type>(mom_itr.getMomentType());
         const xAOD::CaloCluster_v1::MomentType xaod_type =
            static_cast< xAOD::CaloCluster_v1::MomentType >( aod_type );

         // Insert the moment:
         xaod->insertMoment( xaod_type,
                             mom_itr.getMoment().getValue() );
         ATH_MSG_VERBOSE( "Copied moment " << aod_type );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode CaloClusterCnvTool::
   convert( const CaloClusterContainer* aod,
            xAOD::CaloClusterContainer* xaod ) const {
     
     CaloClusterContainer::const_iterator it = aod->begin();
     CaloClusterContainer::const_iterator itE = aod->end();
     CaloClusterCellLinkContainer* ccclc = nullptr;

     for( ; it!= itE; ++it){
       xAOD::CaloCluster* xcluster = new xAOD::CaloCluster();
       xaod->push_back( xcluster );
       CHECK( convert( *it, xcluster, ccclc) );       
     }
     
     return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
