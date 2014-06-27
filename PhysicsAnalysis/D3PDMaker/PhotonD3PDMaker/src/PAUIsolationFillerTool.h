// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/PAUIsolationFillerTool.h
 * @author Mike Hance <hance@hep.upenn.edu>
 * @date March, 2010
 * @brief Fill PAU related variables
 */


#ifndef PHOTOND3PDMAKER_PAUISOLATIONFILLERTOOL_H
#define PHOTOND3PDMAKER_PAUISOLATIONFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include <vector>
#include <string>
#include <map>

namespace PAU{
  class egamma;
}


namespace D3PD {


/**
 * @brief Fill author code and related flags.
 */
class PAUIsolationFillerTool
  : public BlockFillerTool<PAU::egamma>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  PAUIsolationFillerTool (const std::string& type,
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
  virtual StatusCode fill (const PAU::egamma& p);


private:
  std::vector<float> m_fill_EtCone_pt_corrected;
  std::vector<float> m_fill_EtCone_ED_corrected;
  std::vector<float> m_fill_EtCone_ED_corrected_new;
  std::vector<float> m_fill_EtCone_corrected;
  std::vector<float> m_fill_EtCone_corrected_new;

  std::vector<float> m_fill_EtConeIso;
  std::vector<float> m_fill_EtCone_cells35;
  std::vector<float> m_fill_EtCone_cells55;
  std::vector<float> m_fill_EtConeIso_cells35;
  std::vector<float> m_fill_EtConeIso_cells55;

  std::map<int, float*> m_EtCone_pt_corrected;
  std::map<int, float*> m_EtCone_ED_corrected;
  std::map<int, float*> m_EtCone_corrected;
  std::map<int, float*> m_EtCone_ED_corrected_new;
  std::map<int, float*> m_EtCone_corrected_new;

  std::map<int, float*> m_EtConeIso;
  std::map<int, float*> m_EtCone_cells35;
  std::map<int, float*> m_EtCone_cells55;
  std::map<int, float*> m_EtConeIso_cells35;
  std::map<int, float*> m_EtConeIso_cells55;


  // mapping the "fill" vectors to the value vectors
  std::map<std::vector<float>*, std::map<int, float*>*> m_fill_to_value_map;

  // mapping the "fill" vectors to the variable names
  std::map<std::vector<float>*, std::string> m_fill_to_name_map;

  // collection of all maps
  std::vector<std::vector<float>*> m_all_vecs;

  // some more isolation stuff
  float * m_ED_median ;
  float * m_ED_sigma ;
  int   * m_ED_Njets ;
  float * m_ED_median_new ;
  float * m_ED_sigma_new ;
  int   * m_ED_Njets_new ;
  float * m_transmin_etcone40 ;
  float * m_transmax_etcone40 ;
  float * m_transmin_etcone100 ;
  float * m_transmax_etcone100 ;
  float * m_transmin ;
  float * m_transmax ;
  float * m_transmin_loweta ;
  float * m_transmax_loweta ;
};


} // namespace D3PD


#endif // not PHOTOND3PDMAKER_PAUISOLATIONFILLERTOOL_H
