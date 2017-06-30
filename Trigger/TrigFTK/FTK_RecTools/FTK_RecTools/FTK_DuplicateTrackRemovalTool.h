/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// FTK_DuplicateTrackRemovalTool tool
// -------------------------------
// Remove duplicate (overlapping) tracks
// June 2017: Tool created
// Author: Andy Haas, NYU
// e-mail: ahaas@cern.ch
////////////////////////////////////////////////////////////////////////////////

#ifndef __TRIG_FTK_DUPLICATETRACKREMOVAL_TOOL_H__
#define __TRIG_FTK_DUPLICATETRACKREMOVAL_TOOL_H__

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "FTK_RecToolInterfaces/IFTK_DuplicateTrackRemovalTool.h"
#include "FTK_DataProviderInterfaces/IFTK_UncertaintyTool.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"

class FTK_DuplicateTrackRemovalTool : public AthAlgTool, virtual public IFTK_DuplicateTrackRemovalTool
{
 public:
  FTK_DuplicateTrackRemovalTool( const std::string&, const std::string&, const IInterface* );
  virtual ~FTK_DuplicateTrackRemovalTool(){};
  virtual StatusCode initialize();
  virtual  StatusCode finalize  ();
  FTK_RawTrackContainer* removeDuplicates(const FTK_RawTrackContainer* trks);
 private:
    FTK_RawTrackContainer* m_trks_nodups;
    bool match(const FTK_RawTrack* track, const FTK_RawTrack* oldtrack) const;
    const FTK_RawTrack* besttrack(const FTK_RawTrack* track, const FTK_RawTrack* oldtrack) const;
    int m_HW_ndiff;
};

#endif
