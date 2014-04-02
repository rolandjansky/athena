/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNGENTOOLS_REFITSIONLYTOOL_H
#define INDETALIGNGENTOOLS_REFITSIONLYTOOL_H

//////////////////////////////////////////////////////////////////////////
// ================================================
// RefitSiOnly
// ================================================
//
// RefitSiOnlyTool.h
// Header file for RefitSiOnlyTool
//
// Namespace SiRobustAlign
//
// AlgTool to create refitted tracks with only silicon hits present


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetAlignToolInterfaces/ICollectionProcessor.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventUtils/TrkParametersComparisonFunction.h"

namespace Trk {
  class Track;
  class ITrackFitter;
}

class AtlasDetectorID;

namespace InDetAlignment {
  class RefitSiOnlyTool : virtual public InDetAlignment::ICollectionProcessor, public AthAlgTool {
  public:
    RefitSiOnlyTool (const std::string& type, const std::string& name, const IInterface* parent); //constructor
    virtual ~RefitSiOnlyTool(); //destructor

    virtual StatusCode initialize(); ///< Athena Tool methods
    virtual StatusCode finalize();   ///< Athena Tool methods

    /** does necessary configuration to the tool */
    //virtual void configure();
    virtual StatusCode configure();
    /** main processing of track collection -
        refits a track using only filtered hits (e.g. only silicon hits) */
    virtual const DataVector<Trk::Track>* processTrackCollection(const DataVector<Trk::Track>* trks);
    const AtlasDetectorID * m_idHelper;
   
    void dumpTrackCol(DataVector<Trk::Track>* tracks);
   
  private:
    ///refits a track not using non-silicon hits
    Trk::Track* refit(const Trk::Track* tr) const;

    ///determines from flags, if this hit should be included in the fit or not
    bool filterHit(const Identifier& id) const;

       
    ToolHandle<Trk::ITrackFitter> m_ITrkFitter;

    Trk::TrkParametersComparisonFunction*  m_comPar;

    bool                      m_doCosmic;           //!< Flag for cosmics
    bool                      m_OutlierRemoval;     //!< flag for outlier removal

    bool                      m_stripSCTHits;       ///< flag to not consider SCT hits in the refit
    bool                      m_stripPixelHits;     ///< flag to not consider Pixel hits in the refit
    bool                      m_stripTRTHits;       ///< flag to not consider TRT hits in the refit

    int                       m_ParticleNumber;
    Trk::ParticleHypothesis   m_ParticleHypothesis;
    int                       m_trkindex;



  };
} //namespace

#endif // INDETALIGNGENTOOLS_REFITSIONLYTOOL_H
