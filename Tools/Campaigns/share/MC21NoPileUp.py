# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.BeamFlags import jobproperties as bf
bf.Beam.numberOfCollisions.set_Value_and_Lock(0)

from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.doPixelPlanarRadiationDamage.set_Value_and_Lock(True)

from AthenaCommon.Resilience import protectedInclude
protectedInclude('LArConfiguration/LArConfigRun3Old_NoPileup.py')

protectedInclude('PyJobTransforms/HepMcParticleLinkVerbosity.py')
