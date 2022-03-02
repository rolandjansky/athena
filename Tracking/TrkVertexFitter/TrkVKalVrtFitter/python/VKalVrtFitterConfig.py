# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from AthenaCommon.Logging import logging
mlog = logging.getLogger('Trk__VKalVrtFitterConfig')

def TrkVKalVrtFitterCfg(flags):

   acc = ComponentAccumulator()

   kwargs = {}
   from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
   kwargs["Extrapolator"] = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
   kwargs["FirstMeasuredPoint"] = False
   kwargs["MakeExtendedVertex"] = False

   VKalVrt = CompFactory.Trk.TrkVKalVrtFitter(**kwargs)
   mlog.info("VKalVrtFitter created")
   acc.setPrivateTools(VKalVrt)

   return acc
