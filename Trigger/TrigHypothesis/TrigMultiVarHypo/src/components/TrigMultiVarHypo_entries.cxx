/**
 * @file TrigMultiVarHypo_entries.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre ANJOS</a> 
 * $Author: dquarrie $
 * $Revision: 1.2 $
 * $Date: 2007-10-03 07:15:32 $
 *
 * Declares my gaudi factory entries
 */

#include "TrigMultiVarHypo/TrigRingerNeuralHypo.h"
#include "TrigMultiVarHypo/TrigRingerNeuralFex.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

/**
 * Declares the current algorithms available
 */
DECLARE_ALGORITHM_FACTORY( TrigRingerNeuralFex )
DECLARE_ALGORITHM_FACTORY( TrigRingerNeuralHypo )

/**
 * Declares the entries that are present in the factory at libTrigMultiVarHypo
 */
DECLARE_FACTORY_ENTRIES( TrigMultiVarHypo ) {
  DECLARE_ALGORITHM( TrigRingerNeuralFex );
  DECLARE_ALGORITHM( TrigRingerNeuralHypo );
}

