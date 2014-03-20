/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuGirlPerformanceTruthTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUGIRLNSPERFORMANCETRUTHTOOL_H
#define MUGIRLNSPERFORMANCETRUTHTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/NTuple.h"
#include "MuGirlInterfaces/IPerformanceTruthTool.h"
#include "iPatInterfaces/ITruthParameters.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"

class TrigMuonEFTrack;
class MuonFeature;
class AtlasDetectorID;
class Identifier;
class McEventCollection;
class TruthParameters;
class ITruthParameters;

namespace iPatTrackParameters
{
class PerigeeParameters;
}

namespace MuGirlNS
{

/** @class MuGirlPerformanceTruthTool
    @author  Zvi Tarem <Zvi.Tarem@cern.ch>
*/
class CandidateSummary;
class PerformanceTruthTool : virtual public IPerformanceTruthTool, public AthAlgTool
{
public:
    PerformanceTruthTool(const std::string&,const std::string&,const IInterface*);

    /** default destructor */
    virtual ~PerformanceTruthTool ();

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize  ();
    virtual StatusCode printParticle(int i);
    virtual StatusCode getTruthMuons(TruthMuonList& truthMuonList);
    //virtual StatusCode getTruthTrack(const Rec::TrackParticle* pParticle, TruthTrack& truthTrack);
    virtual StatusCode getTruthTrack(const xAOD::TrackParticle* pParticle, TruthTrack& truthTrack,CandidateSummary* summary);
    virtual StatusCode bookNTuple(NTuple::Tuple* pNTuple);
    virtual StatusCode fillNTuple(CandidateSummary* summary);
    //virtual StatusCode fillNTuple(const Rec::TrackParticle* pParticle, CandidateSummary* summary=NULL);

    virtual StatusCode getTruthTrack(const MuonFeature* muonFeature, TruthTrack& truthTrack, CandidateSummary* summary); 	 
    virtual StatusCode getTruthTrack(const TrigMuonEFTrack* muonEFTrack, TruthTrack& truthTrack, CandidateSummary* summary); 	 

    /** member variables for algorithm properties: */
private:
    ToolHandle<ITruthParameters>    m_truthParameters;
    const McEventCollection*        m_pMcEventCollection;

    StringProperty m_sTrackRecordCollection; /**< The name of the track record collection in the store */
    StringProperty m_sMcEventCollection;

    BooleanProperty m_doOnlyMuonEntry;

    NTuple::Item<float>  m_pTruthPdgId;        /**< Truth: Pdg_Id */
    NTuple::Item<float>  m_pTruthPdgMother;    /**< Truth: Mother's PdgID*/
    NTuple::Item<float>  m_pTruthPt;           /**< Truth: pT */
    NTuple::Item<float>  m_pTruthQOverP;       /**< Truth: Inverse pt (multiplied by charge)*/
    NTuple::Item<float>  m_pTruthEta;          /**< Truth: Eta at the origin */
    NTuple::Item<float>  m_pTruthPhi;          /**< Truth: Phi at the origin */
    NTuple::Item<float>  m_pTruthTheta;        /**< Truth: Theta at the origin */
    NTuple::Item<float>  m_pTrutha0;            /**< Truth: a0 at the origin */
    NTuple::Item<float>  m_pTruthz0;            /**< Truth: z0 at the origin */
    NTuple::Item<float>  m_pTruthCharge;       /**< Truth: Charge */
    NTuple::Item<float>  m_pTruthDPt;          /**< Truth: (pT(Truth) - pT(ID)) / pT(Truth) */
    NTuple::Item<float>  m_pTruthDEta;         /**< Truth: Eta(Truth) - Eta(ID) */
    NTuple::Item<float>  m_pTruthDPhi;         /**< Truth: Phi(Truth) - Phi(ID) */
};

} // end of namespace

#endif
