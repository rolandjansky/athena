//-*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_TrackHypoTool_h
#define ViewAlgs_TrackHypoTool_h

#include <string>
#include "AthenaBaseComps/AthAlgTool.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "AthLinks/ElementLinkVector.h"
#include "TestEDM.h"

static const InterfaceID IID_TrackHypoTool("TrackHypoTool", 1, 0);

class TrackHypoTool : public AthAlgTool {
public:
  TrackHypoTool(const std::string& type, const std::string& name,  const IInterface* parent);

  static const InterfaceID& interfaceID() {     
    return IID_TrackHypoTool; 
  }
  
  StatusCode initialize();
  
  DecoratedStatusCode<bool> decision(ElementLinkVector<TrackCollection> ) const;
  
private:
  float m_trackMultiplicity;
};


#endif

