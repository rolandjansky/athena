# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##==============================================================================
## Name:        LogicalFilterCombiner.py
##
## Author:      Marcello Barisonzi (DESY)
## Created:     September 2008
##
## Major modifications:
##  -September 2010 (David Cote): inherits from PyAthena.AthFilterAlgorithm and removed local implementation of py_alg()
##
## Description: This algorithm combines the results of one or more skim filters
##              
##==============================================================================

__doc__ = """This algorithm combines the results of one or more skim filters"""
__version__ = "0.0.1"
__author__  = "Marcello Barisonzi <Marcello.Barisonzi@desy.de>"

import AthenaPython.PyAthena as PyAthena

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.Logging import logging

import tokenize
from io import StringIO

class LogicalFilterCombiner( PyAthena.AthFilterAlgorithm ):
    
    def __init__ ( self, name = 'LogicalFilterCombiner', **kw ):
        ## initialize base class
        kw['name'] = name
        super(LogicalFilterCombiner, self).__init__(**kw)
        self.nEventsProcessed=0
        self.nEventsPassed=0
        
        self.cmdstring = kw.get('cmdstring', '')
        self.prefix    = kw.get('prefix', '')

        # Create the algorithm dictionary
        self.algdict = {}

        return


    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( '************************************' )
        self.cutFlowSvc().setFilterDescription(self.cutID,self.cmdstring)

        # look if parentheses are matched
        if self.cmdstring.count("(") != self.cmdstring.count(")"):
            self.msg.fatal("Mismatched parentheses in filter string: %s" % self.cmdstring)
            return False

        # these parentheses are not logically correct
        if self.cmdstring.count("{") != 0 or \
               self.cmdstring.count("}") != 0 or \
               self.cmdstring.count("[") != 0 or \
               self.cmdstring.count("]") != 0:
            self.msg.fatal("Wrong type of parentheses in filter string: %s" % self.cmdstring)
            return False        

        try:
            tokenobj = tokenize.generate_tokens(StringIO(self.cmdstring).readline)

            self.algdict = {}

            result = []

            for toknum, tokval, _, _, _  in tokenobj:
                if toknum == tokenize.NAME and \
                       tokval != 'or'      and \
                       tokval != 'not'     and \
                       tokval != 'and'     and \
                       tokval != 'True'    and \
                       tokval != 'False':

                    if self.prefix != '':
                        tokval = "%s_%s" % (self.prefix, tokval)

                    self.msg.info("Adding algorithm: %s", tokval)


                    _alg = None

                    _alg = PyAthena.py_alg(tokval)

                    self.algdict.update({tokval : _alg})

                    if not _alg:
                        self.msg.error("Algorithm %s not found" , tokval)
                        return False
                    else:
                        self.msg.debug("Found algorithm: %s -> %s" % (tokval, _alg))

                    exec('self.%s = _alg' % tokval)
                    self.cutFlowSvc().declareChildFilter(tokval,self.cutID)

                    result.extend([
                        (tokenize.STRING, 'self.%s' % tokval),
                        (tokenize.OP,   '.'),
                        (tokenize.STRING, 'filterPassed'),
                        (tokenize.OP, '('),
                        (tokenize.OP, ')')
                        ])

                else: 
                    result.append((toknum, tokval))

            self.cmd = tokenize.untokenize(result)

            self.msg.debug("String changed internally to:\n%s", self.cmd)

            #execute command once to validate
            response = bool(eval(self.cmd))
        except Exception as e:
            self.msg.fatal("Not a valid Python string. Exception: %s" % e)
            import traceback
            self.msg.fatal(traceback.format_exc())
            return False

        self.msg.info("Filter string validated")
        
        return True

    def execute(self):

        self.nEventsProcessed+=1
        for k,v in self.algdict.iteritems():
            self.msg.debug("Alg %s : %s" % (k, v.filterPassed()))
            pass

        

        response = bool(eval(self.cmd))
        if response:
            self.nEventsPassed+=1
        self.msg.debug("Response is: %s" % response)
        self.setFilterPassed(response)
        return True

    def finalize(self):
        self.msg.info("nEventsProcessed is: %s" % self.nEventsProcessed)
        self.msg.info("nEventsPassed is: %s" % self.nEventsPassed)        
        return True


