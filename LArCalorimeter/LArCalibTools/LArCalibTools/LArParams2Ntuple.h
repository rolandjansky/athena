//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/** This algorithm produces a column-wise NTuple
    out of several complete structures. Only the finalize
    method is used, initalize and execute are empty.
   * @author M. Fanti
   * 27. 2. 2004
*/

#ifndef LARPARAMS2NTUPLE_H
#define LARPARAMS2NTUPLE_H

#include "LArCalibTools/LArCond2NtupleBase.h"
#include "LArCalibTools/LArParamsProperties.h"

//using namespace LArParamsProperties ;

class LArParams2Ntuple : public LArCond2NtupleBase
{
 public:

  LArParams2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArParams2Ntuple();

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute() {return StatusCode::SUCCESS;}
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
  //StatusCode ntChannelInfo(HWIdentifier chid,int igain) ;

 private:

  std::vector<std::string> m_classNames ;
  std::vector<std::string> m_detStoreKeys ;
  std::vector<std::string> m_detStoreJo ;
  bool m_allChannels2Ntuple ;
  bool m_useAbstractInterface ;
  std::string m_ntName;

  static const unsigned m_nClasses;

  class DumpFlags {
  private:
    std::vector<bool> m_flags ;
  public:
    DumpFlags()             { for ( unsigned i=0 ; i<m_nClasses ; i++ )  m_flags.push_back(false) ; } 
    void set(unsigned i)    { if ( i<m_nClasses )  m_flags[i] = true ; } 
    void clear(unsigned i)  { if ( i<m_nClasses )  m_flags[i] = false ; } 
    inline bool operator[](unsigned i) const      { return  (i<m_nClasses) && m_flags[i] ; } 
    inline const std::vector<bool>& flags() const { return m_flags ; } 
  } ;

  std::vector<std::string> m_keylist ;
  DumpFlags                m_dumpFlags ;
  std::string              m_suffix ;

  std::vector< std::map< HWIdentifier , DumpFlags > > m_dump_flags_map ;

  const LArCaliPulseParamsComplete*   m_completeCaliPulseParams ;
  const LArDetCellParamsComplete*     m_completeDetCellParams ;
  const LArPhysCaliTdiffComplete*     m_completePhysCaliTdiff ;
  const LArTdriftComplete*            m_completeTdrift ;
  const LArMphysOverMcalComplete*     m_completeMphysOverMcal ;
  const LArRinjComplete*              m_completeRinj ;
  const LArTshaperComplete*           m_completeTshaper ;
  const LArEMEC_CphiComplete*         m_completeEMEC_Cphi ;
  const LArEMEC_HValphaComplete*      m_completeEMEC_HValpha ;
  const LArEMEC_HVbetaComplete*       m_completeEMEC_HVbeta ;
  const LArCableLengthComplete*       m_completeCableLength ;
  const LArCableAttenuationComplete*  m_completeCableAttenuation ;
  //const LArCaliPulseParamsVsCalib*    m_calibCaliPulseParams ;
  const LArOFCBinComplete*            m_completeOFCBin ;


