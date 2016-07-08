/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IINDETZVTOP_TRKPARTVERTEXSTATE_H
#define IINDETZVTOP_TRKPARTVERTEXSTATE_H 

#include "VxVertex/Vertex.h"
#include "Particle/TrackParticle.h"
/**
 *  ZVTOP_Tool helper class
 *  to store the full vertex/tracks & vtx_probability
 * information
 *  @author:      Tatjana Lenz <Tatjana.Lenz@cern.ch>
 */


namespace InDet
{

 class ZVTOP_TrkPartVertexState
 {
public:

    /**Default Constructor */
    ZVTOP_TrkPartVertexState();
    ZVTOP_TrkPartVertexState(double& vtx_prob, Trk::Vertex& vertex, bool& IP, std::vector<const Rec::TrackParticle*>& tracks);

   /**virtual destructor of a class*/
   virtual ~ZVTOP_TrkPartVertexState();

    /**Copy Constructor */
    ZVTOP_TrkPartVertexState (const ZVTOP_TrkPartVertexState& vs); //copy-constructor
    //ZVTOP_VertexState &operator=  (const ZVTOP_VertexState &);  //assignement operator
    
    double& vtx_prob(void);

    Trk::Vertex& vertex(void);

    bool& IP(void);

    std::vector<const Rec::TrackParticle*>&  tracks(void);
 
    inline void set_IP(bool);

protected:
    //vertex probability
    double m_vtx_prob;
    //associated vertex 
    Trk::Vertex m_vertex;
    bool m_IP;
    // associated tracks
    std::vector<const Rec::TrackParticle*> m_tracks; 
};//end of class definitions

inline double&  ZVTOP_TrkPartVertexState::vtx_prob(void)
  {
    return m_vtx_prob;
  }

inline Trk::Vertex& ZVTOP_TrkPartVertexState::vertex(void)
  {
    return m_vertex;
  }

inline std::vector<const Rec::TrackParticle*>& ZVTOP_TrkPartVertexState::tracks(void)
  {
    return m_tracks;
  }
inline bool& ZVTOP_TrkPartVertexState::IP(void)
  {
    return m_IP;
  }
inline void ZVTOP_TrkPartVertexState::set_IP(bool IP)
  {
    m_IP = IP;
  }
}//end of namespace definitions

#endif
