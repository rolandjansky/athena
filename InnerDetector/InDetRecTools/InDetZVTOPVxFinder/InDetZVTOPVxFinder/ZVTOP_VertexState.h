/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IINDETZVTOP_VERTEXSTATE_H
#define IINDETZVTOP_VERTEXSTATE_H 

#include "VxVertex/Vertex.h"
#include "TrkTrack/Track.h"
/**
 *  ZVTOP_Tool helper class
 *  to store the full vertex/tracks & vtx_probability
 * information
 *  @author:      Tatjana Lenz <Tatjana.Lenz@cern.ch>
 */


namespace InDet
{

 class ZVTOP_VertexState
 {
public:

    /**Default Constructor */
    ZVTOP_VertexState();
    ZVTOP_VertexState(double& vtx_prob, Trk::Vertex& vertex, bool& beam_spot, std::vector<const Trk::Track*>& tracks);

   /**virtual destructor of a class*/
   virtual ~ZVTOP_VertexState();

    /**Copy Constructor */
    ZVTOP_VertexState (const ZVTOP_VertexState& vs); //copy-constructor
    //ZVTOP_VertexState &operator=  (const ZVTOP_VertexState &);  //assignement operator
    
    double& vtx_prob(void);

    Trk::Vertex& vertex(void);

    bool& beam_spot(void);

    std::vector<const Trk::Track*>&  tracks(void);
 
    inline void set_beam_spot(bool);

protected:
    //vertex probability
    double m_vtx_prob;
    //associated vertex 
    Trk::Vertex m_vertex;
    bool m_beam_spot;
    // associated tracks
    std::vector<const Trk::Track*> m_tracks; 
};//end of class definitions

inline double&  ZVTOP_VertexState::vtx_prob(void)
  {
    return m_vtx_prob;
  }

inline Trk::Vertex& ZVTOP_VertexState::vertex(void)
  {
    return m_vertex;
  }

inline std::vector<const Trk::Track*>& ZVTOP_VertexState::tracks(void)
  {
    return m_tracks;
  }
inline bool& ZVTOP_VertexState::beam_spot(void)
  {
    return m_beam_spot;
  }
inline void ZVTOP_VertexState::set_beam_spot(bool beam_spot)
  {
    m_beam_spot = beam_spot;
  }
}//end of namespace definitions

#endif