  template < class DATA >
  StatusCode scanReadoutChannels(const DATA*& data_object) ;
  template < class DATA >
  StatusCode scanCalibChannels(const DATA*& data_object) ;
  template < class DATA >
  StatusCode retrieveFromDetStore(const DATA*& data_object) ;

//   inline StatusCode retrieveAbstractInterface(const LArCaliPulseParamsComplete*& data_object)
//   { return m_detStore->retrieve(dynamic_cast<const ILArCaliPulseParams*&>(data_object) ) ; } 
//   inline StatusCode retrieveAbstractInterface(const LArCaliPulseParamsVsCalib*& data_object)
//   { return m_detStore->retrieve(dynamic_cast<const ILArCaliPulseParams*&>(data_object) ) ; } 
//   inline StatusCode retrieveAbstractInterface(const LArDetCellParamsComplete*& data_object)
//   { return m_detStore->retrieve(dynamic_cast<const ILArDetCellParams*&>(data_object) ) ; } 
//   inline StatusCode retrieveAbstractInterface(const LArPhysCaliTdiffComplete*& data_object)
//   { return m_detStore->retrieve(dynamic_cast<const ILArPhysCaliTdiff*&>(data_object) ) ; } 
//   inline StatusCode retrieveAbstractInterface(const LArTdriftComplete*& data_object)
//   { return m_detStore->retrieve(dynamic_cast<const ILArTdrift*&>(data_object) ) ; } 
//   inline StatusCode retrieveAbstractInterface(const LArMphysOverMcalComplete*& data_object)
//   { return m_detStore->retrieve(dynamic_cast<const ILArMphysOverMcal*&>(data_object) ) ; } 
//   inline StatusCode retrieveAbstractInterface(const LArRinjComplete*& data_object)
//   { return m_detStore->retrieve(dynamic_cast<const ILArRinj*&>(data_object) ) ; } 
//   inline StatusCode retrieveAbstractInterface(const LArTshaperComplete*& data_object)
//   { return m_detStore->retrieve(dynamic_cast<const ILArTshaper*&>(data_object) ) ; } 
//   inline StatusCode retrieveAbstractInterface(const LArEMEC_CphiComplete*& data_object)
//   { return m_detStore->retrieve(dynamic_cast<const ILArEMEC_Cphi*&>(data_object) ) ; } 
//   inline StatusCode retrieveAbstractInterface(const LArEMEC_HValphaComplete*& data_object)
//   { return m_detStore->retrieve(dynamic_cast<const ILArEMEC_HValpha*&>(data_object) ) ; } 
//   inline StatusCode retrieveAbstractInterface(const LArEMEC_HVbetaComplete*& data_object)
//   { return m_detStore->retrieve(dynamic_cast<const ILArEMEC_HVbeta*&>(data_object) ) ; } 
//   inline StatusCode retrieveAbstractInterface(const LArCableLengthComplete*& data_object)
//   { return m_detStore->retrieve(dynamic_cast<const ILArCableLength*&>(data_object) ) ; } 
//   inline StatusCode retrieveAbstractInterface(const LArCableAttenuationComplete*& data_object)
//   { return m_detStore->retrieve(dynamic_cast<const ILArCableAttenuation*&>(data_object) ) ; } 

  inline StatusCode retrieveAbstractInterface(const LArCaliPulseParamsComplete*& data_object);
  //inline StatusCode retrieveAbstractInterface(const LArCaliPulseParamsVsCalib*& data_object);
  inline StatusCode retrieveAbstractInterface(const LArDetCellParamsComplete*& data_object);
  inline StatusCode retrieveAbstractInterface(const LArPhysCaliTdiffComplete*& data_object);
  inline StatusCode retrieveAbstractInterface(const LArTdriftComplete*& data_object);
  inline StatusCode retrieveAbstractInterface(const LArMphysOverMcalComplete*& data_object);
  inline StatusCode retrieveAbstractInterface(const LArRinjComplete*& data_object);
  inline StatusCode retrieveAbstractInterface(const LArTshaperComplete*& data_object);
  inline StatusCode retrieveAbstractInterface(const LArEMEC_CphiComplete*& data_object);
  inline StatusCode retrieveAbstractInterface(const LArEMEC_HValphaComplete*& data_object);
  inline StatusCode retrieveAbstractInterface(const LArEMEC_HVbetaComplete*& data_object);
  inline StatusCode retrieveAbstractInterface(const LArCableLengthComplete*& data_object);
  inline StatusCode retrieveAbstractInterface(const LArCableAttenuationComplete*& data_object);
  StatusCode retrieveAbstractInterface(const LArOFCBinComplete*& /*data_object*/){return StatusCode::SUCCESS;};

 

  // dump all channel info to text file
  //------------------------------------
  //std::string m_dumpAllOnlineChannels ; 
  //void dumpChannels() ;
  //void writeToFile(const HWIdentifier & ch, FILE* f) ;

  // Athena / Gaudi services:
};


#endif
