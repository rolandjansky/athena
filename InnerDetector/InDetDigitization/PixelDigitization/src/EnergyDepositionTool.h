/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyDepositionTool.h
//   Header file for class EnergyDepositionTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
// Author: Qi Zeng
// 2015-02-14 (Happy Valentines Day!)
// Description:
//   A general tool to implement Bichsel simultion.
//   Bichsel model can be used to simulate how a single particle deposits energy inside silicon material
//   Instead of a Landau distribution, the simulation is done on collision-by-collision level (i.e. single collision between particle and atom)
//   This model can better describe how deposited charges distribute along incident path
///////////////////////////////////////////////////////////////////


#ifndef PIXELDIGITIZATION_EnergyDepositionTool_H
#define PIXELDIGITIZATION_EnergyDepositionTool_H

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CLHEP/Random/RandomEngine.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/Service.h"
#include "HitManagement/TimedHitPtr.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelConditionsTools/IModuleDistortionsTool.h"
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"

//=============================
// C U S T O M   S T R U C T 
//=============================
struct BichselData
{
  std::vector<double> Array_BetaGammaLog10;
  std::vector<std::vector<double> > Array_BetaGammaLog10_ColELog10;  // ColE = CollisionEnergy in eV
  std::vector<std::vector<double> > Array_BetaGammaLog10_IntXLog10;  // IntX = Integrated Xsection. The unit doesn't matter
  std::vector<double> Array_BetaGammaLog10_UpperBoundIntXLog10;      // upper bound of log10(IntX)
};

//==================== 
//  C L A S S   D E F
//====================
class EnergyDepositionTool : public AthAlgTool {

public:
  
  EnergyDepositionTool( const std::string& type, const std::string& name,const IInterface* parent);

  static const InterfaceID& interfaceID() ;
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual ~EnergyDepositionTool();
  StatusCode initTools();
  
  std::vector<std::pair<double,double> > BichselSim(double BetaGamma, int ParticleType, double TotalLength, double InciEnergy) const;   // output hit record in the format (hit position, energy loss)
  
  std::vector<std::pair<double,double> > ClusterHits(std::vector<std::pair<double,double> >& rawHitRecord, int n_pieces) const;         // cluster hits into n steps (there could be thousands of hit)
  int trfPDG(int pdgId) const;                                                             // convert pdgId to ParticleType. If it is unsupported particle, -1 is returned.
  
  virtual StatusCode depositEnergy(const TimedHitPtr<SiHit> &phit, const InDetDD::SiDetectorElement &Module, std::vector<std::pair<double,double> > &trfHitRecord, std::vector<double> &initialConditions);


// Variables
private:
  EnergyDepositionTool();

  // internal private members //
  double                   m_DeltaRayCut;      // Threshold to identify a delta ray. unit in keV
  std::vector<BichselData> m_BichselData;      // vector to store Bichsel Data. Each entry is for one particle type
  int                      m_nCols;            // number of collisions to simulate each time. This is mainly to save CPU time if necessary
  int                      m_LoopLimit;        // upper limit on number of loops. The default value is optimized for current configuration. People can tune this number in case of ITK upgrade (very forward barrel) or other new situation.
  int    m_numberOfSteps;
  int    m_numberOfCharges;  
  bool				m_disableDistortions;

  bool   m_doBichsel;                                  // re-do charge deposition following Bichsel model ?
  double m_doBichselBetaGammaCut;                      // replace momentum cut
  bool   m_doDeltaRay;                                 // implement Bichsel Model into delta-ray, which does not have truth particle link. 
  bool   m_doPU;                                       // Whether we apply Bichsel model on non-HS particles

  ToolHandle<IModuleDistortionsTool> m_pixDistoTool;

protected:
  ServiceHandle<IAtRndmGenSvc> m_rndmSvc;
  std::string                  m_rndmEngineName;
  CLHEP::HepRandomEngine*      m_rndmEngine;

// Functions
private:
  void simulateBow(const InDetDD::SiDetectorElement * element,double& xi, double& yi, const double zi, double& xf, double& yf, const double zf) const;
  std::pair<int,int> FastSearch(std::vector<double> vec, double item) const;               // A quick implementation of binary search in 2D table
  std::pair<int,int> GetBetaGammaIndices(double BetaGammaLog10, BichselData& iData) const; // get beta-gamma index. This is so commonly used by other functions that a caching would be beneficial
  double GetColE(double BetaGammaLog10, double IntXLog10, BichselData& iData) const;       // return ColE NOT ColELog10 ! unit is eV
  double GetColE(std::pair<int,int> indices_BetaGammaLog10, double IntXLog10, BichselData& iData) const;       // return ColE NOT ColELog10 ! unit is eV
  double GetUpperBound(double BetaGammaLog10, BichselData& iData) const;                   // return IntX upper bound
  double GetUpperBound(std::pair<int,int> indices_BetaGammaLog10, double BetaGammaLog10, BichselData& iData) const;                   // return IntX upper bound
  void SetFailureFlag(std::vector<std::pair<double,double> >& rawHitRecord) const;         // return (-1,-1) which indicates failure in running BichselSim
 };


#endif //PIXELDIGITIZATION_EnergyDepositionTool_H
