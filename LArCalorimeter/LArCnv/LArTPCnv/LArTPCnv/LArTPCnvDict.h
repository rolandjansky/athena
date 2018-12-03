/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARTPCNV_LARTPCNVDICT_H
#define LARTPCNV_LARTPCNVDICT_H

#include "LArTPCnv/LArDigitContainer_p1.h"
#include "LArTPCnv/LArRawChannel_p1.h" 
#include "LArTPCnv/LArRawChannel_p2.h" 
#include "LArTPCnv/LArRawChannelCollection_p1.h"
#include "LArTPCnv/LArRawChannelCollection_p2.h"
#include "LArTPCnv/LArRawChannelContainer_p1.h" 
#include "LArTPCnv/LArRawChannelContainer_p2.h" 
#include "LArTPCnv/LArRawChannelContainer_p3.h" 
#include "LArTPCnv/LArRawChannelContainer_p4.h" 

#include "LArTPCnv/LArFebErrorSummary_p1.h" 

#include "LArTPCnv/LArNoisyROSummary_p1.h" 
#include "LArTPCnv/LArNoisyROSummary_p2.h" 
#include "LArTPCnv/LArNoisyROSummary_p3.h" 
#include "LArTPCnv/LArNoisyROSummary_p4.h" 
#include "LArTPCnv/LArNoisyROSummary_p5.h" 

#include "Identifier/HWIdentifier.h"

struct GCCXML_DUMMY_INSTANTIATION_LARTPCNV {
    std::pair<unsigned int, std::vector<int> >  m_pair1;
    std::vector<std::pair<unsigned int, std::vector<int> >  > m_vpair1;

    std::pair<HWIdentifier, std::vector<int> >  m_pair2;
    std::vector<std::pair<HWIdentifier, std::vector<int> >  > m_vpair2;
};

#endif
