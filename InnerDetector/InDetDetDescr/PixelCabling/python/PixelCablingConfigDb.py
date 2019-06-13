# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService, addTool
addTool("PixelCabling.PixelCablingConfig.getPixelFillCablingData","PixelFillCablingData")
addService("PixelCabling.PixelCablingConfig.getPixelCablingSvc", "PixelCablingSvc")
