/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2JESCalibTool.h
// PACKAGE:  Trigger/TrigTools/TrigT2CaloCalibration
//
// AUTHOR:   N. Anjos <nuno.anjos@cern.ch>
//
//   Tool to apply a JES calibration for L2 trigger jets. 
//   It is based in JetCalibTools/EtaMassEnergyCorr.
//   Correction factors are defined through jobO.
//
// ********************************************************************

#ifndef TRIGT2CALOCALIBRATION_T2JESCALIBTOOL_H
#define TRIGT2CALOCALIBRATION_T2JESCALIBTOOL_H

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/Property.h"

#include "CaloUtils/ToolWithConstants.h"

#include "TrigT2CaloCalibration/IT2HadCalibTool.h"

#include <vector>
#include <math.h>

class T2JESCalibTool :
  public extends<CaloUtils::ToolWithConstants<AthAlgTool>,
                 IT2HadCalibTool>
{
 public:
   /** Constructor */
   T2JESCalibTool(const std::string&,const std::string&,const IInterface*); 
   /** Destructor */
   virtual ~T2JESCalibTool();
   /** athena initialize method */
   virtual StatusCode initialize() override;
   /** athena finalize method */
   virtual StatusCode finalize() override; 
   
   // both energy arguments must be transverse energy in MeV   
   /** @brief Returns a calibrated energy using JES correction 
   * @param[in] Sum of energy for all EM samplings (except sampling 3)
   * @param[in] Sum of energy for all HAD samplings (+ EM sampling 3)
   * @param[in] Eta position of the cluster
   */
   virtual double c_energy(double EMenergy,double HADenergy,double eta) override;
   virtual double c_febenergy(double EMenergy,double HADenergy,double eta) override;
   
 private:
   
   MsgStream m_log;

   Constant<CxxUtils::Array<2> > m_EnergyCorrConstants { this, "JES_Factors" };
   FloatProperty m_etcut { this, "MinEtCut", 10 }; // Non-standard... in GeV!

   int GetEtaBin(double jet_eta);
   double PolLogE(const CaloRec::Array<2> &constants, unsigned bin, double E);
   double GetJES(double jet_e, double jet_eta);

};

#endif
