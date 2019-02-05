/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RadDamageUtil.h
//   Header file for class RadDamageUtil
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
// Author: Ben Nachman, Rebecca Carney
// 2018-07 
// Description:
///////////////////////////////////////////////////////////////////


#ifndef PIXELDIGITIZATION_RADDAMAGEUTIL_H
#define PIXELDIGITIZATION_RADDAMAGEUTIL_H

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CLHEP/Random/RandomEngine.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/Service.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"

#include "TH3.h"
#include "TH2.h"
#include "TH1.h"

#include "EfieldInterpolator.h"

namespace RadDam{

//==================== 
//  C L A S S   D E F
//====================
class RadDamageUtil : public AthAlgTool {

public:
  
  RadDamageUtil( const std::string& type, const std::string& name,const IInterface* parent);

  static const InterfaceID& interfaceID() ;
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual ~RadDamageUtil();
  StatusCode initTools();
  const StatusCode generateRamoMap(TH3F* ramPotentialMap, InDetDD::PixelModuleDesign* module);
  const StatusCode generateEfieldMap(TH1F* eFieldMap, InDetDD::PixelModuleDesign* module);
  StatusCode generateEfieldMap(TH1F* &eFieldMap, InDetDD::PixelModuleDesign* module, double fluence, double biasVoltage, int layer, std::string TCAD_list, bool interpolate);
  const StatusCode generateDistanceTimeMap( TH2F* &distanceMap_e, TH2F* &distanceMap_h, TH1F* &timeMap_e, TH1F* &timeMap_h, TH2F* &lorentzMap_e, TH2F* &lorentzMap_h, TH1F* &eFieldMap, InDetDD::PixelModuleDesign* module);
  
  const std::pair<double,double> getTrappingTimes( double fluence ) const;
  const std::pair<double,double> getMobility( double electricField, double temperature) const;
  double getTanLorentzAngle(double electricField, double temperature, double bField,  bool isHole);

  int m_defaultRamo; //TODO: need to decide what we want to do with this.
  int m_defaultEField; //TODO: need to decide what we want to do with this.
  
  ToolHandle<EfieldInterpolator>                     m_EfieldInterpolator;

private:
  RadDamageUtil();

  double m_betaElectrons; //TODO: should be replaced my DB version 
  double m_betaHoles; //TODO: should be replaced my DB version 

  double alpha(int n, int Nrep, double a); //Poisson solution factor
  double weighting3D(double x, double y, double z, int n, int m, int Nrep, double a, double b);
  double weighting2D(double x, double z, double Lx, double sensorThickness);

public:
  bool m_saveDebugMaps;

protected:
  ServiceHandle<IAtRndmGenSvc> m_rndmSvc;
  std::string                  m_rndmEngineName;
  CLHEP::HepRandomEngine*      m_rndmEngine;
 };

}
#endif //PIXELDIGITIZATION_RADDAMAGEUTIL_H
