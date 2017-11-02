# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging

logRecoFix = logging.getLogger( 'RecoFix_base' )

#The RecoFix base class
class RecoFix_base(object):
    ''' This is the RecoFix base class, which does nothing
    by default. Specific RecoFix types should inherit from it.

    Derived classes should override do-nothing member functions,
    like preInclude, to implement the actual RecoFix.

    Instance variables
      addMetadata (bool): whether to add metaData
      doAODFix (bool): whether to do AODFix
      prevAODFix (string): previous AODFix applied
      newAODFix (string): new AODFix to apply
      isMC (bool): is the input MC?
    '''

    @staticmethod
    def latestAODFixVersion():
        """ Must override this member function in derived classes
        """
        return []

    def __init__(addMetadata = True):
        """ The default constructor.
        """
        self.addMetadata = addMetadata
        return

    def addMetaData(self):
        '''standard function to apply metadata, can overload if necessary for complicated scenarios.'''
        if self.addMetadata:
            logAODFix.debug("in addMetaData")

            from RecExConfig.RecFlags import rec

            suffix="_RAW"
            if rec.readESD():
                suffix="_ESD"
            elif rec.readAOD():
                suffix="_AOD"

            str = "AODFix_" + "-".join(self.latestAODFixVersion()) + suffix

            logRecoFix.info("executing addMetaData, will add as AODFixVersion %s" % str)
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            svcMgr.TagInfoMgr.ExtraTagValuePairs += ["AODFixVersion", str]

