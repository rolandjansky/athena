/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HIEVENTSHAPETRIGGERFAST_H__
#define __HIEVENTSHAPETRIGGERFAST_H__

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "string"

#define __HIEVENTSHAPEFAST_DEBUG__
#ifdef __HIEVENTSHAPEFAST_DEBUG__
  class TH1D;
  #include "stdio.h"
  #define HIEventShapeTriggerFast_MSG_DEBUG(...) {printf("HIEventShapeTriggerFast  ::   ");printf(__VA_ARGS__);}
#else
  #define HIEventShapeTriggerFast_MSG_DEBUG(...) {while(0);}
#endif

class HIEventShapeTriggerFast : public AthAlgorithm
{
public:

  /** Standard Athena-Algorithm Constructor */
  HIEventShapeTriggerFast(const std::string& name, ISvcLocator* pSvcLocator);
  /** Default Destructor */
  ~HIEventShapeTriggerFast() {};


  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  enum{
     N_DET=2,//FullCal, FCal
       FULLCAL=0,
       FCAL   =1,
     N_HAR=2,//v2-v3
       HAR2=0,
       HAR3=1,
  };
  std::string m_cell_container_key;
  std::string m_output_key;
  std::string m_outputaux_key;

  #ifdef __HIEVENTSHAPETRIGGERFAST_DEBUG__
  TH1D* h_EbeVn[N_HAR];
  void PrintHIEventShapeContainer(const xAOD::HIEventShapeContainer *Container);
  #endif
     
};

#endif
