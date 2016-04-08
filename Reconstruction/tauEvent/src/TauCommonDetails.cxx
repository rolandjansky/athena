/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauCommonDetails.cxx
///
/// Implementation of details class for tau event
///
/// Package: Reconstruction/TauEvent
///
/// @author Noel Dawe
/// @author Thomas Burgess  
///
///  Created by Thomas Burgess on 2008-10-20.
///
///  $Id: TauCommonDetails.cxx,v 1.5 2009-05-20 13:10:37 slai Exp $

//TauEvent includes
#include "tauEvent/TauCmp.h"
#include "tauEvent/TauCommonDetails.h"
//Other includes
#include "DataModel/ElementLink.h"
#include "DataModel/ClassName.h"
#include "VxVertex/RecVertex.h"
//C/C++ includes
#include <string>

namespace Analysis {
    //Default constructor
    //Initializes members to default values where applicable
    TauCommonDetails :: TauCommonDetails():
        TauDetails(),
        //Applicable for track & calo seeded tau reconstruction
        m_ipZ0SinThetaSigLeadTrk(TauDetails::DEFAULT),
        m_etOverPtLeadTrk(TauDetails::DEFAULT),
        m_etOverPtLeadLooseTrk(TauDetails::DEFAULT),
        m_leadTrkPt(TauDetails::DEFAULT),
        m_leadLooseTrkPt(TauDetails::DEFAULT),
        m_ipSigLeadTrk(TauDetails::DEFAULT),
        m_ipSigLeadLooseTrk(TauDetails::DEFAULT),
        m_chrgLooseTrk(TauDetails::DEFAULT),
        m_massTrkSys(TauDetails::DEFAULT),
        m_trkWidth2(TauDetails::DEFAULT),
        m_trFlightPathSig(TauDetails::DEFAULT),
        m_secVtx(0),
        m_etEflow(TauDetails::DEFAULT),
        m_mEflow(TauDetails::DEFAULT),
        //Electron LLH ID variables 
        m_Ele_E237E277(TauDetails::DEFAULT),
        m_Ele_PresamplerFraction(TauDetails::DEFAULT),
        m_Ele_ECALFirstFraction(TauDetails::DEFAULT),
        //topocluster ID variables
        m_numCells(0),
        m_numTopoClusters(0),
        m_numEffTopoClusters(TauDetails::DEFAULT),
        m_topoInvMass(TauDetails::DEFAULT),
        m_effTopoInvMass(TauDetails::DEFAULT),
        m_topoMeanDeltaR(TauDetails::DEFAULT),
        m_effTopoMeanDeltaR(TauDetails::DEFAULT),
        //Applicable for calo seeded tau reconstruction
        m_seedCalo_nIsolLooseTrk(TauDetails::DEFAULT_INT),
        m_seedCalo_EMRadius(TauDetails::DEFAULT),
        m_seedCalo_hadRadius(TauDetails::DEFAULT),
        m_seedCalo_etEMAtEMScale(TauDetails::DEFAULT),
        m_seedCalo_etHadAtEMScale(TauDetails::DEFAULT),
        m_seedCalo_isolFrac(TauDetails::DEFAULT),
        m_seedCalo_centFrac(TauDetails::DEFAULT),
        m_seedCalo_stripWidth2(TauDetails::DEFAULT),
        m_seedCalo_nStrip(TauDetails::DEFAULT_INT),
        m_seedCalo_etEMCalib(TauDetails::DEFAULT),
        m_seedCalo_etHadCalib(TauDetails::DEFAULT),
        m_seedCalo_eta(TauDetails::DEFAULT),
        m_seedCalo_phi(TauDetails::DEFAULT),
        m_seedCalo_trkAvgDist(TauDetails::DEFAULT),
        m_seedCalo_trkRmsDist(TauDetails::DEFAULT),
        m_seedCalo_lead2ClusterEOverAllClusterE(TauDetails::DEFAULT),
        m_seedCalo_lead3ClusterEOverAllClusterE(TauDetails::DEFAULT),
        m_seedCalo_caloIso(TauDetails::DEFAULT),
        m_seedCalo_caloIsoCorrected(TauDetails::DEFAULT),
        m_seedCalo_dRmax(TauDetails::DEFAULT),
        //Applicable for track seeded tau reconstruction
        m_seedTrk_EMRadius(TauDetails::DEFAULT),
        m_seedTrk_isolFrac(TauDetails::DEFAULT),
        m_seedTrk_etChrgHadOverSumTrkPt (TauDetails::DEFAULT),
        m_seedTrk_isolFracWide( TauDetails::DEFAULT),
        m_seedTrk_etHadAtEMScale(TauDetails::DEFAULT),
        m_seedTrk_etEMAtEMScale(TauDetails::DEFAULT),
        m_seedTrk_etEMCL(TauDetails::DEFAULT),
        m_seedTrk_etChrgEM(TauDetails::DEFAULT),
        m_seedTrk_etNeuEM(TauDetails::DEFAULT),
        m_seedTrk_etResNeuEM(TauDetails::DEFAULT),
        m_seedTrk_hadLeakEt(TauDetails::DEFAULT),       
        m_seedTrk_sumEMCellEtOverLeadTrkPt(TauDetails::DEFAULT),
        m_seedTrk_secMaxStripEt(TauDetails::DEFAULT),
        m_seedTrk_stripWidth2(TauDetails::DEFAULT),
        m_seedTrk_nStrip(TauDetails::DEFAULT_INT),
        m_seedTrk_etChrgHad(TauDetails::DEFAULT),
        m_seedTrk_nOtherCoreTrk(TauDetails::DEFAULT_INT),
        m_seedTrk_nIsolTrk(TauDetails::DEFAULT_INT),
        m_seedTrk_etIsolEM(TauDetails::DEFAULT),
        m_seedTrk_etIsolHad(TauDetails::DEFAULT),
        // Energy scale
        m_EM_TES_scale(TauDetails::DEFAULT),
        m_LC_TES_precalib(TauDetails::DEFAULT),
	      m_cellBasedEnergyRing1(TauDetails::DEFAULT),
	      m_cellBasedEnergyRing2(TauDetails::DEFAULT),
	      m_cellBasedEnergyRing3(TauDetails::DEFAULT),
	      m_cellBasedEnergyRing4(TauDetails::DEFAULT),
	      m_cellBasedEnergyRing5(TauDetails::DEFAULT),
	      m_cellBasedEnergyRing6(TauDetails::DEFAULT),
	      m_cellBasedEnergyRing7(TauDetails::DEFAULT),
        m_TRT_NHT_OVER_NLT(TauDetails::DEFAULT),
        m_TauJetVtxFraction(0),
        //track filter and pi0 cone
        m_TrackFilterProngs(0),
        m_TrackFilterQuality(0),
        m_Pi0ConeDR(0.)
    {
    }	

