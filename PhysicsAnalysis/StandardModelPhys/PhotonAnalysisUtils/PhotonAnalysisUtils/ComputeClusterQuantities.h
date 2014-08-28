/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COMPUTECLUSTERQUANTITIES_H
#define COMPUTECLUSTERQUANTITIES_H

//  @class ComputeClusterQuantities
//         Tool to compute cluster related quantities
//   @author JF Marchand

/********************************************************************
NAME:     ComputeClusterQuantities.h 
PACKAGE:  PhotonAnalysisUtils
CREATED:  Dec 2011
Updated: 
********************************************************************/

//======================================

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "PhotonAnalysisUtils/IComputeClusterQuantities.h"

#include "CaloIdentifier/CaloCell_ID.h"


class ComputeClusterQuantities : public AthAlgTool, virtual public IComputeClusterQuantities {
 public:
  
  ComputeClusterQuantities(const std::string& type,
			   const std::string& name,
			   const IInterface* parent);
  
  ~ComputeClusterQuantities();
  
  StatusCode initialize();
  
  virtual StatusCode execute(const egamma *eg);
    
  StatusCode finalize();
  
  double return_eS0_real(void)   const;
  double return_eS1_real(void)   const;
  double return_eS2_real(void)   const;
  double return_eS3_real(void)   const;
  double return_EcellS0(void)    const;
  double return_etacellS0(void)  const;
  
 private:  

  double m_eS0_real;
  double m_eS1_real;
  double m_eS2_real;
  double m_eS3_real;
  double m_EcellS0;
  double m_etacellS0;

  const CaloCell_ID* m_calo_helper;

};

inline double ComputeClusterQuantities::return_eS0_real(void)  const { return m_eS0_real; }
inline double ComputeClusterQuantities::return_eS1_real(void)  const { return m_eS1_real; }
inline double ComputeClusterQuantities::return_eS2_real(void)  const { return m_eS2_real; }
inline double ComputeClusterQuantities::return_eS3_real(void)  const { return m_eS3_real; }
inline double ComputeClusterQuantities::return_EcellS0(void)   const { return m_EcellS0;  }
inline double ComputeClusterQuantities::return_etacellS0(void) const { return m_etacellS0;}

#endif 
