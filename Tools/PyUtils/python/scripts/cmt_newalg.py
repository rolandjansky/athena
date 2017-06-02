# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.cmt_newalg
# @purpose streamline and ease the creation of new athena algs
# @author Will Buttinger
# @date September 2014

#Note - this code could use a serious rewrite, I just hacked it together to get something working

from __future__ import with_statement

__version__ = "$Revision: 279982 $"
__author__ = "Will Buttinger"
__doc__ = "streamline and ease the creation of new algorithms"

### imports -------------------------------------------------------------------
import os
import textwrap
import commands
import PyUtils.acmdlib as acmdlib
import fileinput

class Templates:
    alg_hdr_template = """\
#ifndef %(guard)s
#define %(guard)s 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!

%(namespace_begin)s

class %(klass)s: public ::AthAlgorithm { 
 public: 
  %(klass)s( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~%(klass)s(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 

}; 
%(namespace_end)s
#endif //> !%(guard)s
"""

    alg_cxx_template = """\
// %(pkg)s includes
#include "%(namespace_klass)s.h"

%(namespace_begin)s

%(klass)s::%(klass)s( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator ){

  //declareProperty( "Property", m_nProperty ); //example property declaration

}


%(klass)s::~%(klass)s() {}


StatusCode %(klass)s::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode %(klass)s::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode %(klass)s::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  return StatusCode::SUCCESS;
}

%(namespace_end)s
"""


### functions -----------------------------------------------------------------
@acmdlib.command(
    name='cmt.new-alg'
    )
