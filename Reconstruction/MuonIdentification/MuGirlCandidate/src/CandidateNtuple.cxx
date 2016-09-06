/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegment/MuonSegment.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonHoughPatternTools/MuonLayerHoughTool.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/JacobianThetaPToCotThetaPt.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuGirlCandidate/CandidateNtuple.h"
#include "MuGirlCandidate/Candidate.h"
#include "MuGirlCandidate/Chamber.h"
#include "MuGirlCandidate/Station.h"
#include "MuGirlCandidate/Intersection.h"
#include "MuGirlCandidate/Hit.h"
#include "MuGirlCandidate/Segment.h"
#include "MuGirlCandidate/CandidateTool.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "Particle/TrackParticle.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

namespace MuGirlNS
{

CandidateNtuple::CandidateNtuple(MsgStream& log, Muon::MuonIdHelperTool& helper) :
        m_log(log),
        m_idHelperTool(helper),
        m_maxChambers(200),
        m_maxIsect(300),
        m_maxHits(300),
        m_maxSeg(100),
        m_maxHitSegMaps(300),
        m_maxMaxima(300),
        m_maxHoughHits(300)
{}

StatusCode CandidateNtuple::book(NTuple::Tuple* pNTuple)
{
    m_log << MSG::INFO << " CandidateNtuple initialization" << endmsg;

    // Run/Event/Candidate identification
    if (    pNTuple->addItem("MUGIRL/RunNumber",       m_RunNumber).isFailure() ||
            pNTuple->addItem("MUGIRL/EventNumber",     m_EventNumber).isFailure() ||
            pNTuple->addItem("MUGIRL/CandidateNumber", m_CandidateNumber).isFailure())
    {
        m_log << MSG::WARNING << "Cannot addItem(MUGIRL/<Run and event numbers>)" << endmsg;
        return StatusCode::FAILURE;
    }
    // Muon Feature Parameters
    if     (pNTuple->addItem("MUGIRL/MFpt" ,      m_MFpt).isFailure() ||
            pNTuple->addItem("MUGIRL/MFcharge" ,  m_MFcharge).isFailure() ||
            pNTuple->addItem("MUGIRL/MFradius" ,  m_MFradius).isFailure() ||
            pNTuple->addItem("MUGIRL/MFphi" ,     m_MFphi).isFailure() ||
            pNTuple->addItem("MUGIRL/MFhasCombined" ,     m_MFhasCombined).isFailure() ||
            pNTuple->addItem("MUGIRL/MFdir_phi" , m_MFdir_phi).isFailure() ||
            pNTuple->addItem("MUGIRL/MFz" ,       m_MFz).isFailure() ||
            pNTuple->addItem("MUGIRL/MFbeta" ,       m_MFbeta).isFailure() ||
            pNTuple->addItem("MUGIRL/MFdir_z" ,   m_MFdir_z).isFailure() ||
            pNTuple->addItem("MUGIRL/MFeta" ,     m_MFeta).isFailure())
    {
        m_log << MSG::WARNING << "Cannot addItem(MUGIRL/<MuonFeature parameters>)" << endmsg;
        return StatusCode::FAILURE;
    }
    // ID track parameters
    if     (pNTuple->addItem("MUGIRL/IDTrkPt" ,     m_IDTrkPt).isFailure() ||
             pNTuple->addItem("MUGIRL/IDTrkEta" ,    m_IDTrkEta).isFailure() ||
             pNTuple->addItem("MUGIRL/IDTrkPhi" ,    m_IDTrkPhi).isFailure() ||
	     pNTuple->addItem("MUGIRL/IDTrkTheta" ,  m_IDTrkTheta).isFailure() ||
	     pNTuple->addItem("MUGIRL/IDTrka0" ,     m_IDTrka0).isFailure() ||
	     pNTuple->addItem("MUGIRL/IDTrkz0" ,     m_IDTrkz0).isFailure() ||
             pNTuple->addItem("MUGIRL/IDTrkQOverP",  m_IDTrkQOverP).isFailure() ||
             pNTuple->addItem("MUGIRL/IDTrkCharge",  m_IDTrkCharge).isFailure() ||
	     pNTuple->addItem("MUGIRL/IDTrkChi2",    m_IDTrkChi2).isFailure() ||
	     pNTuple->addItem("MUGIRL/IDTrkNDofFr",  m_IDTrkNDofFr).isFailure() ||
	     pNTuple->addItem("MUGIRL/IDTrkChi2pr",  m_IDTrkChi2pr).isFailure() ||
	     pNTuple->addItem("MUGIRL/IDTrkCov11",   m_IDTrkCov11).isFailure() ||
 	     pNTuple->addItem("MUGIRL/IDTrkCov22",   m_IDTrkCov22).isFailure() ||
	     pNTuple->addItem("MUGIRL/IDTrkCov33",   m_IDTrkCov33).isFailure() ||
	     pNTuple->addItem("MUGIRL/IDTrkCov44",   m_IDTrkCov44).isFailure() ||
	     pNTuple->addItem("MUGIRL/IDTrkCov55",   m_IDTrkCov55).isFailure())
    {
        m_log << MSG::WARNING << "Cannot addItem(MUGIRL/<ID track parameters>)" << endmsg;
        return StatusCode::FAILURE;
    }

    if ( pNTuple->addItem("MUGIRL/MuidELoss" ,       m_muidELoss).isFailure() ||
         pNTuple->addItem("MUGIRL/ExtrIDTrkPt" ,     m_extrIDTrkPt).isFailure() ||
         pNTuple->addItem("MUGIRL/ExtrIDTrkEta" ,    m_extrIDTrkEta).isFailure() ||
         pNTuple->addItem("MUGIRL/ExtrIDTrkPhi" ,    m_extrIDTrkPhi).isFailure() ||
         pNTuple->addItem("MUGIRL/ExtrIDTrkTheta" ,  m_extrIDTrkTheta).isFailure() ||
         pNTuple->addItem("MUGIRL/ExtrIDTrka0" ,     m_extrIDTrka0).isFailure() ||
         pNTuple->addItem("MUGIRL/ExtrIDTrkz0" ,     m_extrIDTrkz0).isFailure() ||
         pNTuple->addItem("MUGIRL/ExtrIDTrkQOverP",  m_extrIDTrkQOverP).isFailure() ||
         pNTuple->addItem("MUGIRL/ExtrIDTrkCharge",  m_extrIDTrkCharge).isFailure() )
    {
        m_log << MSG::WARNING << "Cannot addItem(MUGIRL/<ID extrapolated track parameters>)" << endmsg;
        return StatusCode::FAILURE;
    }

    if (   pNTuple->addItem("MUGIRL/QOverP",         m_QOverP).isFailure() ||
 	    pNTuple->addItem("MUGIRL/passNN",        m_passNN).isFailure() ||
 	    pNTuple->addItem("MUGIRL/isMuonFeature",        m_isMuonFeature).isFailure() ||
 	    pNTuple->addItem("MUGIRL/passStau",        m_passStau).isFailure() ||
	    pNTuple->addItem("MUGIRL/NNBarrel",      m_NNBarrel).isFailure() ||
            pNTuple->addItem("MUGIRL/NNEndCap",      m_NNEndCap).isFailure() ||
            pNTuple->addItem("MUGIRL/InnerAngle",    m_InnerAngle).isFailure() ||
            pNTuple->addItem("MUGIRL/InnerDelta",    m_InnerDelta).isFailure() ||
            pNTuple->addItem("MUGIRL/MiddleAngle",   m_MiddleAngle).isFailure() ||
            pNTuple->addItem("MUGIRL/MiddleDelta",   m_MiddleDelta).isFailure() ||
            pNTuple->addItem("MUGIRL/LHR",           m_Lhr).isFailure() ||
            pNTuple->addItem("MUGIRL/Isolation",     m_Isolation).isFailure())
    {
        m_log << MSG::WARNING << "Cannot addItem(MUGIRL/<ANN parameters>)" << endmsg;
        return StatusCode::FAILURE;
    }

    // Chamber parameters
    if (    pNTuple->addItem("MUGIRL/NumChambers",              m_NumChambers, 0, m_maxChambers).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/ChamberID",         m_NumChambers, m_ChamberID).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/ChamberStation",    m_NumChambers, m_ChamberStation).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/ChamberRegion",     m_NumChambers, m_ChamberRegion).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/ChamberDistance",   m_NumChambers, m_ChamberDistance).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/ChamberTechnology", m_NumChambers, m_ChamberTechnology).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/ChamberFineIsect",  m_NumChambers, m_ChamberFineIsect).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/ChamberMdtIsect",   m_NumChambers, m_ChamberMdtIsect).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/ChamberRpcIsect",   m_NumChambers, m_ChamberRpcIsect).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/ChamberTgcIsect",   m_NumChambers, m_ChamberTgcIsect).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/ChamberCscIsect",   m_NumChambers, m_ChamberCscIsect).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/ChamberCombIsect",  m_NumChambers, m_ChamberCombIsect).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/ChamberMdtIsect1",  m_NumChambers, m_ChamberMdtIsect1).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/ChamberRpcIsect1",  m_NumChambers, m_ChamberRpcIsect1).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/ChamberTgcIsect1",  m_NumChambers, m_ChamberTgcIsect1).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/ChamberCscIsect1",  m_NumChambers, m_ChamberCscIsect1).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/ChamberCombIsect1", m_NumChambers, m_ChamberCombIsect1).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/ChamberRZ",         m_NumChambers, m_ChamberRZ).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/ChamberPhi",        m_NumChambers, m_ChamberPhi).isFailure() )
    {
        m_log << MSG::WARNING << "Cannot addItem(MUGIRL/<Chamber parameters>)" << endmsg;
        return StatusCode::FAILURE;
    }

    // Intersection parameters
    if (    pNTuple->addItem("MUGIRL/NumIsections",             m_NumIsects, 0, m_maxIsect).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/IsectType",         m_NumIsects, m_IsectType).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/IsectOrigTech",     m_NumIsects, m_IsectOrigTech).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/IsectOrigDist",     m_NumIsects, m_IsectOrigDist).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/IsectTargTech",     m_NumIsects, m_IsectTargTech).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/IsectTargDist",     m_NumIsects, m_IsectTargDist).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/IsectRegion",       m_NumIsects, m_IsectRegion).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/IsectChamber",      m_NumIsects, m_IsectChamber).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/IsectPosEta",       m_NumIsects, m_IsectPosEta).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/IsectPosPhi",       m_NumIsects, m_IsectPosPhi).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/IsectPosR",         m_NumIsects, m_IsectPosR).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/IsectPosZ",         m_NumIsects, m_IsectPosZ).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/IsectDirEta",       m_NumIsects, m_IsectDirEta).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/IsectDirPhi",       m_NumIsects, m_IsectDirPhi).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/IsectDirZ",         m_NumIsects, m_IsectDirZ).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/IsectSegment",      m_NumIsects, m_IsectSegment).isFailure() )
    {
        m_log << MSG::WARNING << "Cannot addItem(MUGIRL/<Intersection parameters>)" << endmsg;
        return StatusCode::FAILURE;
    }

    // Segment parameters
    if (    pNTuple->addItem("MUGIRL/NumSegments",              m_NumSegs, 0, m_maxSeg).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/SegType",           m_NumSegs, m_SegType).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/SegDistance",       m_NumSegs, m_SegDistance).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/SegRegion",         m_NumSegs, m_SegRegion).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/SegStation",        m_NumSegs, m_SegStation).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/SegHits",           m_NumSegs, m_SegHits).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/SegPosEta",         m_NumSegs, m_SegPosEta).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/SegPosPhi",         m_NumSegs, m_SegPosPhi).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/SegPosR",           m_NumSegs, m_SegPosR).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/SegPosZ",           m_NumSegs, m_SegPosZ).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/SegDirEta",         m_NumSegs, m_SegDirEta).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/SegDirPhi",         m_NumSegs, m_SegDirPhi).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/SegDirZ",           m_NumSegs, m_SegDirZ).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/SegChi2",           m_NumSegs, m_SegChi2).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/SegNdf",            m_NumSegs, m_SegNdf).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/SegProb",           m_NumSegs, m_SegProb).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/SegIsect",          m_NumSegs, m_SegIsect).isFailure() )
    {
        m_log << MSG::WARNING << "Cannot addItem(MUGIRL/<Segment parameters>)" << endmsg;
        return StatusCode::FAILURE;
    }

    // Hit parameters
    if (    pNTuple->addItem("MUGIRL/NumHits",                  m_NumHits, 0, m_maxHits).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitId",             m_NumHits, m_HitId).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitChamber",        m_NumHits, m_HitChamber).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitChamberID",      m_NumHits, m_HitChamberID).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitDeltaEta",       m_NumHits, m_HitDeltaEta).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitDeltaPhi",       m_NumHits, m_HitDeltaPhi).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitEta",            m_NumHits, m_HitEta).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitPhi",            m_NumHits, m_HitPhi).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitZ",              m_NumHits, m_HitZ).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitR",              m_NumHits, m_HitR).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitIsEta",          m_NumHits, m_HitIsEta).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitDistance",       m_NumHits, m_HitDistance).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitTechnology",     m_NumHits, m_HitTechnology).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitRegion",         m_NumHits, m_HitRegion).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitStation",        m_NumHits, m_HitStation).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitFineIsect",      m_NumHits, m_HitFineIsect).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitMdtIsect",       m_NumHits, m_HitMdtIsect).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitRpcIsect",       m_NumHits, m_HitRpcIsect).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitTgcIsect",       m_NumHits, m_HitTgcIsect).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitCscIsect",       m_NumHits, m_HitCscIsect).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitCombIsect",      m_NumHits, m_HitCombIsect).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitMdtIsect1",      m_NumHits, m_HitMdtIsect1).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitRpcIsect1",      m_NumHits, m_HitRpcIsect1).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitTgcIsect1",      m_NumHits, m_HitTgcIsect1).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitCscIsect1",      m_NumHits, m_HitCscIsect1).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitCombIsect1",     m_NumHits, m_HitCombIsect1).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitHough",          m_NumHits, m_HitHough).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitInSegment",      m_NumHits, m_HitInSegment).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitDriftRadius",    m_NumHits, m_HitDriftRadius).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitError",          m_NumHits, m_HitError).isFailure() )
    {
        m_log << MSG::WARNING << "Cannot addItem(MUGIRL/<Hit parameters>)" << endmsg;
        return StatusCode::FAILURE;
    }

    // Hit/Segment Association parameters
    if (    pNTuple->addItem("MUGIRL/NumHitSegMaps",            m_NumHitSegMaps, 0, m_maxHitSegMaps).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitSegMapHit",      m_NumHitSegMaps, m_HitSegMapHit).isFailure() ||
            pNTuple->addIndexedItem("MUGIRL/HitSegMapSeg",      m_NumHitSegMaps, m_HitSegMapSeg).isFailure() )
    {
        m_log << MSG::WARNING << "Cannot addItem(MUGIRL/<Hit/Segment Association>)" << endmsg;
        return StatusCode::FAILURE;
    }


    //Number of Hits and Holes after global fit
    if (pNTuple->addItem("MUGIRL/RefittedTrkSummaryNumMdtHits", m_RefittedTrkSummaryNumMdtHits, 0, m_maxHits).isFailure() ||
        pNTuple->addItem("MUGIRL/RefittedTrkSummaryNumTgcEtaHits",m_RefittedTrkSummaryNumTgcEtaHits,0, m_maxHits).isFailure() ||
        pNTuple->addItem("MUGIRL/RefittedTrkSummaryNumTgcPhiHits",m_RefittedTrkSummaryNumTgcPhiHits,0, m_maxHits).isFailure() ||
        pNTuple->addItem("MUGIRL/RefittedTrkSummaryNumCscEtaHits",m_RefittedTrkSummaryNumCscEtaHits,0, m_maxHits).isFailure() ||
        pNTuple->addItem("MUGIRL/RefittedTrkSummaryNumCscPhiHits",m_RefittedTrkSummaryNumCscPhiHits,0, m_maxHits).isFailure() ||
        pNTuple->addItem("MUGIRL/RefittedTrkSummaryNumRpcEtaHits",m_RefittedTrkSummaryNumRpcEtaHits,0, m_maxHits).isFailure() ||
        pNTuple->addItem("MUGIRL/RefittedTrkSummaryNumRpcPhiHits",m_RefittedTrkSummaryNumRpcPhiHits,0, m_maxHits).isFailure() ||
        pNTuple->addItem("MUGIRL/RefittedTrkSummaryNumMdtHoles",  m_RefittedTrkSummaryNumMdtHoles,0, m_maxHits).isFailure() ||
        pNTuple->addItem("MUGIRL/RefittedTrkSummaryNumTgcEtaHoles",m_RefittedTrkSummaryNumTgcEtaHoles,0, m_maxHits).isFailure() ||
        pNTuple->addItem("MUGIRL/RefittedTrkSummaryNumTgcPhiHoles",m_RefittedTrkSummaryNumTgcPhiHoles,0, m_maxHits).isFailure() ||
        pNTuple->addItem("MUGIRL/RefittedTrkSummaryNumCscEtaHoles",m_RefittedTrkSummaryNumCscEtaHoles,0, m_maxHits).isFailure() ||
        pNTuple->addItem("MUGIRL/RefittedTrkSummaryNumCscPhiHoles",m_RefittedTrkSummaryNumCscPhiHoles,0, m_maxHits).isFailure() ||
        pNTuple->addItem("MUGIRL/RefittedTrkSummaryNumRpcEtaHoles",m_RefittedTrkSummaryNumRpcEtaHoles,0, m_maxHits).isFailure() ||
        pNTuple->addItem("MUGIRL/RefittedTrkSummaryNumRpcPhiHoles",m_RefittedTrkSummaryNumRpcPhiHoles,0, m_maxHits).isFailure())

    {
        m_log << MSG::WARNING << "Cannot addItem(MUGIRL/<Number of hits and holes>)" << endmsg;
        return StatusCode::FAILURE;
    }

    if (pNTuple->addItem("MUGIRL/NumHoughMaxima",         m_NumHoughMaxima, 0, m_maxMaxima).isFailure()    ||
        pNTuple->addIndexedItem("MUGIRL/MaximumSector",   m_NumHoughMaxima, m_MaximumSector).isFailure()   ||
        pNTuple->addIndexedItem("MUGIRL/MaximumDistance", m_NumHoughMaxima, m_MaximumDistance).isFailure() ||
        pNTuple->addIndexedItem("MUGIRL/MaximumRegion",   m_NumHoughMaxima, m_MaximumRegion).isFailure()   ||
        pNTuple->addIndexedItem("MUGIRL/MaximumValue",    m_NumHoughMaxima, m_MaximumValue).isFailure()    ||
        pNTuple->addIndexedItem("MUGIRL/MaximumPosition", m_NumHoughMaxima, m_MaximumPosition).isFailure() ||
        pNTuple->addIndexedItem("MUGIRL/MaximumEta",      m_NumHoughMaxima, m_MaximumEta).isFailure()      ||
        pNTuple->addIndexedItem("MUGIRL/MaximumTheta",    m_NumHoughMaxima, m_MaximumTheta).isFailure()    ||
        pNTuple->addIndexedItem("MUGIRL/MaximumRefPlane", m_NumHoughMaxima, m_MaximumRefPlane).isFailure() ||
        pNTuple->addIndexedItem("MUGIRL/MaximumLowPos",   m_NumHoughMaxima, m_MaximumLowPos).isFailure()   ||
        pNTuple->addIndexedItem("MUGIRL/MaximumUppPos",   m_NumHoughMaxima, m_MaximumUppPos).isFailure()   ||
        pNTuple->addIndexedItem("MUGIRL/TriggerConfirmed",m_NumHoughMaxima, m_TriggerConfirmed).isFailure() )
    {
        m_log << MSG::WARNING << "Cannot addItem(MUGIRL/<Hough transform maxima>)" << endmsg;
        return StatusCode::FAILURE;
    }

    if (pNTuple->addItem("MUGIRL/NumHoughHits",              m_NumHoughHits, 0, m_maxHoughHits).isFailure()    ||
        pNTuple->addIndexedItem("MUGIRL/HoughHitDeltaEta",   m_NumHoughHits, m_HoughHitDeltaEta).isFailure()   ||
        pNTuple->addIndexedItem("MUGIRL/HoughHitEta",        m_NumHoughHits, m_HoughHitEta).isFailure()        ||
        pNTuple->addIndexedItem("MUGIRL/HoughPhiHit",        m_NumHoughHits, m_HoughHitPhi).isFailure()        ||
        pNTuple->addIndexedItem("MUGIRL/HoughHitR",          m_NumHoughHits, m_HoughHitR).isFailure()          ||
        pNTuple->addIndexedItem("MUGIRL/HoughHitZ",          m_NumHoughHits, m_HoughHitZ).isFailure()          ||
        pNTuple->addIndexedItem("MUGIRL/HoughHitIsEta",      m_NumHoughHits, m_HoughHitIsEta).isFailure()      ||
        pNTuple->addIndexedItem("MUGIRL/HoughHitDistance",   m_NumHoughHits, m_HoughHitDistance).isFailure()   ||
        pNTuple->addIndexedItem("MUGIRL/HoughHitTechnology", m_NumHoughHits, m_HoughHitTechnology).isFailure() ||
        pNTuple->addIndexedItem("MUGIRL/HoughHitRegion",     m_NumHoughHits, m_HoughHitRegion).isFailure()     ||
        pNTuple->addIndexedItem("MUGIRL/HoughHitSector",     m_NumHoughHits, m_HoughHitSector).isFailure()     ||
        pNTuple->addIndexedItem("MUGIRL/HoughMaximum",       m_NumHoughHits, m_HoughMaximum).isFailure()       ||
        pNTuple->addIndexedItem("MUGIRL/HoughMuGirlHit",     m_NumHoughHits, m_HoughMuGirlHit).isFailure() )
    {
        m_log << MSG::WARNING << "Cannot addItem(MUGIRL/<Hough transform hits>)" << endmsg;
        return StatusCode::FAILURE;
    }


    /*
    //MS Flag
    if (pNTuple->addItem("MUGIRL/MSRefittedTrkSummaryFlag",m_MSRefittedTrkSummaryFlag).isFailure())
      
      {
	m_log << MSG::WARNING << "Cannot addItem(MUGIRL/<MSFlag>)" << endmsg;
	return StatusCode::FAILURE;
      }*/
    return StatusCode::SUCCESS;
}

