#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# art-description: athenaHLT test of the Dev_pp_run3_v1 menu on 2021 pilot beam data
# art-type: build                                                                  
# art-include: master/Athena                                                       

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data_run3'
ex.args = '-c "setMenu=\'Dev_pp_run3_v1_TriggerValidation_prescale\';doL1Sim=True;setDetDescr=\'ATLAS-R3S-2021-02-00-00\';condOverride= {\'/MDT/Onl/T0BLOB\':\'MDTT0-RUN3-Onl-UPD1-01-BLOB\'};"'
ex.args += ' --dump-config-reload'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
