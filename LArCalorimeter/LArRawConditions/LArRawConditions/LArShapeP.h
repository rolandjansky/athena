/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARSHAPEP
#define LARRAWCONDITIONS_LARSHAPEP

#include <vector> 

/** c-struct reproducing the structure of the persistent data
 * @author S. Laplace
 * @version  0-0-1 , 21/01/2004 */


class LArShapeP
{

public:
    LArShapeP()  {} 
    bool isEmpty() const { return m_vShape.size() == 0 ; } ;
    std::vector<double> m_vShape;
    std::vector<double> m_vShapeDer;
};

     
#endif 

