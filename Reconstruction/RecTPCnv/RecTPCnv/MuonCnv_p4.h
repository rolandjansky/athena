///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// MuonCnv_p4.h 
// Header file for class MuonCnv_p4
// Author: Ketevi A. Assamagan <ketevi@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUONCNV_P4_H 
#define RECTPCNV_MUONCNV_P4_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "RecTPCnv/Muon_p4.h"

// Forward declaration
class MsgStream;
namespace Analysis { class Muon; }

class MuonCnv_p4 : public T_AthenaPoolTPCnvConstBase<Analysis::Muon, Muon_p4>
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  MuonCnv_p4();


  /** Method creating the transient representation of @c Analysis::Muon
   *  from its persistent representation @c Muon_p1
   */
  virtual void persToTrans( const Muon_p4* persObj, 
                            Analysis::Muon* transObj, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c Muon_p1
   *  from its transient representation @c Analysis::Muon
   */
  virtual void transToPers( const Analysis::Muon* transObj, 
                            Muon_p4* persObj, 
                            MsgStream& msg ) const override;
}; 


inline MuonCnv_p4::MuonCnv_p4()
{}

#endif //> RECTPCNV_MUONCNV_P4_H
