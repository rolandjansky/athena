# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon import CfgMgr

log = logging.getLogger(__name__)


def GetUpdatedIsoTrackCones(postfix="", object_types=("Electrons", "Photons", "Muons")):
    """ Return a list of IsolationBuilder algorithms to calculate TTVA moments

    ---------
    Arguments
    ---------
    postfix: Optionally provide a postfix to add to the names of the isolation
             builder algorithm instances
    object_types: The object types to which to add the moments
    """

    import ROOT
    # This is a doubly nested list
    ptcone_list = [
        [ROOT.xAOD.Iso.IsolationType.ptcone40, ROOT.xAOD.Iso.IsolationType.ptcone30, ROOT.xAOD.Iso.IsolationType.ptcone20]
    ]
    trkcor_list = [[ROOT.xAOD.Iso.IsolationTrackCorrection.coreTrackPtr]]

    do_egamma = any(x in object_types for x in ("Electrons", "Photons"))

    algs = []
    for track_pt in (500, 1000):
        for loose_cone in (True, False):
            if loose_cone and not do_egamma:
                # Loose cone isolation variables only for electrons and photons
                continue
            cone_str = "LooseCone" if loose_cone else ""
            name = f"TightTTVA{cone_str}_pt{track_pt}"
            # Build up extra IsolationBuilder kwargs
            kwargs = {}
            if "Electrons" in object_types:
                kwargs["ElIsoTypes"] = ptcone_list
                kwargs["ElCorTypes"] = trkcor_list
                kwargs["ElCorTypesExtra"] = [[]]
                kwargs["CustomConfigurationNameEl"] = name
            if "Photons" in object_types:
                kwargs["PhIsoTypes"] = ptcone_list
                kwargs["PhCorTypes"] = trkcor_list
                kwargs["PhCorTypesExtra"] = [[]]
                kwargs["CustomConfigurationNamePh"] = name
            if "Muons" in object_types:
                kwargs["MuIsoTypes"] = ptcone_list
                kwargs["MuCorTypes"] = trkcor_list
                kwargs["MuCorTypesExtra"] = [[]]
                kwargs["CustomConfigurationNameMu"] = name
            algs.append(
                CfgMgr.IsolationBuilder(
                    f"IsolationBuilderTight{cone_str}{track_pt}{postfix}",
                    TrackIsolationTool=CfgMgr.xAOD__TrackIsolationTool(
                        f"TrackIsolationToolTight{track_pt}",
                        TrackSelectionTool=CfgMgr.InDet__InDetTrackSelectionTool(
                            minPt=track_pt, CutLevel="Loose"
                        ),
                        TTVATool=CfgMgr.CP__TrackVertexAssociationTool(
                            WorkingPoint="Loose",
                        ),
                        CoreTrackEtaRange=0.01 if loose_cone else 0.0,
                    ),
                    **kwargs,
                )
            )
    return algs
