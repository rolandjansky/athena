/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDT_DIGITIZATION_RT_RELATION_DIGITOOL_H
#define MDT_DIGITIZATION_RT_RELATION_DIGITOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MDT_Digitization/MdtDigiToolOutput.h"
#include "MDT_Digitization/IMDT_DigitizationTool.h"

#include "GaudiKernel/RndmGenerators.h"

#include "MDT_Digitization/r_to_t_converter.h"

/*-----------------------------------------------
  
   Created 7-5-2004 by Niels van Eldik

 Digitization tool which uses an rt-relation + smearing to convert MDT digitization 
 input quantities into the output

-----------------------------------------------*/
namespace MuonGM{
  class MuonDetectorManager;
}
class MdtIdHelper;


class RT_Relation_DigiTool : public AthAlgTool, 
			      virtual public IMDT_DigitizationTool {
 public:
  RT_Relation_DigiTool( const std::string& type, 
			 const std::string& name, 
			 const IInterface* parent );

  MdtDigiToolOutput digitize(const MdtDigiToolInput& input);
  
  StatusCode initialize();

  bool initializeTube();

 private:
  double getDriftTime(double radius);
  double getAdcResponse();
  bool  isTubeEfficient(double radius);
  
  double m_effRadius;
  double m_maxRadius;

  std::vector <Rt_relation *> m_rt;

  Rndm::Numbers              m_flatDist;

  const MuonGM::MuonDetectorManager* m_muonGeoMgr;
  const MdtIdHelper*         m_idHelper;

};

inline
double  RT_Relation_DigiTool::getDriftTime(double r)
{	
  // Drift radius --> time converter
  R_to_t_converter RTC;

  // Get the time value and return it
  double time = RTC.convert(r, *m_rt[0]);
  if (time < 0) time = 0;
  return (time);
}

inline 
double   RT_Relation_DigiTool::getAdcResponse(){
  return 76.7;
}

inline
bool  RT_Relation_DigiTool::isTubeEfficient(double radius) {
    
  if ((radius < 0) || (radius > m_maxRadius)) return false;
  if (radius < m_effRadius) return true;
  double eff = 1.0 + (radius-m_effRadius)/(m_effRadius-m_maxRadius);
  if (m_flatDist() <= eff) return true;
  
  return false;
}


#endif
