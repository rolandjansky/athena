/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///@file tauAnalysisHelperObject.h  DEPRECATED DO NOT USE 
///
/// tau analysis helper object data class
///
/// Package: Reconstruction/TauEvent
///
///@author Thomas Burgess <thomas.burgess[]cern.ch>
///@author Michael Heldmann
///
/// Created Oct 12, 2004
///
/// $Id: tauAnalysisHelperObject.h,v 1.16 2008-11-28 09:52:25 slai Exp $


#ifndef TAUREC_TAUANALYSISHELPEROBJECT_H
#define TAUREC_TAUANALYSISHELPEROBJECT_H

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

#include "EventKernel/I4Momentum.h"

#include "FourMom/P4EEtaPhiM.h"

#include <vector>
#include <sys/types.h>

/// DEPRECATED DO NOT USE - MIGRATE TO TauCommonDetails
///
/// tau analysis helper object data class
///
/// object containing additional information (mostly mc truth) 
/// useful for performance analysis, should be superceded by the trueTauObject
/// Package: Reconstruction/TauEvent
class tauAnalysisHelperObject
{
    public:
        // element links
        typedef ElementLink<Rec::TrackParticleContainer>           tauTrack_type;
        typedef ElementLinkVector<Rec::TrackParticleContainer>     tauTrackColl_type;
        typedef tauTrackColl_type::iterator                        tauTrackColl_iterator;

        typedef ElementLink<CaloCellContainer>                     tauCell_type;
        typedef ElementLinkVector<CaloCellContainer>               tauCellColl_type;
        typedef tauCellColl_type::iterator                         tauCellColl_iterator;

        tauAnalysisHelperObject() = default;
        ~tauAnalysisHelperObject() = default;

        /** @name Analysis Helper
         *  set by tauAnalysisHelper::execute()
         */
        //@{
        //! gives the type of jet, which is the pdgid of the highest pT parton (1-6, 15, 22) from history within 0.4 (jobOptions)
        int jettype() const                                 { return m_jettype; }

        //! gives the decaymode of the associated true tau, 1-10 (for numbering convention see ??????)
        double decmode() const                              { return m_decmode; };


        //   -- set Analysis Helper --
        void setJettype( const int jettype )                { m_jettype = jettype; }
        void setDecmode( const int decmode )                { m_decmode = decmode; };
        //@}


        /** @name FastShower Analysis Helper
         *  set by tauFSAnalysisHelper::execute()
         */
        //@{
        //! gives the EMRadius calculated on (pseudo-) triggertower granularity (for comparison with FASTSHOWER)
        double TowEMRadius() const	                  	    { return m_TowEMRadius; }

        //! gives the Isolation calculated on (pseudo-) triggertower granularity (for comparison with FASTSHOWER)
        double TowIsoFrac() const		                        { return m_TowET12Frac; }

        //! gives the EMRadius for radius i ( i = 0.1, 0.2, 0.3, 0.4, 0.5, 0.7, 1.0 )
        double EMRadii( const int idx ) const               { return m_emradii[idx]; };

        //! gives the HADRadius for radius i ( i = 0.1, 0.2, 0.3, 0.4, 0.5, 0.7, 1.0 )
        double HADRadii( const int idx ) const              { return m_hadradii[idx]; };


        //   -- set FastShower Analysis Helper --
        void setTowEMRadius(const double rad)	              { m_TowEMRadius = rad; }
        void setTowIsoFrac(const double frac)	              { m_TowET12Frac = frac; }
        void setEMRadii( const int idx, const double rad )  { m_emradii[idx] = rad; };
        void setHADRadii( const int idx, const double rad ) { m_hadradii[idx] = rad; };
        //@}


        /** @name Impact Parameter Analysis Helper
         *  set by tauAnalysisHelper::execute()
         */
        //@{
        //! gives the impact parameter of track track
        double RawImpactParameter( const int track ) const { return m_RawImpactParameter[track]; }
        double ImpactParameter( const int track ) const	{ return m_ImpactParameter[track]; }
        double SignedImpactParameter( const int track ) const	{ return m_SignedImpactParameter[track]; }
        double ImpactParameterSignificance( const int track ) const	{ return m_ImpactParameterSignificance[track]; }
        double SignedImpactParameterSignificance( const int track ) const	{ return m_SignedImpactParameterSignificance[track]; }