StatusCode CandidateNtuple::fillEventInfo(unsigned nRunNumber, unsigned nEventNumber)
{
    if (m_log.level() <= MSG::DEBUG)
        m_log << MSG::DEBUG << "CandidateNtuple::fillEventInfo(" << nRunNumber << ", " << nEventNumber << ")" << endmsg;

    m_RunNumber   = (long)nRunNumber;
    m_EventNumber = (long)nEventNumber;

    return StatusCode::SUCCESS;
}

StatusCode CandidateNtuple::fillCandidate(int iTrack, Candidate* pCand)
{
     m_isMuonFeature = 0;
    if (m_log.level() <= MSG::DEBUG)
        m_log << MSG::DEBUG << "CandidateNtuple::fillCandidate(" << iTrack << ")" << endmsg;
    m_MFhasCombined = 0;
    if (pCand->hasCombinedMF())
    m_MFhasCombined = 1;
    m_CandidateNumber = iTrack;
    const Trk::Perigee*         pPerigee   = &pCand->IDTrk()->perigeeParameters();
    const Trk::TrackParameters* pMuonEntry = pCand->IDPerigee();
    const xAOD::TrackParticle* pTrk = pCand->IDTrk();
    const TrigMuonEFTrack*     mEFTrack = pCand->getMuonEFTrack();
    const MuonFeature*     mFeature = pCand->getMuonFeature();
    if (pPerigee == NULL && mFeature == NULL && mEFTrack == NULL)
    {
        m_log << MSG::WARNING << "pCand->getIDPerigee(), pCand->getMuonEFTrack() and pCand->getMuonFeature() returned NULL!" << endmsg;
        return StatusCode::RECOVERABLE;
    }
    if (mEFTrack!=NULL)
    {
        m_isMuonFeature = 1; 
        m_MFpt = mEFTrack->pt();  
        m_MFcharge = mEFTrack->charge();
        m_MFradius = mEFTrack->d0();
        m_MFphi = mEFTrack->phi();
        m_MFz = mEFTrack->z0();
        m_MFbeta = -1;
        m_MFeta = mEFTrack->eta();
        m_MFdir_phi = -1;
        m_MFdir_z = -1;
    }
    if (mFeature!=NULL)
    {
        m_isMuonFeature = 1; 
        m_MFpt = fabs(mFeature->pt())*1000.;  //MF pt is in GeV
        m_MFcharge = mFeature->pt()> 0. ? 1 : -1;
        m_MFradius = mFeature->radius();
        m_MFphi = mFeature->phi();
        m_MFz = mFeature->zeta();
        m_MFbeta = mFeature->beta();
        m_MFeta = mFeature->eta();
        m_MFdir_phi = mFeature->dir_phi();
        m_MFdir_z = mFeature->dir_zeta();
    }
    if (pPerigee != NULL)
    {
        m_isMuonFeature = 0; 
        //const Trk::ErrorMatrix& errorMatrix = pPerigee->localErrorMatrix();
        //Trk::JacobianThetaPToCotThetaPt jacob(pPerigee->parameters()[Trk::theta],pPerigee->parameters()[Trk::qOverP]);
        //CLHEP::HepSymMatrix* covMatrixID = new CLHEP::HepSymMatrix(errorMatrix.covariance().similarity(jacob));  

        if (pPerigee->covariance()) {
            const AmgMatrix(5,5)* errorMatrix = pPerigee->covariance();
            AmgSymMatrix(5) covMatrixID;
            Trk::JacobianThetaPToCotThetaPt jacob(pPerigee->parameters()[Trk::theta],pPerigee->parameters()[Trk::qOverP]);
            covMatrixID = errorMatrix->similarity(jacob);

            m_IDTrkCov11  = (covMatrixID)(Trk::d0,Trk::d0);
            m_IDTrkCov22  = (covMatrixID)(Trk::z0,Trk::z0);
            m_IDTrkCov33  = (covMatrixID)(2,2);
            m_IDTrkCov44  = (covMatrixID)(3,3);
            m_IDTrkCov55  = (covMatrixID)(4,4);
        } else {
            m_IDTrkCov11  = 0.;
            m_IDTrkCov22  = 0.;
            m_IDTrkCov33  = 0.;
            m_IDTrkCov44  = 0.;
            m_IDTrkCov55  = 0.;
        }

        m_IDTrkPt = pPerigee->pT();
        m_IDTrkEta = pPerigee->eta();
        m_IDTrkPhi = pPerigee->parameters()[Trk::phi];
        m_IDTrkQOverP = pPerigee->parameters()[Trk::qOverP];
        m_IDTrkCharge = pPerigee->charge();
        m_IDTrkTheta  = 1./tan(pPerigee->parameters()[Trk::theta]);
        m_IDTrka0     = pPerigee->parameters()[Trk::d0];
        m_IDTrkz0     = pPerigee->parameters()[Trk::z0];

    if (pTrk!=NULL)
    {
    	m_IDTrkChi2   = pTrk->chiSquared();
    	m_IDTrkNDofFr = pTrk->numberDoF() ;
        if (m_IDTrkChi2 >0 && m_IDTrkNDofFr>0)
    	    m_IDTrkChi2pr =1.- Genfun::CumulativeChiSquare(pTrk->numberDoF())(pTrk->chiSquared()) ;
	else
            m_IDTrkChi2pr = -100.;
    }
    else 
    {
        m_IDTrkChi2   = -100.;
        m_IDTrkNDofFr = -100 ;
        m_IDTrkChi2pr =-100.;
    }
    //m_IDTrkCov11  = (*covMatrixID)[Trk::d0][Trk::d0];
    //m_IDTrkCov22  = (*covMatrixID)[Trk::z0][Trk::z0];
    //m_IDTrkCov33  = (*covMatrixID)[2][2];
    //m_IDTrkCov44  = (*covMatrixID)[3][3];
    //m_IDTrkCov55  = (*covMatrixID)[4][4];  
    //delete covMatrixID;
    
    m_muidELoss = pCand->muidELoss();

    }
    
    if (pMuonEntry != NULL && pCand->isExtrapolated() )
    {
        m_extrIDTrkPt     = pMuonEntry->pT();
        m_extrIDTrkEta    = pMuonEntry->eta();
        m_extrIDTrkPhi    = pMuonEntry->parameters()[Trk::phi];
        m_extrIDTrkQOverP = pMuonEntry->parameters()[Trk::qOverP];
        m_extrIDTrkCharge = pMuonEntry->charge();
        m_extrIDTrkTheta  = pMuonEntry->parameters()[Trk::theta];
        m_extrIDTrka0     = pMuonEntry->parameters()[Trk::d0];
        m_extrIDTrkz0     = pMuonEntry->parameters()[Trk::z0];
    } else {
        m_extrIDTrkPt     = -100;
        m_extrIDTrkEta    = -100;
        m_extrIDTrkPhi    = -100;
        m_extrIDTrkQOverP = -100;
        m_extrIDTrkCharge = -100;
        m_extrIDTrkTheta  = -100;
        m_extrIDTrka0     = -100;
        m_extrIDTrkz0     = -100;
    }

    m_QOverP = pCand->qOverP();
    m_NNBarrel = pCand->annBarrel();
    m_NNEndCap = pCand->annEndCap();
    m_InnerAngle = pCand->innerAngle();
    m_InnerDelta = pCand->innerDelta();
    m_MiddleAngle = pCand->middleAngle();
    m_MiddleDelta = pCand->middleDelta();
    m_Lhr = pCand->LHR();
    m_Isolation = pCand->Isolation();

     if (pCand->passAnn()) m_passNN = 1;
     else m_passNN = 0;
     if (pCand->passStau()) m_passStau = 1;
     else m_passStau = 0;

     //if (pCand->isMF() || pCand->isEF()) m_isMuonFeature = 1;
    // else m_isMuonFeature = 0;

    m_NumChambers = 0;
    for (ChamberList::const_iterator itCh = pCand->chambers().begin();
            itCh != pCand->chambers().end();
            itCh++)
    {
        if (m_NumChambers >= m_maxChambers)
            break;

        Chamber* pChamber = *itCh;
        Intersection* pFineIsect =  pChamber->intersection(ID_TECH,   Chamber::SAME_DISTANCE);
        Intersection* pMdtIsect  =  pChamber->intersection(MDT_TECH,  Chamber::SAME_DISTANCE);
        Intersection* pRpcIsect  =  pChamber->intersection(RPC_TECH,  Chamber::SAME_DISTANCE);
        Intersection* pTgcIsect  =  pChamber->intersection(TGC_TECH,  Chamber::SAME_DISTANCE);
        Intersection* pCscIsect  =  pChamber->intersection(CSC_TECH,  Chamber::SAME_DISTANCE);
        Intersection* pCombIsect =  pChamber->intersection(COMB_TECH, Chamber::SAME_DISTANCE);
        Intersection* pMdtIsect1 =  pChamber->intersection(MDT_TECH,  Chamber::OTHER_DISTANCE);
        Intersection* pRpcIsect1 =  pChamber->intersection(RPC_TECH,  Chamber::OTHER_DISTANCE);
        Intersection* pTgcIsect1 =  pChamber->intersection(TGC_TECH,  Chamber::OTHER_DISTANCE);
        Intersection* pCscIsect1 =  pChamber->intersection(CSC_TECH,  Chamber::OTHER_DISTANCE);
        Intersection* pCombIsect1 = pChamber->intersection(COMB_TECH, Chamber::OTHER_DISTANCE);

        m_ChamberID[m_NumChambers] = pChamber->id().get_identifier32().get_compact();
        m_ChamberStation[m_NumChambers] = (long)pChamber->station()->id();
        m_ChamberRegion[m_NumChambers] = (long)pChamber->regionType();
        m_ChamberDistance[m_NumChambers] = (long)pChamber->distanceType();
        m_ChamberTechnology[m_NumChambers] = (long)pChamber->technologyType();
        m_ChamberFineIsect[m_NumChambers] = pFineIsect  == NULL ? -1 : (long)(pFineIsect->index());
        m_ChamberMdtIsect[m_NumChambers] = pMdtIsect   == NULL ? -1 : (long)(pMdtIsect->index());
        m_ChamberRpcIsect[m_NumChambers] = pRpcIsect   == NULL ? -1 : (long)(pRpcIsect->index());
        m_ChamberTgcIsect[m_NumChambers] = pTgcIsect   == NULL ? -1 : (long)(pTgcIsect->index());
        m_ChamberCscIsect[m_NumChambers] = pCscIsect   == NULL ? -1 : (long)(pCscIsect->index());
        m_ChamberCombIsect[m_NumChambers] = pCombIsect  == NULL ? -1 : (long)(pCombIsect->index());
        m_ChamberMdtIsect1[m_NumChambers] = pMdtIsect1  == NULL ? -1 : (long)(pMdtIsect1->index());
        m_ChamberRpcIsect1[m_NumChambers] = pRpcIsect1  == NULL ? -1 : (long)(pRpcIsect1->index());
        m_ChamberTgcIsect1[m_NumChambers] = pTgcIsect1  == NULL ? -1 : (long)(pTgcIsect1->index());
        m_ChamberCscIsect1[m_NumChambers] = pCscIsect1  == NULL ? -1 : (long)(pCscIsect1->index());
        m_ChamberCombIsect1[m_NumChambers] = pCombIsect1 == NULL ? -1 : (long)(pCombIsect1->index());
        m_ChamberRZ[m_NumChambers] = pChamber->regionType() == BARREL_REGION ? pChamber->center().perp() : pChamber->center().z();
        m_ChamberPhi[m_NumChambers] = pChamber->center().phi();

        m_NumChambers++;
    }
    if (m_log.level() <= MSG::DEBUG)
        m_log << MSG::DEBUG << "CandidateNtuple::fill filled " << m_NumChambers << " chambers" << endmsg;

    m_NumIsects = 0;
    for (IntersectionList::const_iterator itIsect = pCand->intersections().begin();
            itIsect != pCand->intersections().end();
            itIsect++)
    {
        if (m_NumIsects >= m_maxIsect)
            break;

        Intersection* pIsect = *itIsect;
        Chamber* pChamber = pIsect->chamber();
        Segment* pSeg = pIsect->segment();
        const Amg::Vector3D& dir = pIsect->direction();
        const Amg::Vector3D&  pos = pIsect->position();

        m_IsectType[m_NumIsects] = (long)pIsect->type();
        m_IsectOrigTech[m_NumIsects] = (long)pIsect->originTechnologyType();
        m_IsectOrigDist[m_NumIsects] = (long)pIsect->originDistanceType();
        m_IsectTargTech[m_NumIsects] = (long)pIsect->targetTechnologyType();
        m_IsectTargDist[m_NumIsects] = (long)pIsect->targetDistanceType();
        m_IsectRegion[m_NumIsects] = (long)pIsect->regionType();
        m_IsectChamber[m_NumIsects] = pChamber == NULL ? -1 : (long)(pChamber->index());
        m_IsectPosEta[m_NumIsects] = pos.eta();
        m_IsectPosPhi[m_NumIsects] = pos.phi();
        m_IsectPosR[m_NumIsects] = pos.perp();
        m_IsectPosZ[m_NumIsects] = pos.z();
        m_IsectDirEta[m_NumIsects] = dir.eta();
        m_IsectDirPhi[m_NumIsects] = dir.phi();
        m_IsectDirZ[m_NumIsects] = dir.z();
        m_IsectSegment[m_NumIsects] = pSeg == NULL ? -1 : (long)(pSeg->index());
        m_NumIsects++;
    }
    if (m_log.level() <= MSG::DEBUG)
        m_log << MSG::DEBUG << "CandidateNtuple::fill filled " << m_NumIsects << " intersections" << endmsg;

    m_NumHits = 0;
    for (HitList::const_iterator itHit = pCand->hits().begin();
            itHit != pCand->hits().end();
            itHit++)
    {
        if (m_NumHits >= m_maxHits)
            break;

        MuGirlNS::Hit* pHit = *itHit;
        Chamber* pChamber = pHit->chamber();
        if (pChamber == NULL)
        {
            m_log << MSG::WARNING << "pHit->chamber() returned NULL!" << endmsg;
            return StatusCode::RECOVERABLE;
        }
        Intersection* pFineIsect =  pChamber->intersection(ID_TECH,   Chamber::SAME_DISTANCE);
        Intersection* pMdtIsect  =  pChamber->intersection(MDT_TECH,  Chamber::SAME_DISTANCE);
        Intersection* pRpcIsect  =  pChamber->intersection(RPC_TECH,  Chamber::SAME_DISTANCE);
        Intersection* pTgcIsect  =  pChamber->intersection(TGC_TECH,  Chamber::SAME_DISTANCE);
        Intersection* pCscIsect  =  pChamber->intersection(CSC_TECH,  Chamber::SAME_DISTANCE);
        Intersection* pCombIsect =  pChamber->intersection(COMB_TECH, Chamber::SAME_DISTANCE);
        Intersection* pMdtIsect1 =  pChamber->intersection(MDT_TECH,  Chamber::OTHER_DISTANCE);
        Intersection* pRpcIsect1 =  pChamber->intersection(RPC_TECH,  Chamber::OTHER_DISTANCE);
        Intersection* pTgcIsect1 =  pChamber->intersection(TGC_TECH,  Chamber::OTHER_DISTANCE);
        Intersection* pCscIsect1 =  pChamber->intersection(CSC_TECH,  Chamber::OTHER_DISTANCE);
        Intersection* pCombIsect1 = pChamber->intersection(COMB_TECH, Chamber::OTHER_DISTANCE);
        const Amg::MatrixX& errMat = pHit->RIO()->localCovariance();

        m_HitId[m_NumHits] = pHit->RIO()->prepRawData()->identify().get_identifier32().get_compact();
        m_HitChamber[m_NumHits] = pChamber->index();
        m_HitChamberID[m_NumHits] = pChamber->id().get_identifier32().get_compact();
        m_HitDeltaEta[m_NumHits] = pHit->deltaEta();
        m_HitDeltaPhi[m_NumHits] = pHit->deltaPhi();
        m_HitEta[m_NumHits] = pHit->position().eta();
        m_HitPhi[m_NumHits] = pHit->position().phi();
        m_HitR[m_NumHits] = pHit->position().perp();
        m_HitZ[m_NumHits] = pHit->position().z();
        m_HitIsEta[m_NumHits] = (long)pHit->isEta();
        m_HitDistance[m_NumHits] = (long)pChamber->distanceType();
        m_HitTechnology[m_NumHits] = (long)pChamber->technologyType();
        m_HitRegion[m_NumHits] = (long)pChamber->regionType();
        m_HitStation[m_NumHits] = (long)pChamber->station()->id();
        m_HitFineIsect[m_NumHits] = pFineIsect  == NULL ? -1 : (long)(pFineIsect->index());
        m_HitMdtIsect[m_NumHits] = pMdtIsect   == NULL ? -1 : (long)(pMdtIsect->index());
        m_HitRpcIsect[m_NumHits] = pRpcIsect   == NULL ? -1 : (long)(pRpcIsect->index());
        m_HitTgcIsect[m_NumHits] = pTgcIsect   == NULL ? -1 : (long)(pTgcIsect->index());
        m_HitCscIsect[m_NumHits] = pCscIsect   == NULL ? -1 : (long)(pCscIsect->index());
        m_HitCombIsect[m_NumHits] = pCombIsect  == NULL ? -1 : (long)(pCombIsect->index());
        m_HitMdtIsect1[m_NumHits] = pMdtIsect1  == NULL ? -1 : (long)(pMdtIsect1->index());
        m_HitRpcIsect1[m_NumHits] = pRpcIsect1  == NULL ? -1 : (long)(pRpcIsect1->index());
        m_HitTgcIsect1[m_NumHits] = pTgcIsect1  == NULL ? -1 : (long)(pTgcIsect1->index());
        m_HitCscIsect1[m_NumHits] = pCscIsect1  == NULL ? -1 : (long)(pCscIsect1->index());
        m_HitCombIsect1[m_NumHits] = pCombIsect1 == NULL ? -1 : (long)(pCombIsect1->index());

        m_HitHough[m_NumHits] = -1;  // fill the correspondence later in the loop over the Hough transform

        m_HitInSegment[m_NumHits] = (long)pHit->isInSegment();
        m_HitError[m_NumHits] = Amg::error( errMat, Trk::locX);
        if (pChamber->technologyType() == MDT_TECH)
        {
            const Muon::MdtDriftCircleOnTrack* pMdcot =
                dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(pHit->RIO());
            if (pMdcot == NULL)
                m_log << MSG::WARNING << "pHit->RIO is not a MdtDriftCircleOnTrack" << endmsg;
            else
                m_HitDriftRadius[m_NumHits] = pMdcot->driftRadius();
        }
        m_NumHits++;
    }
    if (m_log.level() <= MSG::DEBUG)
        m_log << MSG::DEBUG << "CandidateNtuple::fill filled " << m_NumHits << " hits" << endmsg;

    //fill segment information
    m_NumSegs = 0;
    m_NumHitSegMaps = 0;
    for (SegmentList::const_iterator itSeg = pCand->segments().begin();
            itSeg != pCand->segments().end();
            itSeg++)
    {
        if (m_NumSegs >= m_maxSeg)
            break;

        Segment* pSeg = *itSeg;
        if (pSeg->muonSegment() == NULL)
        {
            m_log << MSG::WARNING << "pSeg->muonSegment() returned NULL!";
            continue;
        }
        if (pSeg->station() == NULL)
        {
            m_log << MSG::WARNING << "pSeg->station() returned NULL!";
            continue;
        }
        const Trk::FitQuality* pFitQuality = pSeg->fitQuality();
        if (pFitQuality == NULL)
        {
            m_log << MSG::WARNING << "pSeg->fitQuality() returned NULL!";
            continue;
        }

        const Amg::Vector3D& pos  = pSeg->position();
        const Amg::Vector3D& dir = pSeg->direction();
        Intersection* pIsect = pSeg->intersection();

        m_SegType[m_NumSegs] = (long)pSeg->type();
        m_SegDistance[m_NumSegs] = (long)pSeg->distanceType();
        m_SegRegion[m_NumSegs] = (long)pSeg->regionType();
        m_SegStation[m_NumSegs] = (long)pSeg->station()->id();
        m_SegHits[m_NumSegs] = (long)pSeg->muonSegment()->numberOfContainedROTs();
        m_SegPosEta[m_NumSegs] = pos.eta();
        m_SegPosPhi[m_NumSegs] = pos.phi();
        m_SegPosR[m_NumSegs] = pos.perp();
        m_SegPosZ[m_NumSegs] = pos.z();
        m_SegDirEta[m_NumSegs] = dir.eta();
        m_SegDirPhi[m_NumSegs] = dir.phi();
        m_SegDirZ[m_NumSegs] = dir.z();
        m_SegChi2[m_NumSegs] = pFitQuality->chiSquared();
        m_SegNdf[m_NumSegs] = (long)pFitQuality->numberDoF();
        m_SegProb[m_NumSegs] = pSeg->fitProbability();
        m_SegIsect[m_NumSegs] = pIsect == NULL ? -1 : pIsect->index();

        for (HitList::const_iterator itHit = pSeg->hits().begin();
                itHit != pSeg->hits().end();
                itHit++)
        {
            if (m_NumHitSegMaps >= m_maxHitSegMaps)
                break;

            Hit* pHit = *itHit;

            m_HitSegMapHit[m_NumHitSegMaps] = pHit->index();
            m_HitSegMapSeg[m_NumHitSegMaps] = pSeg->index();

            m_NumHitSegMaps++;
        }

        m_NumSegs++;
    }
    if (m_log.level() <= MSG::DEBUG)
        m_log << MSG::DEBUG << "CandidateNtuple::fill filled " << m_NumSegs << " segments with " << m_NumHitSegMaps << " hit mappings" << endmsg;

    if (pCand->RefittedTrack()!=NULL)
    {
        const Trk::Track* refitted_track = (pCand->RefittedTrack()->track());
 
        if (refitted_track!=0) {
            m_RefittedTrkSummaryNumMdtHits=(refitted_track->trackSummary())->get(Trk::numberOfMdtHits);
            m_RefittedTrkSummaryNumTgcEtaHits=(refitted_track->trackSummary())->get(Trk::numberOfTgcEtaHits);
            m_RefittedTrkSummaryNumTgcPhiHits=(refitted_track->trackSummary())->get(Trk::numberOfTgcPhiHits);
            m_RefittedTrkSummaryNumCscEtaHits=(refitted_track->trackSummary())->get(Trk::numberOfCscEtaHits );
            m_RefittedTrkSummaryNumCscPhiHits=(refitted_track->trackSummary())->get(Trk::numberOfCscPhiHits );
            m_RefittedTrkSummaryNumRpcEtaHits=(refitted_track->trackSummary())->get(Trk::numberOfRpcEtaHits );
            m_RefittedTrkSummaryNumRpcPhiHits=(refitted_track->trackSummary())->get(Trk::numberOfRpcPhiHits );
            m_RefittedTrkSummaryNumMdtHoles=(refitted_track->trackSummary())->get(Trk::numberOfMdtHoles);
            m_RefittedTrkSummaryNumTgcEtaHoles=(refitted_track->trackSummary())->get(Trk::numberOfTgcEtaHoles );
            m_RefittedTrkSummaryNumTgcPhiHoles=(refitted_track->trackSummary())->get(Trk::numberOfTgcPhiHoles );
            m_RefittedTrkSummaryNumCscEtaHoles=(refitted_track->trackSummary())->get(Trk::numberOfCscEtaHoles );
            m_RefittedTrkSummaryNumCscPhiHoles=(refitted_track->trackSummary())->get(Trk::numberOfCscPhiHoles );
            m_RefittedTrkSummaryNumRpcEtaHoles=(refitted_track->trackSummary())->get(Trk::numberOfRpcEtaHoles );
            m_RefittedTrkSummaryNumRpcPhiHoles=(refitted_track->trackSummary())->get(Trk::numberOfRpcPhiHoles );
        }
    }
    /*    if (pCand->MSRefittedTrack()!=NULL)
      {
	m_MSRefittedTrkSummaryFlag=1;
      }
    else
      {
	m_MSRefittedTrkSummaryFlag=0;
      }
    */

    if (m_log.level() <= MSG::DEBUG)
        m_log << MSG::DEBUG << "CandidateNtuple::fillCandidate ended" << endmsg;

    return StatusCode::SUCCESS;
}


