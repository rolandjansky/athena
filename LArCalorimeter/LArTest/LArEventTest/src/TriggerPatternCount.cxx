/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArEventTest/TriggerPatternCount.h"
#include "CaloIdentifier/CaloGain.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "TBEvent/TBTriggerPatternUnit.h"
#include <fstream>
#include <stdlib.h>


TriggerPatternCount::TriggerPatternCount(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{ 
  declareProperty("ContainerKey",m_contKey);
  m_pattern_max=31; // Set to m_pattern_count array size-1
  m_event=0;
  for(int i=0;i<m_pattern_max;i++)
    m_pattern_count[i]=0;
  m_pattern_count[m_pattern_max]=0;
}

TriggerPatternCount::~TriggerPatternCount() 
{}

StatusCode TriggerPatternCount::initialize()
{
  return StatusCode::SUCCESS;
}  

StatusCode TriggerPatternCount::execute()
{
  int triggerword;

  //Retrieve the TBTriggerPatternUnit
  const TBTriggerPatternUnit* theTBTriggerPatternUnit;
  StatusCode sc = evtStore()->retrieve(theTBTriggerPatternUnit, "TBTrigPat");

  if (sc.isFailure()) {
    triggerword = 0;
    ATH_MSG_ERROR ( "cannot allocate TBTriggerPatternUnit" );
    //return StatusCode::FAILURE;
  } else {
    triggerword = theTBTriggerPatternUnit->getTriggerWord();
  }

  if(triggerword<m_pattern_max && triggerword>=0) 
    m_pattern_count[triggerword]++;
  else 
    m_pattern_count[m_pattern_max]++;

  m_event++;
  return StatusCode::SUCCESS;
}

#include <stdlib.h>

StatusCode TriggerPatternCount::finalize()
{
  ATH_MSG_INFO ( "TriggerPatternCount has finished." );

  FILE *f;
  f=fopen("TPC.out","w");
  if(f==0) return StatusCode::FAILURE;

  fprintf(f,"########################################\n");
  fprintf(f,"Number of events processed      = %d\n",m_event);
  for(int i=0;i<m_pattern_max;i++)
    if(m_pattern_count[i]>0)
      fprintf(f,"Number of events for pattern %2.2d = %d\n",
	     i,m_pattern_count[i]);
  fprintf(f,"Number of events out of bounds  = %d\n",
	 m_pattern_count[m_pattern_max]);
  fprintf(f,"########################################\n");

  fclose(f);
  return StatusCode::SUCCESS;
}// end finalize-method.
