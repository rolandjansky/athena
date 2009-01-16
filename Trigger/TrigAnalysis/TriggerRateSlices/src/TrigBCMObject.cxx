/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBCMObject.h"

ClassImp(TrigBCMObject )

/* Default Constructor */
TrigBCMObject::TrigBCMObject():TObject() {

    m_bcm_A_toC_trigger = 0;
    m_bcm_C_toA_trigger = 0;
    m_bcm_Wide_trigger = 0;
    m_bcm_Ideal_trigger = 0;

}

/* Detructor */
TrigBCMObject::~TrigBCMObject() {

}
