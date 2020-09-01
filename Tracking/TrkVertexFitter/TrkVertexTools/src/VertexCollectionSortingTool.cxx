/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexTools/VertexCollectionSortingTool.h"
#include "TrkVertexFitterInterfaces/IVertexWeightCalculator.h" 
#include "VxVertex/VxTrackAtVertex.h"
#include <vector> 


 
namespace {
 
 struct Vertex_pair
 {
   double first;
   const xAOD::Vertex* second;
   Vertex_pair(double p1, const xAOD::Vertex* p2)
     : first (p1), second (p2) {}
   bool operator< (const Vertex_pair& other) const
   { return first > other.first; }
 };

} // anonymous namespace


namespace Trk{
 
   //constructor
  VertexCollectionSortingTool::VertexCollectionSortingTool ( const std::string& t, const std::string& n, const IInterface*  p )
          : AthAlgTool ( t,n,p ),
            m_iVertexWeightCalculator("Trk::IVertexWeightCalculator")  
  {
    declareInterface<IVertexCollectionSortingTool> ( this );
    declareProperty ( "VertexWeightCalculator",m_iVertexWeightCalculator);
  }
  
  //destructor
  VertexCollectionSortingTool::~VertexCollectionSortingTool(){}

//initialize
  StatusCode VertexCollectionSortingTool::initialize()
  {
    if ( m_iVertexWeightCalculator.retrieve().isFailure() )
    {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_iVertexWeightCalculator << endmsg;
      return StatusCode::FAILURE;
    }
 
    msg(MSG::INFO) << "Initialization successful" << endmsg;
    return StatusCode::SUCCESS;
  }///EndOfInitialize

  StatusCode VertexCollectionSortingTool::finalize()
  {
    return StatusCode::SUCCESS;
  }

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  VertexCollectionSortingTool::sortVertexContainer(
    const xAOD::VertexContainer& MyVxCont) const
  { 
    std::vector<Vertex_pair> MyVertex_pairs;
    
    xAOD::VertexContainer::const_iterator beginIter = MyVxCont.begin();
    xAOD::VertexContainer::const_iterator endIter = MyVxCont.end();

    for(xAOD::VertexContainer::const_iterator i = beginIter; i!=endIter; ++i) 
      {
        // do not weight dummy!!! (do not delete it either! it is deleted when the original MyVxContainer is deleted in InDetPriVxFinder.cxx)
        if ((*i)->vertexType() != xAOD::VxType::NoVtx)
          {
            double Weight =m_iVertexWeightCalculator->estimateSignalCompatibility(**i);
            MyVertex_pairs.emplace_back(Weight,(*i));
            ATH_MSG_DEBUG("Weight before sorting: " << Weight);
          } 
      }

    if (!MyVertex_pairs.empty())
      {
        std::sort (MyVertex_pairs.begin(), MyVertex_pairs.end());
      }

    xAOD::VertexContainer *NewContainer = new xAOD::VertexContainer();
    xAOD::VertexAuxContainer* auxNewContainer = new xAOD::VertexAuxContainer();
    NewContainer->setStore( auxNewContainer );
    std::string decorationName = (m_iVertexWeightCalculator->name().find("InDetSumPtVertexWeightCalculator") != std::string::npos) ? "sumPt2" : "signalCompatibility";
    SG::AuxElement::Decorator< float > sigWeightDec(decorationName);
    unsigned int vtxCount(1);
    for (std::vector<Vertex_pair>::const_iterator iter = MyVertex_pairs.begin() ; iter != MyVertex_pairs.end(); ++iter )
      {
        ATH_MSG_DEBUG("Weight after sorting: " << (*iter).first);
        xAOD::Vertex* vxCand = new xAOD::Vertex(*(iter->second)); //use copy-constructor, creates a private store
        NewContainer->push_back(vxCand); //private store is now copied to the container store
        if (vtxCount == 1) { 
          vxCand->setVertexType(xAOD::VxType::PriVtx); 
        } else { 
          vxCand->setVertexType(xAOD::VxType::PileUp); 
        }
        sigWeightDec(*vxCand) = iter->first;
        vtxCount++;
      }

    // add dummy at position of first vertex
    xAOD::Vertex* primaryVtx = NewContainer->front();
    xAOD::Vertex* dummyVxCandidate = new xAOD::Vertex();
    NewContainer->push_back ( dummyVxCandidate );
    dummyVxCandidate->setPosition(primaryVtx->position());
    dummyVxCandidate->setCovariancePosition(primaryVtx->covariancePosition());
    dummyVxCandidate->setVertexType(xAOD::VxType::NoVtx);

    return std::make_pair(NewContainer, auxNewContainer);
  }   


}///End!!!