    //Copy constructor
    TauCommonDetails :: TauCommonDetails(const TauCommonDetails &rhs):
        TauDetails(),
        //Applicable for track & calo seeded tau reconstruction
        m_ipZ0SinThetaSigLeadTrk(rhs.m_ipZ0SinThetaSigLeadTrk),
        m_etOverPtLeadTrk(rhs.m_etOverPtLeadTrk),
        m_etOverPtLeadLooseTrk(rhs.m_etOverPtLeadLooseTrk),
        m_leadTrkPt(rhs.m_leadTrkPt),
        m_leadLooseTrkPt(rhs.m_leadLooseTrkPt),
        m_ipSigLeadTrk(rhs.m_ipSigLeadTrk),
        m_ipSigLeadLooseTrk(rhs.m_ipSigLeadLooseTrk),
        m_looseTrk(rhs.m_looseTrk),
        m_looseConvTrk(rhs.m_looseConvTrk),
        m_seedCalo_wideTrk(rhs.m_seedCalo_wideTrk),
        m_seedTrk_wideTrk(rhs.m_seedTrk_wideTrk),
        m_otherTrk(rhs.m_otherTrk),
        m_chrgLooseTrk(rhs.m_chrgLooseTrk),
        m_cellEM012Cluster(rhs.m_cellEM012Cluster),
        m_sumPi0Vec(rhs.m_sumPi0Vec),
        m_massTrkSys(rhs.m_massTrkSys),
        m_trkWidth2(rhs.m_trkWidth2),
        m_trFlightPathSig(rhs.m_trFlightPathSig),
        m_secVtx(rhs.m_secVtx ? new Trk::RecVertex(*rhs.m_secVtx) : 0),
        m_etEflow(rhs.m_etEflow),
        m_mEflow(rhs.m_mEflow),
        m_pi0(rhs.m_pi0),
        //Electron LLH ID variables 
        m_Ele_E237E277(rhs.m_Ele_E237E277),
        m_Ele_PresamplerFraction(rhs.m_Ele_PresamplerFraction),
        m_Ele_ECALFirstFraction(rhs.m_Ele_ECALFirstFraction),
        //topocluster ID variables
        m_numCells(rhs.m_numCells),
        m_numTopoClusters(rhs.m_numTopoClusters),
        m_numEffTopoClusters(rhs.m_numEffTopoClusters),
        m_topoInvMass(rhs.m_topoInvMass),
        m_effTopoInvMass(rhs.m_effTopoInvMass),
        m_topoMeanDeltaR(rhs.m_topoMeanDeltaR),
        m_effTopoMeanDeltaR(rhs.m_effTopoMeanDeltaR),
        //Applicable for calo seeded tau reconstruction
        m_seedCalo_nIsolLooseTrk(rhs.m_seedCalo_nIsolLooseTrk),
        m_seedCalo_EMRadius(rhs.m_seedCalo_EMRadius),
        m_seedCalo_hadRadius(rhs.m_seedCalo_hadRadius),
        m_seedCalo_etEMAtEMScale(rhs.m_seedCalo_etEMAtEMScale),
        m_seedCalo_etHadAtEMScale(rhs.m_seedCalo_etHadAtEMScale),
        m_seedCalo_isolFrac(rhs.m_seedCalo_isolFrac),
        m_seedCalo_centFrac(rhs.m_seedCalo_centFrac),
        m_seedCalo_stripWidth2(rhs.m_seedCalo_stripWidth2),
        m_seedCalo_nStrip(rhs.m_seedCalo_nStrip),
        m_seedCalo_etEMCalib(rhs.m_seedCalo_etEMCalib),
        m_seedCalo_etHadCalib(rhs.m_seedCalo_etHadCalib),
        m_seedCalo_eta(rhs.m_seedCalo_eta),
        m_seedCalo_phi(rhs.m_seedCalo_phi),
        m_seedCalo_trkAvgDist(rhs.m_seedCalo_trkAvgDist),
        m_seedCalo_trkRmsDist(rhs.m_seedCalo_trkRmsDist),
        m_seedCalo_lead2ClusterEOverAllClusterE(rhs.m_seedCalo_lead2ClusterEOverAllClusterE),
        m_seedCalo_lead3ClusterEOverAllClusterE(rhs.m_seedCalo_lead3ClusterEOverAllClusterE),
        m_seedCalo_caloIso(rhs.m_seedCalo_caloIso),
        m_seedCalo_caloIsoCorrected(rhs.m_seedCalo_caloIsoCorrected),
        m_seedCalo_dRmax(rhs.m_seedCalo_dRmax),
        //Applicable for track seeded tau reconstruction
        m_seedTrk_EMRadius(rhs.m_seedTrk_EMRadius),
        m_seedTrk_isolFrac(rhs.m_seedTrk_isolFrac),
        m_seedTrk_etChrgHadOverSumTrkPt(rhs.m_seedTrk_etChrgHadOverSumTrkPt),
        m_seedTrk_isolFracWide(rhs.m_seedTrk_isolFracWide),
        m_seedTrk_etHadAtEMScale(rhs.m_seedCalo_etHadAtEMScale),
        m_seedTrk_etEMAtEMScale(rhs.m_seedTrk_etEMAtEMScale),
        m_seedTrk_etEMCL(rhs.m_seedTrk_etEMCL),
        m_seedTrk_etChrgEM(rhs.m_seedTrk_etChrgEM),
        m_seedTrk_etNeuEM(rhs.m_seedTrk_etNeuEM),
        m_seedTrk_etResNeuEM(rhs.m_seedTrk_etResNeuEM),
        m_seedTrk_hadLeakEt(rhs.m_seedTrk_hadLeakEt),
        m_seedTrk_etChrgEM01Trk(rhs.m_seedTrk_etChrgEM01Trk),
        m_seedTrk_etResChrgEMTrk(rhs.m_seedTrk_etResChrgEMTrk),
        m_seedTrk_sumEMCellEtOverLeadTrkPt(rhs.m_seedTrk_sumEMCellEtOverLeadTrkPt),
        m_seedTrk_secMaxStripEt(rhs.m_seedTrk_secMaxStripEt),
        m_seedTrk_stripWidth2(rhs.m_seedTrk_stripWidth2),
        m_seedTrk_nStrip(rhs.m_seedTrk_nStrip),
        m_seedTrk_etChrgHad(rhs.m_seedTrk_etChrgHad),
        m_seedTrk_nOtherCoreTrk(rhs.m_seedTrk_nOtherCoreTrk),
        m_seedTrk_nIsolTrk(rhs.m_seedTrk_nIsolTrk),
        m_seedTrk_etIsolEM(rhs.m_seedTrk_etIsolEM),
        m_seedTrk_etIsolHad(rhs.m_seedTrk_etIsolHad),
        // Energy scale
        m_EM_TES_scale(rhs.m_EM_TES_scale),
        m_LC_TES_precalib(rhs.m_LC_TES_precalib),
	      m_cellBasedEnergyRing1(rhs.m_cellBasedEnergyRing1),
	      m_cellBasedEnergyRing2(rhs.m_cellBasedEnergyRing2),
	      m_cellBasedEnergyRing3(rhs.m_cellBasedEnergyRing3),
	      m_cellBasedEnergyRing4(rhs.m_cellBasedEnergyRing4),
	      m_cellBasedEnergyRing5(rhs.m_cellBasedEnergyRing5),
	      m_cellBasedEnergyRing6(rhs.m_cellBasedEnergyRing6),
	      m_cellBasedEnergyRing7(rhs.m_cellBasedEnergyRing7),
        m_TRT_NHT_OVER_NLT(rhs.m_TRT_NHT_OVER_NLT),
        m_TauJetVtxFraction(rhs.m_TauJetVtxFraction),
        //track filter and pi0 cone
        m_TrackFilterPass(rhs.m_TrackFilterPass),
        m_TrackFilterProngs(rhs.m_TrackFilterProngs),
        m_TrackFilterQuality(rhs.m_TrackFilterQuality),
        m_Pi0ConeDR(rhs.m_Pi0ConeDR)
        {

        }

