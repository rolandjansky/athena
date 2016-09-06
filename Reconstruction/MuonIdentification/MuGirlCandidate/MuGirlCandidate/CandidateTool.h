/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CandidateTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUGIRLNSCANDIDATETOOL_H
#define MUGIRLNSCANDIDATETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuGirlInterfaces/ICandidateTool.h"
#include "MuGirlCandidate/Distance.h"
#include "MuGirlCandidate/MuGirlNS.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "Identifier/Identifier.h"

class IRegSelSvc;

namespace Rec
{
class IMuidCaloEnergy;
}
namespace MuonGM
{
class MuonDetectorManager;
}

namespace Muon
{
class IMdtDriftCircleOnTrackCreator;
class IMuonClusterOnTrackCreator;
class IMuonSegmentMaker;
class IMuonRdoToPrepDataTool;
class MuonEDMHelperTool; 
class MuonIdHelperTool;
class IMuonSegmentSelectionTool;
class MuonLayerHoughTool;
}

class ICscClusterBuilder;

namespace Trk
{
class IIntersector;
//class Surface;
}

namespace MuGirlNS
{
class Distance;
class Technology;
class StationManger;
class Candidate;
class CandidateNtuple;

/** @class CandidateTool

    This is for the Doxygen-Documentation.
    Please delete these lines and fill in information about
    the Algorithm!
    Please precede every member function declaration with a
    short Doxygen comment stating the purpose of this function.

    @author  Zvi Tarem <zvi@caliper.com>
*/

class CandidateTool : virtual public ICandidateTool, public AthAlgTool
{
public:
    CandidateTool(const std::string&,const std::string&,const IInterface*);

    /** default destructor */
    virtual ~CandidateTool ();

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize  ();

    virtual StatusCode fill(int iTrack, const xAOD::TrackParticle* pTrackParticle, const Trk::TrackParameters* extrMuonLayer, bool bSaveMdtSegmentMakerInfo);
    virtual void fillRefittedTrack(const xAOD::TrackParticle* pRefittedTrack, const Trk::Track* trkRefittedTrack, CandidateSummary* pSummary);
    virtual void fillMSTrack(std::unique_ptr<const xAOD::TrackParticle> pRefittedTrack,
                             std::unique_ptr<const Trk::Track> trkRefittedTrack,
                             CandidateSummary* pSummary);
    virtual void setLHR(double lhr);
    virtual void setAnn(double barrelNN,double endcapNN);
    virtual void setIsolation(int isolation);
    virtual StatusCode bookNTuple(NTuple::Tuple* pNTuple);
    virtual StatusCode fillNTupleEventInfo(unsigned nRunNumber, unsigned nEventNumber);
    virtual StatusCode fillNTuple();
    virtual StatusCode fillNTuple(std::vector<MuonHough::MuonLayerHough::Maximum*> hough_data[16][4][2]);
    //virtual StatusCode fillSummary(CandidateSummary* pSummary, const Trk::MeasuredPerigee* pPerigee);
    virtual StatusCode fillSummary(CandidateSummary* pSummary);
    const MuGirlNS::MuonSegmentList& vectorMuonSegments() const;
    virtual const MdtSegmentMakerInfoList& getMdtSegmentMakerInfo() const;
    virtual const Trk::TrackParameters* getPerigee() const;
    virtual double innerAngle() const;
    virtual double middleAngle() const;
     virtual void setPassANN(bool pass);
     virtual void setPassdoStau(bool pass);
    //virtual void clearMuonSegments();
    virtual const RIO_OnTrackLists& getRpcHitsInSegments();
    virtual const RIO_OnTrackLists& getTgcHitsInSegments();
    virtual void candidateClear();
    virtual bool hasInnerStationSegments();
    virtual bool isHoughMaxAssociated(MuonHough::MuonLayerHough::Maximum* max, int sector, int distance, int region);
    
    
    bool doDecoding() const
    {
      return m_doDecoding;
    }
    bool doCSC() const
    {
        return m_doCSC;
    }
    bool doMDT() const
    {
        return m_doMDT;
    }
    bool doRPC() const
    {
        return m_doRPC;
    }
    bool doTGC() const
    {
        return m_doTGC;
    }
    bool doMdtHough() const
    {
        return m_doMdtHough;
    }
    bool doSegments() const
    {
        return m_doSegments;
    }
    double hitDeltaEta() const
    {
        return m_hitDeltaEta;
    }
    double roadDeltaPhi() const
    {
        return m_roadDeltaPhi;
    }
    double roadScaleFactor() const
    {
        return m_roadScaleFactor;
    }
    const ToolHandle<Muon::IMuonSegmentMaker>& cscSegmentMaker() const
    {
        return m_pCscSegmentMaker;
    }
    const ToolHandle<Muon::IMuonSegmentMaker>& mdtSegmentMaker() const
    {
        return m_pMdtSegmentMaker;
    }
    const ToolHandle<Muon::IMuonSegmentMaker>& rpcSegmentMaker() const
    {
        return m_pRpcSegmentMaker;
    }
    const ToolHandle<Muon::IMuonSegmentMaker>& tgcSegmentMaker() const
    {
        return m_pTgcSegmentMaker;
    }
    const ToolHandle<Muon::IMuonRdoToPrepDataTool>& cscRdoToPrepDataTool() const
    {
        return m_pCscRdoToPrepDataTool;
    }

