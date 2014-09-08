// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetFlavorTagComponentsFillerTool.cxx
 * @author Erik Devetak <edevetak@cern.ch>
 * @date Feb, 2010
 * @brief Block filler tool for Jet BTag related quantities. The class implements most of the code in PhysicsAnalysis/JetTagging/JetTagValidation/JetTagNtuple/tags/JetTagNtuple-00-00-25/src/JetTagNtuple_JetTool.cxx by 
 */

#ifndef JETD3PDMAKER_JETFLAVORTAGCOMPONENTSFILLERTOOL_H
#define JETD3PDMAKER_JETFLAVORTAGCOMPONENTSFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
class Jet;


namespace D3PD {


/**
 * @brief Block filler tool for Jet BTag.
 */
class JetFlavorTagComponentsFillerTool
  : public BlockFillerTool<Jet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetFlavorTagComponentsFillerTool (const std::string& type,
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
  virtual StatusCode fill (const Jet& p);
  
  
private:

  double* m_ip2d_pu;
  double* m_ip2d_pb;
  double* m_ip2d_ntrk;
  double* m_ip3d_ntrk;
  double* m_ip3d_pu;
  double* m_ip3d_pb;
  double* m_sv1_pu;
  double* m_sv1_pb;
  double* m_sv2_pu;
  double* m_sv2_pb;
  double* m_svp_mass;  
  double* m_svp_n2t;   
  double* m_svp_svok;  
  double* m_svp_ntrk;  
  double* m_svp_ntrkv;
  double* m_svp_ntrkj;
  double* m_svp_efrc; 
  double* m_sv0p_mass;  
  double* m_sv0p_n2t;   
  double* m_sv0p_svok;  
  double* m_sv0p_ntrk;  
  double* m_sv0p_ntrkv;
  double* m_sv0p_ntrkj;
  double* m_sv0p_efrc; 
  double* m_jfit_pu; 
  double* m_jfit_pb;
  double* m_jfit_nvtxnt; 
  double* m_jfit_nvtx1t; 
  double* m_jfit_ntrk; 
  double* m_jfit_efrc;   
  double* m_jfit_mass;   
  double* m_jfit_sig3d;  
  bool m_doAllTags;
  std::string m_tagKey;
};


} // namespace D3PD


#endif 