        //   -- set Impact Parameter Analysis Helper --
        void setImpactParameter( const int track, const double ip ) { if ( track < 10 ) m_ImpactParameter[track] = ip; }
        void setRawImpactParameter( const int track, const double ip ) { if ( track < 10 ) m_RawImpactParameter[track] = ip; }
        void setSignedImpactParameter( const int track, const double ip ) { if ( track < 10 ) m_SignedImpactParameter[track] = ip; }
        void setImpactParameterSignificance( const int track, const double ip ) { if ( track < 10 ) m_ImpactParameterSignificance[track] = ip; }
        void setSignedImpactParameterSignificance( const int track, const double ip ) { if ( track < 10 ) m_SignedImpactParameterSignificance[track] = ip; }
        //@}


        /** @name D0 Analysis Helper
         *  set by tauD0AnalysisHelper::execute()
         */
        //@{
        //! gives the profile prf=(E_T^1+E_T^1)/E_T
        double d0prf() { return m_d0prf; };

        //! gives the isolation iso= (E(0.5)-E(0.3))/E(0.3)
        double d0iso() { return m_d0iso; };

        //! gives the isolation iso= (ET(0.5)-ET(0.3))/ET(0.3)
        double d0isoet() { return m_d0isoet; };

        //! gives the track isolation ettr=sum(pT of non tau tracks in 0.5)
        double d0ettr() { return m_d0ettr; };

        //! gives the calo energy in em (em0 em1 em2 em3) only
        double d0etem() { return m_d0etem; };

        //! gives the calo energy in em2
        double d0etem2() { return m_d0etem2; };


        //! gives the calo energy in the em subcluster only
        double d0emclet() { return m_d0emclet; };

        //! gives the eta in the em subcluster only
        double d0emcleta() { return m_d0emcleta; };

        //! gives the phi in the em subcluster only
        double d0emclphi() { return m_d0emclphi; };


        //! gives the calo energy in the had calo only
        double d0hadet() { return m_d0hadet; };

        //! gives the eta in the had calo only
        double d0hadeta() { return m_d0hadeta; };

        //! gives the phi in the had calo only
        double d0hadphi() { return m_d0hadphi; };


        //! gives the calo energy in dR < 0.5
        double d0et05() { return m_d0et05; };

        //! gives the calo eta in dR < 0.5
        double d0eta05() { return m_d0eta05; };

        //! gives the calo phi in dR < 0.5
        double d0phi05() { return m_d0phi05; };


        //! gives the tau type : 1=tau->pi- nu, 2=tau->pi- xpi0 nu, 3=tau->pi- pi+ pi- xpi0 nu
        double d0type() { return m_d0type; };

        //! gives Delta R between the 1 track and the calo cluster
        double d0deltaR1() { return m_d0deltaR1; };

        //! gives E_T/sum(p_T) 
        double d0eTosumpT() { return m_d0eTosumpT; };


        //! gives Delta R between the 1 track and the hadronic part of the calo cluster
        double d0deltaR1had() { return m_d0deltaR1had; };

        //! gives fraction in the third layer em3iso=E_T(EM3)^cluster / E_T(EM3)^tot
        double d0em3iso() { return m_d0em3iso; };

        //! gives mass of the tau tracks and the EM3 subcluster
        double d0mtrem3() { return m_d0mtrem3; };


        //! gives Delta R between the 2 track and the calo cluster
        double d0deltaR2() { return m_d0deltaR2; };

        //! gives number of tracks between 10deg and 30deg of the calo cluster
        double d0ntr1030() { return m_d0ntr1030; };


        //! gives the ratio of the transverse energy in the first two layers of the calo to the total transv. e in a 0.5 cone around the cluster
        double d0EM12isof() { return m_d0EM12isof; };

        //! gives root square of em fraction of pT
        double d0e1e2otaupT() { return m_d0e1e2otaupT; };

        //! gives fraction of pT of the not tau tracks (dR<0.5)
        double d0ettro123() { return m_d0ettro123; };

