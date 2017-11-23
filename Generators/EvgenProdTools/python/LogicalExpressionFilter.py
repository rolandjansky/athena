#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##==============================================================================
## Name:        LogicalExpressionFilter.py
##
## Author:      Will Buttinger
## Created:     November 2014
##
## 
## Description: This algorithm combines the results of one or more filters ..
##                It is similar to LogicalFilterCombiner (from PrimaryDPDMaker) except it manages its own filters
##                It is efficient: only 'executes' the filters if necessary
##              
##==============================================================================

__doc__ = """This algorithm combines the results of one or more filters"""
__version__ = "0.0.1"
__author__  = "Will Buttinger <will@cern.ch>"

import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode
from GaudiKernel.GaudiHandles import *
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.Logging import logging

import tokenize
from cStringIO import StringIO

class LogicalExpressionFilter( PyAthena.Alg ):
    _isLocked=False
    def __init__ ( self, name = 'LogicalExpressionFilter', **kw ):
        ## initialize base class
        kw['name'] = name
        super(LogicalExpressionFilter, self).__init__(**kw)
        self.nEventsProcessed=0
        self.nEventsPassed=0
        self.nEventsProcessedWeighted=0
        self.nEventsPassedWeighted=0
        self.Expression = kw.get('Expression', '')
        self.UseEventWeight = kw.get('UseEventWeight',True)
        self.McEventKey = kw.get('McEventKey','GEN_EVENT')
        self.OutputLevel = kw.get('OutputLevel',3)
        # Create the algorithm dictionary
        self.algdict = {}
        # and a list for the owned algs (we have to call finalize ourself)
        self.ownedAlgs = []
        # Create the filters list
        self.filters = []
        
        #where the transformed cmd goes
        self.cmd = ""

        self._isLocked=True
        
        return

    def __setattr__(self,name,value):
       if self._isLocked and not hasattr(self,name) and name != "_cppHandle": #seems like we need a special exception for cppHandle
          #raise AttributeError("No")
          raise AttributeError("%s not an attribute of %s" % (name,self.getName()))
       return super(LogicalExpressionFilter,self).__setattr__(name,value)

    def initialize(self):
        self.algdict = {}
        filterNames = []
        # go through filters, creating if necessary 
        for filterTypeAndName in self.filters:
           l = filterTypeAndName.split('/')
           filterType = l[0]
           filterName = l[1]
           filterNames += [filterName]
           _alg = PyAthena.py_alg(filterName,'IAlgorithm')
           if not _alg:
              #try to create 
              algmgr = PyAthena.py_svc('ApplicationMgr',iface='IAlgManager')
              if not algmgr:
                  error = 'could not retrieve IAlgManager/ApplicationMgr'
                  self.msg.error (error)
                  raise RuntimeError (error)
              import PyCintex
              _alg = PyCintex.libPyROOT.MakeNullPointer("IAlgorithm")
              if algmgr.createAlgorithm(filterType,filterName,_alg).isFailure() or not _alg:
                  self.msg.error ('could not create alg: ' + filterTypeAndName)
                  raise RuntimeError ('could not create alg: ' + filterTypeAndName)
              #we are responsible for initializing it too 
              if _alg.sysInitialize().isFailure():
                 self.msg.error('Failed to initialize alg: ' + filterTypeAndName)
                 raise RuntimeError('Failed not initialize alg: ' + filterTypeAndName)
              self.ownedAlgs += [_alg]
           self.algdict[filterName] = _alg
       
       
        if self.Expression=="":
           #do a simple and of all the filters given
           self.Expression = " and ".join( filterNames )
        
        self.msg.debug("Filter Expression = " + self.Expression)

        # look if parentheses are matched
        if self.Expression.count("(") != self.Expression.count(")"):
            self.msg.fatal("Mismatched parentheses in filter string: %s" % self.Expression)
            return StatusCode.Failure

        # these parentheses are not logically correct
        if self.Expression.count("{") != 0 or \
               self.Expression.count("}") != 0 or \
               self.Expression.count("[") != 0 or \
               self.Expression.count("]") != 0:
            self.msg.fatal("Wrong type of parentheses in filter string: %s" % self.Expression)
            return StatusCode.Failure        

        
           
        try:
            tokenobj = tokenize.generate_tokens(StringIO(self.Expression).readline)


            result = []

            for toknum, tokval, _, _, _  in tokenobj:
                if toknum == tokenize.NAME and \
                       tokval != 'or'      and \
                       tokval != 'not'     and \
                       tokval != 'and'     and \
                       tokval != 'True'    and \
                       tokval != 'False':


                    #check the token is a known alg 
                    if tokval not in self.algdict:
                       self.msg.error("Unknown alg : " + tokval)
                       return StatusCode.Failure


                    result.extend([
                        (tokenize.STRING, 'self'),
                        (tokenize.OP,   '.'),
                        (tokenize.STRING, 'evalFilter'),
                        (tokenize.OP, '('),
                        (tokenize.OP, '"%s"' % tokval),
                        (tokenize.OP, ')')
                        ])

                else: 
                    result.append((toknum, tokval))

            self.cmd = tokenize.untokenize(result)

            self.msg.debug("String changed internally to:\n%s", self.cmd)

            #execute command once to validate
            #response = bool(eval(self.cmd))
        except Exception, e:
            self.msg.fatal("%s is not a valid Python expression string. Exception: %s" % (self.Expression,e))
            return StatusCode.Failure

        return StatusCode.Success

    def evalFilter(self, filterName):
      if not self.algdict[filterName].isExecuted():
         self.algdict[filterName].sysExecute( )
