// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENT_TRIGMUONEFINFO_H
#define TRIGMUONEVENT_TRIGMUONEFINFO_H

// STL include(s):
#include <map>
#include <string>

// Gaudi/Athena include(s):
#include "AthenaKernel/CLASS_DEF.h"

// Forward declaration(s):
class MsgStream;
class TrigMuonEFTrack;
class TrigMuonEFCbTrack;
class TrigMuonEFInfoTrackContainer;
class TrigMuonEFInfoTrack;

class TrigMuonEFInfo {

	   enum{nRoINumDefault=999};

public:

   /** Default constructor */
   TrigMuonEFInfo();


   TrigMuonEFInfo( unsigned short int theRoI );

   TrigMuonEFInfo( unsigned short int theRoI,
                   std::unique_ptr<TrigMuonEFInfoTrackContainer> tracks);

   /** Constuctor setting legacy variables. */
   TrigMuonEFInfo( unsigned short int theRoI,
                   std::unique_ptr<TrigMuonEFInfoTrackContainer> tracks,
                   std::unique_ptr<TrigMuonEFTrack> spectrometerTrack,
                   std::unique_ptr<TrigMuonEFTrack> extrapolatedTrack,
                   std::unique_ptr<TrigMuonEFCbTrack> combinedTrack);

   /** Copy constructor */
   TrigMuonEFInfo( const TrigMuonEFInfo& rhs );

   /** Assignment operator */
   TrigMuonEFInfo& operator=( const TrigMuonEFInfo& rhs );

   /** Move assignment operator */
   TrigMuonEFInfo& operator=( TrigMuonEFInfo&& rhs );

   /** Destructor */
   ~TrigMuonEFInfo();

   // the check on pointers should be the best way to check.
   // Anyway right now the tracks are created, empty, in the constructor.
   // So here the check is if the track has been set once at least.
   //
   bool hasTrack() const;

   // legacy methods
   bool hasSpectrometerTrack() const ;
   bool hasExtrapolatedTrack() const ;
   bool hasCombinedTrack() const ;


   /* set methods */
   void setRoINum( unsigned short int theRoINum );
   void setNSegments( unsigned short int theNSegments );
   void setNMdtHits( unsigned short int theNMdtHits );
   void setNRpcHits( unsigned short int theNRpcHits );
   void setNTgcHits( unsigned short int theNTgcHits );
   void setNCscHits( unsigned short int theNCscHits );
   void setEtaPreviousLevel( double theEtaPreviousLevel );
   void setPhiPreviousLevel( double thePhiPreviousLevel );
   void addTrack(TrigMuonEFInfoTrack* track);
   // legacy methods
   void setSpectrometerTrack( TrigMuonEFTrack* track );
   void setExtrapolatedTrack( TrigMuonEFTrack* track );
   void setCombinedTrack( TrigMuonEFCbTrack* track );

   /* get methods */
   unsigned short int RoINum() const { return m_roi; }
   unsigned short int NSegments() const { return m_nSegments; }
   unsigned short int NMdtHits() const { return m_nMdtHits; }
   unsigned short int NRpcHits() const { return m_nRpcHits; }
   unsigned short int NTgcHits() const { return m_nTgcHits; }
   unsigned short int NCscHits() const { return m_nCscHits; }
   double EtaPreviousLevel() const { return m_etaPreviousLevel; }
   double PhiPreviousLevel() const { return m_phiPreviousLevel; }
   const TrigMuonEFInfoTrackContainer* TrackContainer() const { return m_trackContainer; }
   // legacy methods
   TrigMuonEFTrack* SpectrometerTrack()  ;
   TrigMuonEFTrack* ExtrapolatedTrack()  ;
   TrigMuonEFCbTrack* CombinedTrack()    ;
   // legacy methods with const correctness for comparison operator
   const TrigMuonEFTrack* legacySpectrometerTrack() const {return m_spectrometerTrack;}
   const TrigMuonEFTrack* legacyExtrapolatedTrack() const {return m_extrapolatedTrack;}
   const TrigMuonEFCbTrack* legacyCombinedTrack()   const {return m_combinedTrack;}

   // checks existence of legacy track
   bool hasLegacyTrack() const;


private:

   unsigned short int m_roi;
   unsigned short int m_nSegments;
   unsigned short int m_nMdtHits;
   unsigned short int m_nRpcHits;
   unsigned short int m_nTgcHits;
   unsigned short int m_nCscHits;
   double m_etaPreviousLevel;
   double m_phiPreviousLevel;

   TrigMuonEFInfoTrackContainer* m_trackContainer;

   // these objects are kept here for backward compatibility.
   // the are not used.
   TrigMuonEFTrack* m_spectrometerTrack;
   TrigMuonEFTrack* m_extrapolatedTrack;
   TrigMuonEFCbTrack* m_combinedTrack;

};


/// Helper function for printing the object
std::string str( const TrigMuonEFInfo& d );
/// Helper operator for printing the object
MsgStream& operator<< ( MsgStream& m, const TrigMuonEFInfo& d );

/// Operator comparing two TrigMuonEFInfo objects for equality
bool operator== ( const TrigMuonEFInfo& a, const TrigMuonEFInfo& b );
/// Operator comparing two TrigMuonEFInfo objects for inequality
inline bool operator!= ( const TrigMuonEFInfo& a, const TrigMuonEFInfo& b ) {
   return !( a == b );
}

/// Comparison with feedback
void diff( const TrigMuonEFInfo& a, const TrigMuonEFInfo& b,
           std::map< std::string, double >& variableChange );

CLASS_DEF( TrigMuonEFInfo , 180322828 , 1 )

#endif // TRIGMUONEVENT_TRIGMUONEFINFO_H
