
/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthViews/ViewHelper.h"
#include "ViewCreatorFetchFromViewROITool.h"

using namespace TrigCompositeUtils;

ViewCreatorFetchFromViewROITool::ViewCreatorFetchFromViewROITool(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent) 
  {}


StatusCode ViewCreatorFetchFromViewROITool::initialize() {
  ATH_CHECK(m_roisWriteHandleKey.initialize());
  ATH_CHECK(m_inViewRoIKey.initialize());
  renounce(m_inViewRoIKey); // Will be read from in-View

  m_viewToFetchFromProbe=m_viewToFetchFrom + "_probe";
  return StatusCode::SUCCESS;
}


StatusCode ViewCreatorFetchFromViewROITool::attachROILinks(TrigCompositeUtils::DecisionContainer& decisions, const EventContext& ctx) const {
  SG::WriteHandle<TrigRoiDescriptorCollection> roisWriteHandle = createAndStoreNoAux(m_roisWriteHandleKey, ctx);
  
  for ( Decision* outputDecision : decisions ) { 
    LinkInfo<ViewContainer> viewToFetchFrom;
    if(!m_viewToFetchFrom.empty()){
      // Look for a specific View, keyed by the View's SG key
      const std::vector<LinkInfo<ViewContainer>> myViews = findLinks<ViewContainer>(outputDecision, viewString(), TrigDefs::allFeaturesOfType);
      bool found = false;
      for(const LinkInfo<ViewContainer>& v : myViews){
        ATH_MSG_DEBUG("view "<<v.link.dataID());
        if(v.link.dataID() == m_viewToFetchFrom or v.link.dataID() == m_viewToFetchFromProbe) {
          found = true;
          viewToFetchFrom = v;
          break;
        }
      }
      if(!found){
        ATH_MSG_ERROR("Of the " << myViews.size() << " Views in the history of Decision object with index " << outputDecision->index()
          << ", none came from a View called " << m_viewToFetchFrom);
        return StatusCode::FAILURE;
      }
    }
    else{

      // Assume the most recent View is the one we fetch from, and that there is exactly one most recent View after any merging
      const std::vector<LinkInfo<ViewContainer>> myView = findLinks<ViewContainer>(outputDecision, viewString(), TrigDefs::lastFeatureOfType);
      if (myView.size() != 1) {
        ATH_MSG_ERROR("Did not find exactly one most-recent '" << viewString() << "' for Decision object index " << outputDecision->index()
          << ", found " << myView.size());
        if (myView.size() > 1) {
          ATH_MSG_ERROR("Was this Decision Object was merged after having followed different reconstruction paths in previous Steps?");
          ATH_MSG_ERROR("Need more information about which of these Views to look in to find the desired '" << m_inViewRoIKey.key() << "' TrigRoiDescriptorCollection");
        }
        return StatusCode::FAILURE;
      }
      viewToFetchFrom = myView.at(0);
    }
    ATH_CHECK(viewToFetchFrom.isValid());

    SG::ReadHandle<TrigRoiDescriptorCollection> roiReadHandle = ViewHelper::makeHandle(*viewToFetchFrom.link, m_inViewRoIKey, ctx);
    ATH_CHECK(roiReadHandle.isValid());

    if (roiReadHandle->size() != 1) {
      ATH_MSG_ERROR("In-view ROI collection '" << m_inViewRoIKey.key() << "' for Decision object index " << outputDecision->index()
        << " has " << roiReadHandle->size() << " TrigRoiDescriptor inside. We require exactly one.");
      return StatusCode::FAILURE;
    }

    if (!m_doResize) { //default
      // This ROI was created in a View, it cannot currently be written out.
      // Make a copy in the full-event context, which can be recorded.
      roisWriteHandle->push_back( new TrigRoiDescriptor( *(roiReadHandle->at(0)) ) );
    }
    else { // force resize
      auto oldroi = roiReadHandle->at(0);
      ATH_MSG_DEBUG( " Old ROI size " << " eta="<< oldroi->eta() << " +- " << oldroi->etaPlus()-oldroi->eta() \
                                      << " phi="<< oldroi->phi() << " +- " << oldroi->phiPlus()-oldroi->phi() \
                                      << " zed="<< oldroi->zed() << " +- " << oldroi->zedPlus()-oldroi->zed());
      ATH_MSG_DEBUG( " New size (eta,phi,zed): " << m_roiEtaWidth.value() << ", " << m_roiPhiWidth.value() << ", " << m_roiZedWidth.value() );
      const double reta  = oldroi->eta();
      const double retap = (m_roiEtaWidth>0 ? reta + m_roiEtaWidth : oldroi->etaPlus());
      const double retam = (m_roiEtaWidth>0 ? reta - m_roiEtaWidth : oldroi->etaMinus());
      const double rphi  = oldroi->phi();
      const double rphip = (m_roiPhiWidth>0 ? rphi + m_roiPhiWidth : oldroi->phiPlus());
      const double rphim = (m_roiPhiWidth>0 ? rphi - m_roiPhiWidth : oldroi->phiMinus());
      const double rzed  = oldroi->zed();
      const double rzedp = (m_roiZedWidth>0 ? rzed + m_roiZedWidth : oldroi->zedPlus());
      const double rzedm = (m_roiZedWidth>0 ? rzed - m_roiZedWidth : oldroi->zedMinus());
      std::unique_ptr<TrigRoiDescriptor> roi = std::make_unique<TrigRoiDescriptor>(reta, retam, retap, rphi, rphim, rphip, rzed, rzedm, rzedp);
      ATH_MSG_DEBUG( " New ROI size " << " eta="<< roi->eta() << " +- " << roi->etaPlus()-roi->eta() \
                                      << " phi="<< roi->phi() << " +- " << roi->phiPlus()-roi->phi() \
                                      << " zed="<< roi->zed() << " +- " << roi->zedPlus()-roi->zed());
      roisWriteHandle->push_back( roi.release() );
    }

    const ElementLink<TrigRoiDescriptorCollection> newRoiEL = ElementLink<TrigRoiDescriptorCollection>(*roisWriteHandle, roisWriteHandle->size() - 1, ctx);

    outputDecision->setObjectLink(roiString(), newRoiEL);
  }

  return StatusCode::SUCCESS;
}
