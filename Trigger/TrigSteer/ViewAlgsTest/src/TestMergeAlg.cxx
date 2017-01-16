/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//#undef NDEBUG 
//--> enable assert
//#include <cassert>

#include <sstream>
#include <sstream>
#include "CxxUtils/make_unique.h"
#include "L1Decoder/TrigIdentifiers.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "ViewAlgs/TrigCompositeTraversal.h"
#include "ViewAlgs/ViewAlgs.h"
#include "FourMomUtils/xAODP4Helpers.h"

#include "./TestMergeAlg.h"

TestMergeAlg::TestMergeAlg(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    //m_inputCollNames("InputCollNames"),
    m_outputRoisContainer("MergedRoIsContainer"), 
    m_outputProxyContainer("MergedOutput"), 
    m_outputProxyAuxContainer("MergedOutputAux."),
    m_distance(-1.) 
{
  declareProperty("RoIDistance", m_distance, "Set distance for RoI Association criteria (-1=no distance applied, merge all");
  //  declareProperty("InputCollNames", m_inputCollNames, "List of Names of input collections");
  declareProperty("InputCollections", m_inputVectorProxyColl, "List of inputs to be merged" );
  declareProperty("MergedRoIsContainer", m_outputRoisContainer, "Name of the RoIs object produced by the merging");
  declareProperty("MergedRoIsContainerAux", m_outputRoisContainer, "Name of the RoIs object produced by the merging");
  declareProperty("MergedProxyContainer", m_outputProxyContainer, "Name of the decisions object (wiht links to specific RoIs)");
  declareProperty("MergedProxyContainerAux", m_outputProxyAuxContainer, "");
}




StatusCode TestMergeAlg::initialize() {
  // here separate names of collections
  // std::string separator(",");
  // std::string::size_type prev_pos = 0, pos = 0;
  // while((pos = m_inputCollNames.find(separator, pos)) != std::string::npos){
  //   std::string substring( m_inputCollNames.substr(prev_pos, pos-prev_pos) );
  //   if (substring=="") continue;
  //   m_inputVectorProxyColl.push_back( SG::ReadHandle< xAOD::TrigCompositeContainer >(substring));
  //   prev_pos = ++pos;    
  // }
  ATH_CHECK( m_inputVectorProxyColl.initialize() );

  ATH_MSG_DEBUG("Vector of Collections to merge has size: "<<m_inputVectorProxyColl.size());
  return StatusCode::SUCCESS;
}



StatusCode TestMergeAlg::validCollections(const std::vector<SG::ReadHandle<xAOD::TrigCompositeContainer>>& input ) const{
  for (auto coll: input ) {
    if ( not coll.isValid() ) {
      ATH_MSG_ERROR("No input object from previous stage with name "<<coll.name());
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}




StatusCode TestMergeAlg::execute() {
  //if ( ! m_inputVectorProxyColl.isValid() ) return StatusCode::FAILURE;
  //  if (! validCollections()) return StatusCode::FAILURE;

  //copying from Tomasz'
  // IProxyDict * view = 0;
  // // redirect handles to that view
  // CHECK(m_outputRoisContainer.setStore(view));
  
  std::vector<SG::ReadHandle<xAOD::TrigCompositeContainer>> input = m_inputVectorProxyColl.makeHandles();
  CHECK( validCollections(input) );

  m_outputRoisContainer = CxxUtils::make_unique< TrigRoiDescriptorCollection >();  
  m_outputProxyContainer = CxxUtils::make_unique< xAOD::TrigCompositeContainer >();
  m_outputProxyAuxContainer = CxxUtils::make_unique< xAOD::TrigCompositeAuxContainer>();  
  m_outputProxyContainer->setStore(m_outputProxyAuxContainer.ptr());

  int full_size = 0;
  for (auto & coll: input) {
    // ATH_CHECK( coll.isValid() ); // Done in TestMergeAlg::validCollections
    ATH_MSG_DEBUG("Running on " << coll->size() );
    full_size += coll->size();
  }

  if (full_size == 0)  return StatusCode::SUCCESS;

  std::map<std::string, std::vector<int> > groupsOfMergedRoIs; // map group index -> list of roIs position in the container to go to the group
  int ngroup = 0; 
  std::stringstream groupname;
  if (m_distance >= 0. && input.size()==1){
    for (auto coll: input) {
      int nRoI = 0;
      std::string collname=coll.name();
      for ( auto it = (*coll.cptr()).begin(); it != (*coll.cptr()).end(); it++ ) {
        groupname.str("");
        groupname << collname << ngroup;
        std::vector<int>* groupPtr = &(groupsOfMergedRoIs[groupname.str()]);
        if (find(groupPtr->begin(), groupPtr->end(), nRoI) == groupPtr->end()){ 
          if (nRoI!=0) ngroup++;
          //groupsOfMergedRoIs[groupname.str()]=std::vector<int>(); // not needed
          groupPtr->push_back(nRoI);
          ATH_MSG_DEBUG("RoI "<< nRoI <<" Generated new group: "<< ngroup);
        }
        else {
          ATH_MSG_DEBUG("RoI "<< nRoI <<" is in group "<< ngroup);
        }
        auto comp = (*it);
        auto objWithLinkToRoI = ViewAlgs::find(comp, ViewAlgs::HasObject("initialRoI"));

        //merging due to distance
        if ( objWithLinkToRoI ) {
          auto roi = objWithLinkToRoI->objectLink<TrigRoiDescriptorCollection>("initialRoI");      
          auto it2 = it+1;
          while(it2 != (*coll.cptr()).end()){
            auto comp2 = (*it2);
            auto obj2WithLinkToRoI = ViewAlgs::find(comp2, ViewAlgs::HasObject("initialRoI"));
            if ( obj2WithLinkToRoI ) {
              auto roi2 = obj2WithLinkToRoI->objectLink<TrigRoiDescriptorCollection>("initialRoI");      
              float_t deltaR = sqrt( pow(((*roi)->eta() - (*roi2)->eta()),2.) 
                                   + pow(xAOD::P4Helpers::deltaPhi((*roi)->phi(), (*roi2)->phi()), 2.));
              if ( deltaR < m_distance ) {
                auto i = std::distance((*coll.cptr()).begin(), it2);
                groupPtr->push_back(i);
                ATH_MSG_DEBUG("found DeltaR ="<<deltaR <<" for pos="<<i);
              }
            }
            ++it2;
          }
        }
        nRoI++;
      }
    }
  }
  else {
    for (auto coll: input) {
      groupname.str("");
      groupname << coll.name() << ngroup;
      for (unsigned int nRoI = 0; nRoI < coll->size(); nRoI++) {
        groupsOfMergedRoIs[groupname.str()].push_back(nRoI);
      }
    }
  }

  ATH_MSG_DEBUG("Merging RoIs into "<<ngroup+1 <<" groups");

  std::stringstream name;
  for (int group = 0; group <= ngroup; group++){
    //create new TC for each group
    xAOD::TrigComposite* proxy  = new xAOD::TrigComposite();          
    m_outputProxyContainer->push_back(proxy);
    name.str(""); 
    name << "Merged" << group;
    proxy->setName( name.str());
    SIMPLE_ASSERT( proxy->name() == name.str() );

    double minEta = 100.;
    double maxEta = 0.;
    double minPhi = 100.;
    double maxPhi = 0.;
    
    for (auto coll: input) {
      std::stringstream groupname;
      groupname << coll.name() << ngroup;
      ATH_MSG_DEBUG("Proxy " << name.str() <<" "<< group <<" has " 
        << groupsOfMergedRoIs[groupname.str()].size() <<" Children Proxies from "<<coll.name());

      //    if (m_createMergedRoI){
      //create Merged RoI
      for (auto roi_count: groupsOfMergedRoIs[groupname.str()]){  
        auto inputcomp = (*coll.cptr())[roi_count];
        auto objWithLinkToRoI = ViewAlgs::find(inputcomp, ViewAlgs::HasObject("initialRoI"));
        if ( objWithLinkToRoI ) {
          auto roi = objWithLinkToRoI->objectLink<TrigRoiDescriptorCollection>("initialRoI");     
          double eta = (*roi)->eta();
          double phi = (*roi)->phi();
          if( eta < minEta)  minEta=eta;
          if( eta >= maxEta) maxEta=eta;
          if( phi < minPhi)  minPhi=phi;
          if( phi >= maxPhi) maxPhi=phi;    
        }
        else {
          ATH_MSG_DEBUG("No path to RoI object for count="<<roi_count);
        }
      }
    }

    //    creating RoIDescripton for this new Proxy
    ATH_MSG_DEBUG("Going to create RoI etamin="<<minEta<<" Etamax="<<maxEta<<" phimin="<<minPhi<<" phimax="<<maxPhi);
    auto trigRoI = new TrigRoiDescriptor(0xffffff, 0u, 0u,
                                        (minEta+(maxEta-minEta)/2.), minEta, maxEta,
                                        (minPhi+(maxPhi-minPhi)/2.), minPhi, maxPhi);
    m_outputRoisContainer->push_back(trigRoI);
    ATH_MSG_DEBUG("Created RoI n. " << m_outputRoisContainer->size() <<": eta="<<trigRoI->eta()<<" phi="<<trigRoI->phi());
    proxy->setObjectLink("initialRoI", ElementLink<TrigRoiDescriptorCollection>(m_outputRoisContainer.name(),m_outputRoisContainer->size() - 1));

    SIMPLE_ASSERT ( proxy->hasObjectLink( "initialRoI" ) );
    SIMPLE_ASSERT ( proxy->linkColClids()[ 0 ] == ClassID_traits< TrigRoiDescriptorCollection >::ID() );
    SIMPLE_ASSERT ( proxy->linkColNames().size() == 1 );
    SIMPLE_ASSERT ( proxy->linkColKeys().size() == 1 );
    SIMPLE_ASSERT ( proxy->linkColIndices().size() == 1 );
    SIMPLE_ASSERT ( proxy->linkColClids().size() == 1 );

    // } 
    // else      {
    /*     
     for (auto roi_count: groupsOfMergedRoIs[group]){ 
     auto inputcomp = (*m_inputProxyContainer.cptr())[roi_count];
     auto objWithLinkToRoI = ViewAlgs::find(inputcomp, ViewAlgs::HasObject("initialRoI"));
     if ( objWithLinkToRoI ) {
     // just copy inital RoIs into output RoIs
     auto roi =  objWithLinkToRoI->object<TrigRoiDescriptor> ("initialRoI");
     //auto roi = objWithLinkToRoI->objectLink<TrigRoiDescriptorCollection>("initialRoI");     
     m_outputRoisContainer->push_back(roi);
     //ATH_MSG_DEBUG("Added RoI n. " << m_outputRoisContainer->size() <<": eta="<<(*roi)->eta()<<" phi="<<(*roi)->phi());
     proxy->setObjectLink("initialRoI", ElementLink<TrigRoiDescriptorCollection>(m_outputRoisContainer.name(),m_outputRoisContainer->size() - 1));
     }
     }*/
    // }

    // add children proxies in one linkvector
    ElementLinkVector<xAOD::TrigCompositeContainer> linkVector;
    linkVector.reserve(m_inputVectorProxyColl.size());

    for (auto coll: input) {
      unsigned int daukey = ElementLink<xAOD::TrigCompositeContainer>(coll.name(),0).key();
      std::stringstream groupname;
      groupname << coll.name() << ngroup;
      for (auto index: groupsOfMergedRoIs[groupname.str()]){
        linkVector.push_back(ElementLink<xAOD::TrigCompositeContainer>(daukey, index));
      }
      ATH_MSG_DEBUG("Proxy "<<proxy->name() <<" Added Detail: ProxyVecIndices of size " << groupsOfMergedRoIs[groupname.str()].size() << 
                    " with  Key = " <<daukey  <<" for collection "<< coll.name());
    }

    ATH_MSG_DEBUG("Created linkVector of size "<< linkVector.size());
    //bool addedVector = ViewAlgs::setObjectLinks( proxy, linkVector); // TODO - use new TrigComposite feature?
    bool addedVector = proxy->addObjectCollectionLinks("childProxies", linkVector);
    if (linkVector.size() != 0) proxy->setDetail("IsComposite", 1);    
    SIMPLE_ASSERT(addedVector);

    int isComposite=0;
    proxy->getDetail("IsComposite", isComposite);
    if (linkVector.size()!=0) SIMPLE_ASSERT(isComposite);
  }


  ATH_MSG_DEBUG("Created "<< m_outputProxyContainer.cptr()->size() <<" output containers. Now read back: ");


  // read back
  for (const auto& outp : *m_outputProxyContainer) {
    ElementLinkVector<xAOD::TrigCompositeContainer> linkVector;
    bool hasChildren = ViewAlgs::recursiveGetObjectLinks(outp, linkVector);
    if (! hasChildren){
      ATH_MSG_ERROR("This proxy in not composite!");
    }
    for (auto link: linkVector ){      
      auto compByElementLink = *link;
      auto objWithLinkToRoI = ViewAlgs::find(compByElementLink, ViewAlgs::HasObject("initialRoI"));  
      if ( objWithLinkToRoI ) {
        auto roi = objWithLinkToRoI->objectLink<TrigRoiDescriptorCollection>("initialRoI");     
        double eta = (*roi)->eta();
        double phi = (*roi)->phi();
        ATH_MSG_DEBUG("Children Proxy from collection "<< link.key()<<" has link to this RoI: eta="<<eta<<" phi="<<phi);
      }
      else{
        ATH_MSG_DEBUG("This Children Proxy from collection "<< link.key()<<" has no link to any RoI");
      }
    }
 
    // some print out
    std::stringstream msg;
    msg.str("");
    msg<<"proxy->linkColClids().size() = " << outp->linkColClids().size() << ": ";    
    for (auto clid:outp->linkColClids()){
      msg << clid << " ";
    }
    ATH_MSG_DEBUG(msg.str());
    
    msg.str("");
    msg << "proxy->linkColNames().size() = "<< outp->linkColNames().size() << ": ";
    for (auto clid:outp->linkColNames()){
      msg << clid << " ";
    }
    ATH_MSG_DEBUG(msg.str());
    
    msg.str("");
    msg << "proxy->linkColKeys().size() = " << outp->linkColKeys().size()  <<  ":";
    for (auto clid:outp->linkColKeys()){
      msg << clid << " ";
    }
    ATH_MSG_DEBUG(msg.str());
    
    msg.str("");
    msg << "proxy->linkColIndices().size() = " << outp->linkColIndices().size()  <<  ":";
    for (auto clid:outp->linkColIndices()){
      msg << clid << " ";
    }
    ATH_MSG_DEBUG(msg.str());

  }

  return StatusCode::SUCCESS;
}



