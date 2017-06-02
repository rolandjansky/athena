# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.cmt_newalg
# @purpose streamline and ease the creation of new athena algs
# @author Will Buttinger
# @date September 2014

#Note - this code could use a serious rewrite, I just hacked it together to get something working

from __future__ import with_statement

__version__ = "$Revision: 623437 $"
__author__ = "Will Buttinger"
__doc__ = "streamline and ease the creation of new python algorithms"

### imports -------------------------------------------------------------------
import os
import textwrap
import commands
import PyUtils.acmdlib as acmdlib
import fileinput

class Templates:
   pyalg_template = """\
# @file:    %(pkg)s/python/%(fname)s
# @purpose: <put some purpose here>
# @author:  <put your name here>

__doc__     = 'some documentation here'
__version__ = '$Revision: 623437 $'
__author__  = '<put your name here>'

import AthenaCommon.SystemOfUnits as Units
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

class %(klass)s (PyAthena.Alg):
    'put some documentation here'
    def __init__(self, name='%(klass)s', **kw):
        ## init base class
        kw['name'] = name
        super(%(klass)s, self).__init__(**kw)

        ## properties and data members
        #self.foo = kw.get('foo', 10) # default value
        return

    def initialize(self):
        self.msg.info('==> initialize...')
        return StatusCode.Success

    def execute(self):
        #here's an example of how to loop over something from xAOD
        #for electron in self.evtStore["ElectronCollection"]: print electron.pt()
        
        return StatusCode.Success

    def finalize(self):
        self.msg.info('==> finalize...')
        return StatusCode.Success

    # class %(klass)s
"""


### functions -----------------------------------------------------------------
@acmdlib.command(
    name='cmt.new-pyalg'
    )
@acmdlib.argument(
    'algname',
    help="name of the new pyalg"
    )
def main(args):
    """create a new algorithm inside the current package. Call from within the package directory

    ex:
     $ acmd cmt new-alg MyAlg
    """
    sc = 0
    
    full_alg_name = args.algname

    #determine the package from the cwd 
    cwd = os.getcwd()
    #check that cmt dir exists (i.e. this is a package)
    if not os.path.isdir(cwd+"/cmt"):
        print "ERROR you must call new-pyalg from within the package you want to add the algorithm to"
        return -1
    full_pkg_name = os.path.basename(cwd)
    print textwrap.dedent("""\
    ::: create pyalg [%(full_alg_name)s] in pkg [%(full_pkg_name)s]""" %locals())

    
    
    #following code borrowed from gen_klass
    hdr = getattr(Templates, 'pyalg_template')
    
    namespace_klass = full_alg_name.replace('::','__')
    namespace_begin,namespace_end = "",""
    namespace = ""
    if full_alg_name.count("::")>0:
        namespace    = full_alg_name.split("::")[0]
        full_alg_name = full_alg_name.split("::")[1]
        namespace_begin = "namespace %s {" % namespace
        namespace_end   = "} //> end namespace %s" % namespace
        pass

    d = dict( pkg=full_pkg_name,
              klass=full_alg_name,
              namespace_begin=namespace_begin,
              namespace_end=namespace_end,namespace_klass=namespace_klass,namespace=namespace,fname=namespace_klass+".py"
              )
    fname = os.path.splitext("python/%s"%namespace_klass)[0]
    #first check doesn't exist 
    if os.path.isfile(fname+'.u'):
       print ":::  ERROR %s.py already exists" % fname
       return -1
    o_hdr = open(fname+'.py', 'w')
    o_hdr.writelines(hdr%d)
    o_hdr.flush()
    o_hdr.close()
    
    #need to create a dummy cxx file in the src folder, if there is nothing in there 
    import glob
    if len(glob.glob('src/*.cxx'))==0:
       #create a dummy file
       o_hdr = open('src/dummy.cxx', 'w')
       o_hdr.writelines('//dummy file created to keep cmt happy when compiling only with python algs')
       o_hdr.flush()
       o_hdr.close()

    #to finish up, call cmt config so that the new algorithm will be captured and genconf run on it
    cwd = os.getcwd()
    try:
        os.chdir('cmt')
        _ = commands.getstatusoutput('cmt config')
    finally:
        os.chdir(cwd)
