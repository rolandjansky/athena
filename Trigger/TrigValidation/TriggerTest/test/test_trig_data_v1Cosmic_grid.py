#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# art-description: Trigger BS->RDO_TRIG athena test of the Cosmic_run3_v1 menu on physics_Main stream from a cosmic run
# art-type: grid
# art-include: master/Athena
# art-include: 22.0/Athena
# art-athena-mt: 4
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.out
# art-output: *.err
# art-output: *.log.tar.gz
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.pmon.gz
# art-output: *perfmon*
# art-output: prmon*
# art-output: *.check*

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athena'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data_cos'
ex.threads = 4
ex.concurrent_events = 4
precommand = ''.join([
  "setMenu='Cosmic_run3_v1';",
  "doCosmics=True;",
  "doL1Sim=True;",
  "doWriteBS=False;",
  "doWriteRDOTrigger=True;",
  "setDetDescr='ATLAS-R3S-2021-01-00-02';",
  "condOverride={'/MDT/Onl/T0BLOB':'MDTT0-RUN3-Onl-UPD1-01-BLOB'};"  # TODO: use R3 HLT cond tag when available
])
ex.args = f'-c "{precommand}"'

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
