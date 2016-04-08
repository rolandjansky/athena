#!/usr/bin/env python
import SimCoreFileGrepper

argDict = {'cmtConfig': 'x86_64-slc5-gcc43-opt',
           'inputFile' : '/afs/cern.ch/user/r/roberth/work/ISF/devval/Simulation/Tests/SimRTT_Tools/share/ISF_G4_muons_log',
           'testIdentifierObj' : 'BLAHBLAH',
           'testOutputFile' : 'test.log',
           'searchList' : '',
           'vetoList' : '',
           'multipleInstanceList' : 'G4Exception((?!PART102).)*$,5'
           }
# fill argDict with the arguments that will be passed to
# SimCoreFileGrepper.SimCoreFileGrepper.__init__

print "calling SimCoreFileGrepper"
myObject = SimCoreFileGrepper.SimCoreFileGrepper(argDict)
myObject.run()
