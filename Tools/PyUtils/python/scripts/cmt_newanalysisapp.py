# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.cmt_newalg
# @purpose streamline and ease the creation of new standalone applications
# @author Will Buttinger
# @date September 2014

#Note - this code could use a serious rewrite, I just hacked it together to get something working

from __future__ import with_statement

__version__ = "$Revision: 802385 $"
__author__ = "Will Buttinger"
__doc__ = "streamline and ease the creation of new standalone applications"

### imports -------------------------------------------------------------------
import os
import textwrap
import commands
import PyUtils.acmdlib as acmdlib
import fileinput

class Templates:
    app_template = """\
    
    
#ifndef %(guard)s
#define %(guard)s 1


#include "AthAnalysisBaseComps/AthAnalysisHelper.h" //tool creation and configuration
#include "POOLRootAccess/TEvent.h" //event looping
#include "GaudiKernel/ToolHandle.h" //for better working with tools

#include "AsgTools/MessageCheck.h" //messaging
using namespace asg::msgUserCode;  //messaging

//ROOT includes
#include "TString.h"
#include "TSystem.h"

int main( int argc, char* argv[] ) {

   IAppMgrUI* app = Gaudi::Init(); //important to do this first, for MessageSvc to exist properly for MSG_INFO macro

   // Open the input file:
   TString fileName = "$ASG_TEST_FILE_MC";
   if( argc < 2 ) {
      ANA_MSG_WARNING( "No file name received, using $ASG_TEST_FILE_MC" );
   } else {
      fileName = argv[1]; //use the user provided file
   }
   ANA_MSG_INFO("Opening file: " << gSystem->ExpandPathName(fileName.Data()) );

   //Here's an example of how you would create a tool of type ToolType, and set a property on it
   //The preferred way to create and configure the tool is with a ToolHandle:
   //ToolHandle<IToolInterface> myTool("ToolType/myTool");
   //AAH::setProperty( myTool, "MyProperty", value );
   //myTool.retrieve(); //this will cause the tool to be created and initialized

   //loop over input file with POOL 
   POOL::TEvent evt(POOL::TEvent::kPOOLAccess); 

   //read modes (constructor argument) are:
   //kPOOLAccess = default (slowest but reads all POOL file types (including xAOD)), 
   //kAthenaAccess, (just xAOD)
   //kClassAccess, (just xAOD, but not some primary xAOD)
   //kBranchAccess (just xAOD, fastest but might not always work)


   evt.readFrom( fileName );

   for(int i=0;i < evt.getEntries(); i++) {
      if( evt.getEntry(i) < 0) { ANA_MSG_ERROR("Failed to read event " << i); continue; }

      //put your code here
      //e.g.
      //const xAOD::EventInfo* evtInfo = 0;
      //evt.retrieve( evtInfo );

   }

   app->finalize(); //optional finalization of all services and tools created by the Gaudi Application
   return 0;
}

#endif //> !%(guard)s
"""

### functions -----------------------------------------------------------------
@acmdlib.command(
    name='cmt.new-analysisapp'
    )
@acmdlib.argument(
    'appname',
    help="name of the new app"
    )
def main(args):
    """create a new algorithm inside the current package. Call from within the package directory

    ex:
     $ acmd cmt new-alg MyAlg
    """
    sc = 0
    
    full_app_name = args.appname

    #determine the package from the cwd 
    cwd = os.getcwd()
    #check that cmt dir exists (i.e. this is a package)
    if not os.path.isdir(cwd+"/cmt"):
        print "ERROR you must call new-analysisapp from within the package you want to add the app to"
        return -1
    full_pkg_name = os.path.basename(cwd)
    print textwrap.dedent("""\
    ::: create analysisapp [%(full_app_name)s] in pkg [%(full_pkg_name)s]""" %locals())

    
    #first we must check that requirements file has the AthAnalysisBaseComps use statement in it
    #also need AsgTools and 
    foundBaseComps=False    
    foundAsgTools=False
    foundPOOLRootAccess=False
    foundROOT=False
    lastUse=0 
    lineCount=0
    for line in open('cmt/requirements'):
        lineCount +=1 
        if not line.startswith("use "): continue
        lastUse=lineCount
        uu = line.split(" ")
        if uu[1].startswith("AthAnalysisBaseComps"): foundBaseComps=True
        if uu[1].startswith("AsgTools"): foundAsgTools=True
        if uu[1].startswith("POOLRootAccess"): foundPOOLRootAccess=True
        if uu[1].startswith("AtlasROOT"): foundROOT=True
        
    if not foundBaseComps or not foundAsgTools or not foundPOOLRootAccess or not foundROOT:
        print ":::  INFO Adding dependencies to requirements file"
        #must add a use statement to the requirements file 
        #put inside private blocks
        lineCount=0
        inPrivate=False
        for line in fileinput.input('cmt/requirements', inplace=1):
            lineCount+=1
            if lineCount==lastUse+1:
                if not inPrivate: print "private"
                print ""
                if not foundBaseComps:
                  print "use AthAnalysisBaseComps AthAnalysisBaseComps-* Control"
                if not foundAsgTools:
                  print "use AsgTools AsgTools-* Control/AthToolSupport"
                if not foundPOOLRootAccess:
                  print "use POOLRootAccess POOLRootAccess-* PhysicsAnalysis"
                if not foundROOT:
                  print "use AtlasROOT AtlasROOT-* External"
                print ""
                if not inPrivate: print "end_private"
            if line.startswith("private"): inPrivate=True
            elif line.startswith("end_private"): inPrivate=False
            print line,
     
    
    
    #following code borrowed from gen_klass
    cxx = getattr(Templates, 'app_template')
    
    namespace_klass = full_app_name.replace('::','__')
    namespace_begin,namespace_end = "",""
    namespace = ""
    if full_app_name.count("::")>0:
        namespace    = full_app_name.split("::")[0]
        full_alg_name = full_app_name.split("::")[1]
        namespace_begin = "namespace %s {" % namespace
        namespace_end   = "} //> end namespace %s" % namespace
        pass

    guard = "%s_%s_H" % (full_pkg_name.upper(), namespace_klass.upper())

    #create util directory if not existing
    if not os.path.exists("util"): 
        print ":::  INFO Creating util directory"
        os.makedirs("util")

    d = dict( pkg=full_pkg_name,
              klass=full_app_name,
              guard=guard,
              namespace_begin=namespace_begin,
              namespace_end=namespace_end,namespace_klass=namespace_klass,namespace=namespace
              )
    fname = os.path.splitext("util/%s"%namespace_klass)[0]


    if os.path.isfile(fname+'.cxx'):
       print ":::  ERROR %s.cxx already exists" % fname
       return -1
    print ":::  INFO Creating %s.cxx" % fname
    o_cxx = open(fname+'.cxx', 'w')
    o_cxx.writelines(cxx%d)
    o_cxx.flush()
    o_cxx.close()

     #append application line
    print ":::  INFO adding 'application %s ../util/%s.cxx' to requirements" % (full_app_name,full_app_name)
    with open("cmt/requirements", "a") as myfile:
         myfile.write("application %s ../util/%s.cxx\n" % (full_app_name,full_app_name))

    #to finish up, call cmt config so that the new algorithm will be captured and genconf run on it
    cwd = os.getcwd()
    try:
        os.chdir('cmt')
        _ = commands.getstatusoutput('cmt config')
    finally:
        os.chdir(cwd)
