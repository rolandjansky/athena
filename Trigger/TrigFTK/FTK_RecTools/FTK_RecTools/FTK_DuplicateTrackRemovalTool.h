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

#ifndef TRIG_FTK_DUPLICATETRACKREMOVAL_TOOL_H
#define TRIG_FTK_DUPLICATETRACKREMOVAL_TOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "FTK_RecToolInterfaces/IFTK_DuplicateTrackRemovalTool.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include <map>

//#define FTKDuplicateTrackRemovalTiming
#define FTKDuplicateTrackRemovalUseMap
//#define FTKDuplicateTrackRemovalTestMultiple

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
    double m_dphi_roughmatch;
#ifdef FTKDuplicateTrackRemovalUseMap
    std::map<double, std::vector<unsigned int> > m_phimap;//keep track of the phi of tracks in the output container - at each phi there could be a vector of track indices...
    void addtophimap(double trackphi, unsigned int pos);
    void removefromphimap(double trackphi, unsigned int pos);
#endif
};

#endif
