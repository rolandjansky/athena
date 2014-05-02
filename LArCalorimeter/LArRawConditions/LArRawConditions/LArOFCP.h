/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LAROFCP
#define LARRAWCONDITIONS_LAROFCP

#include <vector> 

/** c-struct reproducing the structure of the persistent data
 * @author W. Lampl, S. Laplace
 * @version  0-0-1 , 12/12/03 */


class LArOFCP{

public:

    LArOFCP(): m_timeOffset(0) {} 
    bool isEmpty() const { return m_vOFC_a.size() == 0 ; } ;
    float        m_timeOffset;
    std::vector<std::vector<float> > m_vOFC_a;
    std::vector<std::vector<float> > m_vOFC_b;

};

#endif 