        //! gives pT of track 1 / calo et (dR < 0.5)  = p_T^tautrk/(p_T^tau * isoet )
        double d0ett1oEtiso() { return m_d0ett1oEtiso; };

        //! gives pT of track 1 / calo eT (dR < 0.5)  = p_T^tautrk/(p_T^tau * iso )
        double d0ett1oEtisoet() { return m_d0ett1oEtisoet; };

        //! gives the opening angle between the highest pT track and the corresponding em cluster / sin (theta_cluster), taudalpha*taue1e2 = m_tau
        double d0dalpha() { return m_d0dalpha; }

        //! gives sqrt( E_T^tautrk * E_T^em )
        double d0e1e2() { return m_d0e1e2; };

        //! gives the tau mass
        double d0mtau() { return m_d0dalpha*m_d0e1e2; };

        //! gives the invariant mass of track 1 and track 2
        double d0mtr1tr2() { return m_d0mtr1tr2; };

        //! gives the invariant mass of track 1 and track 2 and track 3
        double d0mtr1tr2tr3() { return m_d0mtr1tr2tr3; };


        //! gives the sum of pt of tau tracks
        double d0sumtaupt() { return m_d0sumtaupt; };

        //! gives the sum of pt of non-tau tracks
        double d0sumnontaupt() { return m_d0sumnontaupt; };

        //! gives the sum of pt of all tracks
        double d0sumpt() { return m_d0sumpt; };

        //! gives tower i
        I4Momentum* d0tower( int i ) { return new P4EEtaPhiM( m_towere[i], m_towereta[i], m_towerphi[i], 0.0 ); };

        //! gives the et of tower i
        double d0toweret( int i ) { return m_towere[i]/cosh( m_towereta[i] ); };

        //! gives the number of towers
        int d0nTowers() { return m_towere.size(); };
        //   -- set D0 variables --

        //! gives the sum of et of all towers
        double d0sumtoweret() { double sumet = 0; for ( int i = 0; i < d0nTowers(); i++ ) sumet += d0toweret(i); return sumet; };

        //! gives the uncal energy in layers up to 24.
        double d0uncaletlayers( int l ) { return m_d0uncaletlayers[l]; };

        void setD0prf( double val ) { m_d0prf = val; };
        void setD0iso( double val ) { m_d0iso = val; };
        void setD0isoet( double val ) { m_d0isoet = val; };
        void setD0ettr( double val ) { m_d0ettr = val; };
        void setD0etem( double val ) { m_d0etem = val; };
        void setD0etem2( double val ) { m_d0etem2 = val; };

        void setD0emclet( double val ) { m_d0emclet = val; };
        void setD0emcleta( double val ) { m_d0emcleta = val; };
        void setD0emclphi( double val ) { m_d0emclphi = val; };

        void setD0hadet( double val ) { m_d0hadet = val; };
        void setD0hadeta( double val ) { m_d0hadeta = val; };
        void setD0hadphi( double val ) { m_d0hadphi = val; };

        void setD0et05( double val ) { m_d0et05 = val; };
        void setD0eta05( double val ) { m_d0eta05 = val; };
        void setD0phi05( double val ) { m_d0phi05 = val; };

        void setD0type( double val  ) { m_d0type = val; };
        void setD0deltaR1( double val  ) { m_d0deltaR1 = val; };
        void setD0eTosumpT( double val  ) { m_d0eTosumpT = val; };

        void setD0deltaRhad( double val  ) { m_d0deltaR1had = val; };
        void setD0em3iso( double val  ) { m_d0em3iso = val; };
        void setD0mtrem3( double val  ) { m_d0mtrem3 = val; };

        void setD0deltaR2( double  val ) { m_d0deltaR2 = val; };
        void setD0ntr1030( double val  ) { m_d0ntr1030 = val; };

