/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGALGS_PHOTONTAGBUILDER_H
#define EVENTTAGALGS_PHOTONTAGBUILDER_H

/**
   Tag Builder for Photon
*/

#include "ElectronPhotonTagTools/PhotonTagTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "EventTagAlgs/TagBuilderBase.h"

class PhotonTagBuilder : public AthAlgorithm, public TagBuilderBase
{
public:

  //! Standard constructor.
  PhotonTagBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  
  //! Destructor.
  ~PhotonTagBuilder();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
 
  //! Attribute list name
  std::string m_attributeListName;

  //! number of particles
  int m_MAX_NUMBER;

  //! the photon tag tool
  ToolHandle<PhotonTagTool> m_photonTagTool;
};

#endif
