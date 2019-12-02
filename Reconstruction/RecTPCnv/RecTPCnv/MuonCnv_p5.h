///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// MuonCnv_p5.h 
// Header file for class MuonCnv_p5
// Author: Ketevi A. Assamagan <ketevi@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUONCNV_P5_H 
#define RECTPCNV_MUONCNV_P5_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "RecTPCnv/Muon_p5.h"

// Forward declaration
class MsgStream;
namespace Analysis { class Muon; }

class MuonCnv_p5 : public T_AthenaPoolTPCnvConstBase<Analysis::Muon, Muon_p5>
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  MuonCnv_p5();


  /** Method creating the transient representation of @c Analysis::Muon
   *  from its persistent representation @c Muon_p1
   */
  virtual void persToTrans( const Muon_p5* persObj, 
                            Analysis::Muon* transObj, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c Muon_p1
   *  from its transient representation @c Analysis::Muon
   */
  virtual void transToPers( const Analysis::Muon* transObj, 
                            Muon_p5* persObj, 
                            MsgStream& msg ) const override;
}; 


inline MuonCnv_p5::MuonCnv_p5()
{}

#endif //> RECTPCNV_MUONCNV_P5_H
