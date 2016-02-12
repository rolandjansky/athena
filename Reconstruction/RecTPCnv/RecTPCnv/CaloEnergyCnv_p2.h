///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloEnergyCnv_p1.h 
// Header file for class CaloEnergyCnv_p2
// Author: Keetvi A. Assamagan<keetvi@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_CALOENERGYCNV_P2_H 
#define RECTPCNV_CALOENERGYCNV_P2_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "RecTPCnv/CaloEnergy_p2.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "muonEvent/CaloEnergy.h"

// Forward declaration
class MsgStream;
class CaloEnergy;

/** @brief simple C-stub function for the C++ static method
 *         `CaloEnergyCnv_p2::registerStreamerConverter()`
 */
extern "C" 
void 
caloenergy_cnv_p2_register_streamer();

class CaloEnergyCnv_p2 : public T_AthenaPoolTPPolyCnvBase< Trk::EnergyLoss,
                                                           CaloEnergy, 
                                                           CaloEnergy_p2 >
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  CaloEnergyCnv_p2();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c CaloEnergy
   *  from its persistent representation @c CaloEnergy_p1
   */
  virtual void persToTrans( const CaloEnergy_p2* persObj, 
                            CaloEnergy* transObj, 
                            MsgStream& msg );

  /** Method creating the persistent representation @c CaloEnergy_p1
   *  from its transient representation @c CaloEnergy
   */
  virtual void transToPers( const CaloEnergy* transObj, 
                            CaloEnergy_p2* persObj, 
                            MsgStream& msg );

  /**
   * @brief Register a streamer converter for backwards compatibility
   *        for the vector<DepositInCalo_p1> -> vector<DepositInCalo_p2>
   *        change.
   */
  static void registerStreamerConverter();

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> RECTPCNV_CALOENERGYCNV_P2_H
