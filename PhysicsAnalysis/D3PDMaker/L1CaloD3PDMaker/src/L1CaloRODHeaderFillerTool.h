// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef __D3PD_L1CALORODHEADERFILLERTOOL_H__
#define __D3PD_L1CALORODHEADERFILLERTOOL_H__

/*
  D3PD::L1CaloRODHeaderFillerTool.h
*/

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigT1CaloEvent/RODHeader.h"

namespace D3PD{
  /*
   * Tool for saving the properties of L1Calo ROD Header
   * @author John Morris <john.morris@cern.ch>
   */

  class L1CaloRODHeaderFillerTool : public BlockFillerTool< LVL1::RODHeader > {

  public:
    L1CaloRODHeaderFillerTool( const std::string& type, const std::string& name,const IInterface* parent);

    // Function booking the variables
    StatusCode book();
    // Function filling the variables
    StatusCode fill( const LVL1::RODHeader& p );

  private:

    uint32_t *m_version;
    uint32_t *m_sourceId;
    uint32_t *m_run;
    uint32_t *m_lvl1Id;
    uint32_t *m_bcId;
    uint32_t *m_trigType;
    uint32_t *m_detType;
    std::vector<uint32_t> *m_statusWords;
    uint32_t *m_payloadSize;

  }; // class L1CaloRODHeaderFillerTool

} // namespace D3PD

#endif