    const ToolHandle<ICscClusterBuilder>& cscClusterProviderTool() const
    {
        return m_pCscClusterProviderTool;
    }
    const ToolHandle<Muon::IMuonRdoToPrepDataTool>& mdtRdoToPrepDataTool() const
    {
        return m_pMdtRdoToPrepDataTool;
    }
    const ToolHandle<Muon::IMuonRdoToPrepDataTool>& rpcRdoToPrepDataTool() const
    {
        return m_pRpcRdoToPrepDataTool;
    }
    const ToolHandle<Muon::IMuonRdoToPrepDataTool>& tgcRdoToPrepDataTool() const
    {
        return m_pTgcRdoToPrepDataTool;
    }
    const ToolHandle<Muon::IMdtDriftCircleOnTrackCreator>& mdtDriftCircleCreator() const
    {
        return m_pMdtDriftCircleCreator;
    }
    const ToolHandle<Muon::IMuonClusterOnTrackCreator>& muonClusterCreator() const
    {
        return m_pMuonClusterCreator;
    }
    const ToolHandle<Muon::IMuonClusterOnTrackCreator>& cscClusterCreator() const
    {
        return m_pCscClusterCreator;
    }
    const ToolHandle<Trk::IIntersector>& Intersector() const
    {
        return m_pIntersector;
    }
    const ToolHandle<Rec::IMuidCaloEnergy>& caloEnergyTool() const
    {
        return m_pCaloEnergyTool;
    }
    const ToolHandle<Muon::MuonEDMHelperTool>& muonHelperTool() const
    {
        return m_helperTool;
    }
    const ToolHandle<Muon::MuonIdHelperTool>& muonIdHelperTool() const
    {
        return m_idHelperTool;
    }
    const ToolHandle<Muon::IMuonSegmentSelectionTool>& segmentSelectionTool() const
    {
        return m_segmentSelectionTool;
    }
    const ServiceHandle<IRegSelSvc>& regionSelector() const
    {
        return m_pRegionSelector;
    }
    const MuonGM::MuonDetectorManager* muonManager() const
    {
        return m_pMuonMgr;
    }
    Distance* distance(DistanceType eDist)
    {
        return m_distances[eDist];
    }
    Technology* technology(TechnologyType eTech)
    {
        return m_technologies[eTech];
    }
    StationManger* stationManager() const
    {
        return m_pStationMgr;
    }
    virtual void setSegmentManager(SegmentManager* pSegmentManager);

private:
    template <class T>
    StatusCode retrieve(ToolHandle<T>& pHandle, bool bError = true)
    {
        StatusCode sc = pHandle.retrieve();
        if (sc.isFailure())
        {
            if (bError)
                msg(MSG::ERROR) << "Cannot retrieve tool " << pHandle << endmsg;
            else
                msg(MSG::WARNING) << "Cannot retrieve tool " << pHandle << endmsg;
        }
        else
            msg(MSG::INFO) << "Retrieved tool " << pHandle << endmsg;
        return sc;
    }
    template <class T>
    StatusCode retrieve(ServiceHandle<T>& pHandle, bool bError = true)
    {
        StatusCode sc = pHandle.retrieve();
        if (sc.isFailure())
        {
            if (bError)
                msg(MSG::ERROR) << "Cannot retrieve service " << pHandle << endmsg;
            else
                msg(MSG::WARNING) << "Cannot retrieve service " << pHandle << endmsg;
        }
        else
            msg(MSG::INFO) << "Retrieved service " << pHandle << endmsg;
        return sc;
    }

