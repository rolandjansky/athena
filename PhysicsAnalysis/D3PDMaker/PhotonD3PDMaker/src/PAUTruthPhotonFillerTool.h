// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/PAUTruthPhotonFiller.h
 * @author Mike Hance <hance@hep.upenn.edu>
 * @date January, 2011
 * @brief Fill PAU related variables
 */


#ifndef PHOTOND3PDMAKER_PAUTRUTHPHOTONFILLERTOOL_H
#define PHOTOND3PDMAKER_PAUTRUTHPHOTONFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include <vector>
#include <string>

namespace PAU{
  class egammaTruth;
}


namespace D3PD {


/**
 * @brief Fill author code and related flags.
 */
class PAUTruthPhotonFillerTool
  : public BlockFillerTool<PAU::egammaTruth>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  PAUTruthPhotonFillerTool (const std::string& type,
			    const std::string& name,
			    const IInterface* parent);


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const PAU::egammaTruth& p);


private:
  bool m_fillNewVars;

  float
    * m_partonIsolation15_UE  ,
    * m_partonIsolation20_UE  ,
    * m_partonIsolation25_UE  ,
    * m_partonIsolation30_UE  ,
    * m_partonIsolation35_UE  ,
    * m_partonIsolation40_UE  ,
    * m_partonIsolation45_UE  ,
    * m_partonIsolation60_UE  ,
    * m_partonIsolation15  ,
    * m_partonIsolation20  ,
    * m_partonIsolation25  ,
    * m_partonIsolation30  ,
    * m_partonIsolation35  ,
    * m_partonIsolation40  ,
    * m_partonIsolation45  ,
    * m_partonIsolation60  ;

  float 
    * m_particleIsolation40,
    * m_particleIsolation40_ED_corrected,
    * m_particleIsolation40_ED_corrected_reco,
    * m_particleIsolation40_ED_corrected_new,
    * m_particleIsolation40_ED_corrected_reco_new;

  bool
    * m_isHardProcPhoton,
    * m_isBremPhoton;
};


} // namespace D3PD


#endif
