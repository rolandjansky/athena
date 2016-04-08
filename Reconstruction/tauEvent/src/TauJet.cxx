/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///@file TauJet.cxx
///
///Implementation of tau jet transient class
///
///Package: Reconstruction/tauEvent
///
///@author Thomas Burgess <tburgess-at-cern.ch>
///@author Kyle Cranmer
///@author Michael Heldmann
///@author Lukasz Janyst
///@author Srini Rajagopalan
///@author Elzbieta Richter-Was
///
/// Created 2007-01-13.
///
/// $Id: TauJet.cxx,v 1.30 2009/05/14 14:33:25 tburgess Exp $


#include "VxVertex/RecVertex.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauDetails.h"
#include "tauEvent/Tau1P3PDetails.h"
#include "tauEvent/Tau1P3PExtraDetails.h"
#include "tauEvent/TauRecDetails.h"
#include "tauEvent/TauRecExtraDetails.h"
#include "tauEvent/TauCommonDetails.h"
#include "tauEvent/TauCommonExtraDetails.h"
#include "tauEvent/TauDetailsContainer.h"



namespace Analysis
{
    // Default constructor - for persistency
    TauJet :: TauJet():
        IAthenaBarCode(),
        INavigable (),
        I4Momentum (),
        INavigable4Momentum (),
        TauJetImpl_t(),
        m_author(TauJetParameters :: unknown),
        m_numberOfTracks(0),
        m_tauID(0),
        m_roiWord(0),
        m_hasMergedToCommonDetails(false)
    {


    }

    // Constructor with author
    TauJet :: TauJet(TauJetParameters :: Author author):
        IAthenaBarCode(),
        INavigable (),
        I4Momentum (),
        INavigable4Momentum (),
        TauJetImpl_t(),
        m_author(author),
        m_numberOfTracks(0),
        m_tauID(0),
        m_roiWord(0),
        m_hasMergedToCommonDetails(false)
    {

    }

    // Copy constructor
    TauJet :: TauJet(const TauJet &rhs):
        IAthenaBarCode(rhs),
        INavigable (rhs),
        I4Momentum (rhs),
        INavigable4Momentum (rhs),
        TauJetImpl_t(rhs),
        m_tracks(rhs.m_tracks),
        m_conversionTracks(rhs.m_conversionTracks),
        m_seedCalo_tracks(rhs.m_seedCalo_tracks),
        m_seedTrk_tracks(rhs.m_seedTrk_tracks),
        m_tauDetails(rhs.m_tauDetails),
        m_cluster(rhs.m_cluster),
        m_cellCluster(rhs.m_cellCluster),
        m_jet(rhs.m_jet),
        m_author(rhs.m_author),
        m_numberOfTracks(rhs.m_numberOfTracks),
        m_tauID(0),
        m_roiWord(rhs.m_roiWord),
        m_hasMergedToCommonDetails(rhs.m_hasMergedToCommonDetails),
        m_tauHLVStorage(rhs.m_tauHLVStorage)
    {
        if(rhs.m_tauID) {
            m_tauID = new TauPID(*rhs.m_tauID);
        }

    }

    // Assignment operator
    TauJet &TauJet :: operator =(const TauJet &rhs)
    {
        if(this != &rhs) {
            IAthenaBarCode::operator=(rhs);
            INavigable::operator=(rhs);
            I4Momentum::operator=(rhs);
            INavigable4Momentum::operator=(rhs);
            TauJetImpl_t::operator=(rhs);
            m_tracks=rhs.m_tracks;
            m_conversionTracks=rhs.m_conversionTracks;
            m_seedCalo_tracks=rhs.m_seedCalo_tracks;
            m_seedTrk_tracks=rhs.m_seedTrk_tracks;
            m_tauDetails=rhs.m_tauDetails;
            m_cluster=rhs.m_cluster;
            m_cellCluster=rhs.m_cellCluster;
            m_jet=rhs.m_jet;
            m_author=rhs.m_author;
            m_numberOfTracks=rhs.m_numberOfTracks;
            if(rhs.m_tauID) {
                delete m_tauID;
                m_tauID = new TauPID(*rhs.m_tauID);
            }
            m_roiWord=rhs.m_roiWord;
            m_hasMergedToCommonDetails=rhs.m_hasMergedToCommonDetails;
            m_tauHLVStorage=rhs.m_tauHLVStorage;
        }
        return *this;
    }