    BooleanProperty                                 m_doDecoding;       /**< call the PrepRawData decoders */
    BooleanProperty                                 m_doCSC;            /**< Process CSC hits */
    BooleanProperty                                 m_doMDT;            /**< Process MDT hits */
    BooleanProperty                                 m_doRPC;            /**< Process RPC hits */
    BooleanProperty                                 m_doTGC;            /**< Process TGC hits */
    BooleanProperty                                 m_dosTGC;           /**< Process sTGC hits */
    BooleanProperty                                 m_doMM;             /**< Process MM hits  */
    BooleanProperty                                 m_doMdtHough;       /**< Call the MDT segment maker */
    BooleanProperty                                 m_doSegments;       /**< Create segements (of any kind) */
    StringProperty                                  m_sCscPrepDataCollection;
    StringProperty                                  m_sMdtPrepDataCollection;
    StringProperty                                  m_sRpcPrepDataCollection;
    StringProperty                                  m_sTgcPrepDataCollection;
    StringProperty                                  m_sStgcPrepDataCollection;
    StringProperty                                  m_sMmPrepDataCollection;
    FloatProperty                                   m_hitDeltaEta;      /**< The base eta width of the hit acceptance road */
    FloatProperty                                   m_roadDeltaPhi;     /**< The phi width of crude roads */
    FloatProperty                                   m_roadScaleFactor;  /**< the factor scaling the value of the road width*/
    ToolHandle<Muon::IMuonSegmentMaker>             m_pCscSegmentMaker;
    ToolHandle<Muon::IMuonSegmentMaker>             m_pMdtSegmentMaker;
    ToolHandle<Muon::IMuonSegmentMaker>             m_pRpcSegmentMaker;
    ToolHandle<Muon::IMuonSegmentMaker>             m_pTgcSegmentMaker;
    ToolHandle<ICscClusterBuilder>                  m_pCscClusterProviderTool;
    ToolHandle<Muon::IMuonRdoToPrepDataTool>        m_pCscRdoToPrepDataTool;
    ToolHandle<Muon::IMuonRdoToPrepDataTool>        m_pMdtRdoToPrepDataTool;
    ToolHandle<Muon::IMuonRdoToPrepDataTool>        m_pRpcRdoToPrepDataTool;
    ToolHandle<Muon::IMuonRdoToPrepDataTool>        m_pTgcRdoToPrepDataTool;
    ToolHandle<Muon::IMdtDriftCircleOnTrackCreator> m_pMdtDriftCircleCreator;
    ToolHandle<Muon::IMuonClusterOnTrackCreator>    m_pMuonClusterCreator;
    ToolHandle<Muon::IMuonClusterOnTrackCreator>    m_pCscClusterCreator;
    ToolHandle<Trk::IIntersector>                   m_pIntersector;
    ToolHandle<Rec::IMuidCaloEnergy>                m_pCaloEnergyTool;
    ToolHandle<Muon::MuonEDMHelperTool>             m_helperTool;
    ToolHandle<Muon::MuonIdHelperTool>              m_idHelperTool;
    ToolHandle<Muon::IMuonSegmentSelectionTool>     m_segmentSelectionTool;
    ToolHandle<Muon::MuonLayerHoughTool>            m_pMuonLayerHoughTool;
    ServiceHandle<IRegSelSvc>                       m_pRegionSelector;  /**< The region selector */
    const MuonGM::MuonDetectorManager*              m_pMuonMgr;         /**< The Muon manager */
    Distance*                                       m_distances[DISTANCES]; /**< The list of Distance objects */
    Technology*                                     m_technologies[TECHNOLOGIES]; /**< The list of Technology objects */
    StationManger*                                  m_pStationMgr;

    int                                             m_iTrack;           /**< Candidate index */
    Candidate*                                      m_pCandidate;       /**< The Muon candidate */
    CandidateNtuple*                                m_pCandidateNtuple;

};

} // end of namespace

#endif
