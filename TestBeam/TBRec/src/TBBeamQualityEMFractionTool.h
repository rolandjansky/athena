/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################
//#                                                   #
//# File   : TBBeamQualityEMFractionTool.h            #
//# Package: TBRec                                    #
//#                                                   #
//# Author : Marco Bieri <mbieri@sfu.ca>              #
//#                                                   #
//# Created: July 19/2005                             #
//#####################################################

#ifndef TBBEAMQUALITYEMFRACTIONTOOL_H
#define TBBEAMQUALITYEMFRACTIONTOOL_H

#include "TBBeamQualityTool.h"

//#include "LArDetDescr/LArDetDescrManager.h"

#include "CaloIdentifier/CaloID.h"

class TBBeamQualityEMFractionTool : public TBBeamQualityTool
{
 public:
  TBBeamQualityEMFractionTool(const std::string& type,
			      const std::string& name,
				const IInterface* parent);
  
  virtual ~TBBeamQualityEMFractionTool();
  
  virtual StatusCode initializeTool();
  
  virtual StatusCode accept(std::vector<std::string>);
  
 protected:
  
  StoreGateSvc* m_StoreGate;
  StoreGateSvc* m_detStore;
  
  //LArDetDescrManager* m_larMgr;

  // Identifier Pointers
  const LArEM_ID* m_emecID_help;
  const LArHEC_ID* m_hecID_help;
  const LArFCAL_ID* m_fcalID_help;
  
  // Properties
  
  float m_em_fraccut; // cut ration
  std::vector<double> m_em_fracnumsamp; // sample: numerator
  std::vector<double> m_em_fracdensamp; // sample: denominator
  std::string m_em_fracnumdet;          // detector: numerator
  std::string m_em_fracdendet;          // detector: denominator
  std::map<float,float> m_Mlayer;
  
  float m_em_frac;
  
};
#endif