    // Equality operator
    bool TauJet::operator==( const TauJet& rhs ) const
    {
        if (m_tracks!=rhs.m_tracks) return false;
        if (m_conversionTracks!=rhs.m_conversionTracks) return false;
        if (m_seedCalo_tracks!=rhs.m_seedCalo_tracks) return false;
        if (m_seedTrk_tracks!=rhs.m_seedTrk_tracks) return false;
        if (m_tauDetails!=rhs.m_tauDetails) return false;
        if (m_cluster!=rhs.m_cluster) return false;
        if (m_cellCluster!=rhs.m_cellCluster) return false;
        if (m_jet!=rhs.m_jet) return false;
        if (m_author!=rhs.m_author) return false;
        if (m_numberOfTracks!=rhs.m_numberOfTracks) return false;
        //Compare if pointers are valid
        if ( (m_tauID!=0) && (rhs.m_tauID!=0) ) {
            if ((*m_tauID) != (*rhs.m_tauID)) return false;
        } else if ( ( (m_tauID!=0) && (rhs.m_tauID==0) ) || 
                ( (m_tauID==0) && (rhs.m_tauID!=0) ) )
        {
            //false if only one of the pointers is set
            return false;
        }
        if (m_roiWord!=rhs.m_roiWord) return false;
        if (m_hasMergedToCommonDetails!=rhs.m_hasMergedToCommonDetails)  return false;	
        
        //ToDo:: cmp for tauHLVStorage

        return true;
    }

    // Destructor
    TauJet :: ~TauJet()
    {
        delete m_tauID; m_tauID = 0;
    }

    std::string TauJet::toString() const
    {
        std::stringstream s;
        printOn(s);
        return s.str();
    }

    void TauJet::MergeToCommonDetails(
            TauDetailsContainer* tauDetailsContainer,
            TauDetailsContainer* tauExtraDetailsContainer)
    {
        //Skip merge if merge allready called 
        if ( m_hasMergedToCommonDetails ) {
            return;		
        }
        else {
            m_hasMergedToCommonDetails = true;
        }

        //Copy old details to new common details
        TauCommonDetails* tauCommonDetails=0;
        const Tau1P3PDetails* tau1P3PDetails = 
            details<Tau1P3PDetails>();
        const TauRecDetails* tauRecDetails = 
            details<TauRecDetails>();
        //Check that we have a details container
        if ( tauDetailsContainer != 0 )  {
            //Check that we have at least one detail
            if( (tau1P3PDetails != 0) || (tauRecDetails != 0) ) {
                //Create new common details
                tauCommonDetails=new TauCommonDetails();
                CopyToCommonDetails(tau1P3PDetails,tauCommonDetails);
                CopyToCommonDetails(tauRecDetails,tauCommonDetails);
            }
        }

        //Copy old extra details to new common extra details
        TauCommonExtraDetails* tauCommonExtraDetails = 0;
        const Tau1P3PExtraDetails* tau1P3PExtraDetails =
            details<Tau1P3PExtraDetails>();
        const TauRecExtraDetails* tauRecExtraDetails =
            details<TauRecExtraDetails>();	    
        //Check that we have an extra details container
        if  (tauExtraDetailsContainer!=0) { 
            //Check that we have at least one extra detail
            if( ( tau1P3PExtraDetails != 0) || (tauRecExtraDetails != 0)) {
                //Create new common extra details
                tauCommonExtraDetails=new TauCommonExtraDetails();
                CopyToCommonDetails(tau1P3PExtraDetails,tauCommonExtraDetails);
                CopyToCommonDetails(tauRecExtraDetails,tauCommonExtraDetails);
            }
        }

        //Delete all existing details links
        while(!m_tauDetails.empty())
            m_tauDetails.erase(m_tauDetails.begin());

        //Add new details (if there is a container and a detail)
        if( (tauDetailsContainer!=0) && (tauCommonDetails!=0)) {
            tauDetailsContainer->push_back(tauCommonDetails);
            addDetails(tauDetailsContainer,tauCommonDetails);
        }

        //Add new extra details (if there is a container and a detail)
        if((tauExtraDetailsContainer!=0) && (tauCommonExtraDetails!=0)) {
            tauExtraDetailsContainer->push_back(tauCommonExtraDetails);
            addDetails(tauExtraDetailsContainer,tauCommonExtraDetails);
        }
    }

