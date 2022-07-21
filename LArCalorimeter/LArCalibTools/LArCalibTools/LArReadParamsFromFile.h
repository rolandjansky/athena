//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


/** This algorithm reads an ascii file and fill a paremeters
    structure into the detector store.
   * @author M. Fanti
   * 20.10.2005


*/

#ifndef LARREADPARAMSFROMFILE_H
#define LARREADPARAMSFROMFILE_H
#include "AthenaBaseComps/AthAlgorithm.h"
//#include "GaudiKernel/SmartDataPtr.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include <fstream>
#include <stdio.h>
#include <string>

#include "LArIdentifier/LArOnlineID.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArCalibTools/LArParamsProperties.h"
//using namespace LArParamsProperties ;

template <class DATA>
class LArReadParamsFromFile : public AthAlgorithm
{
 public:
  LArReadParamsFromFile(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArReadParamsFromFile();

  //standard algorithm methods
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override {return StatusCode::SUCCESS;}
  virtual StatusCode stop ATLAS_NOT_THREAD_SAFE () override;

 private:
  const LArOnlineID* m_onlineHelper;
  const LArEM_ID*   m_emId;
  const LArHEC_ID*  m_hecId;
  const LArFCAL_ID* m_fcalId;
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
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
  
  //StatusCode set(LArCaliPulseParamsVsCalib* calib, HWIdentifier chid, int /*gain*/, std::vector<float> data) {
  //  calib->set(chid, data[0], data[1], data[2], data[3], (short)data[4]) ;
  //  return StatusCode::SUCCESS ;
  //`} ;

  // define symLink for all classes
  //--------------------------------

  StatusCode do_symLink ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE (const LArCaliPulseParamsComplete* data) { return detStore()->symLink(data,(ILArCaliPulseParams*)data).ignore() ; } ;
  StatusCode do_symLink ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE (const LArDetCellParamsComplete* data)   { return detStore()->symLink(data,(ILArDetCellParams*)data).ignore()   ; } ;
  StatusCode do_symLink ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE (const LArPhysCaliTdiffComplete* data)   { return detStore()->symLink(data,(ILArPhysCaliTdiff*)data).ignore()   ; } ;
  StatusCode do_symLink ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE (const LArTdriftComplete* data)          { return detStore()->symLink(data,(ILArTdrift*)data).ignore()          ; } ;
  StatusCode do_symLink ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE (const LArMphysOverMcalComplete* data)   { return detStore()->symLink(data,(ILArMphysOverMcal*)data).ignore()   ; } ;
  StatusCode do_symLink ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE (const LArRinjComplete* data)            { return detStore()->symLink(data,(ILArRinj*)data).ignore()            ; } ;
  StatusCode do_symLink ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE (const LArTshaperComplete* data)         { return detStore()->symLink(data,(ILArTshaper*)data).ignore()         ; } ;
  StatusCode do_symLink ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE (const LArEMEC_CphiComplete* data)       { return detStore()->symLink(data,(ILArEMEC_Cphi*)data).ignore()       ; } ;
  StatusCode do_symLink ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE (const LArEMEC_HValphaComplete* data)    { return detStore()->symLink(data,(ILArEMEC_HValpha*)data).ignore()    ; } ;
  StatusCode do_symLink ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE (const LArEMEC_HVbetaComplete* data)     { return detStore()->symLink(data,(ILArEMEC_HVbeta*)data).ignore()     ; } ;
  StatusCode do_symLink ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE (const LArCableLengthComplete* data)     { return detStore()->symLink(data,(ILArCableLength*)data).ignore()     ; } ;
  StatusCode do_symLink ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE (const LArCableAttenuationComplete* data){ return detStore()->symLink(data,(ILArCableAttenuation*)data).ignore(); } ;
//  StatusCode do_symLink(const LArCaliPulseParamsVsCalib* data)  { return detStore()->symLink(data,(ILArCaliPulseParams*)data).ignore() ; };

};

#include "LArCalibTools/LArReadParamsFromFile.icc"

#endif
