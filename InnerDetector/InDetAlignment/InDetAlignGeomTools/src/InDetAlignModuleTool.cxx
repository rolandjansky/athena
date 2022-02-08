/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"

#include "TrkAlignEvent/AlignTSOS.h"
#include "InDetAlignGeomTools/InDetAlignModuleTool.h"

namespace InDet {

  //______________________________________________________________
  InDetAlignModuleTool::InDetAlignModuleTool(const std::string& type, const std::string& name, const IInterface* parent)
    : Trk::AlignModuleTool(type,name,parent)
    , m_trtHelper(nullptr)
  {
    ATH_MSG_DEBUG("constructor of InDetAlignModuleTool");
  }

  //________________________________________________________________________
  InDetAlignModuleTool::~InDetAlignModuleTool()
  {

  }

  //________________________________________________________________________
  StatusCode InDetAlignModuleTool::initialize()
  {
    ATH_MSG_DEBUG("Initialize of InDetAlignModuleTool");

    // initialize AlignModuleTool
    if(AlignModuleTool::initialize().isFailure()) {
      msg(MSG::FATAL)<<"Error initializing AlignModuleTool"<<endmsg;
      return StatusCode::FAILURE;
    }

    // Set up TRT ID helper 
    if (detStore()->retrieve(m_trtHelper, "TRT_ID").isFailure()) {
      msg(MSG::FATAL)<<"Could not get TRT ID helper"<<endmsg;
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  StatusCode InDetAlignModuleTool::finalize()
  {
    ATH_MSG_DEBUG("Finalize of InDetAlignModuleTool");

    if(AlignModuleTool::finalize().isFailure()) {
       msg(MSG::FATAL)<<"Error finalizing AlignModuleTool"<<endmsg;
       return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }
  
  //________________________________________________________________________
  int InDetAlignModuleTool::subDetElementIDHash(Identifier id) const
  {
    if (id.is_valid()) {
      if (m_trtHelper->is_trt(id)) {
        return m_trtHelper->straw_hash(id);
      }
    }
    return 0;
  }

  //________________________________________________________________________
  void InDetAlignModuleTool::setMeasurementProperties(Trk::AlignTSOS * atsos) const
  {
    // for SCT endcaps set fan-out angle
    if (atsos->measType() == Trk::TrackState::SCT) {
      const InDetDD::SiDetectorElement * sidet = dynamic_cast<const InDetDD::SiDetectorElement *> (atsos->detelement());
      if(sidet && sidet->isEndcap()) {
        Amg::Vector2D localp = atsos->rio()->localParameters();
        // the next check is probably not needed as a measurement in the
        // SCT endcap element should always have both local x and local y
        // track parameters (according to Thijs)
//        if(localp.contains(Trk::locX) && localp.contains(Trk::locY)) {
          double sinLocal = sidet->sinStereoLocal( Amg::Vector2D(localp[Trk::locX],localp[Trk::locY]) );
          if(sinLocal)
            atsos->setAlphaStrip(asin(sinLocal));
//        }
      }
    }
  }

}
