/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/* Dear emacs, this is -*-c++-*- */
#ifndef INDETPHYSVALMONITORING_DUMMYTRACKSLIMMINGTOOL_H
#define INDETPHYSVALMONITORING_DUMMYTRACKSLIMMINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/ITrackSlimmingTool.h"

class DummyTrackSlimmingTool: virtual public Trk::ITrackSlimmingTool, public AthAlgTool
{
public:
  DummyTrackSlimmingTool(const std::string&, const std::string&, const IInterface*);
  /** default destructor */
  virtual ~DummyTrackSlimmingTool () {
  }

  /** standard Athena-Algorithm method */
  virtual StatusCode
  initialize() override {
    return StatusCode::SUCCESS;
  }

  /** standard Athena-Algorithm method */
  virtual StatusCode
  finalize() override {
    return StatusCode::SUCCESS;
  }

  /**This method 'skims' interesting information from the passed track, and creates a new one with cloned copies of this
     information
     @param track A reference to the track to be skimmed. It will not be modified in any way.
     @return A 'slimmed' version of 'track', where exactly what information is copied depends on how the tool is
        configured
   */
  virtual Trk::Track* slim(const Trk::Track& track) const override;
private:
};

#endif
