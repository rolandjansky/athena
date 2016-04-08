/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTSIMCELL_H
#define FASTSIMCELL_H

#include "CaloEvent/CaloCell.h"
#include "TileEvent/TileCell.h"
#include "LArRecEvent/LArCell.h"

#include "Identifier/IdentifierHash.h"

class FastSimCaloCell : public CaloCell {
public:
    FastSimCaloCell():CaloCell() {};
    
    void setGain(CaloGain::CaloGain gain=CaloGain::INVALIDGAIN) {m_gain=gain;};
};

class FastSimTileCell : public TileCell {
public:
    FastSimTileCell():TileCell() {};
    
    void setGain(CaloGain::CaloGain gain=CaloGain::INVALIDGAIN) {m_gain=gain;};
};

class FastSimLArCell : public LArCell {
public:
    FastSimLArCell():LArCell() {};
    
    void setGain(CaloGain::CaloGain gain=CaloGain::INVALIDGAIN) {m_gain=gain;};
};

#endif  // FASTSIMCELL_H
