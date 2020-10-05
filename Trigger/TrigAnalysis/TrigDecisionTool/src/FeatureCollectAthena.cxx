/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//only in full Athena
#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS)


#include "TrigDecisionTool/FeatureCollectAthena.h"
#include "TrigNavigation/AccessProxy.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "AthenaBaseComps/AthCheckMacros.h"

namespace Trig{
  namespace FeatureAccessImpl {
    // returns TrigPassBits object appropriate for given object 
    // if there is none returns 0 pointer
    // not this is not templated function, can be changed anytime needed
    const TrigPassBits* getBits(size_t sz, const HLT::TriggerElement* te, const std::string& /*label*/, HLT::NavigationCore* navigation ) {
      if (!sz)
	return 0;

      const TrigPassBits* bits(0);

      if ( navigation->getFeature(te, bits, "passbits" ) ){
	// We have to apply (vague) verification if there are any bits and that they match the feature container
	// this is if there are any bits, 
	// if so that they come from the same TE
	// if so that they match by size

	if ( bits && sz == bits->size() )
	  return bits; // they are good and can be used
      }
      return 0; // bits recording was problematic (Navigation will complain bitterly, no need to handle it here).
    }

    const TrigPassFlags* getFlags(size_t size, const HLT::TriggerElement* te, const std::string& label, HLT::NavigationCore* navigation ) {
      if(size==0) return 0;

      const TrigPassFlags* flags(0);
      if(navigation->getFeature(te, flags, label) && flags && flags->size()==size)
	return flags;

      return 0;
    }

    // L1 specific access ( objects not in the navigation, we need to pick them from different container in SG)
    template<class T>
    void l1collect(const HLT::TriggerElement* te, std::vector<Trig::Feature<T> >& data,  unsigned int condition,
		   const std::vector<T>& (LVL1_ROI::*method)() const, 
		   HLT::TrigNavStructure* navstructure) {

      auto navigation = dynamic_cast<HLT::NavigationCore*>(navstructure);

      // first we need to collect RoI descriptors
      std::vector<Trig::Feature<TrigRoiDescriptor> > rois;
      collect<TrigRoiDescriptor>(te, rois, "initialRoI", condition, "", navigation);

      const LVL1_ROI* lvl1(0);

      if ( !navigation || navigation->getAccessProxy()->retrieve(lvl1, "LVL1_ROI").isFailure() || lvl1 == 0 ) {
	return;
      }

      for(const Trig::Feature<TrigRoiDescriptor>& roi : rois) {
	for(const T& specific : (lvl1->*(method))() ) {
          if (specific.getROIWord() == roi.cptr()->roiWord()) {
	    data.push_back( Trig::Feature<T>( &specific, roi.te(), "") );
	    break;
          }
	}
      }
    }

    template<>
    void collect<Muon_ROI>(const HLT::TriggerElement* te, std::vector<Trig::Feature<Muon_ROI> >& data, const std::string&, unsigned int condition, const std::string&, HLT::TrigNavStructure* navigation) {
      l1collect<Muon_ROI>(te, data, condition, &LVL1_ROI::getMuonROIs, navigation);
    }

    template<>
    void collect<EmTau_ROI>(const HLT::TriggerElement* te, std::vector<Trig::Feature<EmTau_ROI> >& data, const std::string&, unsigned int condition, const std::string&, HLT::TrigNavStructure* navigation) {
      l1collect<EmTau_ROI>(te, data, condition, &LVL1_ROI::getEmTauROIs, navigation);
    }

    template<>
    void collect<Jet_ROI>(const HLT::TriggerElement* te, std::vector<Trig::Feature<Jet_ROI> >& data, const std::string&, unsigned int condition, const std::string&, HLT::TrigNavStructure* navigation) {
      l1collect<Jet_ROI>(te, data, condition, &LVL1_ROI::getJetROIs, navigation);
    }

    template<typename ROI,typename ROICONTAINER>
    void xAODcollect(const HLT::TriggerElement* te, std::vector<Trig::Feature<ROI> >& data, unsigned int condition, HLT::TrigNavStructure* navstructure, const std::string& sgkey){
      // first we need to collect RoI descriptors

      auto navigation = dynamic_cast<HLT::NavigationCore*>(navstructure);


      std::vector<Trig::Feature<TrigRoiDescriptor> > rois;
      collect<TrigRoiDescriptor>(te, rois, "initialRoI", condition, "", navigation);
    
      //get all EmTau RoIs
      const ROICONTAINER* cont;
      if (!navigation || navigation->getAccessProxy()->retrieve(cont,sgkey).isFailure()) {
	REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR,"Feature.cxx:xAODcollect") << "failed retrieving RoI container" << endmsg;

	REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR,"Feature.cxx:xAODcollect") << "\n" << (navigation ? navigation->getAccessProxy()->dump() : "") << endmsg;      
      

	return;
      }

      typename ROICONTAINER::const_iterator emtauit;
      //ROI* found(0);
      for(const auto& it : rois){
	for(emtauit=cont->begin();emtauit!=cont->end();++emtauit){
	  if((*emtauit)->roiWord() == it.cptr()->roiWord()){
	    data.push_back( Trig::Feature<ROI>(*emtauit, it.te(), "") );
	    break;
	  }
	}
      }
    }

    template<>
    void collect<xAOD::EmTauRoI>(const HLT::TriggerElement* te, std::vector<Trig::Feature<xAOD::EmTauRoI> >& data, const std::string&, unsigned int condition, const std::string&, HLT::TrigNavStructure* navigation) {
      xAODcollect<xAOD::EmTauRoI,xAOD::EmTauRoIContainer>(te, data, condition, navigation,"LVL1EmTauRoIs");
    }

    template<>
    void collect<xAOD::MuonRoI>(const HLT::TriggerElement* te, std::vector<Trig::Feature<xAOD::MuonRoI> >& data, const std::string&, unsigned int condition, const std::string&, HLT::TrigNavStructure* navigation) {
      xAODcollect<xAOD::MuonRoI,xAOD::MuonRoIContainer>(te, data, condition, navigation,"LVL1MuonRoIs");
    }

    template<>
    void collect<xAOD::JetRoI>(const HLT::TriggerElement* te, std::vector<Trig::Feature<xAOD::JetRoI> >& data, const std::string&, unsigned int condition, const std::string&, HLT::TrigNavStructure* navigation) {
      xAODcollect<xAOD::JetRoI,xAOD::JetRoIContainer>(te, data, condition, navigation,"LVL1JetRoIs");
    }


    template<>
    TrigPassFlags getFlags(const Trig::Feature<EmTau_ROI>&, const TrigPassFlags *, HLT::NavigationCore* ) {
      return TrigPassFlags();
    }
    template<>
    TrigPassFlags getFlags(const Trig::Feature<Muon_ROI>&, const TrigPassFlags *, HLT::NavigationCore* ) {
      return TrigPassFlags();
    }
    template<>
    TrigPassFlags getFlags(const Trig::Feature<Jet_ROI>&, const TrigPassFlags *, HLT::NavigationCore* ) {
      return TrigPassFlags();
    }
  } // EOF namespace FeatureAccessImpl 
} // EOF namespace Trig
#endif // full Athena env
