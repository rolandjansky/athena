/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARSHAPEP1
#define LARRAWCONDITIONS_LARSHAPEP1

#include <vector> 

/** c-struct reproducing the structure of the persistent data
 * @author S. Laplace
 * @version  0-0-1 , 21/01/2004 */


class LArShapeP1
{

public:
    LArShapeP1()  {} 
    LArShapeP1(const std::vector<float>& shape,
               const std::vector<float>& shapeDer) 
            : m_vShape(shape), m_vShapeDer(shapeDer)  {} 
    bool isEmpty() const { return m_vShape.size() == 0 ; } ;
    std::vector<float> m_vShape;
    std::vector<float> m_vShapeDer;
};

     
#endif 

