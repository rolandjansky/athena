/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSMATERIALTRACKWRITERSVC_H
#define ACTSGEOMETRY_ACTSMATERIALTRACKWRITERSVC_H

#include "ActsGeometry/IActsMaterialTrackWriterSvc.h"

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Property.h"  /*no forward decl: typedef*/

#include <vector>
#include <deque>
#include <mutex>
#include <thread>
#include <atomic>

#include "TTree.h"
#include "TFile.h"

namespace Acts {
  class MaterialTrack;
}

class ActsMaterialTrackWriterSvc : public extends<AthService, IActsMaterialTrackWriterSvc> {
public:
    
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
    
  ActsMaterialTrackWriterSvc( const std::string& name, ISvcLocator* svc );

  void
  write(const Acts::MaterialTrack& mTrack) override;

private:

  std::deque<Acts::MaterialTrack> m_mTracks;
  std::mutex m_writeMutex;
  std::thread m_writeThread;
  std::atomic<bool> m_doEnd;
  TFile* p_tFile;
  TTree* p_tree;

  // tree branches
  double m_treeX0;
  double m_treeL0;
  double m_treeTheta;
  double m_treePhi;
  double m_treeT;
  double m_treedInX0;
  double m_treedInL0;

  //std::vector<std::array<double, 3>> m_treeStepPos;
  std::vector<double> m_treeStepX0;
  std::vector<double> m_treeStepL0;
  std::vector<double> m_treeStepA;
  std::vector<double> m_treeStepZ;
  std::vector<double> m_treeStepRho;
  std::vector<double> m_treeStepT;
  std::vector<double> m_treeStepdInX0;
  std::vector<double> m_treeStepdInL0;
  
  std::vector<double> m_treeStepPosX;
  std::vector<double> m_treeStepPosY;
  std::vector<double> m_treeStepPosZ;
  std::vector<double> m_treeStepPosR;
  std::vector<double> m_treeStepPosPhi;

  std::vector<ULong64_t> m_treeStepGeoID;


  void writerThread();
  void doWrite(const Acts::MaterialTrack &mTrack);

  // jobOptions properties
  Gaudi::Property<std::string> m_filePath{this, "FilePath", "MaterialTracks.root", "Output root file for charged particle"};
  Gaudi::Property<std::string> m_treeName{this, "TreeName", "MaterialTracks", ""};
  Gaudi::Property<size_t> m_maxQueueSize{this, "MaxQueueSize", 5000, "Limit the write queue to this size"};

};


#endif 
