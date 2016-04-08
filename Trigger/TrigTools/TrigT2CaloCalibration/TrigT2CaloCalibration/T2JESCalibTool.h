/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "CaloRec/ToolWithConstantsMixin.h"

#include "TrigT2CaloCalibration/IT2HadCalibTool.h"

#include <vector>
#include <math.h>

class T2JESCalibTool : virtual public IT2HadCalibTool, 
                               public AthAlgTool,
                               public CaloRec::ToolWithConstantsMixin 
{
 public:
   /** Constructor */
   T2JESCalibTool(const std::string&,const std::string&,const IInterface*); 
   /** Destructor */
   ~T2JESCalibTool();
   /** athena initialize method */
   StatusCode initialize();
   /** athena finalize method */
   StatusCode finalize(); 
   
   // These ones must be redefined for ToolWithConstantsMixin
   using AthAlgTool::setProperty;
   virtual StatusCode setProperty (const std::string& propname,
                                   const std::string& value);  
   virtual StatusCode setProperty (const Property& p); 

   // both energy arguments must be transverse energy in MeV   
   /** @brief Returns a calibrated energy using JES correction 
   * @param[in] Sum of energy for all EM samplings (except sampling 3)
   * @param[in] Sum of energy for all HAD samplings (+ EM sampling 3)
   * @param[in] Eta position of the cluster
   */
   double c_energy(double EMenergy,double HADenergy,double eta);
   double c_febenergy(double EMenergy,double HADenergy,double eta);
   
 private:
   
   MsgStream m_log;

   CaloRec::Array<2> m_EnergyCorrConstants;
   double m_etcut;

   int GetEtaBin(double jet_eta);
   double PolLogE(const CaloRec::Array<2> &constants, unsigned bin, double E);
   double GetJES(double jet_e, double jet_eta);

};

#endif
