/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           VertexPositions.h  -  Description
                             -------------------
    begin   : Autumn 2006
    authors : Giacinto Piacquadio (Freiburg University)
    email   : giacinto.piacquadio@cern.ch
    changes :
              
 ***************************************************************************/

#ifndef VXJETVERTEX_VERTEXPOSITIONS_H
#define VXJETVERTEX_VERTEXPOSITIONS_H

#include "EventPrimitives/EventPrimitives.h"
#include "VxJetVertex/JetVtxParamDefs.h"
#include "GaudiKernel/MsgStream.h"

/** The standard namespace for VxVertexPositions */
namespace Trk {

  /** VertexPositions class to represent and store a vertex */
  class VertexPositions {
  public:
    VertexPositions(); //!< default constructor
    VertexPositions(const Amg::VectorX &p); //!< constructor with variable-size vector
    VertexPositions(const VertexPositions& );  //!< copy constructor
    VertexPositions &operator= (const VertexPositions &);  //!< Assignment operator
    
    /** Output Method for MsgStream, to be overloaded by child classes */
    virtual MsgStream& dump(MsgStream& sl) const;
    /** Output Method for std::ostream, to be overloaded by child classes */
    virtual std::ostream& dump(std::ostream& sl) const;

    virtual ~VertexPositions(); //!< standard destructor

    const Amg::VectorX& position() const; //!< return position of vertex

    void setPosition(const Amg::VectorX &);

    /**return number of parameters currently created*/
    static unsigned int numberOfInstantiations() ;

  protected:
    Amg::VectorX m_position; //!< vertex position
    bool m_useWeightTimesPosition; //bool for storing weightTimesPosition

    /** number of objects of this type in memory */
    static unsigned int s_numberOfInstantiations;
  };

/**Overload of << operator for both, MsgStream and std::ostream for debug 
output*/ 
MsgStream& operator << ( MsgStream& sl, const VertexPositions& sf);
std::ostream& operator << ( std::ostream& sl, const VertexPositions& sf); 

} // end of namespace Trk

#endif
