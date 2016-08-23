/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GlobalFitTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUGIRLNSGLOBALFITTOOL_H
#define MUGIRLNSGLOBALFITTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/NTuple.h"
#include "MuGirlInterfaces/IGlobalFitTool.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

class AtlasDetectorID;
class Identifier;
class StoreGateSvc;
class McEventCollection;
class TruthParameters;
class ITruthParameters;
class PerigeeParameters;

namespace Trk
{
  class Track;
  class RIO_OnTrack;
  class MeasurementBase;
  class TrkSurfaceIntersection;
  class StraightLineSurface;
  class CylinderSurface;
  class IIntersector;
}
namespace Muon
{
    class MuonSegment;
    class IMuonCompetingClustersOnTrackCreator;
    class CompetingMuonClustersOnTrack;
    class IMuonTrackFinder;
}
namespace Rec
{
class ICombinedMuonTrackBuilder;
}
namespace MuGirlNS
{

/** @class GlobalFitTool
    @author  Silvia Behar Harpaz <silbehar@tx.technion.ac.il>
*/
class GlobalFitTool : virtual public IGlobalFitTool, public AthAlgTool
{
public:
    GlobalFitTool(const std::string&,const std::string&,const IInterface*);
    /** default destructor */
    virtual ~GlobalFitTool ();

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize  ();
    virtual const Trk::Track* globalFit(const Trk::Track* IDTrack,
                                        const MuonSegmentList & pMuonSegments,
                                        BooleanProperty doNTuple);
    virtual const Trk::Track* globalFit(const MuonSegmentList & pMuonSegments,double qoverp, BooleanProperty doNTuple);
    virtual const Trk::Track* globalFit(const MuonSegmentList & pMuonSegments, const Trk::Perigee* trk1, BooleanProperty doNTuple);

    virtual const Trk::Track* standAloneRefit(const Trk::Track& cbtrk);
   
    virtual StatusCode bookNTuple(NTuple::Tuple* pNTuple, const std::string type);
    virtual StatusCode fillNTuple(const Trk::Track* RefittedTrack);

private:

    ToolHandle<Rec::ICombinedMuonTrackBuilder> m_trackFitter;
    ToolHandle<Muon::MuonIdHelperTool> m_MuonIdHelperTool;
    ToolHandle<Muon::IMuonCompetingClustersOnTrackCreator> m_compClusterCreator;
    ToolHandle<Trk::IIntersector>                   m_pIntersector;
    DoubleProperty m_probCut;

    PerigeeParameters* convertPerigee(const Trk::Perigee* trkPerigee);
    void SortMeasurementSet(Trk::MeasurementSet& inputMbs);
    void SortSegments(const MuonSegmentList & pMuonSegments, MuonSegmentList & pSortedSegments );
    Trk::MeasurementSet prepareMeasurementSet(const MuonSegmentList & pMuonSegments,
                                                  std::vector<const Muon::CompetingMuonClustersOnTrack*>& competingRIO);
    const Trk::Perigee* calculateTrackParameters(const MuonSegmentList & pMuonSegments,double qoverp);
    int m_pRefittedTrkNumTrigHits;
    int m_pRefittedTrkNumMdtHits;
    int m_pRefittedTrkMeasurementSetSize;
    /** member variables for algorithm properties: */
    //         template <class T>
    //         StatusCode retrieve(ToolHandle<T>& pHandle, bool bError = true)
    //         {
    //             StatusCode sc = pHandle.retrieve();
    //             if (sc.isFailure())
    //             {
    //                 if (bError)
    //                 m_log << MSG::ERROR << "Cannot retrieve " << pHandle << endmsg;
    //                 else
    //                 m_log << MSG::WARNING << "Cannot retrieve " << pHandle << endmsg;
    //             }
    //             else
    //             m_log << MSG::INFO << "Retrieved " << pHandle << endmsg;
    //             return sc;
    //         }
    NTuple::Item<float>  m_RefittedTrkPt;        /**< Reffited Track: pT */
    NTuple::Item<float>  m_RefittedTrkEta;       /**< Reffited Track: Eta */
    NTuple::Item<float>  m_RefittedTrkPhi;       /**< Reffited Track: Phi */
    NTuple::Item<float>  m_RefittedTrkTheta;     /**< Reffited Track: Theta */
    NTuple::Item<float>  m_RefittedTrka0;        /**< Refitted Track: a0 */
    NTuple::Item<float>  m_RefittedTrkz0;        /**< Refitted Track: Z0 */
    NTuple::Item<float>  m_RefittedTrkQOverP;    /**< Reffited Track: q/p */
    NTuple::Item<float>  m_RefittedTrkCharge;    /**< Reffited Track: Charge */
    NTuple::Item<float>  m_RefittedTrkChi2;      /**< Reffited Track: Chi^2 */
    NTuple::Item<float>  m_RefittedTrkNDofFr;    /**< Reffited Track: Number of Degrees of Freedom */
    NTuple::Item<float>  m_RefittedTrkChi2pr;    /**< Refitted Track:chi2 probability of the track fit*/
    NTuple::Item<float>  m_RefittedTrkCov11;     /**< Refitted Track: CovarianceMatrix */
    NTuple::Item<float>  m_RefittedTrkCov22;     /**< Refitted Track: CovarianceMatrix */
    NTuple::Item<float>  m_RefittedTrkCov33;     /**< Refitted Track: CovarianceMatrix */
    NTuple::Item<float>  m_RefittedTrkCov44;     /**< Refitted Track: CovarianceMatrix */
    NTuple::Item<float>  m_RefittedTrkCov55;     /**< Refitted Track: CovarianceMatrix */
    
    NTuple::Item<float>  m_RefittedTrkNumTrigHits;
    NTuple::Item<float>  m_RefittedTrkNumMdtHits;
    NTuple::Item<float>  m_RefittedTrkMeasurementSetSize;
    NTuple::Item<float>  m_RefittedTrkMeasurementsOnTrack;
    NTuple::Item<float>  m_RefittedTrkOutliersOnTrack;

    unsigned int m_nrefits;
    unsigned int m_nfailedRefits;
};

} // end of namespace

#endif
