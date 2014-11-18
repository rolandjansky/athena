//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/ISvcLocator.h"

#include "TBMonitoring/TBMWPCMonTool.h"

#include "TBEvent/TBMWPCRaw.h"
#include "TBEvent/TBMWPCRawCont.h"
#include "TBEvent/TBMWPCCont.h"

#include <vector>
#include <string>

/*---------------------------------------------------------*/
TBMWPCMonTool::TBMWPCMonTool(const std::string & type, 
				 const std::string & name,
				 const IInterface* parent)
  : MonitorToolBase(type, name, parent)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);
  declareProperty("histoPathBase",m_path = "/stat"); 
  
  //ADC histogram properties
  declareProperty("MWPCRangeRawCenter",m_range_rawcenter);
  declareProperty("MWPCRangeRawWidth",m_range_rawwidth);
  declareProperty("MWPCRangeRawHits",m_range_rawhits);
  declareProperty("MWPCRangeRawClust",m_range_rawclust);

  declareProperty("MWPCRangeHits",m_range_center);
  declareProperty("MWPCRangeSize",m_range_size);

  if(m_range_rawcenter.size()==0){m_range_rawcenter.push_back(128);m_range_rawcenter.push_back(0);m_range_rawcenter.push_back(128);}
  if(m_range_rawwidth.size()==0){m_range_rawwidth.push_back(20);m_range_rawwidth.push_back(0);m_range_rawwidth.push_back(20);}
  if(m_range_rawhits.size()==0){m_range_rawhits.push_back(20);m_range_rawhits.push_back(0);m_range_rawhits.push_back(20);}
  if(m_range_rawclust.size()==0){m_range_rawclust.push_back(20);m_range_rawclust.push_back(0);m_range_rawclust.push_back(20);}

  if(m_range_center.size()==0){m_range_center.push_back(40);m_range_center.push_back(-20);m_range_center.push_back(20);}
  if(m_range_size.size()==0){m_range_size.push_back(20);m_range_size.push_back(0);m_range_size.push_back(20);}


  declareProperty("MonitorMWPC",m_monitor_mwpc=false);
  declareProperty("MonitorMWPCRaw",m_monitor_mwpcraw=true);
  declareProperty("TestMWPCMonTool",m_testTool=false);

  declareProperty("SGMWPCRawkey",m_SGkeymwpcraw = "MWPCRawCont");
  declareProperty("SGMWPCkey",m_SGkeymwpc = "MWPCCont");


}

/*---------------------------------------------------------*/
TBMWPCMonTool::~TBMWPCMonTool()
/*---------------------------------------------------------*/
{}

