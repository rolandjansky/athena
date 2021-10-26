"""Define methods to construct configured TRT conditions tools and
algorithms needed by TRT Overlay - FIXME should move somewhere else

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def TRT_OnlineFoldersCfg(flags):
    """Setup online conditions folders for TRT data overlay"""
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    acc = ComponentAccumulator()
    acc.merge(addFolders(flags, "/TRT/Onl/ROD/Compress",
                         "TRT_ONL", className='CondAttrListCollection'))
    return acc


def TRT_CablingSvcCfg(flags):
    """Return a ComponentAccumulator for TRT_CablingSvc service"""
    acc = ComponentAccumulator()
    # Properly configure MC/data for TRT cabling
    TRT_FillCablingData_DC3 = CompFactory.TRT_FillCablingData_DC3
    tool = TRT_FillCablingData_DC3(RealData=not flags.Input.isMC)
    acc.addPublicTool(tool)
    # Setup TRT cabling service
    TRT_CablingSvc = CompFactory.TRT_CablingSvc
    acc.addService(TRT_CablingSvc())
    return acc
