# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import os.path
import shutil

def installWebFiles(RTTLibDir, resultsBasePath):
    webDir = os.path.join(RTTLibDir, 'web')
    necessaryFiles = [os.path.join(webDir, f) for f in os.listdir(webDir) if os.path.isfile(os.path.join(webDir, f))]

    for aFile in necessaryFiles:
        if not os.path.exists(os.path.join(resultsBasePath, os.path.basename(aFile))):
            # self.mainLogger.debug('Installing missing web file: %s' % os.path.basename(aFile))
            src = aFile
            dst = resultsBasePath
            shutil.copy(src, dst)
                
