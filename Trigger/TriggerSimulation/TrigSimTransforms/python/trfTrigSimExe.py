# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Executor classes specific to Trigger simulation
"""

import logging
import os
import shutil

from PyJobTransforms.trfExe import athenaExecutor

msg = logging.getLogger(__name__)


class athenaExecutorJO(athenaExecutor):
    """
    Special Athena executor which uses few JO files from current environment.

    This is useful when running athen from the release which is different
    from current release but which needs JO files from current release.
    """

    def _prepAthenaCommandLine(self):

        def _findInPath(relName, searchpath):
            """ Find anf return file name in a search path """
            for dir in searchpath:
                path = os.path.join(dir, relName)
                if os.path.exists(path):
                    return path
            return None

        joFiles = []
        if 'copyJO' in self.conf.argdict:
            joFiles = self.conf.argdict['copyJO'].returnMyValue(name=self._name, substep=self._substep,
                                                                first=self.conf.firstExecutor)
            msg.info('Will try to copy JO files to local directory: %s', joFiles)
        else:
            msg.info('self.conf.argdict: %s', self.conf.argdict)

        # copy JO files to a special directory
        searchpath = os.environ.get('JOBOPTSEARCHPATH', '').split(os.pathsep)
        joDirName = None
        for jo in joFiles:
            joPath = _findInPath(jo, searchpath)
            if not joPath:
                raise OSError('Failed to find {} in $JOBOPTSEARCHPATH'.format(jo))

            joDirName = 'jobOptions.' + self._name
            dst = os.path.join(joDirName, jo)
            os.makedirs(os.path.dirname(dst))
            shutil.copyfile(joPath, dst)

        # call base class to construct athena command line
        athenaExecutor._prepAthenaCommandLine(self)

        # adjust command line to set $JOBOPTSEARCHPATH
        if joDirName:
            self._cmd.insert(0, 'JOBOPTSEARCHPATH="' + joDirName + ':$JOBOPTSEARCHPATH"')
