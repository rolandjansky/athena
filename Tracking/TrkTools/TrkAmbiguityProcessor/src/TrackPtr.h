/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _Trk_TrackPtr_h_
#define _Trk_TrackPtr_h_

#include "TrkTrack/Track.h"

namespace Trk {
   class TrackPtr  {
   public:
      TrackPtr() = delete;

      // disallow copying
      TrackPtr( const TrackPtr &) = delete;
      TrackPtr &operator=(const TrackPtr &) = delete;

      // allow moving
      TrackPtr( TrackPtr &&a) : m_track(a.m_track), m_uid(a.m_uid), m_owner(a.m_owner), m_fitted(a.m_fitted) {
         if (a.m_owner) {
            a.m_owner              = false;
            a.m_track.m_ownedTrack = nullptr;
         }
      }

      TrackPtr( TrackPtr &&a, int uid) : m_track(a.m_track), m_uid(uid), m_owner(a.m_owner), m_fitted(a.m_fitted) {
         if (a.m_owner) {
            a.m_owner              = false;
            a.m_track.m_ownedTrack = nullptr;
         }
      }

      TrackPtr( const Trk::Track *orig_track)       : m_uid(-1), m_owner(false), m_fitted(false) { m_track.m_origTrack  = orig_track; }
      TrackPtr( Trk::Track *new_track, bool fitted) : m_uid(-1), m_owner(true), m_fitted(fitted) { m_track.m_ownedTrack = new_track; }
      TrackPtr( const Trk::Track *orig_track, int uid)       : m_uid(uid), m_owner(false), m_fitted(false)  { m_track.m_origTrack  = orig_track; }
      TrackPtr( Trk::Track *new_track, bool fitted, int uid) : m_uid(uid), m_owner(true),  m_fitted(fitted) { m_track.m_ownedTrack = new_track; }
      ~TrackPtr() {
         if (m_owner) delete m_track.m_ownedTrack;
      }

      Trk::Track *release() {
         assert( (m_owner && m_track.m_ownedTrack) || (!m_owner && m_track.m_origTrack) );
         Trk::Track *tmp = (m_owner ? m_track.m_ownedTrack : new Trk::Track( *m_track.m_origTrack) );
         if (m_owner) {
            m_track.m_ownedTrack = nullptr;
            m_owner=false;
            m_fitted=false;
         }
         return tmp;
      }

      const Trk::Track *operator->() const { assert(track()); return track(); }
      const Trk::Track &operator*() const  { assert(track()); return *track(); }
      const Trk::Track *track() const {
         return (m_owner ? m_track.m_ownedTrack : m_track.m_origTrack);
      }

      Trk::Track *newTrack() {
         return (m_owner ? m_track.m_ownedTrack : nullptr);
      }

      void setNewTrack( Trk::Track *track, bool fitted) {
         if (m_owner) {
            delete m_track.m_ownedTrack;
         }
         m_track.m_ownedTrack = track;
         if (track) {
            m_owner=true;
            m_fitted=fitted;
         }
         else {
            m_owner=false;
            m_fitted=false;
         }
      }
      bool fitted() const { return m_fitted; }
      void forceFitted() { m_fitted=true; }
      int getUid() const { return m_uid; }

   private:
      union {
         Trk::Track *m_ownedTrack;
         const Trk::Track *m_origTrack;
      } m_track;
      const int m_uid;
      bool m_owner;
      bool m_fitted;
   };
}
#endif
