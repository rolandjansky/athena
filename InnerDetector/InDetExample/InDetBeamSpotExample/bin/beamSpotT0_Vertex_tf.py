#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Job transform to run beam spot jobs with template InDetBeamSpotExample/VertexTemplate.py
# through the CAF Task Management System (see https://twiki.cern.ch/twiki/bin/view/Atlas/TierZeroTransformations).
#
# Mandatory arguments that must be passed through the pickled argdict file:
#
# - inputDPDFile
# - outputCOOLFile
# - _attempt
#
# Optionally you can specify the following parameters to get additional output files
# into the dataset:
#
# - outputNtFile
# - taskPostProcSteps
# - taskDb
# - taskDir
#
# Any additional parameters specified in the argdict file will be passed on to
# InDetBeamSpotExample/share/VertexTemplate.
#
__author__  = 'Juerg Beringer'
__version__ = '$Id: beamSpotT0_Vertex_tf.py 487258 2012-03-08 09:53:58Z gwilliam $'

import sys
from InDetBeamSpotExample import TrfUtils

def main():

    cmd = ' '.join(sys.argv)

    trf = TrfUtils.JobRunnerTransform('inputDPDFiles', 'outputNtFile', 'histfile', jobDirOutputName='nt.root',
                                      mandatoryArgs=['_attempt'],
                                      optionalArgs=['outputCOOLFile', 'taskPostProcSteps', 'taskDb', 'taskDir'])

    taskPostProcSteps = trf.argdict.get('taskPostProcSteps', 'T0VertexDefaultProcessing')
    taskDb =            trf.argdict.get('taskDb',            'auth_file:/afs/cern.ch/user/a/atlidbs/private/beamspotdbinfo_w.dat')
    taskDir =           trf.argdict.get('taskDir',           '/afs/cern.ch/user/a/atlidbs/jobs')

    trf.setProdTaskDatabase(taskDb)
    trf.setProdDir(taskDir)

    trf.getJobRunner(joboptionpath='InDetBeamSpotExample/VertexTemplate.py',
                     autoconfparams='DetDescrVersion',
                     taskpostprocsteps=taskPostProcSteps,
                     comment=cmd)
    if 'outputCOOLFile' in trf.argdict:
        trf.addOutput('outputCOOLFile','outputfile','beamspot.db')

    trf.go(cmd)

if __name__ == '__main__':
    main()
