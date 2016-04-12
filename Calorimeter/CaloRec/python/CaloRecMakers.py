# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: CaloRecMakers.py,v 1.8 2009/03/31 16:17:27 droussea Exp $
#
# File: CaloRecMakers.py
# Created: Jul 2006, sss
# Purpose: Create configurables for CaloRec.
#
# This is a prototype for how Configurable instances can be constructed.
#


from CaloRec.CaloRecConf import CaloClusterCopier, CaloClusterMaker, \
     CaloClusterCorrDBWriter, CaloClusterBuilderSW, CaloCell2ClusterMapper, \
     CaloTopoClusterSplitter, CaloTopoClusterMaker 


#
# These should probably be in a common library.
#
def _makeconf (cls, name = None, **kwargs):
    if name != None:
        x = cls(name)
    else:
        x = cls()
    for (k,v) in kwargs.items():
        setattr (x, k, v)
    return x
def _process_tool (alg, tool):
    if type(tool) != type(""):
        alg += tool
        tool = tool.getFullName()
    return tool
def _process_tools (alg, tools):
    return [_process_tool(alg, t) for t in tools]


def make_CaloClusterBuilderSW (name,
                               tower_container,
                               eta_size,
                               phi_size,
                               e_threshold,
                               FillClusterCells,
                               eta_sizep = 3,
                               phi_sizep = 3,
                               eta_SeedGrid = None,
                               phi_SeedGrid = None,
                               eta_Duplicate = None,
                               phi_Duplicate = None,
                               nextra = 0):
    etadup = 2*eta_size - 1
    phidup = 2*phi_size - 1
    return _makeconf (CaloClusterBuilderSW,
                      name,
                      TowerContainer = tower_container,
                      eta_size = eta_size,
                      phi_size = phi_size,
                      eta_sizep = eta_sizep,
                      phi_sizep = phi_sizep,
                      e_threshold = e_threshold,
                      FillClusterCells = FillClusterCells,
                      nextra = nextra,
                      eta_SeedGrid = eta_SeedGrid or eta_size,
                      phi_SeedGrid = phi_SeedGrid or phi_size,
                      eta_Duplicate = eta_Duplicate or etadup,
                      phi_Duplicate = phi_Duplicate or phidup)

def make_CaloClusterCopier (name, output_name, copy_cells = False, use_ClusterPosition=False, etCut=-1.):
    return _makeconf (CaloClusterCopier,
                      name,
                      ClustersName = output_name,
                      CopyCells = copy_cells,
                      UseClusterPosition=use_ClusterPosition,
                      etCut =etCut)


def make_CaloClusterMaker (name,
                           output_key,
                           keep_each_correction = False,
                           maker_tools = [],
                           correction_tools = []):
    alg = _makeconf (CaloClusterMaker,
                     name,
                     ClustersOutputName = output_key,
                     KeepEachCorrection = keep_each_correction)
    alg.ClusterMakerTools = _process_tools (alg, maker_tools)
    alg.ClusterCorrectionTools = _process_tools (alg, correction_tools)
    return alg


def make_CaloClusterCorrDBWriter (name,
                                  key,
                                  correction_tools = []):
    alg = _makeconf (CaloClusterCorrDBWriter,
                     name,
                     key = key)
    alg.ClusterCorrectionTools = _process_tools (alg, correction_tools)
    return alg



def make_CaloCell2ClusterMapper (name,
                                 input_key,
                                 output_key):
    return _makeconf (CaloCell2ClusterMapper,
                      name,
                      ClustersName = input_key,
                      MapOutputName = output_key)


def make_CaloTopoClusterMaker (name, **kw):
    return _makeconf (CaloTopoClusterMaker, name, **kw)

def make_CaloTopoClusterSplitter (name, **kw):
    return _makeconf (CaloTopoClusterSplitter, name, **kw)



