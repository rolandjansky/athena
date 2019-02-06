/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// StauTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUGIRLNSSTAUTOOL_H
#define MUGIRLNSSTAUTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuGirlInterfaces/IStauTool.h"
#include "MuGirlInterfaces/MdtSegmentMakerInfo.h"
#include "MuGirlInterfaces/RpcSegmentInfo.h"
#include "MuGirlStau/StauToolDefinitions.h"
#include "MuGirlStau/StauCalibration.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuGirlStau/StauCollection.h"
#include "TrkTrack/Track.h"
#include "MuGirlInterfaces/IGlobalFitTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"

#include <map>
#include <boost/regex.hpp>
#include "TFile.h"
inline std::vector<std::string> split(const std::string& input, const std::string& delim)
{
//    boost::regex re_delim(delim);
    boost::sregex_token_iterator first(input.begin(), input.end(), boost::regex(delim), -1), last;
    return {first, last};
}

inline std::string methodName(const std::string& prettyFunction)
{
    auto paren = prettyFunction.find('(');
    auto space = prettyFunction.rfind(' ', paren);
    if (space == prettyFunction.npos) space = 0;
    auto name_prefix = prettyFunction.substr(space, paren - space);
    auto names = split(name_prefix, "::");
    auto num_names = names.size();
    return num_names >= 2 ? (names[num_names - 2] + "::" + names[num_names - 1]) : names[0];
}

#define __METHOD_NAME__ methodName(__PRETTY_FUNCTION__)
#define LOG_INFO m_log << MSG::INFO << __METHOD_NAME__ << "() - "
#define LOG_DEBUG m_log << MSG::DEBUG << __METHOD_NAME__ << "() - "
#define LOG_VERBOSE m_log << MSG::VERBOSE << __METHOD_NAME__ << "() - "

namespace Muon
{
class IMdtDriftCircleOnTrackCreator;
class IMuonSegmentMaker;
class MuonSegment;
}
class MdtCalibrationDbSvc;

namespace Rec
{
class IParticleCaloCellAssociationTool;
}

namespace MuGirlNS
{
class StauNTuple;
class IStauBetaTofTool;
class StauRPC;
class StauMDT;
class StauGF;
class StauTileCal;
class StauMDTT;

/** @class StauTool

 This is for the Doxygen-Documentation.
 Please delete these lines and fill in information about
 the Algorithm!
 Please precede every member function declaration with a
 short Doxygen comment stating the purpose of this function.

 @author  shikma bressler <sshikma@tx.technion.ac.il>
 */

class StauTool: public extends1<AthAlgTool, IStauTool>
{
public:
    StauTool(const std::string&, const std::string&, const IInterface*);

    /** default destructor */
    virtual ~StauTool();

    /** standard Athena-Algorithm method */
    StatusCode initialize();
    /** standard Athena-Algorithm method */
    StatusCode finalize();

    /** interface to init necessary variable */
    void initTool(bool doGlobalFit)
    {
        m_doGlobalFit = doGlobalFit;
    }

    /** Main interace to process stau */
    StatusCode processStau(const xAOD::TrackParticle* trkParticle,
            const MuonSegmentList& muonSegments,
            const MuGirlNS::MdtSegmentMakerInfoList& mdtSegmentMakerInfoList,
            const MuGirlNS::RIO_OnTrackLists& rpcHitsInSegments,
            const MuGirlNS::RIO_OnTrackLists& tgcHitsInSegments,
            const Trk::Track* pMuonRefittedTrack = NULL,
            const xAOD::TrackParticle* pMuonTrackParticle = NULL,
            Amg::Vector3D trigMomentum = Amg::Vector3D(0., 0., 0.),
            const Trk::Perigee* trk0 = NULL,
            bool isMuonBetaRefit = false);

    StatusCode processStauCosmic(const xAOD::TrackParticle* trkParticle,
            const MuonSegmentList& muonSegments,
            const MuGirlNS::MdtSegmentMakerInfoList& mdtSegmentMakerInfoList,
            const MuGirlNS::RIO_OnTrackLists& rpcHitsInSegments,
            const MuGirlNS::RIO_OnTrackLists& tgcHitsInSegments,
            Amg::Vector3D trigMomentum);

