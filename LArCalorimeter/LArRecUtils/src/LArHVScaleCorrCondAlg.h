/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// Rewritten from LArHVScaleCorrTool

#ifndef LARRECUTILS_LARHVSCALECORRCONDALG_H
#define LARRECUTILS_LARHVSCALECORRCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ICondSvc.h"

#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/LArID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "LArRecConditions/LArHVData.h"
#include "LArRecConditions/LArHVIdMapping.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include "LArRawConditions/LArHVScaleCorrComplete.h"
#include "LArRawConditions/LArTdriftComplete.h"
#include "LArCOOLConditions/LArHVScaleCorrFlat.h"
#include "LArRecConditions/LArHVCorr.h"

class StoreGateSvc; 
class LArElectrodeID;

class LArHVScaleCorrCondAlg: public AthAlgorithm
{
 public:
  
  // constructor
  LArHVScaleCorrCondAlg(const std::string& name, ISvcLocator* pSvcLocator); 
  
  // destructor 
  virtual ~LArHVScaleCorrCondAlg();
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

 private:

  ServiceHandle<ICondSvc> m_condSvc;
  const LArOnlineID*    m_lar_on_id; 	
  const CaloCell_ID*    m_calocell_id;	
  const LArEM_ID*       m_larem_id;	
  const LArHEC_ID*      m_larhec_id;	
  const LArFCAL_ID*     m_larfcal_id;	
  const LArElectrodeID* m_electrodeID;  
  const CaloDetDescrManager* m_calodetdescrmgr;

  SG::ReadCondHandleKey<LArOnOffIdMapping>  m_cablingKey {this,"keyCabling", "LArOnOffIdMap", "Input key for Id mapping"} ;  
  SG::ReadCondHandleKey<LArHVData> m_hvKey {this, "keyHVdata", "LArHVData", "Input key for HV data from DCS"};
  SG::ReadCondHandleKey<ILArHVScaleCorr> m_onlineHVScaleCorrKey{this, "keyOnlineHVCorr", "LArHVScaleCorr","Input key for HVScaleCorr from conditions database (used online)"};

  SG::WriteCondHandleKey<ILArHVScaleCorr> m_outputHVScaleCorrKey{this, "keyOutputCorr", "LArHVScaleCorrRecomputed","Output key for LArHVScaleCorr"};


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


  unsigned int m_deltatupdate;
  bool m_updateIfChanged;
  bool m_undoOnlineHVCorr;

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
  HASHRANGEVEC cellsIDsToPartition(const std::set<Identifier>& cellsIDvec) const;
  StatusCode getScale(const HASHRANGEVEC& hashranges, 
		      std::vector<float> &vScale, const LArHVData* hvdata, const ILArHVScaleCorr *onlHVCorr, 
		      const LArOnOffIdMapping* cabling) const;

};

#endif
