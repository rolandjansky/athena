//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// PAUconvIPTool.h
// author: F.Polci <francesco.polci@cern.ch>
///////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef PHOTONANALYSISUTILS_CONVIPTOOL_H
#define PHOTONANALYSISUTILS_CONVIPTOOL_H


// Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

#include "PhotonAnalysisUtils/IPAUconvIPTool.h"
#include "CaloDetDescr/CaloDepthTool.h"

#include <string>


class IBeamCondSvc;
class CaloDepthTool;

class PAUconvIPTool : public AthAlgTool, virtual public IPAUconvIPTool {
  
public:

  /** AlgTool like constructor */
  PAUconvIPTool(const std::string&,const std::string&,const IInterface*);
  
  /**Virtual destructor*/
  virtual ~PAUconvIPTool();
  
  /** AlgTool initialize method.*/
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();

  const std::vector<float> convIP(const egamma* g) const;  

private:
  std::string m_beamCondSvcName;  //get beam spot and extrapolate perigee  
  IBeamCondSvc* m_beamCondSvc;

  ToolHandle<CaloDepthTool>  m_calodepth;

};



#endif // PHOTONANALYSISUTILS_CONVIPTOOL_H