    // Fill navigation token
    void TauJet :: fillToken(INavigationToken &token) const
    {
        // Check if one requested for tracks
        NavigationToken<Rec::TrackParticle> *trackToken =
            dynamic_cast<NavigationToken<Rec::TrackParticle>*>(&token);
        if(trackToken) {
            for(unsigned int i = 0; i <= numTrack(); ++i) {
                const Rec::TrackParticle *track = this->track(i);
                if(track)
                    trackToken->setObject(track);
            }
            return;
        }
        // Check if one requested for tracks with alternative token
        NavigationToken<Rec::TrackParticle,double> *altTrackToken =
            dynamic_cast<NavigationToken<Rec::TrackParticle,double>*>(&token);
        if(altTrackToken) {
            for(unsigned int i = 0; i <= numTrack(); ++i) {
                const Rec::TrackParticle *track = this->track(i);
                if(track)
                    altTrackToken->setObject(track);
            }
            return;
        }

        // Check if one requested for Jet
        NavigationToken<Jet> *jetToken =
            dynamic_cast<NavigationToken<Jet>*>(&token);
        const Jet *jet     = this->jet();
        if(jetToken) {
            if(jet != 0)
                jetToken->setObject(jet);
            return;
        }
        // Check if one requested for Jet with alternative token
        NavigationToken<Jet, double> *altJetToken =
            dynamic_cast< NavigationToken<Jet, double>* >(&token);
        if(altJetToken != 0) {
            if(jet != 0)
                altJetToken->setObject(jet);
            return;
        }
        // The token is not supported by this object, passing token
        // to composite consitutents
        if(jet)
            jet->fillToken(token);

        // Try for generic type requests
        for(unsigned int i = 0; i <= this->numTrack(); ++i) {
            const Rec::TrackParticle *track = this->track(i);
            if(track)
                token.trySetObject(track);
        }
    }

    // Fill navigation token
    void TauJet :: fillToken(INavigationToken &token,
            const boost::any &weight) const
    {
        // Check if one requested for tracks
        NavigationToken<Rec::TrackParticle> *trackToken =
            dynamic_cast<NavigationToken<Rec::TrackParticle>*>(&token);
        if(trackToken) {
            for(unsigned int i = 0; i <= numTrack(); ++i) {
                const Rec::TrackParticle *track = this->track(i);
                if(track)
                    trackToken->setObject(track);
            }
            return;
        }
        // Check if one requested for tracks with alternative token
        NavigationToken<Rec::TrackParticle,double> *altTrackToken =
            dynamic_cast<NavigationToken<Rec::TrackParticle,double>*>(&token);
        if(altTrackToken) {
            for(unsigned int i = 0; i <= numTrack(); ++i) {
                const Rec::TrackParticle *track = this->track(i);
                if(track)
                    altTrackToken->setObject(
                            track, boost :: any_cast<double>(weight));
            }
            return;
        }

        NavigationToken<Jet> *jetToken =
            dynamic_cast<NavigationToken<Jet>*>(&token);
        const Jet *jet = this->jet();
        if(jetToken) {
            if(jet != 0)
                jetToken->setObject(jet);
            return;
        }
        // Check if one requested for Jet with alternative token
        NavigationToken<Jet, double> *altJetToken =
            dynamic_cast< NavigationToken<Jet, double>* >(&token);
        if(altJetToken != 0) {
            if(jet != 0)
                altJetToken->setObject(
                        jet, boost :: any_cast<double>(weight));
            return;
        }
        // The token is not supported by this object, passing token
        // to composite consitutents
        if(jet)
            jet->fillToken(token);

        // Try for generic type requests
        for(unsigned int i = 0; i <= this->numTrack(); ++i) {
            const Rec::TrackParticle *track = this->track(i);
            if(track)
                token.trySetObject(track, weight);
        }
    }

