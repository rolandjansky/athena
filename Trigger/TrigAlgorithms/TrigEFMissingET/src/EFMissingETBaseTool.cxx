/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETBaseTool.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Rashid Djilkibaev
CREATED:  March 1, 2006

PURPOSE:  Base Tool for all calo-based EFMissingET tools that require
an EFMissingET object as its input.  The derived tool
must implement an execute(EFMissingET*) method.

 ********************************************************************/
#include "TrigEFMissingET/EFMissingETBaseTool.h"
#include "TrigMissingEtEvent/TrigMissingET.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "TrigTimeAlgs/ITrigTimerSvc.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include <string>
#include <vector>


EFMissingETBaseTool::EFMissingETBaseTool(const std::string& type,
    const std::string& name,
    const IInterface* parent):
  AthAlgTool(type, name, parent)
{
  declareInterface<EFMissingETBaseTool>( this );
  declareProperty("ParentFexName", m_FexName="EFMissingET_Fex");

  m_timersvc=nullptr;
  m_glob_timer=nullptr;
  for (int iDet=0; iDet<4; ++iDet)
    for (int iStep=0; iStep<3; ++iStep)
      m_timer[iDet][iStep] = nullptr;


  /** definition of the meaning for the component flag bits **/
  m_maskProcessing         = 0x0001; // bit  0
  m_maskErrBSconv          = 0x0002; // bit  1
  m_maskErrMuon            = 0x0004; // bit  2 (see also EFMissingET::makeMissingET()!
  m_maskErrFEB             = 0x0008; // bit  3
  m_maskSkipped            = 0x0010; // bit  4
  m_maskCompBigMEtSEtRatio = 0x0020; // bit  5
  m_maskBadCompEnergy      = 0x0040; // bit  6
  m_maskBadEnergyRatio     = 0x0080; // bit  7
  m_maskNoisyEnergyRatio   = 0x0100; // bit  8
  m_maskBadCellQuality     = 0x0200; // bit  9
  m_maskBadCellEnergy      = 0x0400; // bit 10
  m_maskBadCellTime        = 0x0800; // bit 11
  m_maskNoMuonTrack        = 0x1000; // bit 12
  // spare = 0x2000; // bit 13
  m_maskProcessed          = 0x4000; // bit 14
  m_maskCompErrors         = 0x8000; // bit 15

  /** definition of the meaning for the global flag bits (highest ones only) **/
  // spare                    = 0x00010000; // bit 16
  // spare                    = 0x00020000; // bit 17
  // spare                    = 0x00040000; // bit 18
  // spare                    = 0x00080000; // bit 19
  // spare                    = 0x00100000; // bit 20
  // spare                    = 0x00200000; // bit 21
  // spare                    = 0x00400000; // bit 22
  // spare                    = 0x00800000; // bit 23
  // spare                    = 0x01000000; // bit 24
  // spare                    = 0x02000000; // bit 25
  // spare                    = 0x04000000; // bit 26

  m_maskGlobNoisyEnergyRatio = 0x08000000; // bit 27
  m_maskBadEMfraction        = 0x10000000; // bit 28
  m_maskGlobBigMEtSEtRatio   = 0x20000000; // bit 29
  m_maskObjInCrack           = 0x40000000; // bit 30
  m_maskGlobErrors           = 0x80000000; // bit 31

  m_fextype = FexType::OTHER;

}

EFMissingETBaseTool::~EFMissingETBaseTool()
{
}


StatusCode EFMissingETBaseTool::start()
{
  return StatusCode::SUCCESS;
}


StatusCode EFMissingETBaseTool::initialize()
{
  return StatusCode::SUCCESS;
}


StatusCode EFMissingETBaseTool::finalize()
{
  return StatusCode::SUCCESS;
}


StatusCode EFMissingETBaseTool::execute()
{
  return StatusCode::SUCCESS;
}

StatusCode EFMissingETBaseTool::execute(xAOD::TrigMissingET * /* met */ ,
    TrigEFMissingEtHelper * /* metHelper */ ,
    const xAOD::CaloClusterContainer * /* caloCluster */ , const xAOD::JetContainer  * /* jets */,
                                        const xAOD::TrackParticleContainer * /*trackContainer*/,
                                        const xAOD::VertexContainer * /*vertexContainer*/,
                                        const xAOD::MuonContainer * /*muonContainer*/ )
{
  return StatusCode::SUCCESS;
}


