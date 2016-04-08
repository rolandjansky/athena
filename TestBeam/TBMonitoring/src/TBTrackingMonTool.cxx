//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/ISvcLocator.h"

#include "TBMonitoring/TBTrackingMonTool.h"

#include "TBEvent/TBTrack.h"

#include <vector>


/*---------------------------------------------------------*/
TBTrackingMonTool::TBTrackingMonTool(const std::string & type, 
				 const std::string & name,
				 const IInterface* parent)
  : MonitorToolBase(type, name, parent)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);
  declareProperty("histoPathBase",m_path = "/stat");
  declareProperty("MonitorTrack",m_monitor_track=true);

  declareProperty("TestTrackingMonTool",m_testTool=false);

  declareProperty("TrackPos",m_posMax=10);
  declareProperty("TrackPos",m_posMin=-10);
  declareProperty("TrackNumBin",m_numBin=100);

}

/*---------------------------------------------------------*/
TBTrackingMonTool::~TBTrackingMonTool()
/*---------------------------------------------------------*/
{}

/*---------------------------------------------------------*/
StatusCode TBTrackingMonTool:: initialize()
/*---------------------------------------------------------*/
{
  if(m_monitor_track == false){
    ATH_MSG_INFO ( name() << " Not monitoring tracks " );
  }

  //set to true whitin bookHist() 
  m_isBooked = false;

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBTrackingMonTool::bookHists()
/*---------------------------------------------------------*/
{
  // This is called by parent class in initialize().
  // We want to book histos according to 1st event so 
  // we use mybookHist()
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBTrackingMonTool::mybookHists()
/*---------------------------------------------------------*/
{
  ATH_MSG_DEBUG ( "in mybookHists()" );
   
  // Track histos ----------------------------------
  if(m_monitor_track){
    m_histo_track_chi2 = ToolHistoSvc()->book(m_path+"/BeamDetectors/track_chi2","Track : chi2",m_numBin,m_posMin,m_posMax);
    m_histo_track_angle  = ToolHistoSvc()->book(m_path+"/BeamDetectors/track_angle","Track angle",m_numBin,m_posMin,m_posMax);
    m_histo_track_Uslope = ToolHistoSvc()->book(m_path+"/BeamDetectors/track_Uslope","Track u slope",m_numBin,m_posMin,m_posMax);
    m_histo_track_Vslope = ToolHistoSvc()->book(m_path+"/BeamDetectors/track_Vslope","Track v slope",m_numBin,m_posMin,m_posMax);
     m_histo_track_Ucryo  = ToolHistoSvc()->book(m_path+"/BeamDetectors/track_Ucryo","Track u position in cryostat",m_numBin,m_posMin,m_posMax);
     m_histo_track_Vcryo  = ToolHistoSvc()->book(m_path+"/BeamDetectors/track_Vcryo","Track v position in cryostat",m_numBin,m_posMin,m_posMax);
     m_histo_track_Wcryo  = ToolHistoSvc()->book(m_path+"/BeamDetectors/track_Wcryo","Track w position in cryostat",m_numBin,m_posMin,m_posMax);
     
     /*
     m_histo_track_Residualu = new IHistogram1D*[m_bpcnum];
     m_histo_track_Residualv = new IHistogram1D*[m_bpcnum];
     for(int j=0;j<m_bpcnum;j++){
       std::string hname = m_path+"/BeamDetectors/track_residX"+m_bpc_names[j];
       std::string htitle = "Track X residual - BPC  "+m_bpc_names[j];
       m_histo_track_Residualu[j] = ToolHistoSvc()->book(hname,htitle,m_posbin,m_posmin/4,m_posmax/4);

       hname = m_path+"/BeamDetectors/track_residY"+m_bpc_names[j];
       htitle = "Track Y residual - BPC  "+m_bpc_names[j];
       m_histo_track_Residualv[j] = ToolHistoSvc()->book(hname,htitle,m_posbin,m_posmin/4,m_posmax/4);
     
     }
     */
   }

  ATH_MSG_DEBUG ( "histo path: " << m_path + "/BeamDetectors/" );

  SetBookStatus(true);

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBTrackingMonTool::fillHists()
/*---------------------------------------------------------*/
{
  ATH_MSG_DEBUG ( "in fillHists()" );

  // Fill some tacking objects for testing
  if(m_testTool){
    FillRandomDetect();
  }
  
  if(this->histsNotBooked()) {
    this->mybookHists();
  }

  //  First, the track if we monitor track -----------------
  TBTrack * track=NULL;
  if(m_monitor_track){
    //Retrieve TBTrack collection from SG
    StatusCode sc = evtStore()->retrieve(track, "Track");	
    if (sc.isFailure()){
      ATH_MSG_DEBUG ( "TBTrackingMonTool: Retrieval of Track failed" );
    }else {	          
      m_histo_track_chi2->fill(track->getChi2_global(),1.0);
      m_histo_track_Uslope->fill(track->getUslope(),1.0);
      m_histo_track_Vslope->fill(track->getVslope(),1.0);
      //m_histo_track_Ucryo->fill(track->getCryoHitu(),1.0);
      //m_histo_track_Vcryo->fill(track->getCryoHitv(),1.0);
      //m_histo_track_Wcryo->fill(track->getCryoHitw(),1.0);
      //m_histo_track_Residualu[nameind]->fill(track->getResidualu(nameind));
      //m_histo_track_Residualv[nameind]->fill(track->getResidualv(nameind));
    }
  } // storegate


return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TBTrackingMonTool::FillRandomDetect()
/*---------------------------------------------------------*/
{
}
