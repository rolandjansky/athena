/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// SingleTrackDiffAlg.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_SINGLETRACKDIFFALG_H
#define TRK_SINGLETRACKDIFFALG_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {
    class ITrackDiff;


/** @class SingleTrackDiffAlg
 
   This Validation algorithm takes two track collections and performes
   a diff on one track of each collection. From the reference track collection
   the first track is used and from the compared track collection the one which
   fits best to the reference track (minimum chi2). It can be used for
   single track events.
 
   @author Sebastian.Fleischmann@cern.ch
*/

class SingleTrackDiffAlg : public AthAlgorithm {
public:
    /** Standard Athena-Algorithm Constructor */
    SingleTrackDiffAlg(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~SingleTrackDiffAlg();

    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();

private:
    ToolHandle<Trk::ITrackDiff>    m_trackDiffTool;

    std::string         m_referenceTrackCollection; //!< jobOption: name of the reference TrackCollection
    std::string         m_comparedTrackCollection;  //!< jobOption: name of the compared TrackCollection


};

} // close of namespace

#endif // TRK_SINGLETRACKDIFFALG_H

