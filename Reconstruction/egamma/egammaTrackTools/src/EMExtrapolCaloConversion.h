/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EMEXTRAPOLCALOCONVERSION_H
#define EMEXTRAPOLCALOCONVERSION_H

//  @class EMExtrapolCaloConversion
//         Calculate eta direction for conversions using a line from Calo 1st sampling+conversion vertex
//   @author M. Escalier

/********************************************************************
NAME:     EMExtrapolCaloConversion.h 
PACKAGE:  To Decide
CREATED:  Oct 2009
Updated: 
********************************************************************/

//======================================

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "egammaInterfaces/IEMExtrapolCaloConversion.h"

#include "xAODTracking/VertexFwd.h"
#include "xAODEgamma/EgammaFwd.h"


class CaloDepthTool;



class EMExtrapolCaloConversion : public AthAlgTool, 
				 virtual public IEMExtrapolCaloConversion 
{
 public:
  
  EMExtrapolCaloConversion(const std::string& type,
			 const std::string& name,
			 const IInterface* parent);
  
  ~EMExtrapolCaloConversion();
  
  StatusCode initialize();
  
  virtual StatusCode execute(const xAOD::Egamma *eg,const xAOD::Vertex* myvxcandidate,double error_etaS1);
    
  //    virtual StatusCode execute(const CaloCluster* cluster);
  StatusCode finalize();
  
  double returnEta(void) const;
  double returnErrorEta(void) const;
  
 private:  
  //float m_PI;
  
  double m_eta;
  double m_error_eta;
  ToolHandle<CaloDepthTool> m_egammaqdepth;
  std::string m_CaloDepth;
};

inline double EMExtrapolCaloConversion::returnEta(void)   const { return m_eta;}
inline double EMExtrapolCaloConversion::returnErrorEta(void)   const { return m_error_eta;}

#endif 
