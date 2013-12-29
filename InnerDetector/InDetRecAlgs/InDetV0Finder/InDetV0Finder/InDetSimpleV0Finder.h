/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          InDetSimpleV0Finder.h  -  Description
                             -------------------
    begin   : 20-07-2005
    authors : Evelina Bouhova-Thacker (Lancater University), Rob Henderson (Lancater University)
    email   : e.bouhova@cern.ch, r.henderson@lancaster.ac.uk
    changes :

 ***************************************************************************/

#ifndef INDETSIMPLEV0FINDER_INDETV0FINDER_H
#define INDETSIMPLEV0FINDER_INDETV0FINDER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetConversionFinderTools/InDetConversionFinderTools.h"
#include "TrkParticleBase/TrackParticleBase.h"

/**
   @class InDetSimpleV0Finder
   Execute method for the main V0 finding module.
   InDetSimpleV0Finder first gets its input tracks from StoreGate.
*/

/* Forward declarations */
namespace Trk
{
  class IVertexFitter;
  class TrkV0VertexFitter;
  class V0Tools;
  class IExtrapolator;
  class ExtendedVxCandidate;
  class ITrackSelectorTool;
}

namespace InDet
{
  class VertexPointEstimator;

  class InDetSimpleV0Finder : public AthAlgorithm

  {
  public:
    InDetSimpleV0Finder(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~InDetSimpleV0Finder();
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    StatusCode resetStatistics();

  protected:

    ToolHandle < Trk::IVertexFitter > m_iVertexFitter;
    ToolHandle < Trk::IVertexFitter > m_iVKVertexFitter;
    ToolHandle < Trk::V0Tools > m_V0Tools;
    ToolHandle < InDet::ConversionFinderUtils > m_helpertool;
    ToolHandle < Trk::ITrackSelectorTool > m_trkSelector;
    ToolHandle < InDet::VertexPointEstimator > m_vertexEstimator;
    ToolHandle < Trk::IExtrapolator > m_extrapolator;

    bool          m_useV0Fitter;              //!< = true if using TrkV0Fitter, = false if using VKalVert
    double        m_minVertProb;              //!< Minimum vertex probability (0.0001)
    bool          m_samesign;
    bool          m_usePVtracks;
    std::string   m_TrkParticleCollection ;   //!< Name of input track particle collection
    std::string   m_V0CandidatesOutputName;   //!< Name of output container to store results

    long          m_events_processed;
    long          m_V0s_stored;

    bool doFit(const Trk::TrackParameters* track1, const Trk::TrackParameters* track2, Amg::Vector3D &startingPoint);

  };

}//end of namespace InDet

#endif

