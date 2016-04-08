# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""This is not part of the Descriptor heirarchy (despite the name)."""

class ErrorDescriptor:
    """When an error is encountered during Minder construction, an ErrorJobMNinder is created in MinderFactory.
    ErrorDescriptor is a step in the process - it collects all the info needed to make the ErrorJobMinder"""
    def __init__(self,desc):
        'copy stuff from the dead descriptor needed for reporting and error reporting'
        
        # make the attributes visible to PyLint
        self.name              = None
        self.identifiedName    = None
        self.errorMessages     = None
        self.constructionError = None
        self.rttPilotJob       = None
        self.rttATNJob         = None
        self.runPath           = None
        self.resPath           = None
        self.paths             = None
        self.confederation     = None
        self.jobSerialNumber   = None
        self.log               = None
        self.jobGroup          = None
        self.displayClass      = None
        self.displayProcess    = None
        self.displayComponent  = None
        self.hashString        = None
        self.jobDisplayName    = None
        self.jobDocString      = None
        self.jobDocURL         = None
        self.elog              = None
        self.logger            = None

        desckeys = desc.__dict__.keys()
        for k in self.__dict__.keys():
            if k in desckeys:
                self.__dict__[k] = desc.__dict__[k] 
        

    def __str__(self):
        'Provide a string description'

        s = ''
        for item in self.__dict__.items():
            s+= '%30s              %s' % item
        return s

    def dataForXMLNode(self):
        'Nothing to send up to RTTummaryLog (info will be obtined from ErrorJobMinder'
        return {}
    
    # --------------------------------------------------------------
    
    def dump(self):
        'Write tring description to the log'

        self.logger.info('|-------------------------------------------|')
        self.logger.info('|Error job descriptor  dump                 |')
        self.logger.info('|-------------------------------------------|')
        self.logger.info(self.__str__())
        self.logger.info('')
