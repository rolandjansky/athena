// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/PAUDiPhotonFillerTool.h
 * @author Mike Hance <hance@hep.upenn.edu>
 * @date August, 2010
 * @brief Fill PAU related variables for diphotons
 */


#ifndef PHOTOND3PDMAKER_PAUDIPHOTONFILLERTOOL_H
#define PHOTOND3PDMAKER_PAUDIPHOTONFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include <vector>
#include <string>
//#include "IsolationTool/IIsolationTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace PAU{
  class DiPhoton;
}


namespace D3PD {


/**
 * @brief Fill author code and related flags.
 */
class PAUDiPhotonFillerTool
  : public BlockFillerTool<PAU::DiPhoton>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  PAUDiPhotonFillerTool (const std::string& type,
			 const std::string& name,
			 const IInterface* parent);


  virtual ~PAUDiPhotonFillerTool();

  StatusCode initialize();

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
  virtual StatusCode fill (const PAU::DiPhoton& p);


private:

  std::vector<float>   * m_etaLead               ;
  std::vector<float>   * m_etaSublead            ;
  std::vector<float>   * m_z                     ;
  std::vector<float>   * m_zres                  ;
  std::vector<float>   * m_vtxLikelihood         ;
  std::vector<float>   * m_invmass               ;
  std::vector<float>   * m_invmassTrueDir        ;
  std::vector<float>   * m_invmassTrueE          ;
  std::vector<int>     * m_invmass_flag          ;
  std::vector<float>   * m_ptgg                  ;
  std::vector<float>   * m_detagg                ;
  std::vector<float>   * m_cosTh                 ;

  int                  * m_onejet_cutStatus      ;
  int                  * m_onejet_ok             ;
  float                * m_onejet_mgg            ;
  float                * m_onejet_mggj           ;
  	                
  int                  * m_twojet_cutStatus      ;
  int                  * m_twojet_ok             ;
  float                * m_twojet_mgg            ;
  float                * m_twojet_mjj            ;
  
  float                *m_hgg_HPV_zcommon;
  float                *m_hgg_HPV_sigma_zcommon;
  float                *m_hgg_HPV_zcommon_primVtxLH;
  float                *m_hgg_HPV_zcommon_primVtxNN;
  float                *m_hgg_HPV_vtxLikelihood;
  float                *m_hgg_HPV_etaLead_zcommon;
  float                *m_hgg_HPV_etaSublead_zcommon;
  float                *m_hgg_HPV_invmass;
  float                *m_hgg_HPV_etaLead_zcommon_primVtxLH;
  float                *m_hgg_HPV_etaSublead_zcommon_primVtxLH;
  float                *m_hgg_HPV_invmass_primVtxLH;
  float                *m_hgg_HPV_invmass_Ztruth;
  float                *m_hgg_HPV_ptgg    ;
  float                *m_hgg_HPV_detagg  ;
  float                *m_hgg_HPV_dphigg  ;
  float                *m_hgg_HPV_cosTh   ;
  float                *m_hgg_HPV_cosThCS ;

  std::vector<float>   *m_hgg_ptcone20 ;
  std::vector<float>   *m_hgg_ptcone30 ;
  std::vector<float>   *m_hgg_ptcone40 ;


  /** @brief the TrackIsolationTool for photons*/
  //ToolHandle<IIsolationTool> m_photonTrackIsolationTool;

  std::string m_trackParticleContainerName;
  std::string m_vxPrimaryContainerName;
  std::string m_trackMapName;
  bool m_useIDPV;
  

  //const Rec::TrackParticle* getTrackParticle(Trk::VxTrackAtVertex *trkAtVx);

};


} // namespace D3PD


#endif // not EGAMMAD3PDMAKER_PAUFILLERTOOL_H
