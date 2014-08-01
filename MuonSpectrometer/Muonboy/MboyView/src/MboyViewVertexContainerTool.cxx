/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
/////////////////////////////////////////////////////////
#include "MboyView/MboyViewVertexContainerTool.h"

static const InterfaceID IID_IMboyViewVertexContainerTool("MboyViewVertexContainerTool", 1, 0);

const InterfaceID& MboyViewVertexContainerTool::interfaceID()
{
  return IID_IMboyViewVertexContainerTool;
}

MboyViewVertexContainerTool::MboyViewVertexContainerTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<MboyViewVertexContainerTool>(this);

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   


}

MboyViewVertexContainerTool::~MboyViewVertexContainerTool(){}

// Initialize
StatusCode MboyViewVertexContainerTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyViewVertexContainerTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyViewVertexContainerTool::ViewVertexContainer(
               std::string VertexContainerLocation,
               std::ofstream* pOut
){

//Access
  if (!(evtStore()->contains<VxContainer>(VertexContainerLocation))) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage) {
      ATH_MSG_INFO( " Not existing " << VertexContainerLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }
  
  StatusCode sc = StatusCode::SUCCESS;

  const VxContainer* pContainerVertex(0);
  sc = evtStore()->retrieve(pContainerVertex, VertexContainerLocation);
  if (sc.isFailure()) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( "pContainerVertex not found at " << VertexContainerLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }

  double pvx = 0.;
  double pvy = 0.;
  double pvz = 0.;

   if (pContainerVertex->size() > 0) {
     VxContainer::const_iterator fz = pContainerVertex->begin();
     const Trk::RecVertex& primaryVertex = (*fz)->recVertex();
     pvx = primaryVertex.position().x();
     pvy = primaryVertex.position().y();
     pvz = primaryVertex.position().z();
     // Write Out 
     *pOut << "VTX" << " "
	   << VertexContainerLocation << " "
	   << std::setw(12)<<std::setprecision(4) <<pvx  << " "
	   << std::setw(12)<<std::setprecision(4) <<pvy  << " "
	   << std::setw(12)<<std::setprecision(4) <<pvz  << " "
	   << std::endl;
   }
   
   return StatusCode::SUCCESS;
}

