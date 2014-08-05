/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGALGS_PHOTONTAGBUILDER_H
#define EVENTTAGALGS_PHOTONTAGBUILDER_H

/**
   Tag Builder for Photon
*/

#include "ElectronPhotonTagTools/PhotonTagTool.h"

class StoreGateSvc;

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "EventTagAlgs/TagBuilderBase.h"

class PhotonTagBuilder : public Algorithm, public TagBuilderBase
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

  //! StoreGate service.
  StoreGateSvc* m_storeGateSvc;

  //! the photon tag tool
  ToolHandle<PhotonTagTool> m_photonTagTool;

};

#endif
