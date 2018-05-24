# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys

this = sys.modules[__name__]
this.CfgMgr = None

def setCfgMgr (val_CfgMgr) :
    this = sys.modules[__name__]
    this.CfgMgr = val_CfgMgr
    pass

def createAlgorithm (type, name) :
    this = sys.modules[__name__]
    if this.CfgMgr :
        type = "__".join (type.split ("::"))
        return this.CfgMgr.__getattr__ (type) (name)
    else :
        from AnaAlgorithm.AnaAlgorithmConfig import AnaAlgorithmConfig
        return AnaAlgorithmConfig( type + "/" + name )

def createPublicTool (job, type, name) :
    this = sys.modules[__name__]
    if this.CfgMgr :
        type = "__".join (type.split ("::"))
        tool = this.CfgMgr.__getattr__ (type) (name)
        job += tool
        return tool
    else :
        from AnaAlgorithm.AnaAlgorithmConfig import AnaAlgorithmConfig
        tool = AnaAlgorithmConfig( type + "/" + name )
        tool.setIsPublicTool (True)
        job.algsAdd (tool)
        return tool

def addPrivateTool (alg, name, type) :
    this = sys.modules[__name__]
    if this.CfgMgr :
        parent = alg
        path = name.split (".")
        for x in path[0:-1] :
            print path, x
            parent = parent.__getattr__ (x)
            pass
        type = "__".join (type.split ("::"))
        parent.__setattr__ (path[-1], type + "/" + path[-1])
        pass
    else :
        alg.addPrivateTool (name, type)
        pass
