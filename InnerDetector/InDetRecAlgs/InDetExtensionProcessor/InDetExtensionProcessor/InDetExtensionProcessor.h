/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetExtensionProcessor.h, (c) ATLAS Detector Softwareop 
///////////////////////////////////////////////////////////////////

#ifndef INDETEXTENSIONPROCESSOR_H
#define INDETEXTENSIONPROCESSOR_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackExtensionMap.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include <vector>
#include <array>
#include <mutex>

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

class InDetExtensionProcessor : public AthReentrantAlgorithm  {

public:

  //! Default Algorithm constructor with parameters
  InDetExtensionProcessor(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode execute_r(const EventContext& ctx) const;
  virtual StatusCode finalize();

private:
 
  //! process events
  TrackCollection *createExtendedTracks(const TrackCollection *tracks_in,
                                        const TrackExtensionMap *track_extension_map) const;

  //! internal structuring: creates new track with original one plus extension as outliers
  Trk::Track* trackPlusExtension(const Trk::Track*,
                                 const std::vector<const Trk::MeasurementBase*>&) const;

  //
  // --- job options
  //
  SG::ReadHandleKey<TrackCollection>    m_trackName;        //!< Name of input Trackcollection
  bool                                  m_cosmics;          //!< whether we are running cosmics
  SG::ReadHandleKey<TrackExtensionMap>  m_extensionMapName; //!< Name of input extension map
  SG::WriteHandleKey<TrackCollection>   m_newTrackName;     //!< Name of output Trackcollection
  ToolHandle<Trk::ITrackFitter>         m_trackFitter;      //!< track fitter tool handle
  ToolHandle<Trk::ITrackScoringTool>    m_scoringTool;      //!< track scorer tool handle
  Trk::RunOutlierRemoval        m_runOutlier;               //!< whether to run outlier logics
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
  Trk::ParticleHypothesis       m_particleHypothesis; //!< nomen est omen

  // -- statistics protected by mutex
  mutable std::mutex  m_statMutex;
  mutable int         m_Nevents;
  mutable std::vector<int> m_Ninput, m_Nrecognised, m_Nextended, m_Nrejected, m_Nfailed,
                           m_NrecoveryBremFits,m_NbremFits,m_Nfits,m_NnotExtended,m_NlowScoreBremFits,
                           m_NextendedBrem;

  //! internal monitoring: categories for counting different types of extension results
  enum StatIndex {iAll, iBarrel, iTransi, iEndcap , Nregions};
  std::vector<float>  m_etabounds;           //!< eta intervals for internal monitoring

  //! monitoring and validation: does success/failure counting
  void increment_by_eta(std::array<int,4>&,const Trk::Track*,bool=true) const;

};

}

#endif //INDETEXTENSIONPROCESSOR_H

