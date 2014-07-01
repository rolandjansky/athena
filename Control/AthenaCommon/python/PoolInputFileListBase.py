# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
## @file AthenaCommon/python/PoolInputFileListBase.py
## @purpose base class for POOL input file list properties
##
from JobProperties import JobProperty
class PoolInputFileListBase(JobProperty):
    """base class for POOL input file list properties. Its call back will register each file in the list using pool_insertFileToCatalog"""
    allowedTypes = ['list']
    def _do_action(self):
        #first look if we have a POOL_CATALOG around
        import os
        pc_around = (None != os.getenv("POOL_CATALOG"))
        if False == pc_around :
            #may be there is a catalog file in the way
            import os.path
            pc_path = os.path.abspath("PoolFileCatalog.xml")
            if os.path.exists(pc_path):
                pc_around = True
            os.putenv("POOL_CATALOG", "file:" + pc_path)
        for filePath in self.get_Value() :
            import os, sys
            #if there is a pool catalog check whether it contains our filePath 1st
            if (pc_around) :
                import commands
                (rc, result) = commands.getstatusoutput("FClistPFN")
                letsDoIt = 0 != rc | -1 == result.find(filePath)
            else :
                letsDoIt = True
            if letsDoIt:
            # safer as it checks file contents but much slower
            #    FCcommand = "pool_insertFileToCatalog "
                FCcommand = "FCregisterPFN -p "
                rc = os.system(FCcommand + filePath)
                if 0 != rc : sys.exit(rc)
