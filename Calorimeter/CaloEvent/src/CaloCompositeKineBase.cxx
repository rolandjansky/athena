/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloEvent/CaloCompositeKineBase.h"

CaloCompositeKineBase::CaloCompositeKineBase() 
  : P4EEtaPhiM(0.,0.,0.,0.),
    m_status(CaloRecoStatus())
{ }

CaloCompositeKineBase::~CaloCompositeKineBase()
{ }
