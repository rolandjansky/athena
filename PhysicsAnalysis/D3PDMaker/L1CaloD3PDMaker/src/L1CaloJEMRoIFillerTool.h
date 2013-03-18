// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef __D3PD_L1CALOJEMROIFILLERTOOL_H__
#define __D3PD_L1CALOJEMROIFILLERTOOL_H__

/*
  D3PD::L1CaloJEMRoIFillerTool.h
*/

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigT1CaloEvent/JEMRoI.h"

namespace D3PD{
  /*
   * Tool for saving the properties of L1Calo JEM RoI
   * @author John Morris <john.morris@cern.ch>
   */

  class L1CaloJEMRoIFillerTool : public BlockFillerTool< LVL1::JEMRoI > {

    public:
      L1CaloJEMRoIFillerTool( const std::string& type, const std::string& name,const IInterface* parent);

      // Function booking the variables
      StatusCode book();
      // Function filling the variables
      StatusCode fill( const LVL1::JEMRoI& p );

    private:
      int*       m_crate;
      int*       m_jem;
      int*       m_frame;
      int*       m_location;
      int*       m_forward;
      int*       m_hits;
      int*       m_error;
      uint32_t*  m_roiWord;
 
      
  }; // class L1CaloJEMRoIFillerTool
} // namespace D3PD
#endif
