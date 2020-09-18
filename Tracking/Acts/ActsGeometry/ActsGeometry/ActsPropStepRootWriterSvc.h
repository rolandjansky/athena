/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSPROPSTEPROOTWRITERSVC_H
#define ACTSGEOMETRY_ACTSPROPSTEPROOTWRITERSVC_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "Gaudi/Property.h"  /*no forward decl: typedef*/

#include "ActsGeometry/IActsPropStepRootWriterSvc.h"

#include "Acts/EventData/TrackParameters.hpp"

#include <vector>
#include <deque>
#include <mutex>
#include <thread>
#include <atomic>

template <typename T>
class RootPropStepWriter;


namespace Acts {

template <class>
class ExtrapolationCell;

}

class TFile;
class TTree;


class ActsPropStepRootWriterSvc : public extends<AthService, IActsPropStepRootWriterSvc> {
public:
    
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
    
  ActsPropStepRootWriterSvc( const std::string& name, ISvcLocator* svc );

  using StepVector = std::vector<Acts::detail::Step>;

  void
  write(const StepVector& steps) override;

private:
  using queue_item_t = std::pair<size_t, StepVector>;

  //std::shared_ptr<RootPropStepWriter<Acts::TrackParameters>> m_rootEccWriter;
  std::deque<queue_item_t> m_queue;
  std::mutex m_writeMutex;
  std::thread m_writeThread;
  std::atomic<bool> m_doEnd;

  void writeThread();
  void doWrite(const StepVector& steps, size_t evtNum);
  void end();

  // jobOptions properties
  Gaudi::Property<std::string> m_filePath{this, "FilePath", "propsteps.root", "Output root file for charged particle"};
  Gaudi::Property<std::string> m_treeName{this, "TreeName", "propsteps", ""};
  //Gaudi::Property<bool> m_writeBoundary{this, "WriteBoundary", true, ""};
  //Gaudi::Property<bool> m_writeMaterial{this, "WriteMaterial", true, ""};
  //Gaudi::Property<bool> m_writeSensitive{this, "WriteSensitive", true, ""};
  //Gaudi::Property<bool> m_writePassive{this, "WritePassive", true, ""};

  // root branch storage
    TFile*             m_outputFile;    ///< the output file
    TTree*             m_outputTree;    ///< the output tree
    int m_eventNum;
    std::vector<float> m_s_pX;   ///< global position x of the step
    std::vector<float> m_s_pY;   ///< global position y of the step
    std::vector<float> m_s_pZ;   ///< global position z of the step
    std::vector<float> m_s_pR;   ///< global position z of the step
    std::vector<int>   m_s_volumeID;    ///< volume identification
    std::vector<int>   m_s_boundaryID;    ///< boundary identification
    std::vector<int>   m_s_layerID;     ///< layer identification
    std::vector<int>   m_s_approachID;     ///< approach identification
    std::vector<int>   m_s_sensitiveID;   ///< sensitive identification

};


#endif 
