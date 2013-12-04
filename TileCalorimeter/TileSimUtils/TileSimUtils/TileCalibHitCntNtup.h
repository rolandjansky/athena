/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILESIMUTILS_TILECALIBHITCNTNTUP_H
#define TILESIMUTILS_TILECALIBHITCNTNTUP_H

/********************************************************************
 *
 * NAME:     TileCalibHitCntNtup
 * PACKAGE:  offline/TileCalorimeter/TileSimUtils
 *
 * AUTHOR :  A. Solodkov    Sanya.Solodkov@cern.ch
 *           S. Karpov      Sergey.Karpov@cern.ch
 * CREATED:  Jan 15, 2013
 *
 * PURPOSE:  To create Ntuple file from Tile CaloCalibrationHitContainer-s
 *
 *  Input: CaloCalibHitCnt (Calibration hit containers)
 *  Output: NTuple 
 *  Parameters: stream name, ntuple name, max tree size
 *   
 ********************************************************************/

// Atlas includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

// Calo includes
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

#include <string>
#include <vector>

#include <inttypes.h>

class CaloCell_ID;
class CaloDM_ID;

/**
@class TileCalibHitCntNtup
@brief This class creates ntuple with calibration hits container info
*/
class TileCalibHitCntNtup: public AthAlgTool {

public:
  TileCalibHitCntNtup(const std::string& type, const std::string& name, const IInterface* parent); //!< Constructor

  virtual ~TileCalibHitCntNtup(); //!< Destructor

  virtual StatusCode initialize();  //!< intialize method
  virtual StatusCode StoreCNT( const CaloCalibrationHitContainer* ActiveHitCnt,  //!< execute method
                               const CaloCalibrationHitContainer* InactiveHitCnt, 
                               const CaloCalibrationHitContainer* dmHitCnt );
  virtual StatusCode finalize();    //!< finalize method

  static const InterfaceID& interfaceID( ) ; //!< AlgTool InterfaceID

private:

  // handle to THistSvc
  ServiceHandle<ITHistSvc>   m_thistSvc;

  // NTuple parameters
  std::string m_streamName;
  std::string m_ntupleID;
  int64_t m_treeSize;

  // The ntuple
  TTree* m_ntuplePtr;

  // ntuple variables
  long m_act_nchan;
  long m_inact_nchan;
  long m_dm_nchan;

  float m_act_totE;
  float m_act_visE;
  float m_inact_totE;
  float m_inact_visE;
  float m_DM_totE;
  float m_DM_visE;

  float m_barr_totE;
  float m_barr_Em;
  float m_barr_NonEm;
  float m_barr_Invisible;
  float m_barr_Escaped;

  float m_inact_barr_totE;
  float m_inact_barr_Em;
  float m_inact_barr_NonEm;
  float m_inact_barr_Invisible;
  float m_inact_barr_Escaped;

  float m_ext_totE;
  float m_ext_Em;
  float m_ext_NonEm;
  float m_ext_Invisible;
  float m_ext_Escaped;

  float m_inact_ext_totE;
  float m_inact_ext_Em;
  float m_inact_ext_NonEm;
  float m_inact_ext_Invisible;
  float m_inact_ext_Escaped;

  float m_itc_totE;
  float m_itc_Em;
  float m_itc_NonEm;
  float m_itc_Invisible;
  float m_itc_Escaped;

  float m_inact_itc_totE;
  float m_inact_itc_Em;
  float m_inact_itc_NonEm;
  float m_inact_itc_Invisible;
  float m_inact_itc_Escaped;

  float m_gscin_totE;
  float m_gscin_Em;
  float m_gscin_NonEm;
  float m_gscin_Invisible;
  float m_gscin_Escaped;

  float m_inact_gscin_totE;
  float m_inact_gscin_Em;
  float m_inact_gscin_NonEm;
  float m_inact_gscin_Invisible;
  float m_inact_gscin_Escaped;

  std::vector<float> *_m_Total;
  std::vector<float> *_m_Em;
  std::vector<float> *_m_NonEm;
  std::vector<float> *_m_Invisible;
  std::vector<float> *_m_Escaped;
  std::vector<long>  *_m_subCalo;
  std::vector<long>  *_m_section;
  std::vector<long>  *_m_side;
  std::vector<long>  *_m_module;
  std::vector<long>  *_m_tower;
  std::vector<long>  *_m_sample;

  std::vector<float> *_m_inact_Total;
  std::vector<float> *_m_inact_Em;
  std::vector<float> *_m_inact_NonEm;
  std::vector<float> *_m_inact_Invisible;
  std::vector<float> *_m_inact_Escaped;
  std::vector<long>  *_m_inact_subCalo;
  std::vector<long>  *_m_inact_section;
  std::vector<long>  *_m_inact_side;
  std::vector<long>  *_m_inact_module;
  std::vector<long>  *_m_inact_tower;
  std::vector<long>  *_m_inact_sample;

  std::vector<float> *_m_dm_ene;
  std::vector<float> *_m_dm_vis;
  std::vector<long>  *_m_dm_subDet;
  std::vector<long>  *_m_dm_type;
  std::vector<long>  *_m_dm_sampling;
  std::vector<long>  *_m_dm_region;
  std::vector<long>  *_m_dm_eta;
  std::vector<long>  *_m_dm_phi;

  std::string m_ActiveHitContainer;
  std::string m_InactiveHitContainer;
  std::string m_dmHitContainer;

  StoreGateSvc* m_storeGate;
  StoreGateSvc* m_detStore;

  const CaloCell_ID* m_caloCell_ID;
  const CaloDM_ID*   m_caloDM_ID;

  int m_max_chan;

};

#endif