    StatusCode readTagInfo(IOVSVC_CALLBACK_ARGS);
    void clearCandidate();
    bool recalculateFirstSteps();
    bool selectRange(double& min, double& max);
    bool selectRangeTTrack(double& min, double& max);
    void clearNewMdtSegments();
    StatusCode fillStauSummary(const CandidateSummary* summary, CandidateSummary* stauSummary);
    StatusCode fillStauExtras(const CandidateSummary* stauSummary, StauExtras* stauExtras);
    StatusCode fillRHExtras(RHExtras* rhExtras);
    void fillStauContainer();
    void recordCollection();
    void setCandidateAnn(double ann);

    /**ntuple functions*/
    StatusCode bookNTuple(NTuple::Tuple* pNTuple);
    StatusCode fillNTuple();

    //void setCandidateAnn(double);//{}
    bool doStauNtuple()
    {
        return (m_doMuGirlNtuple && m_doStauNtuple);
    }

    double beta()
    {
        return m_beta;
    }
    double mass()
    {
        return m_mass;
    }
    double tzero()
    {
        return m_tzero;
    }

    void setSegmentManager(SegmentManager* pSegmentManager)
    {
        m_pSegmentManager = pSegmentManager;
    }
    SegmentManager* getSegmentManager()
    {
        return m_pSegmentManager;
    }

    FcnStepsData* fcnStepsData()
    {
        return m_pFcnSteps;
    }
    bool addMuToF() const
    {
        return m_addMuToF;
    }
    bool rpcBugFix() const
    {
        return m_rpcBugFix;
    }
    const MuGirlNS::RIO_OnTrackLists* rpcHitsInSegments() const
    {
        return m_pRpcHitsInSegments;
    }
    const MuGirlNS::RIO_OnTrackLists* tgcHitsInSegments() const
    {
        return m_pTgcHitsInSegments;
    }
    const MuGirlNS::MdtSegmentMakerInfoList* mdtSegmentMakerInfoList() const
    {
        return m_pMdtSegmentMakerInfoList;
    }
    ToolHandle<Muon::IMuonSegmentMaker>& mdtSegmentMaker()
    {
        return m_pMdtSegmentMaker;
    }
    const ToolHandle<Muon::IMuonSegmentMaker>& mdtSegmentMaker() const
    {
        return m_pMdtSegmentMaker;
    }
    const ToolHandle<Muon::IMdtDriftCircleOnTrackCreator>& driftCircleCreator() const
    {
        return m_pMdtDriftCircleCreator;
    }
    ToolHandle<IStauBetaTofTool>& tofTool()
    {
        return m_pTofTool;
    }
    const ToolHandle<IStauBetaTofTool>& tofTool() const
    {
        return m_pTofTool;
    }
    ToolHandle<MuGirlNS::IGlobalFitTool>& globalFitTool()
    {
        return m_pGlobalFitTool;
    }
    const ToolHandle<MuGirlNS::IGlobalFitTool>& globalFitTool() const
    {
        return m_pGlobalFitTool;
    }
    const ToolHandle<Rec::IParticleCaloCellAssociationTool>& caloCellAssociationTool() const
    {
        return m_caloCellAssociationTool;
    }

