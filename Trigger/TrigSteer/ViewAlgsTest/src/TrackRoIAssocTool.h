//-*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_TrackRoIAssocTool_h
#define ViewAlgs_TrackRoIAssocTool_h

#include <string>
#include "AthenaBaseComps/AthAlgTool.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthLinks/ElementLinkVector.h"
//#include "TestEDM.h"

static const InterfaceID IID_TrackRoIAssocTool("TrackRoIAssocTool", 1, 0);

class TrackRoIAssocTool : public AthAlgTool {
public:
  TrackRoIAssocTool(const std::string& type, const std::string& name,  const IInterface* parent);

  static const InterfaceID& interfaceID() {     
    return IID_TrackRoIAssocTool; 
  }
  
  StatusCode initialize();
  
  StatusCode associate(SG::ReadHandle< TrackCollection > tracks , SG::ReadHandle< TrigRoiDescriptorCollection> RoIs, SG::WriteHandle< xAOD::TrigCompositeContainer >&   TCs); 
  StatusCode getTrackLinks(const xAOD::TrigComposite* inputTC,  ElementLinkVector<TrackCollection>& tracks);
  
private:

  double m_dR_size;

    

};


#endif

