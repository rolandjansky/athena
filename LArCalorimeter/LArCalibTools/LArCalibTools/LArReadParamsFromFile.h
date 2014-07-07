//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** This algorithm reads an ascii file and fill a paremeters
    structure into the detector store.
   * @author M. Fanti
   * 20.10.2005


*/

#ifndef LARREADPARAMSFROMFILE_H
#define LARREADPARAMSFROMFILE_H
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/SmartDataPtr.h"
#include "CaloIdentifier/CaloIdManager.h"

#include <fstream>
#include <stdio.h>
#include <string>

#include  "StoreGate/StoreGateSvc.h"
#include "LArIdentifier/LArOnlineID.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "LArTools/LArCablingService.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"

#include "LArCalibTools/LArParamsProperties.h"
using namespace LArParamsProperties ;

template <class DATA>
class LArReadParamsFromFile : public Algorithm
{
 public:
  LArReadParamsFromFile(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArReadParamsFromFile();

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute() {return StatusCode::SUCCESS;}
  StatusCode finalize(){return StatusCode::SUCCESS;}
  StatusCode stop();

 private:
  StoreGateSvc* m_detStore; 
  const LArOnlineID* m_onlineHelper;
  const LArEM_ID*   m_emId;
  const LArHEC_ID*  m_hecId;
  const LArFCAL_ID* m_fcalId;
  LArCablingService *m_larCablingSvc;
  // LArConditionsContainerBase::GroupingType m_groupingType ;
  int m_groupingType ;
  std::string m_groupingName ;

  std::string m_file ;
  bool m_useOfflineIdentifier , m_useCalibLines ;
  std::string m_chIdType ;
  std::string m_customKey;

  DATA * m_dataclass ;

  StatusCode readFile() ;

  // define 'set' method for each foreseen data class:
  //---------------------------------------------------

  StatusCode set(LArCaliPulseParamsComplete* complete, HWIdentifier chid, int gain, std::vector<float> data) {
    complete->set(chid, gain, data[0], data[1], data[2], data[3], (short)data[4]) ;
    return StatusCode::SUCCESS ;
  } ;
  StatusCode set(LArDetCellParamsComplete* complete,   HWIdentifier chid, int gain, std::vector<float> data) {
    complete->set(chid, gain, data[0], data[1]) ;
    return StatusCode::SUCCESS ;
  } ;
  StatusCode set(LArPhysCaliTdiffComplete* complete,   HWIdentifier chid, int gain, std::vector<float> data) {
    complete->set(chid, gain, data[0]) ;
    return StatusCode::SUCCESS ;
  } ;
  StatusCode set(LArTdriftComplete* complete,          HWIdentifier chid, int /*gain*/, std::vector<float> data) {
    complete->set(chid, data[0]) ;
    return StatusCode::SUCCESS ;
  } ;
  StatusCode set(LArMphysOverMcalComplete* complete,   HWIdentifier chid, int gain, std::vector<float> data) {
    complete->set(chid, gain, data[0]) ;
    return StatusCode::SUCCESS ;
  } ;
  StatusCode set(LArRinjComplete* complete,   HWIdentifier chid, int /*gain*/, std::vector<float> data) {
    complete->set(chid, data[0]) ;
    return StatusCode::SUCCESS ;
  } ;
  StatusCode set(LArTshaperComplete* complete,   HWIdentifier chid, int /*gain*/, std::vector<float> data) {
    complete->set(chid, data[0]) ;
    return StatusCode::SUCCESS ;
  } ;
  StatusCode set(LArEMEC_CphiComplete* complete,   HWIdentifier chid, int /*gain*/, std::vector<float> data) {
    complete->set(chid, data[0]) ;
    return StatusCode::SUCCESS ;
  } ;
  StatusCode set(LArEMEC_HValphaComplete* complete,   HWIdentifier chid, int /*gain*/, std::vector<float> data) {
    complete->set(chid, data[0]) ;
    return StatusCode::SUCCESS ;
  } ;
  StatusCode set(LArEMEC_HVbetaComplete* complete,   HWIdentifier chid, int /*gain*/, std::vector<float> data) {
    complete->set(chid, data[0]) ;
    return StatusCode::SUCCESS ;
  } ;
  StatusCode set(LArCableLengthComplete* complete,   HWIdentifier chid, int /*gain*/, std::vector<float> data) {
    complete->set(chid, data[0]) ;
    return StatusCode::SUCCESS ;
  } ;
  StatusCode set(LArCableAttenuationComplete* complete,   HWIdentifier chid, int /*gain*/, std::vector<float> data) {
    complete->set(chid, data[0]) ;
    return StatusCode::SUCCESS ;
  } ;
  StatusCode set(LArCaliPulseParamsVsCalib* calib, HWIdentifier chid, int /*gain*/, std::vector<float> data) {
    calib->set(chid, data[0], data[1], data[2], data[3], (short)data[4]) ;
    return StatusCode::SUCCESS ;
  } ;

  // define symLink for all classes
  //--------------------------------

  void do_symLink(const LArCaliPulseParamsComplete* data) { m_detStore->symLink(data,(ILArCaliPulseParams*)data) ; } ;
  void do_symLink(const LArDetCellParamsComplete* data)   { m_detStore->symLink(data,(ILArDetCellParams*)data)   ; } ;
  void do_symLink(const LArPhysCaliTdiffComplete* data)   { m_detStore->symLink(data,(ILArPhysCaliTdiff*)data)   ; } ;
  void do_symLink(const LArTdriftComplete* data)          { m_detStore->symLink(data,(ILArTdrift*)data)          ; } ;
  void do_symLink(const LArMphysOverMcalComplete* data)   { m_detStore->symLink(data,(ILArMphysOverMcal*)data)   ; } ;
  void do_symLink(const LArRinjComplete* data)            { m_detStore->symLink(data,(ILArRinj*)data)            ; } ;
  void do_symLink(const LArTshaperComplete* data)         { m_detStore->symLink(data,(ILArTshaper*)data)         ; } ;
  void do_symLink(const LArEMEC_CphiComplete* data)       { m_detStore->symLink(data,(ILArEMEC_Cphi*)data)       ; } ;
  void do_symLink(const LArEMEC_HValphaComplete* data)    { m_detStore->symLink(data,(ILArEMEC_HValpha*)data)    ; } ;
  void do_symLink(const LArEMEC_HVbetaComplete* data)     { m_detStore->symLink(data,(ILArEMEC_HVbeta*)data)     ; } ;
  void do_symLink(const LArCableLengthComplete* data)     { m_detStore->symLink(data,(ILArCableLength*)data)     ; } ;
  void do_symLink(const LArCableAttenuationComplete* data){ m_detStore->symLink(data,(ILArCableAttenuation*)data); } ;
  void do_symLink(const LArCaliPulseParamsVsCalib* data)  { m_detStore->symLink(data,(ILArCaliPulseParams*)data) ; } ;

};

#include "LArCalibTools/LArReadParamsFromFile.icc"

#endif
