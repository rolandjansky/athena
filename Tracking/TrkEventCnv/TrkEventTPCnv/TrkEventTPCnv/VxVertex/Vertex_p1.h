/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VERTEX_P1_TRK_H
#define VERTEX_P1_TRK_H

/**
 * Persistent representation of the VxVertex/Vertex class
 *
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch> 
 * 
 * July 2006
 */
 
#include <vector>

namespace Trk
{

 class Vertex_p1
 {
  
  public:
  
    Vertex_p1() {}
    virtual ~Vertex_p1() = default;
    
    std::vector<float>  m_position; 
  
 };//end of class definitions

}//end of namespace definitions

#endif // VERTEX_P1_TRK_H
