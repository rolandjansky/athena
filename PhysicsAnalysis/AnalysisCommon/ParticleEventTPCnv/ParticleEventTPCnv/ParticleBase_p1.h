///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ParticleBase_p1.h 
// Header file for class ParticleBase_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_PARTICLEBASE_P1_H 
#define PARTICLEEVENTTPCNV_PARTICLEBASE_P1_H 1

// STL includes
#include <vector>

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLink_p1.h"
#include "DataModelAthenaPool/AthenaBarCode_p1.h"

// Forward declaration
class ParticleBaseCnv_p1;

class ParticleBase_p1
{ 

  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class ParticleBaseCnv_p1;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  ParticleBase_p1();

  /** Destructor: 
   */
  ~ParticleBase_p1() = default;

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// link to the particle's origin
  ElementLinkInt_p1 m_origin;

  /// Charge of the particle
  float m_charge;

  /// switch to know if the particle has charge informations
  bool m_hasCharge;

  /// ParticleDataGroup Id of the particle
  int m_pdgId;

  /// switch to know if the particle has PDG Id informations
  bool m_hasPdgId;

  /// type of data (see @c IParticle for more infos)
  int m_dataType;

  /// athena barcode
  AthenaBarCode_p1 m_athenabarcode;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> PARTICLEEVENTTPCNV_PARTICLEBASE_P1_H