@acmdlib.argument(
    'algname',
    help="name of the new alg"
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
        print "ERROR you must call new-alg from within the package you want to add the algorithm to"
        return -1
    full_pkg_name = os.path.basename(cwd)
    print textwrap.dedent("""\
    ::: create alg [%(full_alg_name)s] in pkg [%(full_pkg_name)s]""" %locals())

    
    #first we must check that requirements file has the AthenaBaseComps use statement in it
    foundBaseComps=False
    hasLibraryLine=False
    lastUse=0 
    lineCount=0
    for line in open('cmt/requirements'):
        lineCount +=1 
        if line.startswith("library") or line.startswith("apply_pattern dual_use_library"): hasLibraryLine=True
        if not line.startswith("use "): continue
        lastUse=lineCount
        uu = line.split(" ")
        if uu[1].startswith("AthenaBaseComps"): foundBaseComps=True
        
    if not foundBaseComps:
        print ":::  INFO Adding AthenaBaseComps to requirements file"
        #must add a use statement to the requirements file 
        #put inside private blocks
        lineCount=0
        inPrivate=False
        for line in fileinput.input('cmt/requirements', inplace=1):
            lineCount+=1
            if lineCount==lastUse+1:
                if not inPrivate: print "private"
                print "use AthenaBaseComps AthenaBaseComps-* Control"
                if not inPrivate: print "end_private"
            if line.startswith("private"): inPrivate=True
            elif line.startswith("end_private"): inPrivate=False
            print line,
        #append library line if necessary
    if not hasLibraryLine:
      with open("cmt/requirements", "a") as myfile:
         myfile.write("library %s *.cxx components/*.cxx\n" % (full_pkg_name))
         myfile.write("apply_pattern component_library\n")
    
    #following code borrowed from gen_klass
    hdr = getattr(Templates, 'alg_hdr_template')
    cxx = getattr(Templates, 'alg_cxx_template')
    
    namespace_klass = full_alg_name.replace('::','__')
    namespace_begin,namespace_end = "",""
    namespace = ""
    if full_alg_name.count("::")>0:
        namespace    = full_alg_name.split("::")[0]
        full_alg_name = full_alg_name.split("::")[1]
        namespace_begin = "namespace %s {" % namespace
        namespace_end   = "} //> end namespace %s" % namespace
        pass

    guard = "%s_%s_H" % (full_pkg_name.upper(), namespace_klass.upper())

    d = dict( pkg=full_pkg_name,
              klass=full_alg_name,
              guard=guard,
              namespace_begin=namespace_begin,
              namespace_end=namespace_end,namespace_klass=namespace_klass,namespace=namespace
              )
    fname = os.path.splitext("src/%s"%namespace_klass)[0]
    #first check doesn't exist 
    if os.path.isfile(fname+'.h'):
       print ":::  ERROR %s.h already exists" % fname
       return -1
    print ":::  INFO Creating %s.h" % fname
    o_hdr = open(fname+'.h', 'w')
    o_hdr.writelines(hdr%d)
    o_hdr.flush()
    o_hdr.close()

    if os.path.isfile(fname+'.cxx'):
       print ":::  ERROR %s.cxx already exists" % fname
       return -1
    print ":::  INFO Creating %s.cxx" % fname
    o_cxx = open(fname+'.cxx', 'w')
    o_cxx.writelines(cxx%d)
    o_cxx.flush()
    o_cxx.close()

    #now add the algorithm to the _entries.cxx file in the components folder 
    #first check they exist 
    if not os.path.isfile("src/components/%s_load.cxx"%full_pkg_name):
       print ":::  INFO Creating src/components/%s_load.cxx"%full_pkg_name
       loadFile = open("src/components/%s_load.cxx"%full_pkg_name,'w')
       loadFile.writelines(""" 
#include "GaudiKernel/LoadFactoryEntries.h"
LOAD_FACTORY_ENTRIES(%(pkg)s)
"""%d)
       loadFile.flush()
       loadFile.close()
      
    if not os.path.isfile("src/components/%s_entries.cxx"%full_pkg_name):
       print ":::  INFO Creating src/components/%s_entries.cxx"%full_pkg_name
       loadFile = open("src/components/%s_entries.cxx"%full_pkg_name,'w')
       if len(namespace_begin)>0:
          d["namespace"] = args.algname.split("::")[0]
          loadFile.writelines("""
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../%(namespace_klass)s.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(%(namespace)s, %(klass)s )

DECLARE_FACTORY_ENTRIES( %(pkg)s ) 
{
  DECLARE_NAMESPACE_ALGORITHM(%(namespace)s, %(klass)s );
}
"""%d)
       else:
          loadFile.writelines("""
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../%(namespace_klass)s.h"

DECLARE_ALGORITHM_FACTORY( %(klass)s )

DECLARE_FACTORY_ENTRIES( %(pkg)s ) 
{
  DECLARE_ALGORITHM( %(klass)s );
}
"""%d)
       loadFile.flush()
       loadFile.close()
    else:
       #first check algorithm not already in _entries file 
       inFile=False
       for line in open("src/components/%s_entries.cxx"%full_pkg_name):
          if len(namespace_begin)==0 and "DECLARE_ALGORITHM" in line and d["klass"] in line: inFile=True
          if len(namespace_begin)>0 and "DECLARE_NAMESPACE_ALGORITHM" in line and d["klass"] in line and d["namespace"]: inFile=True
          
       if not inFile:
         print ":::  INFO Adding %s to src/components/%s_entries.cxx"% (args.algname,full_pkg_name)
         nextAdd=False
         for line in fileinput.input("src/components/%s_entries.cxx"%full_pkg_name, inplace=1):
            if nextAdd and not "{" in line:
               nextAdd=False
               if len(namespace_begin)>0:
                  print """  DECLARE_NAMESPACE_ALGORITHM(%(namespace)s, %(klass)s );"""%d
               else:
                  print """  DECLARE_ALGORITHM( %(klass)s );"""%d
            if line.startswith("DECLARE_FACTORY_ENTRIES"):
               nextAdd=True
               if len(namespace_begin)>0:
                  
                  print """
#include "../%(namespace_klass)s.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY( %(namespace)s, %(klass)s )
"""%d
               else:
                  print """
#include "../%(namespace_klass)s.h"
DECLARE_ALGORITHM_FACTORY( %(klass)s )
"""%d
            print line,
          
    #to finish up, call cmt config so that the new algorithm will be captured and genconf run on it
    cwd = os.getcwd()
    try:
        os.chdir('cmt')
        _ = commands.getstatusoutput('cmt config')
    finally:
        os.chdir(cwd)
