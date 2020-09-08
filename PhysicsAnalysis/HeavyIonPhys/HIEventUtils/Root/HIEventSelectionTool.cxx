/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIEventUtils/HIEventSelectionTool.h"
#include "HIEventUtils/HIVertexSelectionTool.h"
#include "xAODForward/ForwardEventInfoContainer.h"
#include "xAODTracking/VertexContainer.h"
//#include "HIPileupTool/HIPileupTool.h"
#include "HIEventUtils/HIPileupTool.h"

#include <iomanip>
#include <iostream>

namespace HI
{

  const std::string HIEventSelectionTool::m_VERTEX_CNAME="PrimaryVertices";
  const std::string HIEventSelectionTool::m_MBTS_CNAME="MBTSContainer";
  const std::string HIEventSelectionTool::m_FWD_CNAME="MBTSForwardEventInfo";
  const std::string HIEventSelectionTool::m_ZDC_CNAME="ZdcModules";
  const std::string HIEventSelectionTool::m_SHAPE_CNAME="CaloSums"; //HIEventShape

  HIEventSelectionTool::HIEventSelectionTool(const std::string& name) : asg::AsgTool(name),
									m_name(name),
									m_accept("HIEventSelection"),
									m_vertexSelectionTool(""),
									m_hiPileupTool("")
  {
    // MBTS timing cut
    declareProperty("TimeCut",m_timeCut,"maximum DeltaT cut for MBTS timing cut");
    // Vertex
    declareProperty("VertexSumPtCut",m_vtx_min_sumPt,"Minimum sum pT from good tracks for a good vertex");
    declareProperty("VertexNtrkCut",m_vtx_min_nTrk,"Minimum number  good tracks for a good vertex");
    declareProperty("RejectPileup",m_reject_pileup,"If true, cut on events w/ pileup applied");
    declareProperty("ExclusiveMode",m_exclusive_mode,"returns at first failure (for speed), doesn't evaluate subsequent cuts");
    declareProperty("VertexSelectionTool",m_vertexSelectionTool);
    declareProperty("PileupTool",m_hiPileupTool);

    ATH_MSG_DEBUG("Creating HIEventSelectionTool named" << m_name);
  }

  HIEventSelectionTool::~HIEventSelectionTool()
  {
    ATH_MSG_DEBUG(Form("Deleting HIEventSelectionTool named %s",m_name.c_str()));
  }

  StatusCode HIEventSelectionTool::initialize()
  {
  if (m_timeCut > 0.) {
    m_checkTime = true;
    ATH_MSG_INFO( "Maximum delta t = " << m_timeCut );
    m_accept.addCut("goodMBTSTime","good MBTS timing");
   }

    m_accept.addCut("hasPrimaryVertex","has at least one primary vertex");

    m_accept.addCut("pileupVeto","reject events with topological features indicating pileup");

    if (m_vertexSelectionTool.empty())
    {
#ifdef ROOTCORE
      HI::HIVertexSelectionTool* vxSelTool = new HIVertexSelectionTool("HIVxSel");
      ATH_CHECK( vxSelTool->initialize() );
      m_vertexSelectionTool = ToolHandle<HI::IHIVertexSelectionTool>(vxSelTool);
#endif
    }
    ATH_CHECK( m_vertexSelectionTool.retrieve() );

    if (m_hiPileupTool.empty())
    {
#ifdef ROOTCORE
      HI::HIPileupTool* m_hipileuptool = new HIPileupTool("HIpileup");
      ATH_CHECK( m_hipileuptool->initialize() );
      m_hiPileupTool = ToolHandle<HI::IHIPileupTool>(m_hipileuptool);
#endif
    }
    ATH_CHECK( m_hiPileupTool.retrieve() );

    return StatusCode::SUCCESS;
  }

