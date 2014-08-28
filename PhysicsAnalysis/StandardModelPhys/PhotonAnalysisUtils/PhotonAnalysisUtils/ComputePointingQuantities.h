/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COMPUTEPOINTINGQUANTITIES_H
#define COMPUTEPOINTINGQUANTITIES_H

//  @class ComputePointingQuantities
//         Compute pointing related quantities for photons
//   @author JF Marchand

/********************************************************************
NAME:     ComputePointingQuantities.h 
PACKAGE:  PhotonAnalysisUtils
CREATED:  Dec 2011
Updated: 
********************************************************************/

//======================================

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "PhotonAnalysisUtils/IComputePointingQuantities.h"
#include "CaloDetDescr/CaloDepthTool.h"
//#include "TrkExInterfaces/IExtrapolator.h" // getting beam spot
//#include "egammaInterfaces/IEMClusterErrorsTool.h"

#include "egammaEvent/EMErrorDetail.h"


class IEMClusterErrorsTool;
class IEMExtrapolCaloConversion;
class IExtrapolCaloConversion;
class IBeamCondSvc;
class CaloDepthTool;

class ComputePointingQuantities : public AthAlgTool, virtual public IComputePointingQuantities {
 public:
  
  ComputePointingQuantities(const std::string& type,
			    const std::string& name,
			    const IInterface* parent);
  
  ~ComputePointingQuantities();
  
  StatusCode initialize();
  
  virtual StatusCode execute(const egamma *eg);
    
  StatusCode finalize();
  
  double return_CaloPointing_eta(void) const;
  double return_CaloPointing_sigma_eta(void) const;
  double return_CaloPointing_zvertex(void) const;
  double return_CaloPointing_sigma_zvertex(void) const;

  double return_HPV_eta(void) const;
  double return_HPV_sigma_eta(void) const;
  double return_HPV_zvertex(void) const;
  double return_HPV_sigma_zvertex(void) const;

  
 private:  
  double m_PI;
  
  double m_CaloPointing_eta;
  double m_CaloPointing_sigma_eta;
  double m_CaloPointing_zvertex;
  double m_CaloPointing_sigma_zvertex;

  double m_HPV_eta;
  double m_HPV_sigma_eta;
  double m_HPV_zvertex;
  double m_HPV_sigma_zvertex;


  std::string m_EMErrorDetailContainerName;


  //ToolHandle<IEMClusterErrorsTool> m_EMClusterErrorsTool;
  ToolHandle<IEMExtrapolCaloConversion> m_EMExtrapolCaloConversion;
  ToolHandle<IExtrapolCaloConversion> m_PAUExtrapolCaloConversion;
  ToolHandle<CaloDepthTool> m_calodepth;

  std::string m_beamCondSvcName;  //get beam spot and extrapolate perigee  
  IBeamCondSvc* m_beamCondSvc;


};

inline double ComputePointingQuantities::return_CaloPointing_eta(void)           const { return m_CaloPointing_eta;}
inline double ComputePointingQuantities::return_CaloPointing_sigma_eta(void)     const { return m_CaloPointing_sigma_eta;}
inline double ComputePointingQuantities::return_CaloPointing_zvertex(void)       const { return m_CaloPointing_zvertex;}
inline double ComputePointingQuantities::return_CaloPointing_sigma_zvertex(void) const { return m_CaloPointing_sigma_zvertex;}

inline double ComputePointingQuantities::return_HPV_eta(void)           const { return m_HPV_eta;}
inline double ComputePointingQuantities::return_HPV_sigma_eta(void)     const { return m_HPV_sigma_eta;}
inline double ComputePointingQuantities::return_HPV_zvertex(void)       const { return m_HPV_zvertex;}
inline double ComputePointingQuantities::return_HPV_sigma_zvertex(void) const { return m_HPV_sigma_zvertex;}

#endif 
