/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EXTRAPOLCALOCONVERSION_H
#define EXTRAPOLCALOCONVERSION_H

//  @class ExtrapolCaloConversion
//         Calculate eta direction for conversions using a line from Calo 1st sampling+conversion vertex
//   @author M. Escalier

/********************************************************************
NAME:     ExtrapolCaloConversion.h 
PACKAGE:  To Decide
CREATED:  Oct 2009
Updated: 
********************************************************************/

//======================================

#include "CaloDetDescr/CaloDepthTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "PhotonAnalysisUtils/IExtrapolCaloConversion.h"

class CaloCluster;
class CaloDepthTool;
class IBeamCondSvc;

class ExtrapolCaloConversion : public AthAlgTool, virtual public IExtrapolCaloConversion {
 public:
  
  ExtrapolCaloConversion(const std::string& type,
			 const std::string& name,
			 const IInterface* parent);
  
  ~ExtrapolCaloConversion();
  
  StatusCode initialize();
  
  virtual StatusCode execute(const egamma *eg,const Trk::VxCandidate* myvxcandidate,double error_etaS1);
    
  //    virtual StatusCode execute(const CaloCluster* cluster);
  StatusCode finalize();
  
  double returnEta(void) const;
  double returnErrorEta(void) const;
  double returnZvertex(void) const;
  double returnErrorZvertex(void) const;
  
 private:  
  double m_PI;
  
  double m_eta;
  double m_error_eta;
  double m_zvertex;
  double m_error_zvertex;

  ToolHandle<CaloDepthTool>  m_calodepth;
  std::string m_beamCondSvcName;  //get beam spot and extrapolate perigee  
  IBeamCondSvc* m_beamCondSvc;

};

inline double ExtrapolCaloConversion::returnEta(void)          const { return m_eta;}
inline double ExtrapolCaloConversion::returnErrorEta(void)     const { return m_error_eta;}
inline double ExtrapolCaloConversion::returnZvertex(void)      const { return m_zvertex;}
inline double ExtrapolCaloConversion::returnErrorZvertex(void) const { return m_error_zvertex;}

#endif 