    // Parameter accessor for analysis code compatibility
    double TauJet :: parameter(TauJetParameters :: Detail) const
    {
        return TauDetails::DEFAULT;
    }

    void TauJet::removeDetails(int i)
    {
        if(i >= 0 && i <(int)m_tauDetails.size())
            m_tauDetails.erase(m_tauDetails.begin() + i);
        return;
    }

    void TauJet::removeDetails(
            const std::string &name, const std::string& className /*= ""*/ )
    {
        ElementLinkVector<TauDetailsContainer>::iterator iter
            = m_tauDetails.begin();
        while(iter != m_tauDetails.end()) {
            const unsigned int i = iter - m_tauDetails.begin();
            const TauDetails* detail = *m_tauDetails[i];
            if(m_tauDetails.elementDataID(i) == name
                    &&(className.empty() || detail->className() == className))
                iter = m_tauDetails.erase(iter);
            else ++iter;
        }
        return;
    }

    const TauDetails* TauJet::detail(const std::string& className,
            const std::string& name /*= ""*/)const
    {
        size_t n = m_tauDetails.size();
        for(size_t i = 0; i < n; i++) {
            const TauDetails* detail = *m_tauDetails[i];
            if(detail->className() == className &&
                    (name.empty() || m_tauDetails.elementDataID(i) == name))
                return detail;
        }
        return 0;
    }

    const TauDetails* TauJet::detail(int i) const
    {
        if(i >= 0 && i <(int)m_tauDetails.size())
            return *m_tauDetails[i];
        return 0;
    }

    const ElementLink<TauDetailsContainer>
        TauJet::detailElementLink(int i) const
        {
            if(i >= 0 && i <(int)m_tauDetails.size())
                return m_tauDetails[i];
            return ElementLink<TauDetailsContainer>();
        }

    std::string TauJet::detailName(int i) const
    {
        if(i >= 0 && i <(int)m_tauDetails.size())
            return m_tauDetails.elementDataID(i);
        return "";
    }

    int TauJet::nDetails() const
    {
        return m_tauDetails.size();
    }

    void TauJet::CleanDetailsForAOD()
    {
        ElementLinkVector<TauDetailsContainer>::iterator idetail
            = m_tauDetails.begin();
        while(idetail != m_tauDetails.end()) {
            // counter
            const unsigned int i = idetail - m_tauDetails.begin();

            //Remove invalid details
            if (!(*idetail).isValid()) 	{ 
                m_tauDetails.erase(idetail);
                continue;
            }
            const TauDetails* detail =* m_tauDetails[i];
            //Remove 0 details
            if (detail==0) {
                m_tauDetails.erase(idetail);
                continue;
            }
            //Remove any extra detail
            if ( (dynamic_cast<const TauCommonExtraDetails*>(detail)!=0) ||
                    (dynamic_cast<const TauRecExtraDetails*>(detail)!=0) ||
                    (dynamic_cast<const Tau1P3PExtraDetails*>(detail)!=0) ) 
                m_tauDetails.erase(idetail);
            ++idetail;
        }
    }

