/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerRateSlices/TrigL2JetObject.h"

ClassImp(TrigL2JetObject )


TrigL2JetObject::TrigL2JetObject()
  : TObject(),
    m_e(0),
    m_ehad0(0),
    m_eem0(0),
    m_eta(0),
    m_phi(0),
    m_roiWord(0) {
}

TrigL2JetObject::TrigL2JetObject(const TrigL2JetObject & other)
  : TObject(other),
    m_e(0),
    m_ehad0(0),
    m_eem0(0),
    m_eta(0),
    m_phi(0),
    m_roiWord(0) {
}

TrigL2JetObject::~TrigL2JetObject() {
}
