/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARRECUTILS_LARHVSCALECORRTOOL_H
#define LARRECUTILS_LARHVSCALECORRTOOL_H

#include <string>
#include <vector>
#include "GaudiKernel/MsgStream.h"

class CaloCell_ID;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class CaloDetDescrElement; 

class LArHVScaleCorrTool {
public:

  //Internal representation of voltage & current per cell: 
  struct HV_t {
    HV_t(float ihv, float iweight) : hv(ihv), weight(iweight) {};
    float hv=0.0; //voltage, potentially current*R corrected
    float weight=0.0;
  };

  typedef std::vector<HV_t> voltageCell_t;


  LArHVScaleCorrTool(const CaloCell_ID* caloCellID, MsgStream& msg, const std::vector<std::string>& fixHVStrings);
  
  LArHVScaleCorrTool() = delete;

   // destructor 
  virtual ~LArHVScaleCorrTool() = default;
  
  float getHVScale(const CaloDetDescrElement* calodde,  const voltageCell_t& hvlist,
                   MsgStream& msg) const;

 private:

  const LArEM_ID*       m_larem_id;	
  const LArHEC_ID*      m_larhec_id;	
  const LArFCAL_ID*     m_larfcal_id;	
  const float           m_T0;


  float Scale_barrel(const float hv) const;
  float Scale_FCAL1(const float hv) const;
  float champ_e(float hv, float d) const;
  float vdrift(float e,float tempe) const;  
  float InvCharge(float e) const;  
  float Respo(float e,float e_nominal,float tempe) const;  
  float t_drift (float e, float e_nominal,float d, float tempe) const;  
  float EMEC_nominal(const float eta_r) const;    
  float EMEC_gap(const float eta_r, float Zeta) const;
  void buildFixHVList(const std::vector<std::string>& fixHVStrings, MsgStream& msg);



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

};

#endif
