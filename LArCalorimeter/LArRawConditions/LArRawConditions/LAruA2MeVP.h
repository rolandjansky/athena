/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARUA2MEVP
#define LARRAWCONDITIONS_LARUA2MEVP

#include <vector> 
#include "LArElecCalib/ILAruA2MeV.h"
/** c-struct reproducing the structure of the persistent data
 * @author S. Laplace
 * @version  0-0-1 , 16/01/04 */


/* updated for Pool persistency
	July 19, 2005 
*/ 

class LAruA2MeVP{ 
public:

    LAruA2MeVP() : m_uA2MeV(float(ILAruA2MeV::ERRORCODE)) {} 
    LAruA2MeVP(float a) : m_uA2MeV(a) {} 
    bool isEmpty() const { return (m_uA2MeV == float(ILAruA2MeV::ERRORCODE)) ; } ;
    float m_uA2MeV;
};

     
#endif 

