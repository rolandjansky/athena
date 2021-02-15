/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

AUX_VAR( IP2D_pb );
AUX_VAR( IP2D_pc );
AUX_VAR( IP2D_pu );

AUX_VAR( IP3D_pb );
AUX_VAR( IP3D_pc );
AUX_VAR( IP3D_pu );

AUX_VAR( IP2D_cu );
AUX_VAR( IP2D_bu );
AUX_VAR( IP2D_bc );

AUX_VAR( IP3D_cu );
AUX_VAR( IP3D_bu );
AUX_VAR( IP3D_bc );

AUX_VAR( DL1rmu_pb );
AUX_VAR( DL1rmu_pc );
AUX_VAR( DL1rmu_pu );

AUX_VAR( softMuon_pb );
AUX_VAR( softMuon_pc );
AUX_VAR( softMuon_pu );
