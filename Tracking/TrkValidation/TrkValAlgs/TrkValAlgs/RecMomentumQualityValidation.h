/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RecMomentumQualityValidation.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef RECMOMENTUMQUALITYVALIDATION_H
#define RECMOMENTUMQUALITYVALIDATION_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>


class AtlasDetectorID;

namespace Trk {

  class ITruthToTrack;
  class ITrackSelectorTool;

  /** @class RecMomentumQualityValidation

      This is for automatising the 100%, 50%, 20%, 10% tables for (indet) tracks

      @author  Wolfgang Liebig (Wolfgang.Liebig -at- cern.ch)
  */  

class RecMomentumQualityValidation : public AthAlgorithm {

    public:

      /** Standard Athena-Algorithm Constructor */
      RecMomentumQualityValidation(const std::string& name, ISvcLocator* pSvcLocator);
      /** Default Destructor */
      ~RecMomentumQualityValidation();

      /** standard Athena-Algorithm method */
      StatusCode          initialize();
      /** standard Athena-Algorithm method */
      StatusCode          execute();
      /** standard Athena-Algorithm method */
      StatusCode          finalize();

    private:

      /** method: make the output table */
      void printTable() const;
      static void monitorTrackFits(std::vector<unsigned int>&, const double&) ;

      /** properties from JobOptions: */
      std::string    m_inputTrackCollection; //!< job option: the reco track collection name
      std::string    m_trackTruthCollection; //!< job option: the truth track collection name
      ToolHandle<Trk::ITruthToTrack> m_truthToTrack; //!< Tool handle to Trk::ITruthToTrack tool
      ToolHandle<Trk::ITrackSelectorTool> m_trackSelector;   //!< Tool handle to Trk::ITrackSelectorTool
      const AtlasDetectorID* m_idHelper;

      //! define ranges in eta
      enum StatIndex {iAll = 0, iBarrel = 1, iTransi = 2, iEndcap = 3};
      // TOFO: define also eta boundary limits

      /** counters */
      std::vector<unsigned int> m_nHundred, m_nFifty, m_nTwenty, m_nTen, m_nFakeOrLost;
      std::vector<unsigned int> m_tHundred, m_tFifty, m_tTwenty, m_tTen, m_tFakeOrLost;
};

} // end of namespace

#endif 
