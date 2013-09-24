/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetExtensionProcessor.h, (c) ATLAS Detector Softwareop 
///////////////////////////////////////////////////////////////////

#ifndef INDETEXTENSIONPROCESSOR_H
#define INDETEXTENSIONPROCESSOR_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackExtensionMap.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include <vector>

namespace Trk{
  class ITrackScoringTool;
  class ITrackFitter;
}

namespace InDet {

 /** @brief Algorithm to process a TrackExtensionMap, refit the
     extended tracks and decide if the old track is to be
     replaced by the extended one.
     
     @author Markus.Elsing@cern.ch
     */
 
class InDetExtensionProcessor : public AthAlgorithm  {

public:

  //! Default Algorithm constructor with parameters
  InDetExtensionProcessor(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode initialize(); //!< athena method
  virtual StatusCode execute();    //!< athena method
  virtual StatusCode finalize();   //!< athena method

private:
 
  //! process events
  void process();

  //! internal structuring: creates new track with original one plus extension as outliers
  Trk::Track* trackPlusExtension(const Trk::Track*,
                                 const std::vector<const Trk::MeasurementBase*>&) const;

  //
  // --- job options
  //
  std::string                   m_TrackName;        //!< Name of input Trackcollection
  bool                          m_cosmics;          //!< whether we are running cosmics
  std::string                   m_ExtensionMapName; //!< Name of input extension map
  std::string                   m_NewTrackName;     //!< Name of output Trackcollection
  ToolHandle<Trk::ITrackFitter> m_ITrackFitter;     //!< track fitter tool handle
  ToolHandle<Trk::ITrackScoringTool> m_scoringTool; //!< track scorer tool handle
  Trk::RunOutlierRemoval        m_runOutlier;       //!< whether to run outlier logics
  //! switch whether to append a rejected extension as outlier trajectory
  bool                          m_keepFailedExtensionOnTrack;
  //! fitter steering: whether to do the fit with re-calibrated clusters (true) or not
  bool                          m_refitPrds;                  
  //! type of material interaction in extrapolation
  int                           m_matEffects;                 
  bool                          m_suppressHoleSearch;  //!< no hole search ?
  bool                          m_tryBremFit;          //!< brem recovery mode ?
  bool                          m_caloSeededBrem;      //!< run in Calo seeded mode
  float                         m_pTminBrem;           //!< min pt for a brem fit
  
  // -- algorithm members
  const TrackCollection*        m_tracks;
  const TrackExtensionMap*      m_trackExtensionMap;
  TrackCollection*              m_newtracks;          //!< collection pointer for new tracks
  Trk::ParticleHypothesis       m_ParticleHypothesis; //!< nomen est omen

  // -- statistics
  int m_Nevents;
  std::vector<int> m_Ninput, m_Nrecognised, m_Nextended, m_Nrejected, m_Nfailed,
    m_NrecoveryBremFits,m_NbremFits,m_Nfits,m_NnotExtended,m_NlowScoreBremFits, m_NextendedBrem;
  //! internal monitoring: categories for counting different types of extension results
  enum StatIndex {iAll = 0, iBarrel = 1, iTransi = 2, iEndcap = 3};
  std::vector<float>            m_etabounds;           //!< eta intervals for internal monitoring

  //! monitoring and validation: does success/failure counting
  void increment_by_eta(std::vector<int>&,const Trk::Track*,bool=true);

};

} 

#endif //INDETEXTENSIONPROCESSOR_H

