# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
This module bundles the RTT factory functions.
These functions are written as classes to help write test code.
"""
import os.path

from exc2string2          import exc2string2
from Factory_BaseClass    import Factory
from TestRunXMLConverter2 import TestRunXMLConverter
from TestRunXMLConverter2 import FailureTestRunXMLConverter
from RTTSException        import RTTCodingError

# -------------------------------------------------------------------------
# TestRun Factory
# -------------------------------------------------------------------------

class UnknownTestRunException: pass

class TestRunXMLConverterFactory(Factory):
  def __init__(self, logger):
    Factory.__init__(self, logger, self.__class__.__name__)

  def create(self,
             testRun,
             packageResultsDir,
             packageName,
             packageTag,
             containerPackage,
             callBackConverter,
             elementCreator,
             textNodeCreator,
             logger):

    trClassName = testRun.__class__.__name__
    # outputFile = os.path.join(packageResultsDir, packageName, 'rttpackageinfo.xml')
    outputFile = os.path.join(packageResultsDir, 'rttpackageinfo.xml')

    if trClassName == 'UnifiedConfigurationTestRun':
      try:
        converter = TestRunXMLConverter(outputFile, packageName, packageTag, containerPackage,
                                        elementCreator, textNodeCreator, callBackConverter, logger)
      except Exception, e:
        m = 'Error creating TestRun: key %s\nException:\n%s\nTraceback:\n%s' % (trClassName, str(e), exc2string2())
        raise RTTCodingError(m)
      
    elif trClassName == 'FailureTestRun':
      try:
        converter = FailureTestRunXMLConverter(outputFile, packageName, packageTag, containerPackage,
                                               elementCreator, textNodeCreator, callBackConverter, logger)
      except Exception, e:
        m = 'Error creating TestRun: key %s\nException:\n%s\nTraceback:\n%s' % (trClassName, str(e), exc2string2())
        raise RTTCodingError(m)
      
    else:
      raise RTTCodingError('UnknownTestRunException %s' % trClassName)
    

    return converter