#         self.algdict[filterName].sysExecute( self.getContext() ) # for rel. 22
         self.algdict[filterName].setExecuted(True)
      decision = self.algdict[filterName].filterPassed()
      self.msg.verbose(filterName + " decision=" + str(decision))
      return decision

    def resetExecuted(self):
      #reset all the filter algs too 
      for (algName,alg) in self.algdict.items():
         alg.resetExecuted()
      return super(LogicalExpressionFilter,self).resetExecuted()

    def execute(self):
        #extract from first event of McEventCollection
        event_weight=1
        mc = self.evtStore[self.McEventKey]
        if mc.size()==0:
            self.msg.error("No events in McEventCollection!?")
            return StatusCode.Failure
        weights = mc[0].weights()
        if weights.size()==0:
            self.msg.debug("No weights to retrieve")
        else:
            event_weight = weights[0]

        self.nEventsProcessed+=1
        self.nEventsProcessedWeighted+=event_weight
        response = bool(eval(self.cmd)) if self.cmd else True
        if response:
            self.nEventsPassed+=1
            self.nEventsPassedWeighted+=event_weight
        self.msg.debug("My decision is: %s" % response)
        self.setFilterPassed(response)
        return StatusCode.Success

    def finalize(self):
        #call finalize of all the owned algs
        for alg in self.ownedAlgs: 
          if alg.finalize().isFailure():
             return StatusCode.Failure

        self.msg.info("Filter Expression = " + self.Expression)
        efficiency = float(self.nEventsPassed)/float(self.nEventsProcessed) if self.nEventsProcessed!=0 else 0.0
        efficiencyWeighted = float(self.nEventsPassedWeighted)/float(self.nEventsProcessedWeighted) if self.nEventsProcessedWeighted!=0 else 0.0
        self.msg.info("Filter Efficiency = %f [%s / %s]" % (efficiency,self.nEventsPassed,self.nEventsProcessed))
        self.msg.info("Weighted Filter Efficiency = %f [%f / %f]" % (efficiencyWeighted,self.nEventsPassedWeighted,self.nEventsProcessedWeighted))
        print("MetaData: GenFiltEff = %e" % (efficiencyWeighted if self.UseEventWeight else efficiency))
        return StatusCode.Success


    def setup(self):
       self._isLocked=False #need to unlock because if this configurable is pickled and later recreated, if _isLocked is set before another attribute, that attribute is locked out (seems that unpickling doesn't cause __init__ to be recalled?) 
       #check for spurious properties of myself, which would indicate the user has made mistakes
       self.filters = [ d.getFullName() for d in self.getChildren() ]
       return super(LogicalExpressionFilter,self).setup()
