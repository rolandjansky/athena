/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2SampCalibTool.h
// PACKAGE:  Trigger/TrigTools/TrigT2CaloCalibration
//
// AUTHORS:   C. Osuna <Carlos.Osuna@ifae.es>
//            P. Conde <Patricia,Conde.Muino@cern.ch>
//
// MODIFIED:  M. Schram <malachi.schram@cern.ch>
//
//   Tool to apply a sampling calibration for jets/taus. 
//   It is based on what is done in JetCalib package for offline.
//   Weights are being specified through jobO.
//
//   New conditional database functionality added via DataHandle & handle() method
//
// ********************************************************************

#ifndef TRIGT2CALOCALIBRATION_T2SAMPCALIBTOOL_H
#define TRIGT2CALOCALIBRATION_T2SAMPCALIBTOOL_H

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/Property.h"
#include "StoreGate/DataHandle.h"

#include "GaudiKernel/IIncidentListener.h"

#include "TrigT2CaloCalibration/IT2HadCalibTool.h"
#include "TrigT2CaloCalibration/T2CaloJetCalib_dBObj.h"

#include <vector>
#include <math.h>

class StoreGateSvc;//Needed for DetStore
class IAthenaOutputStreamTool;//Needed to write to POOL/root file
class IIOVRegistrationSvc;//Needed for register with COOL
class IIncidentSvc;//Need to retrieve dB at initialize

class T2SampCalibTool : virtual public IT2HadCalibTool, public AthAlgTool, virtual public IIncidentListener  {
 public:
  /** Constructor */
  T2SampCalibTool(const std::string&,const std::string&,const IInterface*); 
  /** Destructor */
  ~T2SampCalibTool();
  /** athena initialize method */
  StatusCode initialize();
  /** athena finalize method */
   StatusCode finalize(); 
   
   // both energy arguments must be transverse energy in MeV   
   /** @brief Returns a calibrated energy using Sampling calibration 
   * @param[in] Sum of energy for all EM samplings (except sampling 3)
   * @param[in] Sum of energy for all HAD samplings (+ EM sampling 3)
   * @param[in] Eta position of the cluster
   */
   double c_energy(double EMenergy,double HADenergy,double eta);
   double c_febenergy(double EMenergy,double HADenergy,double eta);
   
 private:
   
   MsgStream m_log;
   
   void get_weight(double e, double CaloWeight[2]);
   
   std::vector<double> m_etareg;
   std::vector<double> m_vecut;
   std::vector<double> m_Weights;
   double m_wt[52][2][4];
   
   int m_nbin_eta;
   int m_ient;
   int m_ieta;
   int m_nreg;
   int m_nSamp;
   int m_nEReg;
   double m_ecut;
   double m_etcut;
   double m_etmid;
   
   //What I think I need to create object to dB (April 9th 2008)
   bool m_createDbObj;
   bool m_printDbObj;
   bool m_readFromDb;

   std::string m_obj_key;
   const DataHandle<T2CaloJetCalib_dBObj> m_lvl2_calib_handle;

   //Need detector store for dB
   StoreGateSvc* m_detStore;
   //Global dB object for reading
   T2CaloJetCalib_dBObj* m_lvl2_calib;


   ///////////////////////////////////
   // Methods used to write dB info //
   ///////////////////////////////////
   StatusCode createCondObject();

   //////////////////////////////////
   // Methods used to read dB info //
   //////////////////////////////////
   StatusCode readFromDb();
   StatusCode assignWeights();
   virtual void handle(const Incident& incident);

   /////////////////////////////////
   // Method to print out dB info //
   /////////////////////////////////
   StatusCode printCondObjects();

};

#endif