StatusCode CandidateNtuple::fillHoughData(Candidate* pCand, std::vector<MuonHough::MuonLayerHough::Maximum*> hough_data[16][4][2])
{
    if (m_log.level() <= MSG::DEBUG)
      m_log << MSG::DEBUG << "CandidateNtuple::fillHoughData" << endmsg;

    m_NumHoughMaxima=0;
    m_NumHoughHits = 0;
    for (unsigned int sector=0;sector<16;sector++) {
      for (unsigned int distance=0;distance<DISTANCES;distance++) {
        for (unsigned int region=0;region<REGIONS;region++) {
          std::vector<MuonHough::MuonLayerHough::Maximum*> maxvec = hough_data[sector][distance][region];
          for (unsigned int maximum=0; maximum<maxvec.size(); maximum++) {
          
            if (m_NumHoughMaxima >= m_maxMaxima) break;

            MuonHough::MuonLayerHough::Maximum* max = maxvec[maximum];

            float reference_position;
            float posmin;
            float posmax;
            int   dummy = 0;
            max->hough->pars(max->binposmin, dummy, reference_position,posmin);
            max->hough->pars(max->binposmax, dummy, reference_position,posmax);

            float eta_sign = +1.;
            if ( (region==0 && max->pos<0.) || (region==1 && reference_position<0.) ) eta_sign = -1.;

            float max_eta = (region)? -eta_sign*log( tan ( atan2(fabsf(max->pos),fabsf(reference_position))/2. ) ) : 
                                      -eta_sign*log( tan ( atan2(fabsf(reference_position),fabsf(max->pos))/2. ) ); 

            m_MaximumSector[m_NumHoughMaxima]    = sector;
            m_MaximumDistance[m_NumHoughMaxima]  = distance;
            m_MaximumRegion[m_NumHoughMaxima]    = region;
            m_MaximumValue[m_NumHoughMaxima]     = max->max;
            m_MaximumPosition[m_NumHoughMaxima]  = max->pos;
            m_MaximumEta[m_NumHoughMaxima]       = max_eta; 
            m_MaximumTheta[m_NumHoughMaxima]     = max->theta;
            m_MaximumRefPlane[m_NumHoughMaxima]  = reference_position;
            m_MaximumLowPos[m_NumHoughMaxima]    = posmin;
            m_MaximumUppPos[m_NumHoughMaxima]    = posmax;
            m_TriggerConfirmed[m_NumHoughMaxima] = max->triggerConfirmed;

            // start looping over the hits
            std::vector<MuonHough::Hit*>::iterator it = max->hits.begin(); 
            while (it!=max->hits.end() && m_NumHoughHits < m_maxHoughHits) {
              if ( (*it)->prd ) {
                Identifier id = (*it)->prd->identify();

                // check if the identifier is valid
                if (m_idHelperTool.isMdt(id)) {
                  if (!m_idHelperTool.mdtIdHelper().valid(id)) {
                    m_log << MSG::ERROR << "CandidateNtuple::fillHoughData, PrepData loop: MDT id 0x" << id << " is invalid!" << endmsg;
                    (*it)->prd->dump(m_log);
                  }
                } else if (m_idHelperTool.isRpc(id)) {
                  if (!m_idHelperTool.rpcIdHelper().valid(id)) {
                    m_log << MSG::ERROR << "CandidateNtuple::fillHoughData, PrepData loop: RPC id 0x" << id << " is invalid!" << endmsg;
                    (*it)->prd->dump(m_log);
                  }
                } else if (m_idHelperTool.issTgc(id)) {
                  if (!m_idHelperTool.stgcIdHelper().valid(id)) {
                    m_log << MSG::ERROR << "CandidateNtuple::fillHoughData, PrepData loop: sTGC id 0x" << id << " is invalid!" << endmsg;
                    (*it)->prd->dump(m_log);
                   }
                } else if (m_idHelperTool.isMM(id)) {
                  if (!m_idHelperTool.mmIdHelper().valid(id)) {
                    m_log << MSG::ERROR << "CandidateNtuple::fillHoughData, PrepData loop: MM id 0x" << id << " is invalid!" << endmsg;
                    (*it)->prd->dump(m_log);
                  }
                } else if (m_idHelperTool.isCsc(id)) {
                  if (!m_idHelperTool.cscIdHelper().valid(id)) {
                    m_log << MSG::ERROR << "CandidateNtuple::fillHoughData, PrepData loop: CSC id 0x" << id << " is invalid!" << endmsg;
                    (*it)->prd->dump(m_log);
                  }
                }

                // retrieve the global position of the PrepRawData
                const Amg::Vector2D locPos = (*it)->prd->localPosition();
                const Trk::TrkDetElementBase* detEl = (*it)->prd->detectorElement();
                if (detEl==0) {
                    m_log << MSG::ERROR << "CandidateNtuple::fillHoughData, PrepRawData loop: detector element retrieved from the PRD is invalid!" << endmsg;
                    continue;
                }
                Amg::Vector3D gPos,dummy(1.,1.,1.);
                detEl->surface( id ).localToGlobal(locPos,dummy,gPos);

                m_HoughHitDeltaEta[m_NumHoughHits] = gPos.eta() - max_eta;
                m_HoughHitEta[m_NumHoughHits]      = gPos.eta();
                m_HoughHitPhi[m_NumHoughHits]      = gPos.phi();
                m_HoughHitR[m_NumHoughHits]        = gPos.perp();
                m_HoughHitZ[m_NumHoughHits]        = gPos.z();
                m_HoughHitIsEta[m_NumHoughHits]    = (m_idHelperTool.measuresPhi(id))? 0 : 1;
                m_HoughHitDistance[m_NumHoughHits] = distance;
                m_HoughHitRegion[m_NumHoughHits]   = region;
                m_HoughHitSector[m_NumHoughHits]   = sector;

                connectHitToHough(pCand,id,m_NumHoughMaxima,m_NumHoughHits);
                m_NumHoughHits++;
              }
              else if ( (*it)->tgc ) {
                std::vector< const Muon::TgcPrepData* > tgc = (*it)->tgc->etaCluster.hitList;
                for (unsigned int tgc_hit=0; tgc_hit<tgc.size(); tgc_hit++) {
                  Identifier id = tgc[tgc_hit]->identify();
                  // check if the identifier is valid
                  if (!m_idHelperTool.tgcIdHelper().valid(id)) {
                      m_log << MSG::ERROR << "CandidateNtuple::fillHoughData, Tgc PrepData loop: id 0x" << id << " is invalid!" << endmsg;
                      //(*it)->tgc->dump(m_log);
                  }

                  const Amg::Vector2D locPos = tgc[tgc_hit]->localPosition();
                  const Trk::TrkDetElementBase* detEl = tgc[tgc_hit]->detectorElement();
                  if (detEl==0) {
                    m_log << MSG::ERROR << "CandidateNtuple::fillHoughData, Tgc Prepdata loop:: detector element retrieved from the TgcPrepData is invalid!" << endmsg;
                    continue;
                  }
                  Amg::Vector3D gPos, dummy(1.,1.,1.);
                  detEl->surface( id ).localToGlobal(locPos, dummy, gPos);

                  m_HoughHitDeltaEta[m_NumHoughHits]  = gPos.eta() - max_eta;
                  m_HoughHitEta[m_NumHoughHits]       = gPos.eta();
                  m_HoughHitPhi[m_NumHoughHits]       = gPos.phi();
                  m_HoughHitR[m_NumHoughHits]         = gPos.perp();
                  m_HoughHitZ[m_NumHoughHits]         = gPos.z();
                  m_HoughHitIsEta[m_NumHoughHits]     = (m_idHelperTool.measuresPhi(id))? 0 : 1;
                  m_HoughHitDistance[m_NumHoughHits]  = distance;
                  m_HoughHitRegion[m_NumHoughHits]    = region;
                  m_HoughHitSector[m_NumHoughHits]    = sector;

                  connectHitToHough(pCand,id,m_NumHoughMaxima,m_NumHoughHits);
                  m_NumHoughHits++;
                }
              }
              ++it;
            }
            m_NumHoughMaxima++;
          }
        }
      }
    }

    if (m_log.level() <= MSG::DEBUG)
        m_log << MSG::DEBUG << "CandidateNtuple::fillHoughdata: number of Hough Maxima processed: " << m_NumHoughMaxima 
                         << ",  number of Hough Hits processed: " << m_NumHoughHits << endmsg;

    if (m_log.level() <= MSG::DEBUG)
        m_log << MSG::DEBUG << "CandidateNtuple::fillHoughData ended" << endmsg;

    return StatusCode::SUCCESS;
}


