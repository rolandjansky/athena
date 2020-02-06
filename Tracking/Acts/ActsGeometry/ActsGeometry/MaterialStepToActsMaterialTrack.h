/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MaterialMapping.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MATERIALSTEPTOACTSMATRERIALTRACK_H
#define MATERIALSTEPTOACTSMATRERIALTRACK_H

// Athena & Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TString.h"
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include "GeoPrimitives/GeoPrimitives.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkGeometry/MaterialStepCollection.h"

#ifdef TRKDETDESCR_MEMUSAGE
#include "TrkDetDescrUtils/MemoryLogger.h"
#endif

class TTree;
class ITHistSvc;

/** @class MaterialStepToActsMaterialTrack

A simple algorithm that throws random points through detector and associates them with
the given/found layer.

@author Andreas.Salzburger@cern.ch

*/

class MaterialStepToActsMaterialTrack : public AthAlgorithm {

public:

  /** Standard Athena-Algorithm Constructor */
  MaterialStepToActsMaterialTrack(const std::string& name, ISvcLocator* pSvcLocator);

  /** Default Destructor */
  ~MaterialStepToActsMaterialTrack();

  /** standard Athena-Algorithm method */
  StatusCode          initialize();

  /** standard Athena-Algorithm method */
  StatusCode          execute();

  /** standard Athena-Algorithm method */
  StatusCode          finalize();

private:

  TTree* m_outputTree;

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

  std::vector<float> m_step_x;       ///< step x position
  std::vector<float> m_step_y;       ///< step y position
  std::vector<float> m_step_z;       ///< step z position
  std::vector<float> m_step_length;  ///< step length
  std::vector<float> m_step_X0;      ///< step material x0
  std::vector<float> m_step_L0;      ///< step material l0
  std::vector<float> m_step_A;       ///< step material A
  std::vector<float> m_step_Z;       ///< step material Z
  std::vector<float> m_step_rho;     ///< step material rho

  /** output / input steering */
  SG::ReadHandleKey<Trk::MaterialStepCollection>  m_inputMaterialStepCollection;
  std::string                                m_ActsFileName;
  std::string                                m_ActsDirName;
  std::string                                m_ActsTreeName;
  ITHistSvc *                                m_thistSvc;

};

#endif
