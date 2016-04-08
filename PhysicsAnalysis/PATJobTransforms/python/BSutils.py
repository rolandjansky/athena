# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Functions to remove existing BS files from working dir before the job
# starts and to rename the BS file to the name given to the trf.
#
# Taken from Reconstruction/RecJobTransforms/scripts/csc_RDOtoBS_trf.py
# so they can be used by other transform scripts.
#

import os, re
class _BSutils():
    def match_deleteOld_BS_datafile(self,pattern,path,listfilenames):
        """  Scans the path directory looking for old BS files
        and it deletes them """
        p=re.compile(pattern)
        for i in listfilenames:
            if p.match(i):
                try:
                    os.remove(i)
                    print 'BSutils.py: Found the old BS data file in run directory ', p.match(i).group(),' and deleted'
                except Exception,err:
                    print "BSutils.py: --> error:",err
                    import traceback,sys
                    traceback.print_exc( file = sys.stdout )
                    raise RuntimeError('BSutils.py: failed deleting old BS file %s ' % i)


    def match_rename_BS_datafile(self,args,path,listfilenames):
        """ Renames the generated BS file with the name from the
        transform arguments. The file is found by regular exp
        using args[0] and is renamed to args[1] """
        print "match_rename_BS_datafile",args
        p=re.compile(args[0])
        for i in listfilenames:
            if p.match(i):
                try:
                    os.rename(p.match(i).group(),args[1])
                    print 'BSutils.py: Found the BS data file ', p.match(i).group(), ' and renamed to ',args[1]
                except Exception,err:
                    print "BSutils.py: --> error:",err
                    import traceback,sys
                    traceback.print_exc( file = sys.stdout )
                    raise RuntimeError('BSutils.py: failed to rename BS file %s ' % i)
                
BSutils=_BSutils()
