/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerRateSlices/TrigMissingEtObject.h"

ClassImp(TrigMissingEtObject )


TrigMissingEtObject::TrigMissingEtObject()
  : m_Ex       (0),	    
    m_Ey       (0), 
    m_EtMiss   (0),   
    m_EtSum    (0) {  
}

TrigMissingEtObject::TrigMissingEtObject(const TrigMissingEtObject & other)
  : TObject(other),
    m_Ex       ( other.m_Ex       ),
    m_Ey       ( other.m_Ey       ),
    m_EtMiss   ( other.m_EtMiss   ),
    m_EtSum    ( other.m_EtSum    ) {
}

TrigMissingEtObject::~TrigMissingEtObject() {
}

void TrigMissingEtObject::reset() {
    m_Ex        = 0;
    m_Ey        = 0;
    m_EtMiss    = 0;
    m_EtSum     = 0;
}
