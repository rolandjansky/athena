# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging

logRecoFix_r210 = logging.getLogger( 'RecoFix_r210' )

from RecoFix_base import RecoFix_base

class RecoFix_r210(RecoFix_base):
    ''' This class just performs RecoFix on 21.0.X releases
    '''

    @staticmethod
    def latestAODFixVersion():
        """The latest version of the AODFix. Use same sort order as in AODFix"""
        return ["IDTide", "egammaStrips", "trklinks"]