    void TauJet::CopyToCommonDetails(
            const Tau1P3PDetails *tau1P3PDetails,
            TauCommonDetails *tauCommonDetails)
    {
        if((tau1P3PDetails==0) ||(tauCommonDetails==0)) return;
        tauCommonDetails->m_ipZ0SinThetaSigLeadTrk =
            tau1P3PDetails->m_z0SinThetaSig;
        tauCommonDetails->m_ipSigLeadTrk = tau1P3PDetails->m_signD0Trk3P;
        tauCommonDetails->m_massTrkSys = tau1P3PDetails->m_massTrk3P;
        tauCommonDetails->m_trkWidth2 = tau1P3PDetails->m_rWidth2Trk3P;
        tauCommonDetails->m_trFlightPathSig =
            tau1P3PDetails->m_trFlightPathSig;
        tauCommonDetails->m_etEflow = tau1P3PDetails->m_etEflow;
        tauCommonDetails->m_seedTrk_nStrip = tau1P3PDetails->m_numStripCells;
        tauCommonDetails->m_seedTrk_stripWidth2 =
            tau1P3PDetails->m_stripWidth2;
        tauCommonDetails->m_seedTrk_EMRadius = tau1P3PDetails->m_emRadius;
        tauCommonDetails->m_seedTrk_isolFrac = tau1P3PDetails->m_ET12Frac;
        tauCommonDetails->m_seedTrk_etIsolHad = tau1P3PDetails->m_etIsolHAD;
        tauCommonDetails->m_seedTrk_etIsolEM = tau1P3PDetails->m_etIsolEM;
        tauCommonDetails->m_seedTrk_etChrgHad = tau1P3PDetails->m_etChrgHAD;
        tauCommonDetails->m_seedTrk_nOtherCoreTrk =
            tau1P3PDetails->m_nAssocTracksCore;
        tauCommonDetails->m_seedTrk_nIsolTrk =
            tau1P3PDetails->m_nAssocTracksIsol;
        tauCommonDetails->m_seedTrk_etChrgHadOverSumTrkPt =
            tau1P3PDetails->m_etChrgHADoverPttot;
        tauCommonDetails->m_seedTrk_isolFracWide =
            tau1P3PDetails->m_etIsolFrac;
        tauCommonDetails->m_seedTrk_etHadAtEMScale =
            tau1P3PDetails->m_etHadAtEMScale;
        tauCommonDetails->m_seedTrk_etEMAtEMScale =
            tau1P3PDetails->m_etEMAtEMScale;
        tauCommonDetails->m_seedTrk_etEMCL = tau1P3PDetails->m_etEMCL;
        tauCommonDetails->m_seedTrk_etChrgEM = tau1P3PDetails->m_etChrgEM;
        tauCommonDetails->m_seedTrk_etNeuEM = tau1P3PDetails->m_etNeuEM;
        tauCommonDetails->m_seedTrk_etResNeuEM = tau1P3PDetails->m_etResNeuEM;
        tauCommonDetails->m_seedTrk_sumEMCellEtOverLeadTrkPt =
            tau1P3PDetails->m_sumEtCellsLArOverLeadTrackPt;
        tauCommonDetails->m_seedTrk_hadLeakEt =
            tau1P3PDetails->m_hadronicLeak;
        tauCommonDetails->m_seedTrk_secMaxStripEt =
            tau1P3PDetails->m_secondaryMax;

        tauCommonDetails->m_cellEM012Cluster=tau1P3PDetails->m_cellEM012Cluster;
        tauCommonDetails->m_sumPi0Vec=tau1P3PDetails->m_sumEM;
        if(tau1P3PDetails->m_secVertex)  {
            delete tauCommonDetails->m_secVtx;
            tauCommonDetails->m_secVtx=new Trk::RecVertex(
                    *(tau1P3PDetails->m_secVertex));
        }
        tauCommonDetails->m_pi0=tau1P3PDetails->m_pi0;
        tauCommonDetails->m_seedTrk_etChrgEM01Trk=
            tau1P3PDetails->m_etChrgEM01Trk;
        tauCommonDetails->m_seedTrk_etResChrgEMTrk=
            tau1P3PDetails->m_etResChrgEMTrk;
    }

