#include<iostream>
// includes, system
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <vector>

// includes, project-specific
#include "McTrack.h"
#include "TrkTrackState.h"
#include "RecTrack.h"
#include "DkfTrack.h"
#include "EventData.h"
#include "FileReader.h"
#include "TrackFitter.h"
#include "CudaFitter.h"


int main(int argc, char* argv[]) {
  if(argc<2) {
    std::cout<<"usage: CudaFitterTest input_file"<<std::endl;
    return 0;
  }
  std::cout<<"input file: "<<argv[1]<<std::endl;
  std::vector<EventData*> vpEvents;
  vpEvents.clear();

  int nEvents = 0;

  std::ifstream inputFile(argv[1]);

  FileReader* pFR = new FileReader();

  while(nEvents < 90000) {
    inputFile.clear();
    inputFile.seekg(0, inputFile.beg);
    nEvents += pFR->readEvents(inputFile,vpEvents);
  }

  inputFile.close();

  delete pFR;

  std::cout<<nEvents<<" events are read from "<<argv[1]<<std::endl;
  std::vector<const RecTrack*> vpTracksGlobal;
  vpTracksGlobal.clear();
  for(std::vector<EventData*>::iterator evIt = vpEvents.begin();evIt!=vpEvents.end();++evIt)
    std::copy((*evIt)->m_tracks.begin(),(*evIt)->m_tracks.end(),std::back_inserter(vpTracksGlobal));

  std::cout<<"Total "<<vpTracksGlobal.size()<<" tracks for fitting"<<std::endl;

  CudaFitter* pCF = new CudaFitter();

  pCF->fit(vpTracksGlobal);

  delete pCF;

  for(std::vector<EventData*>::iterator it = vpEvents.begin();it!=vpEvents.end();++it)
    delete (*it);
	
  return 0;
}