        void setD0EM12isof( double val  ) { m_d0EM12isof = val; };
        void setD0e1e2otaupT( double val  ) { m_d0e1e2otaupT = val; };
        void setD0ettro123( double val  ) { m_d0ettro123 = val; };
        void setD0ett1oEtiso( double  val ) { m_d0ett1oEtiso = val; };
        void setD0ett1oEtisoet( double  val ) { m_d0ett1oEtisoet = val; };
        void setD0dalpha( double val  ) { m_d0dalpha = val; }
        void setD0e1e2( double  val ) { m_d0e1e2 = val; };

        void setD0mtr1tr2( double  val ) { m_d0mtr1tr2 = val; };
        void setD0mtr1tr2tr3( double  val ) { m_d0mtr1tr2tr3 = val; };

        void setD0sumtaupt( double  val ) { m_d0sumtaupt = val; };
        void setD0sumnontaupt( double  val ) { m_d0sumnontaupt = val; };
        void setD0sumpt( double  val ) { m_d0sumpt = val; };

        void clearD0towers() {
          m_towere.clear();
          m_towereta.clear();
          m_towerphi.clear();
        }
        void addD0tower( const double e, const double eta, const double phi ) { m_towere.push_back( e ); m_towereta.push_back( eta ); m_towerphi.push_back( phi ); };
        void addD0tower( const I4Momentum *mom ) { m_towere.push_back( mom->e() ); m_towereta.push_back( mom->eta() ); m_towerphi.push_back( mom->phi() ); };

        void setD0uncaletlayers( int l, double val ) { m_d0uncaletlayers[l] = val; };
        //@}


        /** @name D0_emcluster emcluster selected according to the algo from D0 
         *  set by tauD0Helper::execute()
         */
        //@{
        tauCellColl_type&  d0_emcluster() { return m_d0_emcluster; }
        const tauCellColl_type&  d0_emcluster() const { return m_d0_emcluster; }
        int d0_emcluster_nCells() const			           { return m_d0_emcluster.size(); }
        tauCellColl_iterator d0_emcluster_begin()      { return m_d0_emcluster.begin(); }
        tauCellColl_iterator d0_emcluste_end()         { return m_d0_emcluster.end(); }

        const CaloCell* d0_emcluster_cell( uint i) const   { return *m_d0_emcluster[i]; }

        //   -- add cells to emcluster --
        void d0_emcluster_addCell(const CaloCellContainer* cellContainer, const CaloCell* theCell) {
            tauCell_type theLink;
            theLink.toContainedElement( *cellContainer, (CaloCell*) theCell );
            m_d0_emcluster.push_back(theLink);
        }

        //@}
        /** @name D0_05_Tracks (all tracks within dR < 0.5 of the calo cluster) 
         *  set by tauD0Helper::execute()
         */
        //@{
        int d0_05_nTracks() const			                         { return m_d0_05_Tracks.size(); }
        tauTrackColl_type&  d0_05_tracks() { return m_d0_05_Tracks; }
        const tauTrackColl_type&  d0_05_tracks() const { return m_d0_05_Tracks; }
        tauTrackColl_iterator d0_05_track_begin()              { return m_d0_05_Tracks.begin(); }
        tauTrackColl_iterator d0_05_track_end()                { return m_d0_05_Tracks.end(); }

        const Rec::TrackParticle* d0_05_track( uint i) const   { return *m_d0_05_Tracks[i]; }

        //   -- add Tracks --
        void d0_05_addTrack(const Rec::TrackParticleContainer* trackContainer, const Rec::TrackParticle* theTrack) {
            tauTrack_type theLink;
            theLink.toContainedElement( *trackContainer, (Rec::TrackParticle*) theTrack );
            m_d0_05_Tracks.push_back(theLink);
        }
        //@}

        /** @name D0_tau_Tracks (tracks within dR < 0.5, fullfilling tauTrack criteria) 
         *  set by tauD0Helper::execute()
         */
        //@{
        int d0_tau_nTracks() const			                         { return m_d0_tau_Tracks.size(); }
        tauTrackColl_type&  d0_tau_tracks() { return m_d0_tau_Tracks; }
        const tauTrackColl_type&  d0_tau_tracks() const { return m_d0_tau_Tracks; }
        tauTrackColl_iterator d0_tau_track_begin()              { return m_d0_tau_Tracks.begin(); }
        tauTrackColl_iterator d0_tau_track_end()                { return m_d0_tau_Tracks.end(); }

