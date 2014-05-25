/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloAffectedRegionInfo_H
#define CaloAffectedRegionInfo_H

/**
 * @file  CaloAffectedRegionInfo.h
 * @author M.Escalier, G.Unal
 * @date June, 2009
 * @brief object to store information about affected regions in calorimeter
 */


class CaloAffectedRegionInfo {
 public:

  /** @brief enum to classify different problems */
  enum type_problem {
     missingReadout=0,      //  missing readout
     HVaffected=1,          // HV not nominal
     HVdead=2,              // 0 HV on both sides of electrodes
     CalibrationProblem=3,  // region potentially badly calibrated
     ReadoutErrors=4,       // region with severe readout errors
     ReadoutWarnings=5,     // region with readout warnings
     Unknown=-1
  };

  /**
   * @brief default constructor
   * Creates empty list
   */
  CaloAffectedRegionInfo(void);

 /**
  * @brief constructor with parameters
  * @param parameter_eta_min, parameter_eta_max  eta range of affected region
  * @param parameter_phi_min, parameter_phi_max  phi range of affected region
  * @param parameter_layer_min, parameter_layer_max range of layer affected (could be one or several)
  * @param current_problem   type of problem in the affected region, according to enum
  * @param rate   rate of problem (1 = all the events affected)
  */
  CaloAffectedRegionInfo(float parameter_eta_min,float parameter_eta_max,float parameter_phi_min,float parameter_phi_max,int parameter_layer_min,int parameter_layer_max,type_problem current_problem,float parameter_rate=1.);


 /**
  * @brief modify settings of affected region
  * @param parameter_eta_min, parameter_eta_max  eta range of affected region
  * @param parameter_phi_min, parameter_phi_max  phi range of affected region
  * @param parameter_layer_min, parameter_layer_max range of layer affected (could be one or several)
  * @param current_problem   type of problem in the affected region, according to enum 
  * @param rate   rate of problem (1 = all the events affected)
  */
  void FillCaloAffectedRegionInfo(float  parameter_eta_min ,float parameter_eta_max,float parameter_phi_min ,float  parameter_phi_max,int  parameter_layer_min,int parameter_layer_max ,type_problem current_problem, float parameter_rate=1.);

  /**
   * @brief  print info 
   */
  void PrintInfo(void) const;

  /** @brief get eta min of region */
  float get_eta_min() const;
  /** @brief get eta max of region */
  float get_eta_max() const;
  /** @brief get phi min of region */
  float get_phi_min() const;
  /** @brief get phi max of region */
  float get_phi_max() const;
  /** @brief get layer min of region */
  int get_layer_min() const;
  /** @brief get layer max of region */
  int get_layer_max() const;
  /** @brief get problem type  */
  int get_problem()   const;
  /** @brief get rate of problem */
  float get_rate()    const;

 private:
  float m_eta_min,m_eta_max;
  float m_phi_min,m_phi_max;
  float m_rate;
  int m_layer_min,m_layer_max;
  enum type_problem m_problem;
};

#endif
