# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def BeamSpotCondAlgCfg(flags, name="BeamSpotCondAlg", **kwargs):
    """Configure the BeamSpotCondAlg."""
    acc = ComponentAccumulator()

    from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
    acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Beampos",
                                    "/Indet/Beampos", className="AthenaAttributeList"))

    BeamSpotCondAlg = CompFactory.BeamSpotCondAlg
    acc.addCondAlgo(BeamSpotCondAlg(name, **kwargs))

    return acc