        const Rec::TrackParticle* d0_tau_track( uint i) const   { return *m_d0_tau_Tracks[i]; }

        //   -- add Tracks --
        void d0_tau_addTrack(const Rec::TrackParticleContainer* trackContainer, const Rec::TrackParticle* theTrack) {
            tauTrack_type theLink;
            theLink.toContainedElement( *trackContainer, (Rec::TrackParticle*) theTrack );
            m_d0_tau_Tracks.push_back(theLink);
        }
        //@}

        /** @name D0_nontau_Tracks (tracks within dR < 0.5, fullfilling tauTrack criteria) 
         *  set by tauD0Helper::execute()
         */
        //@{
        int d0_nontau_nTracks() const			                         { return m_d0_nontau_Tracks.size(); }
        tauTrackColl_type&  d0_nontau_tracks() { return m_d0_nontau_Tracks; }
        const tauTrackColl_type&  d0_nontau_tracks() const { return m_d0_nontau_Tracks; }
        tauTrackColl_iterator d0_nontau_track_begin()              { return m_d0_nontau_Tracks.begin(); }
        tauTrackColl_iterator d0_nontau_track_end()                { return m_d0_tau_Tracks.end(); }

        const Rec::TrackParticle* d0_nontau_track( uint i) const   { return *m_d0_nontau_Tracks[i]; }

        //   -- add Tracks --
        void d0_nontau_addTrack(const Rec::TrackParticleContainer* trackContainer, const Rec::TrackParticle* theTrack) {
            tauTrack_type theLink;
            theLink.toContainedElement( *trackContainer, (Rec::TrackParticle*) theTrack );
            m_d0_nontau_Tracks.push_back(theLink);
        }
        //@}

    protected:
        int    m_decmode=-1;
        int    m_jettype=-1;

        double m_emradii[6]={0};
        double m_hadradii[6]={0};
        double m_TowEMRadius=-1111;
        double m_TowET12Frac=-1111;

        double m_ImpactParameter[10]={0};
        double m_RawImpactParameter[10]={0};
        double m_SignedImpactParameter[10]={0};
        double m_ImpactParameterSignificance[10]={0};
        double m_SignedImpactParameterSignificance[10]={0};

        // d0 variables
        double m_d0prf=-1111;
        double m_d0iso=-1111;
        double m_d0isoet=-1111;
        double m_d0ettr=-1111;
        double m_d0etem=-1111;
        double m_d0etem2=-1111;
        double m_d0emclet=-1111;
        double m_d0emcleta=-1111;
        double m_d0emclphi=-1111;
        double m_d0et05=-1111;
        double m_d0eta05=-1111;
        double m_d0phi05=-1111;

        double m_d0hadet=-1111;
        double m_d0hadeta=-1111;
        double m_d0hadphi=-1111;

        double m_d0type=-1111;
        double m_d0deltaR1=-1111;
        double m_d0eTosumpT=-1111;

        double m_d0deltaR1had=-1111;
        double m_d0em3iso=-1111;
        double m_d0mtrem3=-1111;

        double m_d0deltaR2=-1111;
        double m_d0ntr1030=-1111;

        double m_d0EM12isof=-1111;
        double m_d0e1e2otaupT=-1111;
        double m_d0ettro123=-1111;
        double m_d0ett1oEtiso=-1111;
        double m_d0ett1oEtisoet=-1111;
        double m_d0dalpha=-1111;
        double m_d0e1e2=-1111;

        double m_d0mtr1tr2=-1111;
        double m_d0mtr1tr2tr3=-1111;

        double m_d0sumtaupt=-1111;
        double m_d0sumnontaupt=-1111;
        double m_d0sumpt=-1111;

        std::vector<double> m_towere;
        std::vector<double> m_towereta;
        std::vector<double> m_towerphi;

        double m_d0uncaletlayers[25]={0};

        tauCellColl_type m_d0_emcluster;

        tauTrackColl_type m_d0_05_Tracks;
        tauTrackColl_type m_d0_tau_Tracks;
        tauTrackColl_type m_d0_nontau_Tracks;
};


#endif
