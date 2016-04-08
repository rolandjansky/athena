# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Factory_BaseClass    import Factory
from JobsXMLReader        import JobsXMLReader
from JobsXMLReader        import JobsXMLReaderATN
from exc2string2          import exc2string2
from RTTSException        import RTTCodingError
# -------------------------------------------------------------------------

class JobsXMLReaderFactory(Factory):
    def __init__(self, logger):
        Factory.__init__(self, logger, self.__class__.__name__)
        self.dict = {
            'PBS':              JobsXMLReader,
            'Dummy':            JobsXMLReader,
            'FullDummy':        JobsXMLReader,
            'EmptyDummy':       JobsXMLReader,
            'LinuxInteractive': JobsXMLReader,
            'LSFBatch':         JobsXMLReader,
            'ATN':              JobsXMLReaderATN
            }

    def create(self, mode, element, rttPilotJobs, packageName, dataSetCatalog, chainStoreMaker,  descFactory):
        if not self.dict.has_key(mode):
            msg = 'JobsXMLReaderFactory dictionary has no key: %s' % mode
            self.logger.error(msg)
            return None
        
        try:
            reader    = self.dict[mode](element, rttPilotJobs, packageName, dataSetCatalog, chainStoreMaker,  self.logger, descFactory)
        except:
            msg  = 'Error making JobsXMLReader\n'
            msg += exc2string2()
            self.logger.error(msg)
            raise RTTCodingError(msg)
        return reader

