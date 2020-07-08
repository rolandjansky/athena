/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "CaloUtils/ToolWithConstants.h"

#include "TrigT2CaloCalibration/IT2HadCalibTool.h"

#include <vector>
#include <math.h>

class T2GSCalibTool : 
  public extends<CaloUtils::ToolWithConstants<AthAlgTool>,
                 IT2HadCalibTool>
{
 public:
   /** Constructor */
   T2GSCalibTool(const std::string&,const std::string&,const IInterface*); 
   /** Destructor */
   virtual ~T2GSCalibTool();
   /** athena initialize method */
   virtual StatusCode initialize() override;
   /** athena finalize method */
   virtual StatusCode finalize() override;
   
   /** @brief Returns the response for each calo layer 
   * @param[in] Fraction of jet energy deposited in the layer
   * @param[in] Jet energy after JES calibration
   * @param[in] Eta position of the jet
   */
   virtual double c_energy(double fLayer,double jetEnergy,double eta) override;
   virtual double c_febenergy(double fLayer,double jetEnergy,double eta) override;

 private:
   
   MsgStream m_log;

   Constant<CxxUtils::Array<3> >    m_calibConstants { this, "CalibConstants"};
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
