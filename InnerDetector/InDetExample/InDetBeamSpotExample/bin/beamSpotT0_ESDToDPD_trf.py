#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Job transform to run beam spot jobs with template InDetBeamSpotExample/ESDToDPDTemplate.py
# through the CAF Task Management System (see https://twiki.cern.ch/twiki/bin/view/Atlas/TierZeroTransformations).
#
# Mandatory arguments that must be passed through the pickled argdict file:
#
# - inputESDFile
# - outputDPDFile
# - _attempt
#
# Any additional parameters specified in the argdict file will be passed on to
# InDetBeamSpotExample/share/ESDToDPDTemplate.py.
#
__author__  = 'Juerg Beringer'
__version__ = '$Id: beamSpotT0_ESDToDPD_trf.py 303684 2010-06-07 04:36:21Z beringer $'

import sys
from InDetBeamSpotExample import TrfUtils

cmd = ' '.join(sys.argv)

trf = TrfUtils.JobRunnerTransform('inputESDFile', 'outputDPDFile', optionalArgs=['taskDb', 'taskDir'])

taskDb =            trf.argdict.get('taskDb',            'auth_file:/afs/cern.ch/user/a/atlidbs/private/beamspotdbinfo_w.dat')
taskDir =           trf.argdict.get('taskDir',           '/afs/cern.ch/user/a/atlidbs/jobs')

trf.setProdTaskDatabase(taskDb)
trf.setProdDir(taskDir)

trf.getJobRunner(joboptionpath='InDetBeamSpotExample/ESDToDPDTemplate.py',
                 mandatoryArgs=['_attempt'],
                 autoconfparams='DetDescrVersion',
                 comment=cmd)
if 'alignmentfile' in trf.argdict:
    trf.runner.addFilesToPoolFileCatalog(trf.argdict['alignmentfile'])

trf.go(cmd)
