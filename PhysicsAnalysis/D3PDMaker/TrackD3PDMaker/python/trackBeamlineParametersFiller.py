# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: trackBeamlineParametersFiller.py 527323 2012-11-22 18:22:23Z ssnyder $
#
# @file TrackD3PDMaker/python/trackBeamlineParametersFiller.py
# @author scott snyder <snyder@bnl.gov>
# @date June 2012
# @brief Configure fillers for the beamline parameters of a track.
#


from D3PDMakerCoreComps.SimpleAssociation   import SimpleAssociation
from D3PDMakerCoreComps.flagTestLOD         import flagTestLOD
from TrackD3PDMaker.TrackD3PDMakerFlags     import TrackD3PDFlags
import TrackD3PDMaker


def trackBeamlineParametersFiller (tobj,
                                   blockprefix = '',
                                   prefix = '',
                                   suffix = '_wrtBL',
                                   parametersLOD = None,
                                   errorsLOD = None,
                                   covarianceLOD = None):

    """Add blocks to TOBJ to write track parameters / errors / covariance
as evaluated at the beamline.

TOBJ may represent one of Track, TrackParameters, or TrackParticle.

BLOCKPREFIX is a prefix to add onto the block names.

PREFIX and SUFFIX are added to variable names.

If PARAMETERSLOD is None, then whether or not the parameters are written
is controlled by TrackD3PDFlags.trackParametersAtBeamLineLevelOfDetails.
Otherwise, it should be the level of detail for the block.

ERRORSLOD and COVARIANCELOD are similar for the errors and covariances.
"""

    def make_lod (lod, flagthresh):
        if lod != None:
            return lod
        return flagTestLOD ('trackParametersAtBeamLineLevelOfDetails >= %d and reqlev >= %d' % (flagthresh, flagthresh), TrackD3PDFlags)

    BLParamsAssoc = SimpleAssociation \
                    (tobj,
                     TrackD3PDMaker.TrackBeamlineParametersAssociationTool,
                     blockname = blockprefix + 'BLParamsAssoc',
                     prefix = prefix,
                     suffix = suffix)
    BLParamsAssoc.defineBlock (make_lod (parametersLOD, 1),
                               blockprefix + 'BLParams',
                               TrackD3PDMaker.TrackParametersFillerTool,
                               DefaultValue = -9999)

    BLErrorsAssoc = SimpleAssociation \
                    (BLParamsAssoc,
                     TrackD3PDMaker.TrackParametersErrorMatrixAssociationTool,
                     blockname = blockprefix + 'BLErrorsAssoc')
    BLErrorsAssoc.defineBlock (make_lod (parametersLOD, 2),
                               blockprefix + 'BLErrors',
                               TrackD3PDMaker.ErrorMatrixErrorFillerTool,
                               DefaultValue = -9999)
    BLErrorsAssoc.defineBlock (make_lod (parametersLOD, 3),
                               blockprefix + 'BLCovariance',
                               TrackD3PDMaker.ErrorMatrixCovarianceFillerTool,
                               DefaultValue = -9999)
    
    return
