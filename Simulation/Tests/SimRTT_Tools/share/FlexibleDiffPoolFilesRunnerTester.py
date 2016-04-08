#!/usr/bin/env python
import FlexibleDiffPoolFilesRunner

argDict = {'cmtConfig': 'x86_64-slc5-gcc43-opt',
           'fileName' : 'root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/rtt/prod/rtt/rel_4/devval/x86_64-slc5-gcc43-opt/offline/DigitizationTests/mc10_ttbar_LVL1_on_noNoise/DigitizationOutput.pool.root',
           'refFileName' : 'root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/rtt/prod/rtt/rel_4/dev/CMTCONFIG/offline/DigitizationTests/mc10_ttbar_LVL1_on_noNoise/DigitizationOutput.pool.root',
           'doDetailedChecks' : 'False',
           'platformDependent' : 'True',
           'testIdentifierObj' : 'BLAHBLAH'
           }
# fill argDict with the arguments that will be passed to
# FlexibleDiffPoolFilesRunner.FlexibleDiffPoolFilesRunner.__init__

myObject = FlexibleDiffPoolFilesRunner.FlexibleDiffPoolFilesRunner(argDict)
myObject.run()
