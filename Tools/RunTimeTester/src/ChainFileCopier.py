# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#from Tracer import Tracer
#class ChainFileCopier(Tracer):
import os
import shutil
class ChainFileCopier:
    def __init__(self, chainFiles, runPath, chainFileArea, logger):
        self.logger      = logger
        self.copyTable   = [(os.path.join(runPath,f), os.path.join(chainFileArea,f)) for f in chainFiles]
        self.copiedFiles = {} 

    def copyFiles(self):
        """
        Checks if file is valid (for now, valid = exists). If so, copy to
        chain store area.
        """

        msg  = '\n%s::copyFiles: files to copy begin\n:' % self.__class__.__name__
        for src, dest in self.copyTable:
            msg += '   %s %s\n' % (src, dest)
        msg  += '%s::copyFiles: files to copy end' % self.__class__.__name__
        self.logger.info(msg)
            

        # src is the original chain file out name (no directory)
        # dest is where the file is now. 

        for src, dest in self.copyTable:
            
            # this test could be performed a sepcialised
            # FileVerifier object for different test criteria
            
            if not os.path.exists(src):
                self.logger.warning('Chain file out not present, skipping: %s %s' % (src, dest)) 
                continue

            # src might refer to a file in a subdirectory
            dirName = os.path.dirname(dest)
            if dirName:
                try:
                    os.makedirs(dirName)
                except:
                    if not os.path.exists(dirName):
                        self.logger.error('Cannot make subdirectory in Chain store: %s' % dirName)
                        continue
            try:
                shutil.copyfile(src, dest)
            except:
                self.logger.error('Error copying chain file %s to %s' % (src, dest))
            else:
                self.logger.info('Copied chain file file %s to %s' % (src, dest))
                self.copiedFiles[src] = dest

            msg = 'Copied chain files\n:'
            for item in self.copiedFiles.items():
                msg += '   %s %s' % item
            self.logger.info(msg)

    def fileLocation(self, f):
        "Return new location if file has been copied"
        
        if f in self.copiedFiles:
            return self.copiedFiles[f]
        return None

        
    def __str__(self):

        s = '\n'
        s+='ChainFileCopier  start\n'
        s+='   Chain Files to copy:\n '
        for f in self.copyTable: s+='      %s %s\n' % f
        s+='   \n Copied chain files:\n '
        for f in self.copiedFiles.items(): s+='      %s %s\n' % f
        s+='ChainJobMinder  end\n'
        
        return s
