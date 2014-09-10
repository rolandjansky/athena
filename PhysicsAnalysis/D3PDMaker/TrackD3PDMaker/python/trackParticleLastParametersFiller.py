# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: trackParticleLastParametersFiller.py 526670 2012-11-19 18:50:06Z ssnyder $
#
# @file TrackD3PDMaker/python/trackParticleLastParametersFiller.py
# @author scott snyder <snyder@bnl.gov>
# @date May 2012
# @brief Configure fillers for last parameters of a TrackParticle.
#


from D3PDMakerCoreComps.SimpleAssociation   import SimpleAssociation
import TrackD3PDMaker


def trackParticleLastParametersFiller (tpobj, covarianceLOD = 99):

    TPLastParameters = SimpleAssociation \
        (tpobj,
         TrackD3PDMaker.TrackParticleLastParametersAssociationTool,
         suffix = '_LM')
    TPLastParameters.defineBlock (1, 'TPLastQoverP',
                                  TrackD3PDMaker.TrackParametersFillerTool,
                                  LocalType = "NONE",
                                  FillPhi = False)
    TPLastParameters.defineBlock (3, 'TPLastParameters',
                                  TrackD3PDMaker.TrackParametersFillerTool,
                                  LocalType = "CARTESIAN",
                                  FillThetaAndQoverP = False)
    TPLastErrors = SimpleAssociation \
                   (TPLastParameters,
                    TrackD3PDMaker.TrackParametersErrorMatrixAssociationTool)
    TPLastErrors.defineBlock (2, 'TPLastQoverPErr',
                              TrackD3PDMaker.ErrorMatrixErrorFillerTool,
                              LocalType = "NONE",
                              FillPhi = False)
    TPLastErrors.defineBlock (3, 'TPLastParametersErr',
                              TrackD3PDMaker.ErrorMatrixErrorFillerTool,
                              LocalType = "CARTESIAN",
                              FillThetaAndQoverP = False)

    TPLastErrors.defineBlock (covarianceLOD, 'TPLastParametersCov',
                              TrackD3PDMaker.ErrorMatrixCovarianceFillerTool,
                              LocalType = "CARTESIAN")

    return

