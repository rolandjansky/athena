#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
PandaJobRunner is a JobRunner for running jobs on the grid using pathena.

Written by Juerg Beringer (LBNL) in 2009.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id: PandaJobRunner.py 345959 2011-02-15 15:20:18Z gwilliam $'


from JobRunner import JobRunner
import os


# Default template of job submission script
gridSubmitScript = """#!/bin/sh
echo ''
echo "Setting up release %(release)s"
%(cmdsetup)s
echo ''
echo 'Setting up standard grid environment ...'
source /afs/cern.ch/atlas/offline/external/GRID/ddm/DQ2Clients/setup.sh
voms-proxy-init -voms atlas
source /afs/cern.ch/atlas/offline/external/GRID/DA/panda-client/latest/etc/panda/panda_setup.sh
echo ''
echo "Creating grid job submission directory (`date`) ..."
griddir=$TestArea/gridsubmit/%(jobname)s
mkdir -p $griddir
cd $griddir
echo ''
echo "Copying files (`date`) ..."
%(cmdjobpreprocessing)s
cp -p %(configfile)s .
%(cmdcopyfiles)s
echo ''
echo "Creating POOL file catalog (`date`) ..."
%(cmddefinepoolcatalog)s
echo ''
echo "Running pathena (`date`) ..."
pathena `basename %(configfile)s` %(joboptionpath)s --extOutFile %(outputfilestring)s --inDS %(inputds)s --outDS %(griduser)s.%(jobname)s --nFilesPerJob %(filesperjob)s --nFiles %(maxjobs)s --site %(gridsite)s
echo "Postprocessing (`date`) ..."
%(cmdjobpostprocessing)s
# Deliberately not removing grid job submit directory under $TestArea
# cd ..
# rm -rf $griddir
"""


class PandaJobRunner(JobRunner):
    """PandaJobRunner - run jobs on the grid using pathena"""

    def __init__(self,**params):
        """Constructor (takes any number of parameters as an argument)."""
        JobRunner.__init__(self)

        # Set user-specified parameters only after PandaJobRunner defaults
        self.setParam('inputds','','Input dataset name (for grid jobs)',insertAtFront=True)
        self.setParam('griduser','','User part of grid job name, e.g. JuergBeringer',insertAtFront=True)
        self.setParam('gridsite','AUTO','Site name where jobs are sent (default:AUTO)',insertAtFront=True)
        self.setParam('script',gridSubmitScript)
        self.setParam('addinputtopoolcatalog',False)
        for k in params.keys():
            self.setParam(k,params[k])
        self.checkParams()

    def submitJob(self,jobConfig):
        """Submit a JobRunner job as a grid job."""
        scriptfile = jobConfig['scriptfile']
        logfile = jobConfig['logfile']
        os.system('%s 2>&1 | tee %s' % (scriptfile,logfile))
        return None
