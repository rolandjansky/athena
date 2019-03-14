/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARRECUTILS_LARHVCORRTOOL_H
#define LARRECUTILS_LARHVCORRTOOL_H

//#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "LArElecCalib/ILArHVCorrTool.h"
#include "LArElecCalib/ILArHVTool.h"

#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/DataHandle.h"
#include "LArCabling/LArCablingLegacyService.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/LArID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include "LArRawConditions/LArHVScaleCorrComplete.h"
#include "LArRawConditions/LArTdriftComplete.h"

class StoreGateSvc; 
class LArElectrodeID;

class LArHVCorrTool: public AthAlgTool,
		     virtual public ILArHVCorrTool
{
 public:
  
  // constructor
  LArHVCorrTool(const std::string& type, 
		const std::string& name, 
		const IInterface* parent); 
  
  // destructor 
  virtual ~LArHVCorrTool();
  
  // retrieve methods 
  float Scale(const HWIdentifier& id)  const;
  float Scale(const Identifier& id)  const;
  float Scale(const IdentifierHash& hash) const;
  
  // initialize and finalize methods
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  //IOV Callback functions
  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS);

  static const InterfaceID& interfaceID() { 
    return ILArHVCorrTool::interfaceID();}

  StatusCode record();

  bool updateOnLastCallback() const {return m_updateOnLastCallback;}

 private:

  mutable bool m_cacheFilled;
  mutable bool m_updateOnLastCallback;

  const LArOnlineID*    m_lar_on_id; 	
  const CaloCell_ID*    m_calocell_id;	
  const LArEM_ID*       m_larem_id;	
  const LArHEC_ID*      m_larhec_id;	
  const LArFCAL_ID*     m_larfcal_id;	
  const LArElectrodeID* m_electrodeID;  
  const CaloDetDescrManager* m_calodetdescrmgr;

  ToolHandle<LArCablingLegacyService>  m_cablingService;  
  ToolHandle<ILArHVTool> m_hvtool;

  float Scale_barrel(const float hv) const;
  float Scale_FCAL1(const float hv) const;
  float champ_e(float hv, float d) const;
  float vdrift(float e,float tempe) const;  
  float InvCharge(float e) const;  
  float Respo(float e,float e_nominal,float tempe) const;  
  float t_drift (float e, float e_nominal,float d, float tempe) const;  
  float EMEC_nominal(const float eta_r) const;    
  float EMEC_gap(const float eta_r, float Zeta) const;
  StatusCode buildFixHVList();
  float m_T0;
  bool m_allCallBack;

  /*##########################################################################
    definitions of new functions and variables                       units
    ---------------------------------------------------------------------
    d              gap distancia between cells in calorimeters       cm
    nominal        subdetector nominal voltage                       volt
    E              electric field in the cell                        kvolt/cm
    E_nominal      nominal electric field in the cell                kvolt/cm
    T              temperature of cryostat                           Kelvin
    Zsamp          Z coordinate for samplings in EMEC                mm
    champ_e        computes electric field                           kvolt/cm
    vdrift         computes drift velocity                           mm/micro_s
    Charge         computes recombination charge                     Coulomb
    Respo          computes the correction factors to HV!=nominal    -
    t_drift        computes drift times                              ns
    EMEC_nominal   finds nominal voltages in EMEC                    volt
    EMEC_gap       finds gap distances in EMEC                       mm
    ########################################################################## 
  */

  //Internal cache:
  mutable LArTdriftComplete* m_Tdrift;

  mutable std::vector<float> m_vScale;

  bool m_ownScale;

  std::string m_keyOutput;
  std::string m_keyOutputTd;
  std::string m_folderName;

  bool m_doTdrift;
  unsigned int m_deltatupdate;
  bool m_updateIfChanged;

  struct HVfix_t {
    unsigned int subdet;   // 0-1-2-3 for EMB-EMEC-HEC-FCAL
    unsigned int layer_min;
    unsigned int layer_max;
    float eta_min;
    float eta_max;
    float phi_min;
    float phi_max;
    float corr;
  };

  std::vector<HVfix_t> m_HVfix;    
  std::vector<std::string> m_fixHVStrings;

  enum HVPARTITION {FCAL=0,HEC,EMECIW,EMECOW,EMECPS,EMB,EMBPS,NHVPARTITION}; //x2 for the side  
  typedef std::vector<std::pair<IdentifierHash,IdentifierHash> > HASHRANGEVEC;
  HASHRANGEVEC m_hashRanges[2];//x2 for the side
  HASHRANGEVEC m_completeRange;
  HASHRANGEVEC electrodeIDsToPartition(const std::vector<HWIdentifier>& electrodeIDvec) const;
  StatusCode getScale(const HASHRANGEVEC& hashranges) const;
};

#endif
