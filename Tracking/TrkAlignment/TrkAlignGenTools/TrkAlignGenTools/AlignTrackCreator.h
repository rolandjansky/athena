/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNGENTOOLS_ALIGN_TRACK_CREATOR_H
#define TRKALIGNGENTOOLS_ALIGN_TRACK_CREATOR_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkAlignInterfaces/IAlignTrackCreator.h"

#include <vector>

/**
   @file AlignTrackCreator.h
   @class AlignTrackCreator
   
   @brief Tool used to create an AlignTrack containing all TSOS on a track, including scatterers.

   @author Robert Harrington <roberth@bu.edu>
   @date 1/5/08
*/

class AtlasDetectorID;

namespace Trk {

  class AlignTrack;
  class AlignTSOS;
  class MeasurementTypeID;
  class IAlignModuleTool;
  class IAlignResidualCalculator;
  
  class AlignTrackCreator : virtual public IAlignTrackCreator, public AthAlgTool {

  public:
    AlignTrackCreator(const std::string& type, const std::string& name,
		     const IInterface* parent);
    virtual ~AlignTrackCreator();

    StatusCode initialize();
    StatusCode finalize();

    /** creates AlignTrack containing all TSOS on track */
    bool processAlignTrack(AlignTrack* track) const;
   
  private:

    // private variables
    ToolHandle<IAlignModuleTool>                  m_alignModuleTool;
    ToolHandle<IAlignResidualCalculator>          m_residualCalculator;

    const AtlasDetectorID*   m_idHelper;
    MeasurementTypeID* m_measTypeIdHelper;

    mutable std::vector< std::pair<int,int> > m_goodEventList; //!> good events read in from ASCII file

    std::string m_eventListName;             //!> name of event list ASCII file    
    bool m_writeEventList;                   //!> write selected events to event list ASCII file
    bool m_requireOverlap;                   //!> keep only tracks that pass through 2 or more AlignModules
    bool m_removeATSOSNotInAlignModule;      //!> remove AlignTSOS not in AlignModules
    bool m_includeScatterers;                //!> includes scatterers on track

  }; // end class

} // end namespace

#endif // TRKALIGNGENTOOLS_ALIGN_TRACK_CREATOR_H