StatusCode EFMissingETBaseTool::getTime(int iDet, int iStep, float *timerTime){
  *timerTime = 0;
  if(m_timersvc && iDet<4 && iDet>=0 && iStep<3 && iStep>=0) {
    if (!(m_timer[iDet][iStep])) return StatusCode::SUCCESS;

    *timerTime = m_timer[iDet][iStep]->elapsed();
    return StatusCode::SUCCESS;
  }
  return StatusCode::RECOVERABLE;
}

StatusCode EFMissingETBaseTool::getTimeEM(float *timerTime){
  *timerTime = 0;
  if ( !(m_timer[0][0]) || !(m_timer[0][1]) || !(m_timer[0][2]) )
    return StatusCode::SUCCESS;

  if(m_timersvc) {
    *timerTime = m_timer[0][0]->elapsed() + m_timer[0][1]->elapsed() + m_timer[0][2]->elapsed();
    return StatusCode::SUCCESS;
  }
  return StatusCode::RECOVERABLE;
}

StatusCode EFMissingETBaseTool::getTimeHEC(float *timerTime){
  *timerTime = 0;
  if ( !(m_timer[1][0]) || !(m_timer[1][1]) || !(m_timer[1][2]) )
    return StatusCode::SUCCESS;

  if(m_timersvc) {
    *timerTime = m_timer[1][0]->elapsed() + m_timer[1][1]->elapsed() + m_timer[1][2]->elapsed();
    return StatusCode::SUCCESS;
  }
  return StatusCode::RECOVERABLE;
}


StatusCode EFMissingETBaseTool::getTimeTile(float *timerTime){
  *timerTime = 0;
  if ( !(m_timer[2][0]) || !(m_timer[2][1]) || !(m_timer[2][2]) )
    return StatusCode::SUCCESS;

  if(m_timersvc) {
    *timerTime = m_timer[2][0]->elapsed() + m_timer[2][1]->elapsed() + m_timer[2][2]->elapsed();
    return StatusCode::SUCCESS;
  }
  return StatusCode::RECOVERABLE;
}


StatusCode EFMissingETBaseTool::getTimeFCal(float *timerTime){
  *timerTime = 0;
  if ( !(m_timer[3][0]) || !(m_timer[3][1]) || !(m_timer[3][2]) )
    return StatusCode::SUCCESS;

  if(m_timersvc) {
    *timerTime = m_timer[3][0]->elapsed() + m_timer[3][1]->elapsed() + m_timer[3][2]->elapsed();
    return StatusCode::SUCCESS;
  }
  return StatusCode::RECOVERABLE;
}


StatusCode EFMissingETBaseTool::getTimeRegionSelector(float *timerTime){
  *timerTime = 0;
  if ( !(m_timer[0][0]) || !(m_timer[1][0]) || !(m_timer[2][0]) || !(m_timer[3][0]) )
    return StatusCode::SUCCESS;

  if(m_timersvc) {
    *timerTime = m_timer[0][0]->elapsed() + m_timer[1][0]->elapsed() + m_timer[2][0]->elapsed() + m_timer[3][0]->elapsed();
    return StatusCode::SUCCESS;
  }
  return StatusCode::RECOVERABLE;
}


StatusCode EFMissingETBaseTool::getTimeLoadCollections(float *timerTime){
  *timerTime = 0;
  if ( !(m_timer[0][1]) || !(m_timer[1][1]) || !(m_timer[2][1]) || !(m_timer[3][1]) )
    return StatusCode::SUCCESS;

  if(m_timersvc) {
    *timerTime = m_timer[0][1]->elapsed() + m_timer[1][1]->elapsed() + m_timer[2][1]->elapsed() + m_timer[3][1]->elapsed();
    return StatusCode::SUCCESS;
  }
  return StatusCode::RECOVERABLE;
}


StatusCode EFMissingETBaseTool::getTimeLoop(float *timerTime){
  *timerTime = 0;
  if ( !(m_timer[0][2]) || !(m_timer[1][2]) || !(m_timer[2][2]) || !(m_timer[3][2]) )
    return StatusCode::SUCCESS;

  if(m_timersvc) {
    *timerTime = m_timer[0][2]->elapsed() + m_timer[1][2]->elapsed() + m_timer[2][2]->elapsed() + m_timer[3][2]->elapsed();
    return StatusCode::SUCCESS;
  }
  return StatusCode::RECOVERABLE;
}
