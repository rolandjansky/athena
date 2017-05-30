/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DSTARVERTEXING_H
#define DSTARVERTEXING_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h" 
#include "DataModel/DataVector.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include <cmath>
#include <vector>
#include <string>
#include <map>

namespace Trk {class IVertexFitter;}

class DstarVertexing: public AthAlgTool, public DerivationFramework::IAugmentationTool {

 public: 
  DstarVertexing( const std::string& t, const std::string& n, const IInterface*  p );
  virtual ~DstarVertexing(); 
  
  virtual StatusCode  initialize();
  virtual StatusCode  addBranches() const;
  virtual StatusCode  finalize();
  
 private: 
  ToolHandle < Trk::IVertexFitter > m_iVertexFitter;
  xAOD::VertexContainer* D0VxContainer;
  xAOD::VertexAuxContainer* D0AuxVxContainer;
  std::string                      m_D0VxContainerName;
  std::string                      m_JetsCollection;
  double                           m_JetPtCut;

}; 

#endif 
