# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging

logAODFix_r21 = logging.getLogger( 'AODFix_r21' )

from AODFix_base import AODFix_base

class AODFix_r21(AODFix_base):
    ''' This class just performs AODFix on 21.0.X releases

    Instance variables
      addMetadata (bool) (inherited): whether to add metaData
      doAODFix (bool) (inherited): whether to do AODFix
      prevAODFix (string): previous AODFix applied
      newAODFix (string): new AODFix to apply
    '''

    @staticmethod
    def latestAODFixVersion():
        """The latest version of the AODFix."""
        return "r21v1"

    def muon_postSystemRec(self, topSequence):
        """This fixes the muon momenta to match that of its primary TrackParticle.
        JIRA: https://its.cern.ch/jira/browse/ATLASRECTS-3121
        Reco meeting: https://indico.cern.ch/event/532806/
        Incremented version to r21v1.
        """
        from xAODMuonCnv.xAODMuonCnvConf import xAOD__MuonAODFixAlg
        topSequence+=xAOD__MuonAODFixAlg()


    def postSystemRec(self):
        if self.doAODFix:

            logAODFix_r21.debug("Executing AODFix_r21_postSystemRec")

            # Muon AODfix
            self.muon_postSystemRec(topSequence)
