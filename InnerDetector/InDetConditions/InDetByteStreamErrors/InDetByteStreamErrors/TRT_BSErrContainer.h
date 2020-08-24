//Dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDetByteStreamErrors package
 -----------------------------------------
 ***************************************************************************/


#ifndef TRT_BYTESTREAMERRORS_TRT_BSERRCONTAINER_H
#define TRT_BYTESTREAMERRORS_TRT_BSERRCONTAINER_H

#include "AthenaKernel/CLASS_DEF.h"
#include <stdint.h>
#include <set> 

class TRT_BSErrContainer {
  
 public: 

  TRT_BSErrContainer() = default;

  //Getters, for entire sets:
  const std::set<std::pair<uint32_t,uint32_t> > &
    getRobErrorSet( void ) const { return m_rob_status_errors;}
  
  const std::set<uint32_t> & getMissingErrorSet( void ) const {
    return m_missing_errors;}
  
  const std::set<uint32_t> & getErrorErrorSet( void ) const {
    return m_error_errors;}

  const std::set<uint32_t> & getSidErrorSet( void ) const {
    return m_sid_errors;}

  const std::set<std::pair<uint32_t,uint32_t> >& getL1ErrorSet() const {
    return m_l1id_pair_errors;}

  const std::set<std::pair<uint32_t,uint32_t> >& getBCIDErrorSet() const {
    return m_bcid_pair_errors;}

  void add_missing_error(const uint32_t DTMROC_idx ) {
    m_missing_errors.insert( DTMROC_idx );
  }

  void add_error_error(const uint32_t DTMROC_idx ) {
    m_error_errors.insert( DTMROC_idx ); 
  }
  
  void add_sid_error(const uint32_t DTMROC_idx ) {
    m_sid_errors.insert( DTMROC_idx ); 
  }
  

  void add_l1id_error( uint32_t DTMROC_idx, uint32_t l1id ) {
    m_l1id_pair_errors.insert(std::make_pair( DTMROC_idx, l1id));
  }

  void add_bcid_error( uint32_t DTMROC_idx, uint32_t bcid ) {
    m_bcid_pair_errors.insert(std::make_pair( DTMROC_idx, bcid )); 
  }

  void add_rob_error( uint32_t robSourceId, uint32_t robStatus ) {
    m_rob_status_errors.insert(std::make_pair(robSourceId, robStatus));
  }

 private:
  std::set<uint32_t> m_missing_errors;
  std::set<uint32_t> m_error_errors;
  std::set<uint32_t> m_sid_errors;

  std::set<std::pair<uint32_t,uint32_t> > m_l1id_pair_errors;
  std::set<std::pair<uint32_t,uint32_t> > m_bcid_pair_errors;
  std::set<std::pair<uint32_t,uint32_t> > m_rob_status_errors;

};



CLASS_DEF(TRT_BSErrContainer,1324847776,1)


#endif // TRT_BYTESTREAMERRORS_TRT_BSERRCONTAINER_H
