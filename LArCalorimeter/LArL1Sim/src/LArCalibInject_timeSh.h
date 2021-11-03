/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARL1SIM_LARCALIBINJECT_TIMESH_H
#define LARL1SIM_LARCALIBINJECT_TIMESH_H
// +======================================================================+
// +                                                                      +
// + Author ........: Denis O. Damazio                                    +
// + Institut ......: BNL                                                 +
// + Creation date .: 18/11/2013                                          +
// +                                                                      +
// +======================================================================+
//
// ....... include
//

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"

#include "LArDigitization/LArHitEMap.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include "LArElecCalib/ILArfSampl.h"

/**
   @brief The aim of this algorithm is the simulation of the LAr analogue Super-Cell sums. 

   It includes correct allocation of cells to Super-Cell, pulse profile as a function of time, saturation, appropriate noise addition, pile-up. <br>
   The resulting signals are an input to the Preprocessor (which in turn performs digitization, filtering, bunch crossing id., noise suppression,...) <br>
   Since it needs hits, the simulation only takes "simul" datasets as input, NOT digitized datasets.

   @warning although the output is not digitized

   @author Denis O. Damazio (BNL)
 */

class LArCalibInject_timeSh : public AthReentrantAlgorithm
{
//
// >>>>>>>> public methods
//
 public:
  /** constructor */
  LArCalibInject_timeSh(const std::string& name, ISvcLocator* pSvcLocator);

  /** destructor */
  ~LArCalibInject_timeSh();
//
// ..... Gaudi algorithm hooks
//
  /**  Read ascii files for auxiliary data (puslse shapes, noise, etc...) */
  StatusCode initialize();
  /**       Create  LAr  object
      save in TES (2 containers: 1 EM, 1 hadronic)
  */
  StatusCode execute(const EventContext& context) const;

  StatusCode finalize();


 private:

  /// Property: Fraction of Energy Sampled (conditions input).
  SG::ReadCondHandleKey<ILArfSampl> m_fracSKey
  {this, "FracSKey", "LArfSampl", "SG Key of fSamplS conditions object"};
 
  template <class T>
  const T* retrieve(const EventContext& context, SG::ReadCondHandleKey<T> handleKey) const {
	SG::ReadCondHandle<T> handle( handleKey, context);
	if ( not handle.isValid() ) {
		ATH_MSG_ERROR("could not retrieve : " << handle.key() );
		return nullptr;
	} else return handle.cptr();
  }

//
// >>>>>>>> private algorithm parts
//


  /** hit map */
  SG::ReadHandleKey<LArHitEMap> m_hitMapKey{this,"LArHitEMapKey","LArHitEMap","Input HitMap from cell hits"};
  /** output Lar Digits Cell container */
  SG::WriteHandleKey<LArHitEMap> m_hitMapOutKey {this, "LArHitEMapOutKey","LArHitEMapOut","Output LArDigit container"};
  /** seedless option */

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  const CaloCell_ID*     m_calocell_id;
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG Key for CaloDetDescrManager in the Condition Store" };

  bool m_seedLess;

};

#endif

