/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page ParticleEventAthenaPool_page ParticleEventAthenaPool

@section ParticleEventAthenaPool_introductionParticleEventAthenaPool Introduction
This package contains POOL converters for Particles and jobOs for AOD making.

@section ParticleEventAthenaPool_packagecontentParticleEventAthenaPool Package Contents
ParticleEventAthenaPool contains the following topOs and jobOs:

- RecExToAOD_topOptions.py : topO for RecEx -> AOD
- RecExToCombAOD_topOptions.py : topO for RecEx -> fastSim/full AOD
- RecExToESD_topOptions.py  : topO for RecEx -> ESD
- ESDtoAOD_topOptions.py : topO for ESD -> AOD
- ESDtoCombAOD_topOptions.py : topO for ESD -> fastSim/full AOD

- FastSimToAOD_topOptions.py : topO for fast sim -> fastSim AOD

- optXYZ.py : options for XYZ. E.g., optESDtoAOD.py is for the AOD making from ESD

- AOD_Builder_jobOptions.py : containes jobO for AOD builders
- AOD_OutputList_jobOptions.py : sets list of AODs
- AOD_PoolCnv_jobOptions.py : lists POOL converters needed for AOD making

- AODFlags.py : flags for AOD making



*/
