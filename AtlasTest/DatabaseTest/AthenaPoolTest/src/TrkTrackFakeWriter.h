/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRACKFAKEWRITER_H
#define TRKTRACKFAKEWRITER_H

/**
* @file TrkTrackFakeWriter.h
*
* @brief Test Algorithm for POOL I/O uses TrkTracks as test data
*
* @author Ed Moyse <Edward.Moyse@cern.ch>
* @author RD Schaffer <R.D.Schaffer@cern.ch>
*
* $Id: TrkTrackFakeWriter.h,v 1.6 2008-02-14 16:25:51 emoyse Exp $
*
*/

/**
* @class TrkTrackFakeWriter
*
* @brief Test Algorithm for POOL I/O uses TrkTracks as test data
*
*/


#include "AthenaBaseComps/AthAlgorithm.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "FakeTrackBuilder.h"
class PixelID;
class SCT_ID;
class TRT_ID;


class TrkTrackFakeWriter : public AthAlgorithm {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
public:

    /// Algorithm constructor
  TrkTrackFakeWriter(const std::string &name,ISvcLocator *pSvcLocator);

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

    /// Avoid use of default constructor
  TrkTrackFakeWriter();

    /// Avoid use of copy constructor
  TrkTrackFakeWriter(const TrkTrackFakeWriter&);

    /// Avoid use of copy operator
  TrkTrackFakeWriter &operator=(const TrkTrackFakeWriter&);

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:

    /// Access to event store
    std::string  m_pixMgrLocation;                    //!< Location of pixel Manager 
    const InDetDD::PixelDetectorManager*  m_pixMgr;   //!< Detector Manager
    unsigned int m_eventcounter;
    bool         m_doInDet;
    bool         m_addBrokenTracks;

};

#endif // INDETRAWDATAFAKEWRITER_H