    void TauJet::CopyToCommonDetails(
            const TauRecDetails *tauRecDetails,
            TauCommonDetails *tauCommonDetails)
    {
        if((tauRecDetails==0) ||(tauCommonDetails==0)) return;

        tauCommonDetails->m_ipSigLeadLooseTrk =
            tauRecDetails->m_ipSigLeadTrack;
        tauCommonDetails->m_etOverPtLeadLooseTrk =
            tauRecDetails->m_etOverPtLeadTrack;
        tauCommonDetails->m_leadLooseTrkPt =
            tauRecDetails->m_leadingTrackPT;
        tauCommonDetails->m_chrgLooseTrk =
            tauRecDetails->m_chargeLooseTracks;
        tauCommonDetails->m_seedCalo_nIsolLooseTrk =
            tauRecDetails->m_nTracksdrdR;
        tauCommonDetails->m_seedCalo_EMRadius = tauRecDetails->m_emRadius;
        tauCommonDetails->m_seedCalo_hadRadius = tauRecDetails->m_hadRadius;
        tauCommonDetails->m_seedCalo_etEMAtEMScale =
            tauRecDetails->m_sumEmCellEt;
        tauCommonDetails->m_seedCalo_etHadAtEMScale =
            tauRecDetails->m_sumHadCellEt;
        tauCommonDetails->m_seedCalo_isolFrac = tauRecDetails->m_ET12Frac;
        tauCommonDetails->m_seedCalo_centFrac =
            tauRecDetails->m_centralityFraction;
        tauCommonDetails->m_seedCalo_stripWidth2 =
            tauRecDetails->m_stripWidth2;
        tauCommonDetails->m_seedCalo_nStrip = tauRecDetails->m_numStripCells;
        tauCommonDetails->m_seedCalo_etEMCalib = tauRecDetails->m_etEMCalib;
        tauCommonDetails->m_seedCalo_etHadCalib = tauRecDetails->m_etHadCalib;
        tauCommonDetails->m_seedCalo_eta = tauRecDetails->m_etaCalo;
        tauCommonDetails->m_seedCalo_phi = tauRecDetails->m_phiCalo;

        //Copy only if we don't have a tau1P3P seed
        if(!hasAuthor(TauJetParameters::tau1P3P)){
            tauCommonDetails->m_sumPi0Vec=tauRecDetails->m_sumEM;
            tauCommonDetails->m_trFlightPathSig =
                tauRecDetails->m_trFlightPathSig;
        }

        if(tauRecDetails->m_secVertex) {
            delete tauCommonDetails->m_secVtx;
            tauCommonDetails->m_secVtx =
                new Trk::RecVertex(*(tauRecDetails->m_secVertex));
        }
        tauCommonDetails->m_looseTrk=
            tauRecDetails->m_looseTracks;
    }

    void TauJet::CopyToCommonDetails(
            const Tau1P3PExtraDetails *tau1P3PExtraDetails,
            TauCommonExtraDetails *tauCommonExtraDetails)
    {
        if((tau1P3PExtraDetails==0) ||(tauCommonExtraDetails==0)) return;
        tauCommonExtraDetails->setSumPtTrk(tau1P3PExtraDetails->m_sumPTTracks);
        tauCommonExtraDetails->m_closestEtaTrkVertCell=
            tau1P3PExtraDetails->m_closestEtaTrkVertCell;
        tauCommonExtraDetails->m_closestEtaTrkCell=
            tau1P3PExtraDetails->m_closestEtaTrkCell;
        tauCommonExtraDetails->m_closestPhiTrkVertCell=
            tau1P3PExtraDetails->m_closestPhiTrkVertCell;
        tauCommonExtraDetails->m_closestPhiTrkCell=
            tau1P3PExtraDetails->m_closestPhiTrkCell;
        tauCommonExtraDetails->m_etaTrkCaloSamp=
            tau1P3PExtraDetails->m_etaTrackCaloSamp;
        tauCommonExtraDetails->m_phiTrkCaloSamp=
            tau1P3PExtraDetails->m_phiTrackCaloSamp;
    }

    void TauJet::CopyToCommonDetails(
            const TauRecExtraDetails *tauRecExtraDetails,
            TauCommonExtraDetails *tauCommonExtraDetails)
    {
        if((tauRecExtraDetails==0) ||(tauCommonExtraDetails==0)) return;
        tauCommonExtraDetails->m_seedCalo_nEMCell=
            tauRecExtraDetails->m_numEMCells;
        tauCommonExtraDetails->m_seedCalo_stripEt=
            tauRecExtraDetails->m_stripET;
        tauCommonExtraDetails->m_seedCalo_EMCentFrac=
            tauRecExtraDetails->m_emCentralityFraction;
        tauCommonExtraDetails->m_seedCalo_sumCellEnergy=
            tauRecExtraDetails->m_sumCellE;
        tauCommonExtraDetails->m_seedCalo_sumEMCellEnergy=
            tauRecExtraDetails->m_sumEMCellE;
        tauCommonExtraDetails->m_sumPtLooseTrk=
            tauRecExtraDetails->m_sumPTTracks;
    }

    void TauJet::clearHLV()
    {
      m_tauHLVStorage.clear();
    }

} //Namespace