void CandidateNtuple::connectHitToHough(Candidate* pCand, Identifier id, int hough_maximum, int hough_hit) {
    int NumHits = 0;

    m_HoughMaximum[hough_hit]       = hough_maximum;
    m_HoughMuGirlHit[hough_hit]     = -1;

    MuGirlNS::TechnologyType technology = TECHNOLOGIES;
    if ( m_idHelperTool.isMdt(id) )  technology = MDT_TECH;
    if ( m_idHelperTool.isTgc(id) )  technology = TGC_TECH;
    if ( m_idHelperTool.isRpc(id) )  technology = RPC_TECH;
    if ( m_idHelperTool.isCsc(id) )  technology = CSC_TECH;
    if ( m_idHelperTool.issTgc(id) ) technology = STGC_TECH;
    if ( m_idHelperTool.isMM(id) )   technology = MM_TECH;

    m_HoughHitTechnology[hough_hit] = technology;

    //uint32_t hough_hit_id = id.get_identifier32().get_compact();

    //for (int i=0;i<m_NumHits;i++) {
    for (HitList::const_iterator itHit = pCand->hits().begin();
            itHit != pCand->hits().end();
            itHit++)
    {
      MuGirlNS::Hit* pHit = *itHit;

      if ( id == pHit->RIO()->prepRawData()->identify() ) {
        m_HitHough[NumHits]             = hough_hit;
        m_HoughMuGirlHit[hough_hit]     = NumHits;
        // correspondence found, then return
        return;
      }
      NumHits++;
    }
}


} //end of namespace
