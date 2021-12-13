# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#
# @file TrackD3PDMaker/python/TrackParticleImpactParameters.py
# @author scott snyder <snyder@bnl.gov>
# @date 2009
# @brief Add to a D3PD object blocks filling in impact parameter information.
#


import TrackD3PDMaker
from D3PDMakerCoreComps.SimpleAssociation   import SimpleAssociation
from InDetRecExample.TrackingCommon import getInDetTrackToVertexTool

def TrackParticleImpactParameters (TPD3PDObject,
                                   prefix = 'track',
                                   beam_suffix = 'beam',
                                   pv_suffix = 'pv',
                                   pvunbiased_suffix = 'pvunbiased'):
    """Add to a D3PD object blocks filling in impact parameter information.

TPD3PDObject should be a D3PD object for a TrackParticle
(could be an association).  This will add information for impact
parameters with respect to the beam spot and the primary vertex,
and the unbiased impact parameter with respect to the primary vertex.
The variable names are constructed using the prefix and *_suffix
arguments."""


    #
    # Beamspot
    #
    BSPerigeeAssoc = SimpleAssociation \
                     (TPD3PDObject,
                      TrackD3PDMaker.TrackParticlePerigeeAtBSAssociationTool,
                      blockname = prefix + 'BSPerigeeAssoc',
                      prefix = prefix,
                      TrackToVertexTool = getInDetTrackToVertexTool())
    BSPerigeeAssoc.defineBlock (1, prefix + 'Impact' + beam_suffix,
                                TrackD3PDMaker.PerigeeFillerTool,
                                FillThetaAndQoverP = False,
                                FillPhi = False,
                                Suffix = beam_suffix,
                                DefaultValue = -9999)
    BSCovarAssoc = SimpleAssociation \
                   (BSPerigeeAssoc,
                    TrackD3PDMaker.PerigeeCovarianceAssociationTool,
                    blockname = prefix + 'BSPerigeeCovarAssoc')
    BSCovarAssoc.defineBlock (1, prefix + 'ImpactSig' + beam_suffix,
                              TrackD3PDMaker.ImpactSigmaFillerTool,
                              Suffix = beam_suffix)

    #
    # Primary vertex
    #
    PVPerigeeAssoc = SimpleAssociation \
                     (TPD3PDObject,
                      TrackD3PDMaker.TrackParticlePerigeeAtPVAssociationTool,
                      blockname = prefix + 'PVPerigeeAssoc',
                      prefix = prefix,
                      TrackToVertexTool = getInDetTrackToVertexTool())
    PVPerigeeAssoc.defineBlock (1, prefix + 'Impact' + pv_suffix,
                                TrackD3PDMaker.PerigeeFillerTool,
                                FillThetaAndQoverP = False,
                                FillPhi = False,
                                Suffix = pv_suffix,
                                DefaultValue = -9999)
    PVCovarAssoc = SimpleAssociation \
                   (PVPerigeeAssoc,
                    TrackD3PDMaker.PerigeeCovarianceAssociationTool,
                    blockname = prefix + 'PVPerigeeCovarAssoc')
    PVCovarAssoc.defineBlock (1, prefix + 'ImpactSig' + pv_suffix,
                              TrackD3PDMaker.ImpactSigmaFillerTool,
                              Suffix = pv_suffix)

