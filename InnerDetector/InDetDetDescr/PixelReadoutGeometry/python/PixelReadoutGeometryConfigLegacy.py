# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def PixelReadoutManager(name="PixelReadoutManager", **kwargs):
    return CfgMgr.InDetDD__PixelReadoutManager(name, **kwargs)