    //Destructor
    TauCommonDetails :: ~TauCommonDetails()
    {
        delete m_secVtx;
    }

    //Assignment operator
    TauCommonDetails &TauCommonDetails::operator=(
            const TauCommonDetails &rhs)
    {
        if(this != &rhs)
        {
            //Applicable for track & calo seeded tau reconstruction
            m_ipZ0SinThetaSigLeadTrk=rhs.m_ipZ0SinThetaSigLeadTrk;
            m_etOverPtLeadTrk=rhs.m_etOverPtLeadTrk;
            m_etOverPtLeadLooseTrk=rhs.m_etOverPtLeadLooseTrk;
            m_leadTrkPt=rhs.m_leadTrkPt;
            m_leadLooseTrkPt=rhs.m_leadLooseTrkPt;
            m_ipSigLeadTrk=rhs.m_ipSigLeadTrk;  
            m_ipSigLeadLooseTrk=rhs.m_ipSigLeadLooseTrk;	    	    
            m_looseTrk=rhs.m_looseTrk;
            m_looseConvTrk=rhs.m_looseConvTrk;
            m_seedCalo_wideTrk=rhs.m_seedCalo_wideTrk;
            m_seedTrk_wideTrk=rhs.m_seedTrk_wideTrk;
            m_otherTrk=rhs.m_otherTrk;
            m_chrgLooseTrk=rhs.m_chrgLooseTrk;
            m_cellEM012Cluster=rhs.m_cellEM012Cluster;
            m_sumPi0Vec=rhs.m_sumPi0Vec;
            m_massTrkSys=rhs.m_massTrkSys;
            m_trkWidth2=rhs.m_trkWidth2;
            m_trFlightPathSig=rhs.m_trFlightPathSig;
            delete m_secVtx;
            if (rhs.m_secVtx)
            {
                m_secVtx=new Trk::RecVertex(*rhs.m_secVtx);
            }
            m_etEflow=rhs.m_etEflow; 
            m_mEflow=rhs.m_mEflow; 
            m_pi0 =rhs.m_pi0;
            //Electron LLH ID variables 
            m_Ele_E237E277=rhs.m_Ele_E237E277;
            m_Ele_ECALFirstFraction=rhs.m_Ele_ECALFirstFraction;
            //topocluster ID variables 
            m_numCells=rhs.m_numCells;
            m_numTopoClusters=rhs.m_numTopoClusters;
            m_numEffTopoClusters=rhs.m_numEffTopoClusters;
            m_topoInvMass=rhs.m_topoInvMass;
            m_effTopoInvMass=rhs.m_effTopoInvMass;
            m_topoMeanDeltaR=rhs.m_topoMeanDeltaR;
            m_effTopoMeanDeltaR=rhs.m_effTopoMeanDeltaR;
            //Applicable for calo seeded tau reconstruction
            m_seedCalo_nIsolLooseTrk=rhs.m_seedCalo_nIsolLooseTrk;
            m_seedCalo_EMRadius=rhs.m_seedCalo_EMRadius;
            m_seedCalo_hadRadius=rhs.m_seedCalo_hadRadius;
            m_seedCalo_etEMAtEMScale=rhs.m_seedCalo_etEMAtEMScale;
            m_seedCalo_etHadAtEMScale=rhs.m_seedCalo_etHadAtEMScale;
            m_seedCalo_isolFrac=rhs.m_seedCalo_isolFrac;
            m_seedCalo_centFrac=rhs.m_seedCalo_centFrac;
            m_seedCalo_stripWidth2=rhs.m_seedCalo_stripWidth2;
            m_seedCalo_nStrip=rhs.m_seedCalo_nStrip;
            m_seedCalo_etEMCalib=rhs.m_seedCalo_etEMCalib;
            m_seedCalo_etHadCalib=rhs.m_seedCalo_etHadCalib;
            m_seedCalo_eta=rhs.m_seedCalo_eta;
            m_seedCalo_phi=rhs.m_seedCalo_phi;
            m_seedCalo_trkAvgDist=rhs.m_seedCalo_trkAvgDist;
            m_seedCalo_trkRmsDist=rhs.m_seedCalo_trkRmsDist;
            m_seedCalo_lead2ClusterEOverAllClusterE=rhs.m_seedCalo_lead2ClusterEOverAllClusterE;
            m_seedCalo_lead3ClusterEOverAllClusterE=rhs.m_seedCalo_lead3ClusterEOverAllClusterE;
            m_seedCalo_caloIso=rhs.m_seedCalo_caloIso;
            m_seedCalo_caloIsoCorrected=rhs.m_seedCalo_caloIsoCorrected;
            m_seedCalo_dRmax=rhs.m_seedCalo_dRmax;
            //Applicable for track seeded tau reconstruction
            m_seedTrk_EMRadius=rhs.m_seedTrk_EMRadius;
            m_seedTrk_isolFrac=rhs.m_seedTrk_isolFrac;
            m_seedTrk_nStrip=rhs.m_seedTrk_nStrip;
            m_seedTrk_etChrgHad=rhs.m_seedTrk_etChrgHad;
            m_seedTrk_nOtherCoreTrk=rhs.m_seedTrk_nOtherCoreTrk;
            m_seedTrk_isolFracWide=rhs.m_seedTrk_isolFracWide;
            m_seedTrk_etHadAtEMScale=rhs.m_seedCalo_etHadAtEMScale;
            m_seedTrk_etEMAtEMScale=rhs.m_seedTrk_etEMAtEMScale;
            m_seedTrk_etEMCL=rhs.m_seedTrk_etEMCL;
            m_seedTrk_etChrgEM=rhs.m_seedTrk_etChrgEM;
            m_seedTrk_etNeuEM=rhs.m_seedTrk_etNeuEM;
            m_seedTrk_etResNeuEM=rhs.m_seedTrk_etResNeuEM;
            m_seedTrk_hadLeakEt=rhs.m_seedTrk_hadLeakEt;
            m_seedTrk_sumEMCellEtOverLeadTrkPt=rhs.m_seedTrk_sumEMCellEtOverLeadTrkPt;
            m_seedTrk_etChrgEM01Trk=rhs.m_seedTrk_etChrgEM01Trk;
            m_seedTrk_etResChrgEMTrk=rhs.m_seedTrk_etResChrgEMTrk;
            m_seedTrk_secMaxStripEt=rhs.m_seedTrk_secMaxStripEt;
            m_seedTrk_stripWidth2=rhs.m_seedTrk_stripWidth2;
            m_seedTrk_nIsolTrk=rhs.m_seedTrk_nIsolTrk;
            m_seedTrk_etIsolEM=rhs.m_seedTrk_etIsolEM;
            m_seedTrk_etIsolHad=rhs.m_seedTrk_etIsolHad;
            // Energy scale
            m_EM_TES_scale=rhs.m_EM_TES_scale;
            m_LC_TES_precalib=rhs.m_LC_TES_precalib;
	          m_cellBasedEnergyRing1=rhs.m_cellBasedEnergyRing1;
	          m_cellBasedEnergyRing2=rhs.m_cellBasedEnergyRing2;
	          m_cellBasedEnergyRing3=rhs.m_cellBasedEnergyRing3;
	          m_cellBasedEnergyRing4=rhs.m_cellBasedEnergyRing4;
	          m_cellBasedEnergyRing5=rhs.m_cellBasedEnergyRing5;
	          m_cellBasedEnergyRing6=rhs.m_cellBasedEnergyRing6;
	          m_cellBasedEnergyRing7=rhs.m_cellBasedEnergyRing7;
            m_TRT_NHT_OVER_NLT=rhs.m_TRT_NHT_OVER_NLT;
            m_TauJetVtxFraction=rhs.m_TauJetVtxFraction;
            //track filter and pi0 cone
            m_TrackFilterPass=rhs.m_TrackFilterPass;
            m_TrackFilterProngs=rhs.m_TrackFilterProngs;
            m_TrackFilterQuality=rhs.m_TrackFilterQuality;
            m_Pi0ConeDR=rhs.m_Pi0ConeDR;
        }
        return *this;
    }

