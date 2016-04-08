# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Evaluate                     import Evaluate
from xml.dom.minidom              import parse

import os
def extractDCubeBaseDir(dCubeCfgFile):
    if not os.path.exists(dCubeCfgFile): return 'unknown'
    
    theDom = parse(dCubeCfgFile)
    return Evaluate('ref_dir/text()', theDom.documentElement)

