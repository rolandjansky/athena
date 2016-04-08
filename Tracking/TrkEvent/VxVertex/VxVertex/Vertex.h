/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXVERTEX_VERTEX_H
#define VXVERTEX_VERTEX_H
#include "GeoPrimitives/GeoPrimitives.h"

/**
 * @class Trk::Vertex 
 * 
 * This class is a simplest representation of a vertex candidate.
 * The 3-position (X,Y,Z) is stored.   
 * <br>                        
 *   begin   : Autumn 2003
 * <br>
 *   changes : 11.02.04 added docu
 *   @authors Andreas Wildauer (CERN PH-ATC), andreas.wildauer@cern.ch          
 */

class MsgStream;
class RecVertexCnv_p1;

namespace Trk 
{
  class Vertex 
  {
  public:
  
  /**
   * Contructors: default, copy and a constructor taking a 
   * vertex position (Amg::Vector3D) as argument.
   */
    Vertex(); //!< default constructor
    Vertex(const Amg::Vector3D &p); //!< constructor with Amg::Vector3D (== Amg::Vector3D)
    Vertex(const Vertex& );  //!< copy constructor
    Vertex &operator= (const Vertex &);  //!< Assignement operator
    
    /** Output Method for MsgStream, to be overloaded by child classes */
    virtual MsgStream& dump(MsgStream& sl) const;
    /** Output Method for std::ostream, to be overloaded by child classes */
    virtual std::ostream& dump(std::ostream& sl) const;

    virtual ~Vertex(); //!< standard destructor

    const Amg::Vector3D& position() const; //!< return position of vertex

    /**return number of parameters currently created*/
    static unsigned int numberOfInstantiations() ;

  private:
    friend class ::RecVertexCnv_p1;

    Amg::Vector3D m_position; //!< vertex position

    /** number of objects of this type in memory */
    static unsigned int s_numberOfInstantiations;
  };

/**Overload of << operator for both, MsgStream and std::ostream for debug 
output*/ 
MsgStream& operator << ( MsgStream& sl, const Vertex& sf);
std::ostream& operator << ( std::ostream& sl, const Vertex& sf); 

} // end of namespace Trk

#endif

