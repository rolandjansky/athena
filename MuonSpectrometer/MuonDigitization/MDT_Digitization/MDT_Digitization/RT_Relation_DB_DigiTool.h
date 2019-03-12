/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDT_DIGITIZATION_RT_RELATION_DB_DIGITOOL_H
#define MDT_DIGITIZATION_RT_RELATION_DB_DIGITOOL_H

/*-----------------------------------------------
Created 8-10-2009 by Dinos Bachas
Digitization tool which uses an rt-relation from Conditions Database
Adopted from RT_Relation_DigiTool
-----------------------------------------------*/

#include "MDT_Digitization/MdtDigiToolOutput.h"
#include "MDT_Digitization/IMDT_DigitizationTool.h"
#include "Identifier/Identifier.h"

#include "MdtCalibData/TrRelation.h"
#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibData/IRtResolution.h"

#include "MdtCalibData/MdtRtRelation.h"
#include "MdtCalibSvc/MdtCalibrationDbSvc.h"
#include "MdtCalibData/MdtFullCalibData.h"

#include "CLHEP/Random/RandFlat.h"
//#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandGaussZiggurat.h"

//#include "MuonIdHelpers/MdtIdHelper.h"

#include "AthenaBaseComps/AthAlgTool.h"

namespace MuonGM{
  class MuonDetectorManager;
}

class RT_Relation_DB_DigiTool : public AthAlgTool, virtual public IMDT_DigitizationTool
{
  public:
    //Constructor
    RT_Relation_DB_DigiTool( const std::string& type, const std::string& name, const IInterface* parent );
    
    //Methods
    virtual StatusCode initialize() override;
    virtual MdtDigiToolOutput digitize(const MdtDigiToolInput& input, CLHEP::HepRandomEngine *rndmEngine) override final;
    bool initializeTube();
  
  private:
    //Methods
    double getDriftTime(double radius,Identifier DigitId,CLHEP::HepRandomEngine *rndmEngine) const;
    double getAdcResponse(double radius,CLHEP::HepRandomEngine *rndmEngine) const;
    bool   isTubeEfficient(double radius,CLHEP::HepRandomEngine *rndmEngine) const;
    
    //Data members
    double m_effRadius;
    double m_maxRadius;
    const MuonGM::MuonDetectorManager* m_muonGeoMgr;
    
  protected:
    ServiceHandle<MdtCalibrationDbSvc> m_calibDbSvc;
};


inline bool RT_Relation_DB_DigiTool::isTubeEfficient(double radius,CLHEP::HepRandomEngine *rndmEngine) const
{
  if ((radius < 0) || (radius > m_maxRadius)) return false;
  if (radius < m_effRadius) return true;
  double eff = 1.0 + (radius-m_effRadius)/(m_effRadius-m_maxRadius);
  if (CLHEP::RandFlat::shoot(rndmEngine,0.0, 1.0) <= eff) return true;
  
  return false;
}
#endif
