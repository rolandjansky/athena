//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ELECTRONTOPHOTONTOOL_H
#define ELECTRONTOPHOTONTOOL_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "PhotonAnalysisUtils/IElectronToPhotonTool.h"

#include "egammaInterfaces/IegammaBaseTool.h"
#include "egammaInterfaces/IEMFourMomBuilder.h"
#include "egammaInterfaces/IEMClusterTool.h"

#include "InDetConversionFinderTools/SingleTrackConversionTool.h"

#include <iostream>
#include <fstream>
#include <string>

namespace Analysis{
  class Electron;
}

class ElectronToPhotonTool : public AthAlgTool, virtual public IElectronToPhotonTool, virtual public IIncidentListener {
  
public:
  
  /** AlgTool like constructor */
  ElectronToPhotonTool(const std::string&,const std::string&,const IInterface*);
  
  /**Virtual destructor*/
  virtual ~ElectronToPhotonTool();
  
    /** handle for incident service */
  virtual void handle(const Incident&) {};

  /** AlgTool initialize method.*/
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();

  Analysis::Photon* ConvertElectronToPhoton(const egamma* eg, bool conv, const std::string& egDetailContainerName) ;

private:

  template <class T> T* gimeContainer(const std::string& containerName) ;
  bool makeupConversionVertex(Analysis::Photon* ph, const egamma* eg) ;
  bool hasBadCovarianceMatrix(const Analysis::Electron* el);

  ToolHandle<IEMClusterTool>                   m_clusterTool;
  ToolHandle<IEMFourMomBuilder>                m_fourMomBuilderTool;
  ToolHandle<IegammaBaseTool>                  m_emshowerBuilderTool;
  ToolHandle<InDet::SingleTrackConversionTool> m_singleTrkConvTool;

  std::string m_trackParticleContainerName;

};


#endif 

