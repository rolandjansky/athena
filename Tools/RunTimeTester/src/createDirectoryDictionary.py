# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RTTSException import RTTInputError
from RTTpath       import RTTpath

def getSegs():
    return   {'AtlasHLT'        : 'hlt',
              'AtlasProduction' : 'offline',
              'AtlasPoint1'     : 'point1',
              'AtlasTier0'      : 'tier0',
              'AtlasP1HLT'      : 'p1hlt'
              }

def findSeg(project): return  getSegs()[project]

def createDirectoryDictionary(argBag, withoutReleaseName=False):
    """Sets up a dictionary containing the various paths concerned with
    results.
    e.g. <resBase>/12.4.0/kit/x86_64_slc4_gcc34_opt
    <resBase>/rel_0/bug/kit/x86_64_slc4_gcc34_opt
    """
        
        
    dict = {}
    
    path = RTTpath(argBag.base)
    dict['base'] = str(path)
    
    if not withoutReleaseName:
        path.postpend(argBag.release)                  # 12.4.0, rel_0

    if argBag.isNightly: path.postpend(argBag.originalBranch) # dev or bugfix

    path.postpend(argBag.runType)                  # kit or build
    path.postpend(argBag.cmtConfig)                # target CMT config

    segs = getSegs()
    if not (argBag.topProject in segs or argBag.otherProject in segs):
        msg = 'Unknown top/otherProject to path mapping \ntopProject:%s \notherProject:%s\nmapping:%s' % (
            str(argBag.topProject), str(argBag.otherProject), str(segs))
        raise RTTInputError(msg)

    if argBag.topProject in segs:
        pathSegment = segs[argBag.topProject]
        if argBag.otherProject in segs:
            pathSegment  += '_' + segs[argBag.otherProject]
    else:
        pathSegment = segs[argBag.otherProject]
            
    path.postpend(pathSegment)

    dict['packages'] = str(path)
    
    return dict
