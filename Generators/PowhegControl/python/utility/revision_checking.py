# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import os
import subprocess

def check_svn_revision(path):
    """! Infer used SVN code revision.
    
    @param path location of the code whose SVN revision will be checked.
    
    If the target path is not an SVN repository, a manually maintained text file database of revisions will be
    consulted as a fallback.
    
    If neither the SVN check nor the fallback solution finds a revision, 'unknown' will be returned.
    
    @author Stefan Richter  <stefan.richter@cern.ch>
    """
    command = "svn info {path}".format(path=os.path.realpath(path)) # expand symbolic links
    p = subprocess.Popen(command.split(), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    (svninfo, _) = p.communicate()
    for line in svninfo.split('\n'):
        if "Revision:" in line:
            return line.split()[-1]
    try:
        # As a fallback solution, try to read the revision from a text database
        with open("/afs/cern.ch/atlas/offline/external/powhegbox/ATLASOTF-00-03-13/ProcessRevisions.txt", "r") as dbfile:
            for line in dbfile:
                tokens = line.split(" : ")
                if tokens[0] == os.path.basename(path): # found the process of interest
                    return tokens[1][1:] # return the revision number without the leading 'r'
    except:
        pass
    return "unknown"
