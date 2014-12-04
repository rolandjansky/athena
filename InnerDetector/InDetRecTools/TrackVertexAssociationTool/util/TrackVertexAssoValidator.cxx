/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Used for validation the track vertex association tools in RootCore implementation

#include <memory>
#include <cstdlib>

#include <TFile.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH1D.h>

#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#endif

//#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

#include "TrackVertexAssociationTool/LooseTrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/TightTrackVertexAssociationTool.h"

int main(int argc, char** argv)
{
  xAOD::Init();

  TFile *infile=new TFile("/tmp/boliu/mc14_13TeV.147446.PowhegPythia8_AZNLO_Zee_DiLeptonFilter.recon.AOD.e3059_s1982_s2008_r5787_tid01572724_00/AOD.01572724._005433.pool.root.1");
//  TFile *infile=new TFile("../run/AOD.nomet.pool.root");

  xAOD::TEvent event;

  event.readFrom(infile);

  CP::LooseTrackVertexAssociationTool *trktovxtool=new CP::LooseTrackVertexAssociationTool("LooseTrackVertexAssociationTool");

//  trktovxtool->msg().setLevel( MSG::DEBUG);
  trktovxtool->msg().setLevel( MSG::ERROR);

  trktovxtool->setProperty("dzSinTheta_cut", 100.);

  Long64_t nentries = event.getEntries();

  std::cout<<"Total Number of Events: "<<nentries<<std::endl;

  for(Long64_t entry = 0; entry < nentries; ++entry)
  {

    event.getEntry( entry );

    const xAOD::TrackParticleContainer *trkCont=0;
    const xAOD::VertexContainer *vxCont=0;

    event.retrieve(trkCont, "InDetTrackParticles");
    event.retrieve(vxCont, "PrimaryVertices");



    xAOD::TrackVertexAssociationMap trktovxmap=trktovxtool->getUniqueMatchMap(*trkCont, *vxCont);

    std::cout<<"Size: " << trktovxmap[vxCont->at(0)].size()<<std::endl;


    ElementLink< xAOD::VertexContainer> match_vx=trktovxtool->getUniqueMatchVertexLink(*(trkCont->at(2)), *vxCont);

    if(match_vx.isValid())
    {
      std::cout<< match_vx <<std::endl;
      std::cout<< *match_vx <<std::endl;
      std::cout<< (*match_vx)->z() <<std::endl;
    }
  }

  infile->Close();


  return 0;
}

