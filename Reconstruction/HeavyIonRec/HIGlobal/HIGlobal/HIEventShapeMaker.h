/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HIEVENTSHAPEMAKER_H__
#define __HIEVENTSHAPEMAKER_H__

#include "AthenaBaseComps/AthAlgorithm.h"
#include "HIGlobal/HICaloHelper.h"
#include <string>
#include "xAODHIEvent/HIEventShapeContainer.h"

#include "GaudiKernel/ToolHandle.h"
#include "IHIEventShapeFiller.h"

class CaloCell;
class TH2D;

class HIEventShapeMaker : public AthAlgorithm
{
public:

  /** Standard Athena-Algorithm Constructor */
  HIEventShapeMaker(const std::string& name, ISvcLocator* pSvcLocator);
  /** Default Destructor */
  ~HIEventShapeMaker() {};


  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  std::string m_tower_container_key;
  std::string m_output_key;
  std::string m_output_aux_key;

  TH2D* h2_eta_maxSamp ;
  TH2D* h2_eta_samp    ;
     
  ToolHandle<IHIEventShapeFiller> m_HIEventShapeFillerTool;
  void PrintHIEventShapeContainer(const xAOD::HIEventShapeContainer *Container);




};

#endif
