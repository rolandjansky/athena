/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRACKFAKEREADER_H
#define TRKTRACKFAKEREADER_H

/**
 * @file TrkTrackFakeReader.h
 *
 * @brief Test Algorithm for POOL I/O uses TrkTracks as test data
 *
 * @author Ed Moyse <Edward.Moyse@cern.ch>
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: TrkTrackFakeReader.h,v 1.2 2005-01-10 18:05:52 schaffer Exp $
 *
 */

/**
 * @class TrkTrackFakeReader
 *
 * @brief Test Algorithm for POOL I/O uses TrkTracks as test data
 *
 */

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include <string>


namespace Trk {
  class Track;
}
class TrkTrackFakeReader : public AthAlgorithm {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
public:

    /// Algorithm constructor
    TrkTrackFakeReader(const std::string &name,ISvcLocator *pSvcLocator);
  
    /// Algorithm initialize at begin of job
    virtual StatusCode initialize();

    /// Algorithm execute once per event
    virtual StatusCode execute(); 

    /// Algorithm finalize at end of job
    virtual StatusCode finalize();
  
    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
private:
  
    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:

    void compareTracks(const Trk::Track* ref, const Trk::Track* readTrk);
 
    /// Access to event store
    std::string  m_pixMgrLocation;                    //!< Location of pixel Manager 
    const InDetDD::PixelDetectorManager*  m_pixMgr;   //!< Detector Manager
};

#endif // INDETRAWDATAFAKEREADER_H