/*---------------------------------------------------------*/
StatusCode TBMWPCMonTool:: initialize()
/*---------------------------------------------------------*/
{
  if(m_monitor_mwpc == false){
    ATH_MSG_INFO ( name() << " Not monitoring mwpc " );
  }

  //set to true whitin bookHist() 
  m_isBooked = false;

  m_mwpcnames[0]="X2"; m_mwpcnames[1]="Y2";m_mwpcnames[2]="X3";m_mwpcnames[3]="Y3";m_mwpcnames[4]="X4";
  m_mwpcnames[5]="Y4";m_mwpcnames[6]="X5";m_mwpcnames[7]="Y5";

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBMWPCMonTool::bookHists()
/*---------------------------------------------------------*/
{
  // This is called by parent class in initialize().
  // We want to book histos according to 1st event so 
  // we use mybookHist()
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBMWPCMonTool::mybookHists()
/*---------------------------------------------------------*/
{
  ATH_MSG_DEBUG ( "in mybookHists()" );

  m_path += "/BeamDetectors/MWPC/";

   if(m_monitor_mwpc){
     for( int i=0;i<8;i++){
       std::string hname = m_path+m_mwpcnames[i]+"center";
       std::string htitle = "MWPC  "+m_mwpcnames[i]+ " Center Pos";
       m_histo_mwpcCenter[i]=ToolHistoSvc()->book(hname,htitle,(int) m_range_center[0],m_range_center[1],m_range_center[2]);
       
       hname = m_path+m_mwpcnames[i]+"clustersize";
       htitle = "MWPC  "+m_mwpcnames[i]+ " Cluster size";
       m_histo_mwpcSize[i]=ToolHistoSvc()->book(hname,htitle,(int) m_range_size[0],m_range_size[1],m_range_size[2]);


     }
   }

   if(m_monitor_mwpcraw){
     // this must be the same as in TBCnv subfragment for MWPC 
     for( int i=0;i<8;i++){
       std::string hname = m_path+m_mwpcnames[i]+"centerwire";
       std::string htitle = "MWPC  "+m_mwpcnames[i]+ " Center wire No";
       m_histo_mwpcrawC[i]=ToolHistoSvc()->book(hname,htitle,(int) m_range_rawcenter[0],m_range_rawcenter[1],m_range_rawcenter[2]);
       
       hname = m_path+m_mwpcnames[i]+"widthinwires";
       htitle = "MWPC  "+m_mwpcnames[i]+ " Width in N of wires";
       m_histo_mwpcrawN[i]=ToolHistoSvc()->book(hname,htitle,(int) m_range_rawwidth[0],m_range_rawwidth[1],m_range_rawwidth[2]);

       hname = m_path+m_mwpcnames[i]+"hitdist";
       htitle = "MWPC  "+m_mwpcnames[i]+ " Number of hits by event";
       m_histo_mwpcrawHitDist[i]=ToolHistoSvc()->book(hname,htitle,(int) m_range_rawhits[0],m_range_rawhits[1],m_range_rawhits[2]);

       hname = m_path+m_mwpcnames[i]+"clustdist";
       htitle = "MWPC  "+m_mwpcnames[i]+ " Number of clusters by event";
       m_histo_mwpcrawClustDist[i]=ToolHistoSvc()->book(hname,htitle,(int) m_range_rawclust[0],m_range_rawclust[1],m_range_rawclust[2]);

     }
     std::string hname = m_path+"Hits";
     std::string htitle =  "Number of event with hits vs plane number";
     m_histo_mwpcrawHits=ToolHistoSvc()->book(hname,htitle,8,0,8);
   }

   ATH_MSG_DEBUG ( "histo path: " << m_path + "/BeamDetectors/" );
   ATH_MSG_DEBUG ( "Leaving mybookHists()" );
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBMWPCMonTool::fillHists()
/*---------------------------------------------------------*/
{
  ATH_MSG_DEBUG ( "In fillHists()" );

  // Fill some tacking objects for testing
  if(m_testTool){
    FillRandomDetect();
  }
  
  if(this->histsNotBooked()) {
    this->mybookHists();
  }

  TBMWPCCont * mwpcCont=NULL;
  TBMWPCRawCont * mwpcRawCont=NULL;

  // MWPC monitor
  if(m_monitor_mwpc){
    StatusCode sc = evtStore()->retrieve(mwpcCont,m_SGkeymwpc);
    if (sc.isFailure()){
      ATH_MSG_DEBUG 
        ( "BeamDetectorMonitoring: Retrieval of TBMWPCCont failed key="<< m_SGkeymwpc );
    } else {
      
      TBMWPCCont::const_iterator it_mwpc = mwpcCont->begin();
      TBMWPCCont::const_iterator last_mwpc = mwpcCont->end();
      
      for(it_mwpc = mwpcCont->begin();it_mwpc!=last_mwpc;it_mwpc++){	
	// Check if adcRawCont is in sync with m_adcNames - defined in book hist

	std::string name = (*it_mwpc)->getDetectorName() ;
	int i = 0;
	while(i<8) {
	  if(m_mwpcnames[i]==name) break;
	  else i++;
	}
	if(i==8) {
          ATH_MSG_DEBUG ( " Unmatching MWPC name");
          continue;
        }

	const TBMWPC * mwpc = (*it_mwpc);
	if(!mwpc->isOverflow()){
	  std::vector<float> posc = mwpc->getCPos();
	  for(unsigned int k=0;k<posc.size();k++)	m_histo_mwpcCenter[i]->fill(posc[k]);
	  if(posc.size())
            ATH_MSG_DEBUG ( name << " pos=" <<posc[0]);
	  std::vector<float> sizec = mwpc->getClusterSizeC();
	  for(unsigned int k=0;k<sizec.size();k++)	m_histo_mwpcSize[i]->fill(sizec[k]);
	}

      }
    }

  }  
  if(m_monitor_mwpcraw){
    StatusCode sc = evtStore()->retrieve(mwpcRawCont,m_SGkeymwpcraw);
    if (sc.isFailure()){
      ATH_MSG_DEBUG 
        ( "BeamDetectorMonitoring: Retrieval of TBMWPCRawCont failed. key="<< m_SGkeymwpcraw );
    } else {
      
      TBMWPCRawCont::const_iterator it_mwpc = mwpcRawCont->begin();
      TBMWPCRawCont::const_iterator last_mwpc = mwpcRawCont->end();
      
      for(it_mwpc = mwpcRawCont->begin();it_mwpc!=last_mwpc;it_mwpc++){	

	ATH_MSG_DEBUG ((*it_mwpc)->getDetectorName());

	std::string name = (*it_mwpc)->getDetectorName() ;
	int i = 0;
	while(i<8) {
	  if(m_mwpcnames[i]==name) break;
	  else i++;
	}
	if(i==8) {
          ATH_MSG_DEBUG ( " Unmatching MWPC name");
          continue;
        }

	const TBMWPCRaw * mwpc = (*it_mwpc);
	

	if(!mwpc->isOverflow()){
	  std::vector<int> wirec = mwpc->getCwireno();
	  // number of cluster :
	  m_histo_mwpcrawClustDist[i]->fill(wirec.size());
	  // Centers of clusters
	  for(unsigned int k=0;k<wirec.size();k++) {
	    m_histo_mwpcrawC[i]->fill(wirec[k]);
	    //	    log<< MSG::INFO << "  MWPC "<< i <<" wirecenter = "<< wirec[k] << endreq;continue;
	  }

	  std::vector<int> wiren = mwpc->getNwires();
	  //	log<< MSG::DEBUG << mwpc->getDetectorName()<< " " << wiren[0]  << endreq;continue;
	  int nhit=0;
	  // Sizes of clusters & number of hits
	  for(unsigned int k=0;k<wiren.size();k++) {
	    m_histo_mwpcrawN[i]->fill(wiren[k]);
	    nhit+=wiren[k];
	  }
	  m_histo_mwpcrawHitDist[i]->fill(nhit);
	  if(nhit) m_histo_mwpcrawHits->fill(i);
	}
	

      }
    }

  }  // End of mwpcraw mon

return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TBMWPCMonTool::FillRandomDetect()
/*---------------------------------------------------------*/
{
}
