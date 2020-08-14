/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelDigitization/EnergyDepositionTool.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date January, 2020
 * @brief Provides energy deposition in sensor. Default:Bichsel model
 */

#ifndef PIXELDIGITIZATION_EnergyDepositionTool_H
#define PIXELDIGITIZATION_EnergyDepositionTool_H

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CLHEP/Random/RandomEngine.h"
#include "GaudiKernel/ToolHandle.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/Service.h"
#include "HitManagement/TimedHitPtr.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"

#include "InDetIdentifier/PixelID.h"
#include "PixelConditionsData/PixelDistortionData.h"
#include "StoreGate/ReadCondHandleKey.h"

//=============================
// C U S T O M   S T R U C T 
//=============================
struct BichselData {
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

    std::vector<std::pair<double,double> > BichselSim(double BetaGamma, int ParticleType, double TotalLength, double InciEnergy, CLHEP::HepRandomEngine *rndmEngine) const;   // output hit record in the format (hit position, energy loss)

    std::vector<std::pair<double,double> > ClusterHits(std::vector<std::pair<double,double> >& rawHitRecord, int n_pieces) const;         // cluster hits into n steps (there could be thousands of hit)
    int trfPDG(int pdgId) const;                                                             // convert pdgId to ParticleType. If it is unsupported particle, -1 is returned.

    virtual StatusCode depositEnergy(const TimedHitPtr<SiHit> &phit, const InDetDD::SiDetectorElement &Module, 
        std::vector<std::pair<double,double> > &trfHitRecord, std::vector<double> &initialConditions, CLHEP::HepRandomEngine *rndmEngine);

    // Variables
  private:
    EnergyDepositionTool();

    const PixelID* m_pixelID{nullptr};

    std::vector<BichselData> m_BichselData;      // vector to store Bichsel Data. Each entry is for one particle type

    Gaudi::Property<int> m_numberOfSteps
    {this, "numberOfSteps", 50, "Geant4:number of steps for PixelPlanar"};

    Gaudi::Property<int> m_numberOfCharges
    {this, "numberOfCharges", 10, "Geant4:number of charges for PixelPlanar"};

    Gaudi::Property<bool> m_disableDistortions
    {this, "DisableDistortions", false, "Disable simulation of module distortions"};

    Gaudi::Property<bool> m_doBichsel
    {this, "doBichsel", true, "re-do charge deposition following Bichsel model"};

    Gaudi::Property<double> m_doBichselBetaGammaCut
    {this, "doBichselBetaGammaCut", 0.1, "minimum beta-gamma for particle to be re-simulated through Bichsel Model"};

    Gaudi::Property<bool> m_doDeltaRay
    {this, "doDeltaRay", false, "whether we simulate delta-ray using Bichsel model"};

    Gaudi::Property<double> m_DeltaRayCut
    {this, "DeltaRayCut", 117.0, "Cut of delta ray [keV]"};

    Gaudi::Property<bool> m_doPU
    {this, "doPU", true, "Whether we apply Bichsel model on PU"};

    Gaudi::Property<int> m_nCols
    {this, "nCols", 1, "Number of collision for each sampling"};

    Gaudi::Property<int> m_LoopLimit
    {this, "LoopLimit", 100000, "Limit assuming 1 collision per sampling"};

    SG::ReadCondHandleKey<PixelDistortionData> m_distortionKey
    {this, "PixelDistortionData", "PixelDistortionData", "Output readout distortion data"};

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
