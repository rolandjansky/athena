/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IRingerProcedure.cxx 667905 2015-05-18 19:07:55Z wsfreund $
#include "RingerSelectorTools/procedures/IRingerProcedure.h"

#if !(RINGER_USE_NEW_CPP_FEATURES || defined(FORCE_RINGER_PROCEDURE_TYPE_CONST_EXPR))
template< typename procedure_t >
const bool RingerProcedureType<procedure_t>::is_pre_processor = 
    Ringer::is_base_of<PreProcessing::IPreProcessor,procedure_t>::value;

template< typename procedure_t >
const bool RingerProcedureType<procedure_t>::is_discriminator = 
    Ringer::is_base_of<Discrimination::IDiscriminator,procedure_t>::value;

template< typename procedure_t >
const bool RingerProcedureType<procedure_t>::is_threshold = 
    Ringer::is_base_of<Discrimination::IThreshold,procedure_t>::value;
#endif




