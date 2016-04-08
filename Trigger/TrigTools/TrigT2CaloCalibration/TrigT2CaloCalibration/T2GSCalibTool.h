/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2GSCalibTool.h
// PACKAGE:  Trigger/TrigTools/TrigT2CaloCalibration
//
// AUTHOR:   N. Anjos <nuno.anjos@cern.ch>
//
//   Tool to apply Global Sequential Calibration for L2 trigger jets. 
//   It is based in JetCalibTools/JetPropertyInvertResponseTool.
//   Correction factors are defined through jobO.
//
// ********************************************************************

#ifndef TRIGT2CALOCALIBRATION_T2GSCALIBTOOL_H
#define TRIGT2CALOCALIBRATION_T2GSCALIBTOOL_H

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/Property.h"

#include "CaloRec/ToolWithConstantsMixin.h"

#include "TrigT2CaloCalibration/IT2HadCalibTool.h"

#include <vector>
#include <math.h>

class T2GSCalibTool : virtual public IT2HadCalibTool, 
                               public AthAlgTool,
                               public CaloRec::ToolWithConstantsMixin 
{
 public:
   /** Constructor */
   T2GSCalibTool(const std::string&,const std::string&,const IInterface*); 
   /** Destructor */
   ~T2GSCalibTool();
   /** athena initialize method */
   StatusCode initialize();
   /** athena finalize method */
   StatusCode finalize(); 
   
   // These ones must be redefined for ToolWithConstantsMixin
   using AthAlgTool::setProperty;
   virtual StatusCode setProperty (const std::string& propname,
                                   const std::string& value);  
   virtual StatusCode setProperty (const Property& p); 

   /** @brief Returns the response for each calo layer 
   * @param[in] Fraction of jet energy deposited in the layer
   * @param[in] Jet energy after JES calibration
   * @param[in] Eta position of the jet
   */
   double c_energy(double fLayer,double jetEnergy,double eta);
   double c_febenergy(double fLayer,double jetEnergy,double eta);

 private:
   
   MsgStream m_log;

   CaloRec::Array<3>                 m_calibConstants;
   std::vector<double>               m_etaBins;
   std::vector<std::vector<double> > m_ptBins;
   std::vector<std::vector<double> > m_propertyBins;

   double                   m_etaMin;
   double                   m_etaMax;
   double                   m_etaGapMin;
   double                   m_etaGapMax;
   double                   m_ptMin;
   double                   m_ptMax;

   double CalculateCorrection(double jetProperty, 
			      double pT, double eta);

   double interpolate(double pT, double jetProperty,
		      std::vector<double> ptBins,
		      std::vector<double> propertyBins,
		      CaloRec::Array<2> responseValue);

   double extrapolate(double pT, double jetProperty,
		      std::vector<double> ptBins,
		      std::vector<double> propertyBins,
		      CaloRec::Array<2> responseValue);

   double bilinearInterpolation(double x1, double y1, 
				double x2, double y2, 
				double z11,double z12,
				double z21,double z22,
				double x, double y);

   double linearInterpolation(double x1, double x2, 
			      double z1,double z2,
			      double x);

};

#endif