 StatusCode HIEventSelectionTool::finalize()
{

  ATH_MSG_INFO( "Finalizing event selection tool." );
  return StatusCode::SUCCESS;

}

const asg::AcceptInfo& HIEventSelectionTool::getAcceptInfo() const
  {
   asg::AcceptData acceptData ( &m_accept );
   if(m_checkTime){
    checkMBTSTime( acceptData );
   }
    checkVertex( acceptData );
    return m_accept;
  }

  //R.Longo - 14/10/2019 - From checkMBTSTime to checkMBTSTime( asg::AcceptData& acceptData )
  void HIEventSelectionTool::checkMBTSTime( asg::AcceptData& acceptData ) const
  {
    const xAOD::ForwardEventInfoContainer* forwardEventInfoContainer = 0;
    if(evtStore()->retrieve(forwardEventInfoContainer,m_FWD_CNAME).isFailure())
    {
      ATH_MSG_ERROR("Could not retrieve ForwardEventInfo object w/ name " << m_FWD_CNAME);
      return;
    }

    acceptData.setCutResult("goodMBTSTime",false);
    const xAOD::ForwardEventInfo* forwardEventInfo = forwardEventInfoContainer->at(0);
    int countA = forwardEventInfo->countA();
    int countC = forwardEventInfo->countC();
    float timeA = forwardEventInfo->timeA();
    float timeC = forwardEventInfo->timeC();

    if (countA==0||countC==0) return; // if either time undefined, return false;

    float timeDiff = timeA - timeC;

    if ( (std::abs(timeDiff)<m_timeCut) ) acceptData.setCutResult("goodMBTSTime",true);

    return;
  }


  //R.Longo - 14/10/2019 - Introducting acceptData instead of m_accept
  std::string HIEventSelectionTool::dumpSelection() const
  {
    asg::AcceptData acceptData ( &m_accept );
    std::stringstream ss;
    for(unsigned int icut=0; icut < acceptData.getNCuts(); icut++)
    {
      std::string cname=acceptData.getCutName(icut);
      ss << std::setw(15) << cname
	    << std::setw(10) << acceptData.getCutResult(cname)
	    << std::setw(40) << acceptData.getCutDescription(cname)
	    << std::endl;
    }
    return ss.str();
  }

//R.Longo - 14/10/2019 - From checkVertex to checkVertex( asg::AcceptData& acceptData )
void HIEventSelectionTool::checkVertex( asg::AcceptData& acceptData ) const
  {
    unsigned int numPrimaryVertices=getNPrimaryVertices();
    acceptData.setCutResult("hasPrimaryVertex", (numPrimaryVertices > 0) );

    const xAOD::ZdcModuleContainer* zdcMod = 0;
     if(evtStore()->retrieve(zdcMod,m_ZDC_CNAME).isFailure())
     {
       ATH_MSG_ERROR("Could not retrieve ZdcModule object w/ name " << m_ZDC_CNAME);
       return;
     }

     const xAOD::HIEventShapeContainer* hiev = 0;
     if(evtStore()->retrieve(hiev,m_SHAPE_CNAME).isFailure())
     {
        ATH_MSG_ERROR("Could not retrieve HIEventShape object w/ name " << m_SHAPE_CNAME);
        return;
     }

    if (m_reject_pileup) acceptData.setCutResult("pileupVeto", m_hiPileupTool->is_pileup( *hiev, *zdcMod ));
    else acceptData.setCutResult("pileupVeto", true );


    return;
  }


unsigned int HIEventSelectionTool::getNPrimaryVertices() const
  {
    unsigned int nPriVx = 0;

    const xAOD::VertexContainer* pv_container = 0;
    if(evtStore()->retrieve(pv_container,m_VERTEX_CNAME).isFailure())
    {
      ATH_MSG_ERROR("Could not retrieve VertexContainer object w/ name " << m_VERTEX_CNAME);
      return 0;
    }
    //if( pv_container->size() < 2) return 0;

    for(const auto vert: *pv_container)
    {
      if (m_vertexSelectionTool->accept(*vert)) nPriVx++;
    }

    return nPriVx;

  }

} // HI
