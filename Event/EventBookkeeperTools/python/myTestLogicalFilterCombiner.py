# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##==============================================================================
## Name:        myTestLogicalFilterCombiner.py  (completely inspired from LogicalFilterCombiner.py)
##
## Author:      Joao Firmino da Costa (DESY) 
## Created:     July 2010
##
## Description: This algorithm combines the results of one or more skim filters
##              
##==============================================================================

__doc__ = """This algorithm combines the results of one or more skim filters"""
__version__ = "0.0.1"
__author__  = "Joao Costa <joao.firmino.da.costa@desy.de>"

import AthenaPython.PyAthena as PyAthena

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.Logging import logging


import tokenize
from cStringIO import StringIO

class myTestLogicalFilterCombiner( PyAthena.AthFilterAlgorithm ):
    
    def __init__ ( self, name = 'myTestLogicalFilterCombiner', **kw ):
        ## initialize base class
        kw['name'] = name
        super(myTestLogicalFilterCombiner, self).__init__(**kw)
        
        self.cmdstring = kw.get('cmdstring', '')
        self.prefix    = kw.get('prefix', '')

        # Create the algorithm dictionary
        self.algdict = {}
        #self.EB = PyAthena.EventBookkeeper()
     

        
        return


    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( '************************************' )

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



        #self.cutFlow = PyAthena.py_svc('CutFlowSvc',iface='ICutFlowSvc')
        #   return StatusCode.Success
        self.msg.info("Should be registered automatically")
        #self.EB = self.cutFlow.selfRegisterFilter(self.name(),"TopFilter")


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

                    _alg = py_alg(tokval)
                    self.cutFlowSvc().declareChildFilter(tokval,self.cutID)
                   
                    
                    self.algdict.update({tokval : _alg})

                    if not _alg:
                        self.msg.error("Algorithm %s not found" , tokval)
                        return False
                    else:
                        self.msg.debug("Found algorithm: %s -> %s" % (tokval, _alg))
           

                    exec('self.%s = _alg' % tokval)

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
            #self.execute()
        except Exception, e:
            self.msg.fatal("Not a valid Python string. Exception: %s" % e)
            return False

        self.msg.info("Filter string validated")

   

        return True

    def execute(self):

        for k,v in self.algdict.iteritems():
#            self.msg.debug("Alg %s : %s" % (k, v.filterPassed()))
            pass

        

        response = bool(eval(self.cmd))
        
        if response :
            self.msg.info("Accepted Event in child filters")



        self.setFilterPassed(response)
        return True

    def finalize(self):
        return True


# retrieve algs --- this will be standard in 14.2.30
def py_alg(algname,iface='Algorithm'):
    algmgr = PyAthena.py_svc('ApplicationMgr',iface='IAlgManager')
    if not algmgr:
        msg = logging.getLogger('PyAthena.py_alg')
        error = 'could not retrieve IAlgManager/ApplicationMgr'
        msg.error (error)
        raise RuntimeError (error)
    
    import cppyy
    alg = cppyy.libPyROOT.MakeNullPointer(iface)
    if not algmgr.getAlgorithm(algname, alg).isSuccess():
        return
    return alg


#def py_svc(svcname='CutFlowSvc',iface='CutFlowSvc'):

#    svc = PyAthena.py_svc('CutFlowSvc',iface='ICutFlowSvc')
#    return  svc