    bool TauCommonDetails::operator==(const TauCommonDetails& rhs) const {
        //Double comparisons
        if (!TauCmp::cmp(m_ipZ0SinThetaSigLeadTrk,rhs.m_ipZ0SinThetaSigLeadTrk))
            return false;
        if (!TauCmp::cmp(m_etOverPtLeadTrk,rhs.m_etOverPtLeadTrk))
            return false;
        if (!TauCmp::cmp(m_etOverPtLeadLooseTrk,rhs.m_etOverPtLeadLooseTrk))
            return false;
        if (!TauCmp::cmp(m_leadTrkPt,rhs.m_leadTrkPt))
            return false;
        if (!TauCmp::cmp(m_leadLooseTrkPt,rhs.m_leadLooseTrkPt))
            return false;
        if (!TauCmp::cmp(m_ipSigLeadTrk,rhs.m_ipSigLeadTrk))
            return false;
        if (!TauCmp::cmp(m_ipSigLeadLooseTrk,rhs.m_ipSigLeadLooseTrk))
            return false;
        if (!TauCmp::cmp(m_chrgLooseTrk,rhs.m_chrgLooseTrk))
            return false;
        if (!TauCmp::cmp(m_massTrkSys,rhs.m_massTrkSys))
            return false;
        if (!TauCmp::cmp(m_trkWidth2,rhs.m_trkWidth2))
            return false;
        if (!TauCmp::cmp(m_trFlightPathSig,rhs.m_trFlightPathSig))
            return false;
        if (!TauCmp::cmp(m_etEflow,rhs.m_etEflow))
            return false;
        if (!TauCmp::cmp(m_mEflow,rhs.m_mEflow))
            return false;
        if (!TauCmp::cmp(m_seedCalo_nIsolLooseTrk,rhs.m_seedCalo_nIsolLooseTrk))
            return false;
        if (!TauCmp::cmp(m_seedCalo_EMRadius,rhs.m_seedCalo_EMRadius))
            return false;
        if (!TauCmp::cmp(m_seedCalo_hadRadius,rhs.m_seedCalo_hadRadius))
            return false;
        if (!TauCmp::cmp(m_seedCalo_etEMAtEMScale,rhs.m_seedCalo_etEMAtEMScale))
            return false;
        if (!TauCmp::cmp(m_seedCalo_etHadAtEMScale,rhs.m_seedCalo_etHadAtEMScale))
            return false;
        if (!TauCmp::cmp(m_seedCalo_isolFrac,rhs.m_seedCalo_isolFrac))
            return false;
        if (!TauCmp::cmp(m_seedCalo_centFrac,rhs.m_seedCalo_centFrac))
            return false;
        if (!TauCmp::cmp(m_seedCalo_stripWidth2,rhs.m_seedCalo_stripWidth2))
            return false;
        if (!TauCmp::cmp(m_seedCalo_etEMCalib,rhs.m_seedCalo_etEMCalib))
            return false;
        if (!TauCmp::cmp(m_seedCalo_etHadCalib,rhs.m_seedCalo_etHadCalib))
            return false;
        if (!TauCmp::cmp(m_seedCalo_eta,rhs.m_seedCalo_eta))
            return false;
        if (!TauCmp::cmp(m_seedCalo_phi,rhs.m_seedCalo_phi))
            return false;
        if (!TauCmp::cmp(m_seedCalo_trkAvgDist,rhs.m_seedCalo_trkAvgDist))
            return false;
        if (!TauCmp::cmp(m_seedCalo_trkRmsDist,rhs.m_seedCalo_trkRmsDist))
            return false;
        if (!TauCmp::cmp(m_seedCalo_lead2ClusterEOverAllClusterE,rhs.m_seedCalo_lead2ClusterEOverAllClusterE))
            return false;
        if (!TauCmp::cmp(m_seedCalo_lead3ClusterEOverAllClusterE,rhs.m_seedCalo_lead3ClusterEOverAllClusterE))
            return false;
        if (!TauCmp::cmp(m_seedCalo_caloIso,rhs.m_seedCalo_caloIso))
            return false;
        if (!TauCmp::cmp(m_seedCalo_caloIsoCorrected,rhs.m_seedCalo_caloIsoCorrected))
            return false;
        if (!TauCmp::cmp(m_seedCalo_dRmax,rhs.m_seedCalo_dRmax))
            return false;
        if (!TauCmp::cmp(m_seedTrk_EMRadius,rhs.m_seedTrk_EMRadius))
            return false;
        if (!TauCmp::cmp(m_seedTrk_isolFrac,rhs.m_seedTrk_isolFrac))
            return false;
        if (!TauCmp::cmp(m_seedTrk_etChrgHadOverSumTrkPt,rhs.m_seedTrk_etChrgHadOverSumTrkPt))
            return false;
        if (!TauCmp::cmp(m_seedTrk_isolFracWide,rhs.m_seedTrk_isolFracWide))
            return false;
        if (!TauCmp::cmp(m_seedTrk_etHadAtEMScale,rhs.m_seedTrk_etHadAtEMScale))
            return false;
        if (!TauCmp::cmp(m_seedTrk_etEMAtEMScale,rhs.m_seedTrk_etEMAtEMScale))
            return false;
        if (!TauCmp::cmp(m_seedTrk_etEMCL,rhs.m_seedTrk_etEMCL))
            return false;
        if (!TauCmp::cmp(m_seedTrk_etChrgEM,rhs.m_seedTrk_etChrgEM))
            return false;
        if (!TauCmp::cmp(m_seedTrk_etNeuEM,rhs.m_seedTrk_etNeuEM))
            return false;
        if (!TauCmp::cmp(m_seedTrk_etResNeuEM,rhs.m_seedTrk_etResNeuEM))
            return false;
        if (!TauCmp::cmp(m_seedTrk_hadLeakEt,rhs.m_seedTrk_hadLeakEt))
            return false;
        if (!TauCmp::cmp(m_seedTrk_sumEMCellEtOverLeadTrkPt,rhs.m_seedTrk_sumEMCellEtOverLeadTrkPt))
            return false;
        if (!TauCmp::cmp(m_seedTrk_secMaxStripEt,rhs.m_seedTrk_secMaxStripEt))
            return false;
        if (!TauCmp::cmp(m_seedTrk_stripWidth2,rhs.m_seedTrk_stripWidth2))
            return false;
        if (!TauCmp::cmp(m_seedTrk_etIsolEM,rhs.m_seedTrk_etIsolEM))
            return false;
        if (!TauCmp::cmp(m_seedTrk_etIsolHad,rhs.m_seedTrk_etIsolHad))
            return false;
        if (!TauCmp::cmp(m_seedTrk_etChrgHad,rhs.m_seedTrk_etChrgHad))
            return false;

        //integer comparisons
        if (m_seedCalo_nStrip!=rhs.m_seedCalo_nStrip)
            return false;
        if (m_seedTrk_nStrip!=rhs.m_seedTrk_nStrip)
            return false;
        if (m_seedTrk_nOtherCoreTrk!=rhs.m_seedTrk_nOtherCoreTrk)
            return false;
        if (m_seedTrk_nIsolTrk!=rhs.m_seedTrk_nIsolTrk)
            return false;

        //Vector comparisons
        if (!TauCmp::vcmp(m_seedTrk_etChrgEM01Trk,rhs.m_seedTrk_etChrgEM01Trk))
            return false;	
        if (!TauCmp::vcmp(m_seedTrk_etResChrgEMTrk,
                    rhs.m_seedTrk_etResChrgEMTrk))
            return false;

        //Other comparisons
        if (!(m_looseTrk==rhs.m_looseTrk))
            return false;
        if (!(m_looseConvTrk==rhs.m_looseConvTrk))
            return false;
        if (!(m_seedCalo_wideTrk==rhs.m_seedCalo_wideTrk))
            return false;
        if (!(m_seedTrk_wideTrk==rhs.m_seedTrk_wideTrk))
            return false;      
        if (!(m_otherTrk==rhs.m_otherTrk))
            return false;    
        if (!(m_cellEM012Cluster==rhs.m_cellEM012Cluster))
            return false;
        if (!(m_sumPi0Vec==rhs.m_sumPi0Vec))
            return false;
        if (!(m_pi0==rhs.m_pi0))
            return false;
        //Check that either both or none are there
        if ( ( (m_secVtx==0) && (rhs.m_secVtx!=0) ) ||
                ( (m_secVtx!=0) && (rhs.m_secVtx==0) ) )	    
            return false; 
        //Use dump to compare if both vertices are there
        if ( (m_secVtx!=0) && (rhs.m_secVtx!=0)) 
            if((m_secVtx->position()-rhs.m_secVtx->position()).mag()>1e-6)
                return false;	
        //Electron LLH ID variables 
        if (!(m_Ele_E237E277==rhs.m_Ele_E237E277))
            return false;
        if (!(m_Ele_PresamplerFraction==rhs.m_Ele_PresamplerFraction))
            return false;
        if (!(m_Ele_ECALFirstFraction==rhs.m_Ele_ECALFirstFraction))
            return false;

        //topocluster ID variables
        if (!(m_numCells==rhs.m_numCells))
            return false;
        if (!(m_numTopoClusters==rhs.m_numTopoClusters))
            return false;
        if (!(m_numEffTopoClusters==rhs.m_numEffTopoClusters))
            return false;
        if (!(m_topoInvMass==rhs.m_topoInvMass))
            return false;
        if (!(m_effTopoInvMass==rhs.m_effTopoInvMass))
            return false;
        if (!(m_topoMeanDeltaR==rhs.m_topoMeanDeltaR))
            return false;
        if (!(m_effTopoMeanDeltaR==rhs.m_effTopoMeanDeltaR))
            return false;

        // Energy scale
        if (m_EM_TES_scale != rhs.m_EM_TES_scale)
            return false;
        if (m_LC_TES_precalib != rhs.m_LC_TES_precalib)
            return false;
	      if(m_cellBasedEnergyRing1 != rhs.m_cellBasedEnergyRing1)
      		return false;
      	if(m_cellBasedEnergyRing2 != rhs.m_cellBasedEnergyRing2)
      		return false;
      	if(m_cellBasedEnergyRing3 != rhs.m_cellBasedEnergyRing3)
      		return false;
      	if(m_cellBasedEnergyRing4 != rhs.m_cellBasedEnergyRing4)
      		return false;
      	if(m_cellBasedEnergyRing5 != rhs.m_cellBasedEnergyRing5)
      		return false;
      	if(m_cellBasedEnergyRing6 != rhs.m_cellBasedEnergyRing6)
      		return false;
      	if(m_cellBasedEnergyRing7 != rhs.m_cellBasedEnergyRing7)
      		return false;
        if(m_TRT_NHT_OVER_NLT != rhs.m_TRT_NHT_OVER_NLT)
          return false;

        // Tau Jet Vertex Fraction
        if(m_TauJetVtxFraction != rhs.m_TauJetVtxFraction)
          return false;

        //track filter and pi0 cone
        if(m_TrackFilterPass != rhs.m_TrackFilterPass)
          return false;
        if(m_TrackFilterProngs != rhs.m_TrackFilterProngs)
          return false;
        if(m_TrackFilterQuality != rhs.m_TrackFilterQuality)
          return false;
        if(m_Pi0ConeDR != rhs.m_Pi0ConeDR)
          return false;

        return true;
    }

    //Class name
    std::string TauCommonDetails::s_className = ClassName<TauCommonDetails>::name();

    const std::string& TauCommonDetails::className() const
    {
        return s_className;
    }

} //Namespace analysis
