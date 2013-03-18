// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef __D3PD_L1CALOCMMROIFILLERTOOL_H__
#define __D3PD_L1CALOCMMROIFILLERTOOL_H__

/*
  D3PD::L1CaloCMMRoIFillerTool.h
*/

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigT1CaloEvent/CMMRoI.h"

namespace D3PD{
  /*
   * Tool for saving the properties of L1Calo CMM RoIs
   * @author John Morris <john.morris@cern.ch>
   */

  class L1CaloCMMRoIFillerTool : public BlockFillerTool< LVL1::CMMRoI > {

    public:
      L1CaloCMMRoIFillerTool( const std::string& type, const std::string& name,const IInterface* parent);

      // Function booking the variables
      StatusCode book();
      // Function filling the variables
      StatusCode fill( const LVL1::CMMRoI& p );

    private:
      int*       m_jetEtHits;
      int*       m_sumEtHits;
      int*       m_missingEtHits;
      int*       m_ex;
      int*       m_ey;
      int*       m_et;
      int*       m_jetEtError;
      int*       m_sumEtError;
      int*       m_missingEtError;
      int*       m_missingEtSigError;
      int*       m_exError;
      int*       m_eyError;
      int*       m_etError;
      uint32_t*  m_jetEtRoiWord;
      uint32_t*  m_energyRoiWord0;
      uint32_t*  m_energyRoiWord1;
      uint32_t*  m_energyRoiWord2;
      
  }; // class L1CaloCMMRoIFillerTool
} // namespace D3PD
#endif
