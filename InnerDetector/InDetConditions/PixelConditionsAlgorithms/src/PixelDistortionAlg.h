/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDISTORTIONALG_H
#define PIXELDISTORTIONALG_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "DetDescrConditions/DetCondCFloat.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelDistortionData.h"

#include "InDetIdentifier/PixelID.h"
#include "AthenaKernel/IAthRNGSvc.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/Property.h"

#include "CLHEP/Units/SystemOfUnits.h"

class PixelDistortionAlg : public AthAlgorithm {  
  public:
    PixelDistortionAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelDistortionAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;

  private:
    /** @brief Flag controlling how module distortions are taken into account:
      case 0 -----> No distorsions implemented;
      case 1 -----> Set curvature (in 1/meter) and twist (in radiant) equal for all modules;
      case 2 -----> Read curvatures and twists from textfile containing Survey data;
      case 3 -----> Set curvature and twist from Gaussian random generator with mean and RMS coming from Survey data;
      case 4 -----> Read curvatures and twists from database;
     */
    Gaudi::Property<int> m_inputSource
    {this, "InputSource", 4, "Source of module distortions: 0 (none), 1 (constant), 2 (text file), 3 (random), 4 (database)"};

    Gaudi::Property<int> m_version
    {this, "Version", 1, "Version number"};

    Gaudi::Property<double> m_R1
    {this, "R1", 0.1/CLHEP::meter, "Fixed distortion parameters: radius of curvature"}; //corresponding to a sagitta of 50 um
    
    Gaudi::Property<double> m_R2
    {this, "R2", 0.1/CLHEP::meter, "Fixed distortion parameters: radius of curvature"}; //corresponding to a sagitta of 50 um

    Gaudi::Property<double> m_twist
    {this, "tantwist", 0.0005,"Fixed distortion parameters: twist angle"}; 

    Gaudi::Property<double> m_mean_R
    {this, "Mean_R", 0.12/CLHEP::meter, "Random distortion parameters: Mean of radius of curvature"}; //Mean value from Survey
    
    Gaudi::Property<double> m_RMS_R
    {this, "RMS_R", 0.08/CLHEP::meter, "Random distortion parameters: RMS of curvature radius"}; //RMS value from Survey

    Gaudi::Property<double> m_mean_twist
    {this, "Mean_twist", -0.0005,"Random distortion parameters: Mean twist angle"}; //Mean value from Survey

    Gaudi::Property<double> m_RMS_twist
    {this, "RMS_twist", 0.0008,"Random distortion parameters: RMS of twist angle"}; //RMS value from Survey

    Gaudi::Property<bool> m_write
    {this, "WriteData", false, "Record data in storegate"}; 

    Gaudi::Property<std::string> m_textFileName
    {this, "TextFileName", "PixelSurveyDistortions.txt","Read distortions from this file"}; 

    const PixelID* m_pixelID;

    ServiceHandle<IAthRNGSvc> m_rndmSvc
    {this, "RndmSvc", "AthRNGSvc", ""};  //!< Random number service

    ServiceHandle<ICondSvc> m_condSvc
    {this, "CondSvc", "CondSvc", ""};

    SG::ReadCondHandleKey<DetCondCFloat> m_readKey
    {this, "ReadKey", "/Indet/PixelDist", "Input readout distortion folder"};

    SG::WriteCondHandleKey<PixelDistortionData> m_writeKey
    {this, "WriteKey", "PixelDistortionData", "Output readout distortion data"};

};

#endif
