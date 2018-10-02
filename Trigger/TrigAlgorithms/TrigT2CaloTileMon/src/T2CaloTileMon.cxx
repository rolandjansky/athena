/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2CaloTileMon.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloTileMon
//
// AUTHOR:   D.O. Damazio, Jens Damaske
//
// - Add variables for job option controlled region limits, set defaults
//   to most likely values.
// - Add function EtaPhiRange to return the maximum and minimum eta or phi
//   values to use when calculating energy sums over a region  - R. Soluk
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "TrigT2CaloTileMon/TrigT2TileJet.h"
#include "TrigT2CaloTileMon/TrigT2Tower.h"

#include "TrigT2CaloTileMon/T2CaloTileMon.h"
#include "TrigT2CaloCommon/IAlgToolCalo.h"

class ISvcLocator;

T2CaloTileMon::T2CaloTileMon(const std::string & name, ISvcLocator* pSvcLocator) :
               T2CaloBase(name, pSvcLocator)
{
   declareProperty("TrigEMClusterKey",m_trigEmClusterKey = "T2CaloTrigEMCluster");
   declareProperty("L1ForceEta",m_l1eta = -10.0);
   declareProperty("L1ForcePhi",m_l1phi = -10.0);
   declareProperty("EtaMin",m_etamin);
   declareProperty("EtaMax",m_etamax);
   declareProperty("PhiMin",m_phimin);
   declareProperty("PhiMax",m_phimax);

}

T2CaloTileMon::~T2CaloTileMon()
{
}


HLT::ErrorCode T2CaloTileMon::hltInitialize()
{
   return HLT::OK;
}


HLT::ErrorCode T2CaloTileMon::hltExecute(const HLT::TriggerElement* inputTE,
					HLT::TriggerElement* /*outputTE*/)
{
   // Time total T2CaloTileMon execution time.
   if ( m_timersvc ) m_timer[0]->start();

#ifndef NDEBUG
   ATH_MSG_INFO( "in execute()"  );
#endif

   const TrigRoiDescriptor* roiDescriptor = 0;
   HLT::ErrorCode hltStatus = getFeature(inputTE, roiDescriptor);

   if ( hltStatus == HLT::OK ) {
#ifndef NDEBUG
     ATH_MSG_DEBUG( *roiDescriptor  );
#endif
   }
   else
   {
      ATH_MSG_WARNING( " Failed to find RoiDescriptor "  );
      return hltStatus;
   }

   // End LVL1 part
   double etamin, etamax, phimin, phimax;
   if ( (m_l1eta<-9.9)&&(m_l1phi<-9.9)){
      etamin = std::max(-2.5,roiDescriptor->eta() - m_etaWidth);
      etamax = std::min( 2.5,roiDescriptor->eta() + m_etaWidth);
      phimin = HLT::wrapPhi(roiDescriptor->phi() - m_phiWidth);
      phimax = HLT::wrapPhi(roiDescriptor->phi() + m_phiWidth);
   } else {
      etamin = std::max(-2.5,m_l1eta - m_etaWidth);
      etamax = std::min( 2.5,m_l1eta + m_etaWidth);
      phimin = HLT::wrapPhi(m_l1phi - m_phiWidth);
      phimax = HLT::wrapPhi(m_l1phi + m_phiWidth);
   }

#ifndef NDEBUG
   ATH_MSG_DEBUG( " etamin = "<< etamin  );
   ATH_MSG_DEBUG( " etamax = "<< etamax  );
   ATH_MSG_DEBUG( " phimin = "<< phimin  );
   ATH_MSG_DEBUG( " phimax = "<< phimax  );
#endif

   ///   TrigRoiDescriptor* newroi = new TrigRoiDescriptor( roiDescriptor->eta(), etamin, etamax, 
   ///			                                   roiDescriptor->phi(), phimin, phimax ); 

   ///   attachFeature( outputTE, newroi, "T2CaloTimeMon");
   /// Tile RS shoudl map to it's own requirements niw - we can 
   ///still use d-pi .. pi wrapping
 
   const TrigRoiDescriptor newroi( roiDescriptor->eta(), etamin, etamax, 
				   roiDescriptor->phi(), phimin, phimax ); 


#ifndef NDEBUG
   ATH_MSG_DEBUG( " Making TrigEMCluster " );
#endif


   //std::vector<xAOD::TrigEMCluster*> vec_clus;
   // Ok, ignoring LVL1 and forcing a position

   ToolHandleArray<IAlgToolCalo>::iterator it = m_emAlgTools.begin();
   if ( m_timersvc ) m_timer[1]->start();

   //TrigEMCluster* ptrigEmCluster = new TrigT2TileJet();
   xAOD::TrigEMCluster* ptrigEmCluster = new xAOD::TrigEMCluster();
   for (; it < m_emAlgTools.end(); it++)
   {

     //      if ((*it)->execute(*ptrigEmCluster,etamin,etamax,phimin,phimax).isFailure() )
     if ((*it)->execute(*ptrigEmCluster,newroi, caloDDENull).isFailure() )
       {
         ATH_MSG_WARNING( "T2Calo AlgToolTileMon returned Failure"  );
         return HLT::TOOL_FAILURE;
      }
   }
   delete ptrigEmCluster;  // in for loop will be destroyed automatically

   if ( m_timersvc ) m_timer[1]->stop();

   // Time total T2CaloTileMon execution time.
   if ( m_timersvc ) m_timer[0]->stop();

   return HLT::OK;
}


HLT::ErrorCode T2CaloTileMon::hltFinalize(){

#ifndef NDEBUG
    ATH_MSG_INFO( "in finalize()"  );
#endif

   return HLT::OK;
}
