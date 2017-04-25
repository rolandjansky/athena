/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigMultiVarHypo/TrigL2CaloRingerFex.h"
#include "TrigMultiVarHypo/TrigL2CaloRingerHypo.h"

/**
 * Declares the current algorithms available
 */
DECLARE_ALGORITHM_FACTORY( TrigL2CaloRingerFex  )
DECLARE_ALGORITHM_FACTORY( TrigL2CaloRingerHypo )

/**
 * Declares the entries that are present in the factory at libTrigMultiVarHypo
 */
DECLARE_FACTORY_ENTRIES( TrigMultiVarHypo ) {
  DECLARE_ALGORITHM( TrigL2CaloRingerFex  );
  DECLARE_ALGORITHM( TrigL2CaloRingerHypo );
}

