/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackFilter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKFILTER_H
#define INDETTRACKFILTER_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>

class AtlasDetectorID;


namespace InDet 
{

  /** @class TrackFilter

      Select potentially interesting events to slim the amount of events to process each time
      
      @author  Christian Schmitt <Christian.Schmitt@cern.ch>
  */  

  class TrackFilter : public AthAlgorithm
    {
    public:

       /** Standard Athena-Algorithm Constructor */
       TrackFilter(const std::string& name, ISvcLocator* pSvcLocator);
       /** Default Destructor */
       ~TrackFilter();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();
       /** standard Athena-Algorithm method */
       StatusCode          execute();
       /** standard Athena-Algorithm method */
       StatusCode          finalize();

    private:
      
      /** member variables for algorithm properties: */
      std::vector<std::string> m_inputTrackCollectionNames;     //!< Name of the Track Collections to read in

      int m_minTRTHits;                           //!< Minimal number of TRT Hits
      int m_minSCTHits;                           //!< Minimal number of SCT Hits
      int m_minPixelHits;                         //!< Minimal number of Pixel Hits
      int m_minIDHits;                            //!< Minimal number of ID Hits
      int m_minSCTonlyHits;                       //!< Minimal number of SCT Hits for SCT only tracks
      int m_minTRTonlyHits;                       //!< Minimal number of TRT Hits for TRT only tracks

      int m_evcount;                              //!< Event counter
      int m_accept;                               //!< Counter of accepted events

      const AtlasDetectorID*                       m_idHelper; //<! Detector ID helper
      
    }; 
} // end of namespace

#endif 
