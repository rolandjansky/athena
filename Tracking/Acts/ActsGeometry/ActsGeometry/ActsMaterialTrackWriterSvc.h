/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSMATERIALTRACKWRITERSVC_H
#define ACTSGEOMETRY_ACTSMATERIALTRACKWRITERSVC_H

#include "ActsGeometryInterfaces/IActsMaterialTrackWriterSvc.h"

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "Gaudi/Property.h"  /*no forward decl: typedef*/

#include <vector>
#include <deque>
#include <mutex>
#include <thread>
#include <atomic>

#include "TTree.h"
#include "TFile.h"

class ActsMaterialTrackWriterSvc : public extends<AthService, IActsMaterialTrackWriterSvc> {
public:

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  ActsMaterialTrackWriterSvc( const std::string& name, ISvcLocator* svc );

  virtual void
  write(const Acts::RecordedMaterialTrack& mTrack) override;

private:

  std::deque<Acts::RecordedMaterialTrack> m_mTracks;
  std::mutex m_writeMutex;
  std::thread m_writeThread;
  std::atomic<bool> m_doEnd;
  TFile* p_tFile;
  TTree* p_tree;

  float m_v_x;    ///< start global x
  float m_v_y;    ///< start global y
  float m_v_z;    ///< start global z
  float m_v_px;   ///< start global momentum x
  float m_v_py;   ///< start global momentum y
  float m_v_pz;   ///< start global momentum z
  float m_v_phi;  ///< start phi direction
  float m_v_eta;  ///< start eta direction
  float m_tX0;    ///< thickness in X0/L0
  float m_tL0;    ///< thickness in X0/L0

  std::vector<float> m_step_sx;      ///< step x (start) position (optional)
  std::vector<float> m_step_sy;      ///< step y (start) position (optional)
  std::vector<float> m_step_sz;      ///< step z (start) position (optional)
  std::vector<float> m_step_x;       ///< step x position
  std::vector<float> m_step_y;       ///< step y position
  std::vector<float> m_step_z;       ///< step z position
  std::vector<float> m_step_ex;      ///< step x (end) position (optional)
  std::vector<float> m_step_ey;      ///< step y (end) position (optional)
  std::vector<float> m_step_ez;      ///< step z (end) position (optional)
  std::vector<float> m_step_length;  ///< step length
  std::vector<float> m_step_X0;      ///< step material x0
  std::vector<float> m_step_L0;      ///< step material l0
  std::vector<float> m_step_A;       ///< step material A
  std::vector<float> m_step_Z;       ///< step material Z
  std::vector<float> m_step_rho;     ///< step material rho


  void writerThread();
  void doWrite(const Acts::RecordedMaterialTrack &mTrack);

  // jobOptions properties
  Gaudi::Property<std::string> m_filePath{this, "FilePath", "MaterialTracks.root", "Output root file for charged particle"};
  Gaudi::Property<std::string> m_treeName{this, "TreeName", "material-tracks", ""};
  Gaudi::Property<size_t> m_maxQueueSize{this, "MaxQueueSize", 5000, "Limit the write queue to this size"};

};


#endif
