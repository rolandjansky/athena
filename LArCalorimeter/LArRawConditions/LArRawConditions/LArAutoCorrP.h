/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARAUTOCORRP
#define LARRAWCONDITIONS_LARAUTOCORRP

#include "Identifier/HWIdentifier.h"
#include <vector> 

/** c-struct reproducing the structure of the persistent data
 * @author W. Lampl, S. Laplace
 * @version  0-0-1 , 12/12/03 
 * Changes: 
 *   - 08/01/2004: float -> double 
 *   - 21/01/2004: double to float :-) (only the transient will be double, but not the persistent)*/

/*  Modified for POOL persistency
	Hong Ma
	July 15, 2005
*/ 


class LArAutoCorrP
{
public: 

    LArAutoCorrP() {} 
    bool isEmpty() const { return (m_vAutoCorr.size() == 0); }
    std::vector<double> m_vAutoCorr;

};

#endif 

