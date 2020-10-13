#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# art-description: Produce web display comparing Run-3 trigger to Run-2 (legacy) trigger using legacy monitoring NTUP_PHYSVAL from previous nightlies
# art-type: grid
# art-include: master/Athena
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
# art-output: HLTconfig*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml
# art-output: PHYSVAL_WEB
# art-html: PHYSVAL_WEB

from TrigValTools.TrigValSteering import Test, CheckSteps
from TrigAnalysisTest.TrigAnalysisSteps import add_physvalweb_steps

downloadLegacyTrig = CheckSteps.DownloadRefStep('DownloadLegacyTriggerNTUP')
downloadLegacyTrig.artjobname = 'test_trigAna_RDOtoPhysval_v7Primaries_grid.py'

downloadRun3Trig = CheckSteps.DownloadRefStep('DownloadRun3TriggerNTUP')
downloadRun3Trig.artjobname = 'test_trigAna_RDOtoPhysval_v1Dev_grid.py'
downloadRun3Trig.args += ' --dst="."'

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [downloadLegacyTrig,downloadRun3Trig]
# Only keep relevant checks from the defaults
test.check_steps = [chk for chk in CheckSteps.default_check_steps(test)
                    if type(chk) in (CheckSteps.LogMergeStep, CheckSteps.CheckLogStep)]

# Add web display steps
slice_names = [
    'JetMon', 'TauMon', 'MuonMon', 'IDMon',
    'BphysMon', 'HLTCaloESD', 'ResultMon', 'BjetMon',
    'METMon', 'MinBiasMon', 'Egamma']
add_physvalweb_steps(test, slice_names)

import sys
sys.exit(test.run())
