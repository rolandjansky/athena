/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTK_RoadMarketTool.h"
#include "TrigFTKSim/FTKSetup.h"

#include <TTree.h>
#include <TBranch.h>

FTK_RoadMarketTool::FTK_RoadMarketTool(const std::string& algname, const std::string &name, const IInterface *ifc) :
  AthAlgTool(algname,name,ifc),
  m_nbanks(0), m_data(0x0),
  m_roadoutput(this), m_roadinput(this),
  m_log( msgSvc() , name ),
  m_SaveRoads(true),
  m_tree(0x0)
{
  declareInterface<FTK_RoadMarketToolI>(this);

  declareProperty("SaveRoads",m_SaveRoads,"Save the roads in the SG");
}

FTK_RoadMarketTool::~FTK_RoadMarketTool()
{
  if (m_data) delete [] m_data;
}


StatusCode FTK_RoadMarketTool::initialize()
{

  m_log << MSG::INFO << "FTK_RoadMarketTool::initialize()" << endmsg;
  m_log << MSG::INFO << "SaveRoads = " << m_SaveRoads << endmsg;  
  return StatusCode::SUCCESS;
}


StatusCode FTK_RoadMarketTool::finalize()
{
  m_log << MSG::INFO << "FTK_RoadMarketTool::finalize()" << endmsg;
  return StatusCode::SUCCESS;
}


/** Set the number of the banks and allocate the memory */
void FTK_RoadMarketTool::setNBanks(int nbanks)
{ 
  m_nbanks = nbanks; 

  m_data = new FTKRoadStream*[m_nbanks];
  for (int ib=0;ib<m_nbanks;++ib) { // bank loop
    // create the stream object and its branch
    m_data[ib] = new FTKRoadStream();
  }
}


void FTK_RoadMarketTool::initRoads()
{;}


/** reset the content of the road stream from the previous event data */
void FTK_RoadMarketTool::clearRoads()
{
  for (int ib=0;ib<m_nbanks;++ib) {
    m_data[ib]->clear();
  }
}


/** prepare the roads to be fitted */
void FTK_RoadMarketTool::prepareFits()
{
  // reset road and bank position for the following call within the
  // TrackFitter loop over the roads
  // Also need to rebuild the map for FTKRoadStream::findRoad
  for (int ib=0;ib!=m_nbanks;++ib) {
    m_data[ib]->buildRoadMap();
    // expand the roads to prepare the list of the road to fit
    m_data[ib]->prepareFitList();
  }

}


/** connect m_data to an existing TTree. The Tree can then save the current m_data status if required */
void FTK_RoadMarketTool::ConnectRoads(TTree *tree,  const char *fmt, 
				      const std::vector<int> &regions,
				      int bufsize) 
{
  if (!m_SaveRoads) {
    m_log << MSG::INFO << "Roads store is disabled" << endmsg;
    return;
  }

  m_tree = tree;
  m_bnamefmt = fmt;
  m_goodreg = regions;
  m_bufsize = bufsize;

  TBranch *tmpBranch;

  for (unsigned ib=0;ib!=m_goodreg.size();++ib) {
    int ireg = m_goodreg[ib];
    const char  *bname = Form(m_bnamefmt.c_str(),ireg);

    if(m_bufsize)
      // if buffer size is specified (eg, 16000)
      tmpBranch = m_tree->Branch(bname,&m_data[ireg],m_bufsize);
    else
      // use default buffer size
      tmpBranch = m_tree->Branch(bname,&m_data[ireg]);

    if( tmpBranch )
      tmpBranch->SetCompressionLevel(1);
  }

}
