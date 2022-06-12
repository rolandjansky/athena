/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthViews/ViewHelper.h"
#include "CxxUtils/phihelper.h"
#include "ViewCreatorDVROITool.h"

#include "TrigCompositeUtils/TrigCompositeUtils.h"

ViewCreatorDVROITool::ViewCreatorDVROITool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent)
  : base_class(type, name, parent)
{}


StatusCode ViewCreatorDVROITool::initialize()  {
   ATH_CHECK( m_roisWriteHandleKey.initialize() );

   return StatusCode::SUCCESS;
}


StatusCode ViewCreatorDVROITool::attachROILinks( TrigCompositeUtils::DecisionContainer& decisions,
                                       const EventContext& ctx ) const
{
   ATH_MSG_DEBUG("ViewCreatorDVROITool::attachROILinks");
   // ===================================================================================== //
   // ===================================================================================== //
   // Create output RoI collection

   SG::WriteHandle<TrigRoiDescriptorCollection> roisWriteHandle =  TrigCompositeUtils::createAndStoreNoAux(m_roisWriteHandleKey, ctx);
   // ===================================================================================== //
   ATH_MSG_DEBUG("Create output RoI collection");

   // Only expect one object in container
   if(decisions.size()!=1) {
      ATH_MSG_DEBUG("Did not find exactly one decision object in decision container containing " << decisions.size() << " decisions");
   }

   for (TrigCompositeUtils::Decision* outputDecision : decisions) {

      ATH_MSG_DEBUG("Loop over decisions");

      const std::vector< TrigCompositeUtils::LinkInfo<xAOD::TrigCompositeContainer> > dvSeedLinks = TrigCompositeUtils::findLinks< xAOD::TrigCompositeContainer >(outputDecision, TrigCompositeUtils::featureString(), TrigDefs::lastFeatureOfType);

      if (dvSeedLinks.size() != 1) {
         ATH_MSG_DEBUG("Did not find exactly one most-recent " << m_featureLinkName << " for Decision object index "
         << outputDecision->index() << ", found " << dvSeedLinks.size());
      }

      for (const auto& dvLink : dvSeedLinks) {
         // This is needed to merge the RoIs from each dv
         std::unique_ptr<TrigRoiDescriptor> superRoI = std::make_unique<TrigRoiDescriptor>();
         superRoI->setComposite(true);
         superRoI->manageConstituents(false);

         if(!dvLink.link.isValid()) {
            ATH_MSG_DEBUG("Received invalid hitDV link from decision object! " << m_featureLinkName << " : " << dvLink.link);
            continue;
         }

         ATH_MSG_DEBUG(" --- Find Link");
         const xAOD::TrigCompositeContainer* dvCont = static_cast<const xAOD::TrigCompositeContainer*> ( (*dvLink.link)->container() );
         ATH_MSG_DEBUG(" --- Found " << dvCont->size() << " presel DV seeds linked from decision object.");
         ATH_MSG_DEBUG("     ");

         for(const xAOD::TrigComposite* dv : *dvCont) {
            if ( !(dv->isAvailable<float>("hitDV_seed_eta")
                && dv->isAvailable<float>("hitDV_seed_phi")) ) continue;

            float dvEta = dv->getDetail<float>("hitDV_seed_eta");
            float dvPhi = dv->getDetail<float>("hitDV_seed_phi");

            ATH_MSG_DEBUG( "DV seed eta = " << dvEta <<
                           " phi = " << dvPhi );

            // create ROIs
            ATH_MSG_DEBUG("Adding RoI to RoI container");
            ATH_MSG_DEBUG( "  ** Imposing Z constraint while building RoI" );
            double etaMinus = dvEta - m_roiEtaWidth;
            double etaPlus  = dvEta + m_roiEtaWidth;

            double phiMinus = CxxUtils::wrapToPi( dvPhi - m_roiPhiWidth );
            double phiPlus  = CxxUtils::wrapToPi( dvPhi + m_roiPhiWidth );

            // Should retrieve beamspot offset from somewhere
            double zMinus = -1. * m_roiZWidth;
            double zPlus  = m_roiZWidth;

            std::unique_ptr<TrigRoiDescriptor> newROI =
               std::make_unique<TrigRoiDescriptor>(   dvEta, etaMinus, etaPlus,
                                                      dvPhi, phiMinus, phiPlus,
                                                      0.,zMinus,zPlus );

            superRoI->push_back( newROI.release() );
         }


         roisWriteHandle->push_back(superRoI.release());
         const ElementLink< TrigRoiDescriptorCollection > roiEL = ElementLink< TrigRoiDescriptorCollection >( *roisWriteHandle, roisWriteHandle->size() - 1, ctx );
         outputDecision->setObjectLink( TrigCompositeUtils::roiString(), roiEL );

         ATH_MSG_DEBUG("PRINTING DECISION");
         ATH_MSG_DEBUG( *outputDecision );
      }
   }

   ATH_MSG_DEBUG("return StatusCode::SUCCESS");
   return StatusCode::SUCCESS;
}
