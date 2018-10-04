# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import subprocess

def check_svn_revision(path):
    """! Infer used SVN code revision.
    
    @param path location of the code whose SVN revision will be checked.
    
    An Athena error will be logged if the revision check fails. 

    @author Stefan Richter  <stefan.richter@cern.ch>
    """
    command = "svn info {path}".format(path=path)
    p = subprocess.Popen(command.split(), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    (svninfo, _) = p.communicate()
    for line in svninfo.split('\n'):
        if "Revision:" in line:
            return line.split()[-1]
    return "unknown"