    const ServiceHandle<MdtCalibrationDbSvc>& mdtCalibrationDbSvc()
    {
        return m_pMdtCalibDbSvc;
    }
    double idP() const
    {
        return m_idP;
    }
    virtual double stauP()
    {
        return m_p;
    }
    const Trk::Track* idTrack() const
    {
        return m_pIdTrack;
    }
    virtual MdtSegments* newMdtSegments()
    {
        return m_pNewMdtSegments;
    }
    const MuonGM::MuonDetectorManager* muonMgr()
    {
        return m_pMuonMgr;
    }
    void setRefittedTrack(const Trk::Track* pTrack);
    const Trk::Track* refittedTrack()
    {
        return m_pRefittedTrack;
    }
    StauRPC* pStauRPC()
    {
        return m_pStauRPC;
    }
    StauMDT* pStauMDT()
    {
        return m_pStauMDT;
    }
    StauGF* pStauGF()
    {
        return m_pStauGF;
    }
    StauTileCal* pStauTileCal()
    {
        return m_pStauTileCal;
    }
    StauMDTT* pStauMDTT()
    {
        return m_pStauMDTT;
    }
    bool noIDTrack()
    {
        return (m_pIdTrack == NULL);
    }
    double rpcTimeShift()
    {
        return m_rpcTimeShift;
    }
    double tileEnergyCut()
    {
        return m_tileEnergyCut;
    }
    bool doGlobalFit() const
    {
        return m_doGlobalFit;
    }
    bool doMdt() const
    {
        return m_doMdt;
    }
    bool doRpc() const
    {
        return m_doRpc;
    }
    bool isData() const
    {
        return m_isData;
    }
    bool mdtSmearFactor() const
    {
        return m_mdtSmearFactor;
    }
    bool rpcSmearFactor() const
    {
        return m_rpcSmearFactor;
    }
    bool tileSmearFactor() const
    {
        return m_tileSmearFactor;
    }
    bool doTileCal() const
    {
        return m_doTileCal;
    }
    void setRpcHitsLists(StauRpcHitsLists* rpcHitsLists)
    {
        m_rpcHitsByStations = rpcHitsLists;
    }
    std::map<StauTechnology, BetaChi2> techContribution2Chi2()
    {
        return m_techContribution2Chi2;
    }
    std::map<StauTechnology, BetaAvg> techBetaAvg()
    {
        return m_techBetaAvg;
    }
    double avgBeta(StauTechnology eTech)
    {
        return m_techBetaAvg[eTech].beta;
    }
    double rmsBeta(StauTechnology eTech)
    {
        return m_techBetaAvg[eTech].rms;
    }
    std::map<StauTechnology, BetaChi2> techBetaChi2()
    {
        return m_techBetaChi2;
    }
    double beta(StauTechnology eTech)
    {
        return m_techBetaChi2[eTech].beta;
    }
    double chi2(StauTechnology eTech)
    {
        return m_techBetaChi2[eTech].chi2;
    }
    double dof(StauTechnology eTech)
    {
        return m_techBetaChi2[eTech].dof;
    }
    double lowerBetaLimit()
    {
        return m_lowerBetaLimit;
    }
    double upperBetaLimit()
    {
        return m_upperBetaLimit;
    }

    double lowerTTrackLimit()
    {
        return m_lowerTTrackLimit;
    }
    double upperTTrackLimit()
    {
        return m_upperTTrackLimit;
    }
    inline bool hasSummary()
    {
        return m_hasSummary;
    }
    bool doCalibration() const
    {
        return m_doCalibration;
    }
    inline StauCalibration& calibration()
    {
        return m_calibration;
    }
private:

    /** add muon tof flag  */
    bool m_addMuToF;

    /** rpc bug fix flag - to use on files digitized in releases 12.0.6 - 15.0.0 */
    bool m_rpcBugFix;

    /** rpc time shift - to correct jitter calibration */
    double m_rpcTimeShift;

    /** Use only tile cells with energy larger than this */

    double m_tileEnergyCut;
    /** */
    bool m_doGlobalFit;
    bool m_doMdt;
    bool m_doRpc;
    bool m_doTileCal;
    bool m_isData;
    double m_mdtSmearFactor;
    double m_rpcSmearFactor;
    double m_tileSmearFactor;
    bool m_doCalibration;

    /* calibration */
    std::string m_mdtCalibFileName;
    std::string m_rpcCalibFileName;
    std::string m_caloCalibFileName;

    /** Stau collection - save info in store gate */
    StauCollection* m_pCollection;

    /** tool handles */
    ToolHandle<Muon::IMuonSegmentMaker> m_pMdtSegmentMaker;
    ToolHandle<Muon::IMdtDriftCircleOnTrackCreator> m_pMdtDriftCircleCreator;
    ToolHandle<IStauBetaTofTool> m_pTofTool;
    ToolHandle<MuGirlNS::IGlobalFitTool> m_pGlobalFitTool;
    ToolHandle<Rec::IParticleCaloCellAssociationTool> m_caloCellAssociationTool; //!< Tool to make the step-wise extrapolation

