/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXTENDEDVXCANDIDATE_CNV_P1_H
#define EXTENDEDVXCANDIDATE_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "VxVertex/ExtendedVxCandidate.h"
#include "TrkEventTPCnv/VxVertex/ExtendedVxCandidate_p1.h"
#include "VxVertex/VxCandidate.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

/**
 * Converter for Transient/Persistent separation
 * of the VxVertex/ExtendedVxCandidate class.
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 *
 * March 2007
 */

class MsgStream;

class ExtendedVxCandidateCnv_p1 : public T_AthenaPoolTPPolyCnvBase< Trk::VxCandidate, Trk::ExtendedVxCandidate, Trk::ExtendedVxCandidate_p1>
{

 public:
 
/** 
 * Default constructor   
 */
   ExtendedVxCandidateCnv_p1():m_vxCandidateConverter(0),m_errorMatrixConverter(0){} 
  
/**
 * Conversion methods
 */ 
  void  persToTrans(const Trk::ExtendedVxCandidate_p1 * persObj, Trk::ExtendedVxCandidate * transObj, MsgStream &log);

  void  transToPers(const Trk::ExtendedVxCandidate * transObj, Trk::ExtendedVxCandidate_p1 * persObj, MsgStream &log);
    
 private:
  
  ITPConverterFor<Trk::VxCandidate> *m_vxCandidateConverter;
  ITPConverterFor<Trk::ErrorMatrix> *m_errorMatrixConverter;	 

};

#endif //EXTENDEDVXCANDIDATE_CNV_P1_H
