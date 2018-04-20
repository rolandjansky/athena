#ifndef GEOMACTS_MATERIALTRACKWRITER_SERVICE
#define GEOMACTS_MATERIALTRACKWRITER_SERVICE

#include "GeomACTS/IMaterialTrackWriterSvc.h"

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Property.h"  /*no forward decl: typedef*/

#include "GeomACTS/Extrapolation/RootExCellWriter.hpp"

#include "ACTS/EventData/TrackParameters.hpp"

#include <vector>
#include <deque>
#include <mutex>
#include <thread>
#include <atomic>


class TFile;

namespace Acts {

  class MaterialTrack;


class MaterialTrackWriterSvc : public extends<AthService, IMaterialTrackWriterSvc> {
public:
    
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
    
  MaterialTrackWriterSvc( const std::string& name, ISvcLocator* svc );

  void
  write(const MaterialTrack& mTrack) override;

private:

  std::deque<MaterialTrack> m_mTracks;
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


  void doWrite();

  // jobOptions properties
  Gaudi::Property<std::string> m_filePath{this, "FilePath", "MaterialTracks.root", "Output root file for charged particle"};
  Gaudi::Property<std::string> m_treeName{this, "TreeName", "MaterialTracks", ""};

};

}


#endif 