    ServiceHandle<MdtCalibrationDbSvc> m_pMdtCalibDbSvc;
    ServiceHandle<IAtRndmGenSvc> m_randSvc;
    std::string m_randStreamName;

    const MuonGM::MuonDetectorManager* m_pMuonMgr; /* the muon detector manager*/

    /** member variables for algorithm properties: */
    double m_lowerBetaLimit; /** the lower beta limit */
    double m_upperBetaLimit; /** the upper beta limit */
    double m_betaTolerance;
    double m_gfBetaTolerance; /** the global fit beta tolerance */

    double m_minBeta; /** the lower beta limit - after fixing the range */
    double m_maxBeta; /** the upper beta limit - after fixing the range*/

    double m_lowerTTrackLimit; /** the lower ttrack limit */
    double m_upperTTrackLimit; /** the upper ttrack limit */

    double m_minTTrack; /** the lower ttrack limit - after fixing the range */
    double m_maxTTrack; /** the upper ttrack limit - after fixing the range*/

    //bool m_mdtOnTheEdge;
    double m_lowerLimitNoMdt;
    double m_upperLimitNoMdt;

    /** NTuple related parameters */
    StauNTuple* m_pStauNtuple; //the ntuple
    bool m_doMuGirlNtuple;
    bool m_doStauNtuple;

    /** The 'technologies' */
    StauRPC* m_pStauRPC;
    StauMDT* m_pStauMDT;
    StauGF* m_pStauGF;
    StauTileCal* m_pStauTileCal;
    StauMDTT* m_pStauMDTT;

    /** segment manager related parameters */
    std::vector<const Muon::MuonSegment*> m_mdtSegments;
    SegmentManager* m_pSegmentManager;

    /** the tool input */
    unsigned int m_skipStation;
    const MuGirlNS::MdtSegmentMakerInfoList* m_pMdtSegmentMakerInfoList;
    const MuGirlNS::RIO_OnTrackLists* m_pRpcHitsInSegments;
    const MuGirlNS::RIO_OnTrackLists* m_pTgcHitsInSegments;
    double m_idP;
    const xAOD::TrackParticle* m_pTrkParticle;
    const Trk::Track* m_pIdTrack;
    Amg::Vector3D m_trigP;

    /** the tool output */
    std::map<StauTechnology, BetaChi2> m_techBetaChi2;
    std::map<StauTechnology, BetaAvg> m_techBetaAvg;
    std::map<StauTechnology, BetaChi2> m_techContribution2Chi2;
    double m_beta;   //the beta to the AOD - for now m_beta = m_betaMS
    double m_mass;
    double m_tzero;
    double m_p;

    double m_tTrack;
    MdtSegments* m_pNewMdtSegments; // list of segments with the new beta
    const Trk::Track* m_pRefittedTrack;
    const Trk::Track* m_pMuonRefittedTrack;
    StauRpcHitsLists* m_rpcHitsByStations; //list of RPC hits
    bool m_hasSummary; //true if a stau summary was created to this candidate. Default is false;

    /** single step parameters
     clean this parameters when call to the FCN */
    FcnStepsData* m_pFcnSteps;

    /** candidate summary parameters */
    int m_numMdtSegs;
    int m_numMdtHitsInSeg;

    /** Calibration */
    StauCalibration m_calibration;

    /** */
    void printFcnSteps();
    void clearFcnSteps();
    void initializeCandidate(const xAOD::TrackParticle* trkParticle,
            const MuGirlNS::MuonSegmentList& muonSegments,
            const MuGirlNS::MdtSegmentMakerInfoList& mdtSegmentMakerInfoList,
            const MuGirlNS::RIO_OnTrackLists& rpcHitsInSegments,
            const MuGirlNS::RIO_OnTrackLists& tgcHitsInSegments,
            const Trk::Track* pMuonRefittedTrackParticle,
            const xAOD::TrackParticle* pMuonTrackParticle,
            Amg::Vector3D trigMomentum);
    double stauMomentum();
    double stauMass();
    double computeTechnologyBeta(StauTechnology eTech, double minBeta, double maxBeta);
    void updateTechBetaChi2(StauTechnology eTech);
    void printTechBetaChi2();
    void updateTechContribution2Chi2();
    void printTechContribution2Chi2();
};
} // end of namespace

#endif
