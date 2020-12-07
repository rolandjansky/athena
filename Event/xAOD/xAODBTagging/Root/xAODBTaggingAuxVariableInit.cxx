/*
 1;95;0c Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s).
#include "xAODBTagging/ftagfloat_t.h"
#include "AthContainers/AuxElement.h"

// Helper macro for registering the types of auxiliary variables in memory.
#define AUX_VAR(NAME)                                                          \
  static const SG::AuxElement::Accessor<ftagfloat_t> NAME(#NAME)

// Register the correct types for a select set of auxiliary variables.
AUX_VAR( rnnip_pb );
AUX_VAR( rnnip_pc );
AUX_VAR( rnnip_pu );
AUX_VAR( rnnip_ptau );

AUX_VAR( DL1_pb );
AUX_VAR( DL1_pc );
AUX_VAR( DL1_pu );

AUX_VAR( DL1r_pb );
AUX_VAR( DL1r_pc );
AUX_VAR( DL1r_pu );

AUX_VAR( MV2c00_discriminant );
AUX_VAR( MV2c10_discriminant );
AUX_VAR( MV2c20_discriminant );
AUX_VAR( MV2c10mu_discriminant );
AUX_VAR( MV2c10rnn_discriminant );
AUX_VAR( SMT_discriminant );
