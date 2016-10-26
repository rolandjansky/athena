# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.cmt_newasgtool
# @purpose streamline and ease the creation of new asg tools
# @author Will Buttinger
# @date September 2014

#Note - this code could use a serious rewrite, I just hacked it together to get something working

from __future__ import with_statement

__version__ = "$Revision: 756300 $"
__author__ = "Will Buttinger"
__doc__ = "streamline and ease the creation of new AsgTool"

### imports -------------------------------------------------------------------
import os
import textwrap
import commands
import PyUtils.acmdlib as acmdlib
import fileinput

class Templates:
    tool_hdr_template = """\
#ifndef %(guard)s
#define %(guard)s 1

///
///Example usage code:
///Athena only:
///To configure the tool, add the following to your joboptions:
/// myTool = CfgMgr.%(namespace_klass)s("ToolInstanceName",Property=4.0)
/// import ROOT
/// myTool.ENumProperty = ROOT.%(namespace_klass)s.Val2
///To use the tool in your algorithm (can use regular ToolHandle or asg::AnaToolHandle):
/// #include "%(pkg)s/I%(klass)s.h"
/// ToolHandle<I%(klass)s> myTool("%(namespace2)s%(klass)s/ToolInstanceName")
/// CHECK( myTool.retrieve() );
/// myTool->isSelected(...); //or whatever methods are implemented in I%(klass)s
///
///Dual use: Alternative for c++ configuration and usage (with or without athena):
/// #include "AsgTools/AnaToolHandle.h"
/// #include "%(pkg)s/I%(klass)s.h"
/// asg::AnaToolHandle<I%(klass)s> myTool("%(namespace2)s%(klass)s/ToolInstanceName");
/// myTool.setProperty( "Property", 4.0 );
/// myTool.setProperty( "ENumProperty", 
/// myTool.initialize();
/// myTool->isSelected(....); //or whatever methods are implemented in I%(klass)s



#include "AsgTools/AsgTool.h"

#include "%(pkg)s/I%(klass)s.h"

%(namespace_begin)s

class %(klass)s: public asg::AsgTool, public virtual I%(klass)s { 
 public: 
   //constructor for athena can be created using special macro
   //Note: if you add a second interface to your tool, you must use: ASG_TOOL_CLASS2( ToolName, Interface1, Interface2) 
   ASG_TOOL_CLASS( %(klass)s , I%(klass)s )
   //add another constructor for non-athena use cases
   %(klass)s( const std::string& name );

   /// Initialize is required by AsgTool base class
   virtual StatusCode  initialize() override;

   // Example code: (part of skeleton code, delete as necessary)
   // Following method is required by I%(klass)s base class (part of skeleton code, delete as necessary)
   // DO NOT put default argument values here ... only put them in the interface class
   virtual bool isSelected(const xAOD::IParticle& particle, bool someAdditionalArgument) const override;

 private: 
   double m_nProperty;
   unsigned int m_enumProperty; //do not make the actual property an enum - not supported in athena
}; 
%(namespace_end)s
#endif //> !%(guard)s
"""

    tool_cxx_template = """\
// %(pkg)s includes
#include "%(pkg)s/%(namespace_klass)s.h"

%(namespace_begin)s

%(klass)s::%(klass)s( const std::string& name ) : asg::AsgTool( name ){

  declareProperty( "Property", m_nProperty = 3.0 , "Please describe the property here"); //example property declaration with it's default value
  declareProperty( "ENumProperty", m_enumProperty = Val1 , "Please define enums inside your classes, not just in namespaces" );
}


StatusCode %(klass)s::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  //
  //Make use of the property values to configure the tool
  //Tools should be designed so that no method other than setProperty is called before initialize
  //

  return StatusCode::SUCCESS;
}

//
//the lines below for the example interface method implementation
//This example is for a tool that selects particles with |eta| < "Property" value .. using 'someAdditionalArgument' to flip the cut
//
bool %(klass)s::isSelected( const xAOD::IParticle& particle , bool someAdditionalArgument ) const {
   if(someAdditionalArgument) return ( fabs(particle.eta()) >= m_nProperty );
   return ( fabs(particle.eta()) < m_nProperty );
}


%(namespace_end)s
"""

    tool_interface_template = """\
#ifndef %(guard)s
#define %(guard)s 1

#include "AsgTools/IAsgTool.h"

#include "xAODBase/IParticle.h"

class I%(klass)s : public virtual asg::IAsgTool {
   public:
      ASG_TOOL_INTERFACE( I%(klass)s ) //declares the interface to athena

      //below is an example method, delete as necessary
      //this is where you specify default values for additional arguments
      virtual bool isSelected(const xAOD::IParticle& particle, bool someAdditionalArgument = false) const = 0;

      //declare enums for properties here too, so that they are accessible through just the interface header
      //example of an enum you might end up using for your properties .. please put enums in a class! (not just a namespace)
      enum MyEnumProperty { Val1=1 , Val2=2 };

};

#endif //> !%(guard)s
"""
    tool_test_template = """ \
<?xml version="1.0"?>
<atn>
   <TEST name="%(pkg)s_makeCheck" type="makecheck">
      <package>%(fullpkgpath)s</package>
      <timelimit>5</timelimit>
      <author> PLEASE ENTER A NAME </author>
      <mailto> PLEASEENTER@cern.ch </mailto>
      <expectations>
         <errorMessage> Athena exited abnormally </errorMessage>
         <errorMessage>differ</errorMessage>
         <returnValue>0</returnValue>
      </expectations>
   </TEST>
</atn>
"""

    tool_testcpp_template = """ \
   
#include "AsgTools/AnaToolHandle.h"
#include "%(pkg)s/I%(klass)s.h"

//next line needed for using the concrete tool in this unit test
//external users cannot do this
#include "../Root/%(namespace_klass)s.cxx"

using namespace asg::msgUserCode;

int main() {
  ANA_CHECK_SET_TYPE (int); //makes ANA_CHECK return ints if exiting function
#ifdef ROOTCORE
  StatusCode::enableFailure();
  ANA_CHECK (xAOD::Init ());
#endif

   asg::AnaToolHandle<I%(klass)s> myTool("%(namespace2)s%(klass)s/ToolInstanceName");
   ANA_CHECK( myTool.setProperty( "Property", 4.0 ) );
   ANA_CHECK( myTool.setProperty( "EnumProperty", I%(klass)s::Val2 ) );
   ANA_CHECK( myTool.initialize() );

   //myTool->isSelected(....); put test code here

   //next line is how to gain access to concrete instance of the tool ... only ok for unit tests
   //%(namespace2)s%(klass)s* tool = dynamic_cast<%(namespace2)s%(klass)s*>(&*myTool);

   return 0; //zero = success
}
"""

    tool_testcppg_template = """ \
    
#include "AsgTools/AnaToolHandle.h"
#include "%(pkg)s/I%(klass)s.h"

#include "AsgTools/UnitTest.h"
#include <gtest/gtest.h>

//next line needed for using the concrete tool in this unit test                                                      
//external users cannot do this                                                                                       
#include "../Root/%(namespace_klass)s.cxx" 


using namespace asg::msgUserCode;

//first arg: name of fixture (if applicable), second arg: name of the test (special prefixes have special meanings)
//see https://twiki.cern.ch/twiki/bin/view/AtlasComputing/RootCore#Unit_Tests for more details
TEST (%(klass)sTest, basicTest) {
   asg::AnaToolHandle<I%(klass)s> myTool("%(namespace2)s%(klass)s/ToolInstanceName");
   ASSERT_SUCCESS (myTool.setProperty( "Property", 4.0 ));
   ASSERT_SUCCESS (myTool.setProperty( "EnumProperty", I%(klass)s::Val2 ));
   ASSERT_SUCCESS (myTool.initialize());

//   ASSERT_TRUE( myTool->isSelected( ...goodObject.. ) );

   //next line is how to gain access to concrete instance of the tool ... only ok for unit tests                      
   //%(namespace2)s%(klass)s* tool = dynamic_cast<%(namespace2)s%(klass)s*>(&*myTool);                                
                             


}

int main (int argc, char **argv)
{
  ANA_CHECK_SET_TYPE (int);
#ifdef ROOTCORE
  StatusCode::enableFailure();
  ANA_CHECK (xAOD::Init ());
#endif
  ::testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS();
}
"""

### functions -----------------------------------------------------------------
@acmdlib.command(
    name='cmt.new-asgtool'
    )
@acmdlib.argument(
    'toolname',
    help="name of the new tool"
    )
@acmdlib.argument(
    '--googleTest',
    action='store_true',
    default=False,
    help='Create a google test instead of a unit test'
    )
def main(args):
    """create a new AsgTool inside the current package with a corresponding interface class. Call from within the package directory

    ex:
     $ acmd cmt new-asgtool MyTool
    """
    sc = 0
    
    full_tool_name = args.toolname

    #determine the package from the cwd 
    cwd = os.getcwd()
    #check that cmt dir exists (i.e. this is a package)
    if not os.path.isdir(cwd+"/cmt"):
        print "ERROR you must call new-asgtool from within the package you want to add the tool to"
        return -1
    full_pkg_name = os.path.basename(cwd)
    print textwrap.dedent("""\
    ::: create asg tool [%(full_tool_name)s] in pkg [%(full_pkg_name)s]""" %locals())

    full_pk_path = os.getcwd().replace(os.environ['TestArea']+'/',"")

    
    #first we must check that requirements file has the AsgTools use statement in it
    #it must also have '../Root/*.cxx' in the library declaration
    #so look for patterns of: library xxxx or apply_pattern dual_use_library ...
    foundBaseComps=False
    hasxAODBase=False
    hasAtlasROOT=False
    haslcgdict=False
    lastUse=0 
    lineCount=0
    libraryLines = []
    hasLibraryLine=False
    for line in open('cmt/requirements'):
        lineCount +=1 
        if line.startswith("apply_pattern lcgdict"): haslcgdict=True
        if line.startswith("library") or line.startswith("apply_pattern dual_use_library"): hasLibraryLine=True
        if line.startswith("library "):
           if not ("../Root/*.cxx" in line):
              print ":::  INFO Adding Root directory to library compilation in requirements file (%s -> %s)" % (line.rstrip(), line.rstrip() + " ../Root/*.cxx")
              libraryLines += [lineCount]
        
        if not line.startswith("use "): continue
        lastUse=lineCount
        uu = line.split(" ")
        if uu[1].startswith("AsgTools"): foundBaseComps=True
        if uu[1].startswith("xAODBase"): hasxAODBase=True
        if uu[1].startswith("AtlasROOT"): hasAtlasROOT=True;
    if not hasLibraryLine:
      with open("cmt/requirements", "a") as myfile:
         myfile.write("library %s *.cxx ../Root/*.cxx components/*.cxx\n" % (full_pkg_name))
         myfile.write("apply_pattern component_library\n")
    
    if not foundBaseComps or len(libraryLines)>0 or not hasxAODBase or not hasAtlasROOT:
        
        #must add a use statement to the requirements file 
        #put inside public blocks
        lineCount=0
        inPrivate=False
        if not foundBaseComps: print ":::  INFO Adding AsgTools to requirements file"
        if not hasAtlasROOT: print ":::  INFO Adding AtlasROOT to requirements file"
        if not hasxAODBase: print ":::  INFO Adding xAODBase to requirements file"
        for line in fileinput.input('cmt/requirements', inplace=1):
            lineCount+=1
            if (not foundBaseComps or not hasAtlasROOT or not hasxAODBase) and lineCount==lastUse+1:
                if inPrivate: print "end_private"
                if not foundBaseComps:
                  print ""
                  print "use AsgTools AsgTools-* Control/AthToolSupport"
                  print ""
                if not hasAtlasROOT:
                  print "#uncomment the next line to use ROOT libraries in your package"
                  print "#use AtlasROOT AtlasROOT-* External"
                  print ""
                if not hasxAODBase:
                  print "#next line only needed for example code in asg tool...please remove if not using"
                  print "use xAODBase xAODBase-* Event/xAOD"
                  print ""
                if inPrivate: print "private"
            if line.startswith("private"): inPrivate=True
            elif line.startswith("end_private"): inPrivate=False
            if lineCount in libraryLines: print line.rstrip() + " ../Root/*.cxx\n",
            else: print line,
    
    if not haslcgdict:
       #append lcgdict pattern to requirements
       print ":::  INFO Adding Reflex Dictionary pattern (lcgdict) to requirements file"
       with open("cmt/requirements", "a") as myfile:
         myfile.write("")
         myfile.write("#Reflex Dictionary Generation:\n")
         myfile.write("private\n")
         myfile.write("use AtlasReflex AtlasReflex-* External\n")
         myfile.write("apply_pattern lcgdict dict=%s selectionfile=selection.xml headerfiles=\"../%s/%sDict.h\"\n" % (full_pkg_name,full_pkg_name,full_pkg_name))
         myfile.write("end_private\n")
         myfile.flush()
         myfile.close()



    
    #following code borrowed from gen_klass
    hdr = getattr(Templates, 'tool_hdr_template')
    cxx = getattr(Templates, 'tool_cxx_template')
    iface = getattr(Templates, 'tool_interface_template')
    testxml = getattr(Templates, 'tool_test_template')
    testcpp = getattr(Templates,'tool_testcpp_template')
    testcppg = getattr(Templates, 'tool_testcppg_template')
    
    namespace_klass = full_tool_name.replace('::','__')
    namespace_begin,namespace_end = "",""
    namespace = ""
    if full_tool_name.count("::")>0:
        namespace    = full_tool_name.split("::")[0]
        full_tool_name = full_tool_name.split("::")[1]
        namespace_begin = "namespace %s {" % namespace
        namespace_end   = "} //> end namespace %s" % namespace
        pass

    guard = "%s_%s_H" % (full_pkg_name.upper(), namespace_klass.upper())
    dictguard = "%s_%sDICT_H" % (full_pkg_name.upper(),full_pkg_name.upper())

    d = dict( pkg=full_pkg_name, fullpkgpath=full_pk_path, 
              klass=full_tool_name,
              guard=guard,dictguard=dictguard,
              namespace_begin=namespace_begin,
              namespace_end=namespace_end,namespace_klass=namespace_klass,namespace=namespace,namespace2=namespace
              )
    if not namespace == "": d["namespace2"]+="::"

    fname = os.path.splitext("%s/%s"%(full_pkg_name,namespace_klass))[0]
    
    #create package header folder if not existing already
    if not os.path.isdir(full_pkg_name):
       print ":::   INFO Creating %s folder" % full_pkg_name
       os.mkdir(full_pkg_name)

    #first check doesn't exist 
    if os.path.isfile(fname+'.h'):
       print ":::  WARNING %s.h already exists .. not overwriting" % fname
    else:
      print ":::  INFO Creating %s.h" % fname
      o_hdr = open(fname+'.h', 'w')
      o_hdr.writelines(hdr%d)
      o_hdr.flush()
      o_hdr.close()

    d["guard"] = "%s_I%s_H" % (full_pkg_name.upper(), full_tool_name.upper())

    fname = os.path.splitext("%s/I%s"%(full_pkg_name,full_tool_name))[0]
    if os.path.isfile(fname+'.h'):
       print ":::  WARNING %s.h already exists .. not overwriting" % fname
    else:
      print ":::  INFO Creating %s.h" % fname
      o_iface = open(fname+'.h', 'w')
      o_iface.writelines(iface%d)
      o_iface.flush()
      o_iface.close()

    #create Root folder if not existing already
    if not os.path.isdir("Root"):
       os.mkdir("Root")

    fname = os.path.splitext("Root/%s"%(namespace_klass))[0]
    if os.path.isfile(fname+'.cxx'):
       print ":::  WARNING %s.cxx already exists .. not overwriting" % fname
    else:
      print ":::  INFO Creating %s.cxx" % fname
      o_cxx = open(fname+'.cxx', 'w')
      o_cxx.writelines(cxx%d)
      o_cxx.flush()
      o_cxx.close()

    #if lcgdict generating, create/add selection.xml and create/add MyPackageDict.h
    if True:
       if not os.path.isfile("%s/%sDict.h"%(full_pkg_name,full_pkg_name)):
         print ":::  INFO Creating %s/%sDict.h" % (full_pkg_name,full_pkg_name)
         loadFile = open("%s/%sDict.h" % (full_pkg_name,full_pkg_name),'w')
         loadFile.writelines("""
#ifndef %(dictguard)s
#define %(dictguard)s

#include "%(pkg)s/%(namespace_klass)s.h"

#endif
"""%d)
         loadFile.flush()
         loadFile.close()
       else:
         print ":::  INFO Adding %s to %s/%sDict.h" % (namespace_klass,full_pkg_name,full_pkg_name)
         defineLine=False
         doneLine=False
         for line in fileinput.input("%s/%sDict.h"%(full_pkg_name,full_pkg_name), inplace=1):
            if defineLine and not doneLine:
               print """
#include "%(pkg)s/%(namespace_klass)s.h"
"""%d
               doneLine=True
            if line.startswith("#define"): defineLine=True
            print line,
         #FIXME: should check actually inserted it!
       
       if not os.path.isfile("%s/selection.xml"%(full_pkg_name)):
         print ":::  INFO Creating %s/selection.xml" % (full_pkg_name)
         loadFile = open("%s/selection.xml" % (full_pkg_name),'w')
         if not namespace == "":
            loadFile.writelines("""
<lcgdict>
   <namespace name="%(namespace)s" />
      <class name="%(namespace)s::%(klass)s" />
      <class name="I%(klass)s" />
</lcgdict>
"""%d)
         else:
            loadFile.writelines("""
<lcgdict>
   <class name="%(klass)s" />
   <class name="I%(klass)s" />
</lcgdict>
"""%d)
            loadFile.flush()
            loadFile.close()
       else:
          print ":::  INFO Adding %s and I%s to %s/selection.xml" % (namespace_klass,full_tool_name,full_pkg_name)
          doNamespace=False
          if not namespace == "":
             #check if namespace already defined in file
             if ('<namespace name="%s"' % namespace) not in open(("%s/selection.xml" % (full_pkg_name))).read():
                doNamespace=True
          dictLine=False
          done=False
          for line in fileinput.input("%s/selection.xml"%(full_pkg_name), inplace=1):
               if dictLine and not done:
                  if doNamespace: print """
   <namespace name="%(namespace)s" />"""%d
                  if not namespace == "":
                     print """
      <class name="%(namespace)s::%(klass)s" />
      <class name="I%(klass)s" />"""%d
                  else:
                      print """
      <class name="%(klass)s" />
      <class name="I%(klass)s" />"""%d
                  done = True
               if ((namespace=="" or doNamespace) and "<lcgdict>" in line) or (not namespace =="" and ('<namespace name="%s"' % namespace) in line):
                  dictLine=True
               print line,


    #check we have a test directory with appropriate xml file in it
    if not os.path.isdir("test"):
       os.mkdir("test")
    if not os.path.isfile("test/%s.xml"%full_pkg_name):
       print ":::  INFO Creating test/%s.xml"%full_pkg_name
       loadFile = open("test/%s.xml"%full_pkg_name,'w')
       loadFile.writelines(testxml%d)
       loadFile.flush()
       loadFile.close()


    print ":::  INFO Adding 'make check' test to requirements file"
    with open("cmt/requirements", "a") as myfile:
      myfile.write("")
      myfile.write("#test for %s ... compile and execute it with 'make check' \n"%namespace_klass)
      myfile.write("private\n")
      myfile.write("use TestTools      TestTools-*         AtlasTest\n")
      if args.googleTest: 
         myfile.write("use AtlasGoogleTest AtlasGoogleTest-* External\n")
         myfile.write("apply_pattern UnitTest_run unit_test=gt_%s\n"%namespace_klass)
         myfile.write("macro_append gt_%s_test_dependencies \" %s %sMergeComponentsList \"\n" % (namespace_klass, full_pkg_name, full_pkg_name))
      else:
         myfile.write("apply_pattern UnitTest_run unit_test=ut_%s\n"%namespace_klass)
         myfile.write("macro_append ut_%s_test_dependencies \" %s %sMergeComponentsList \"\n" % (namespace_klass, full_pkg_name, full_pkg_name))
      myfile.write("end_private\n")
      myfile.flush()
      myfile.close()

    if args.googleTest:
       if not os.path.isfile("test/gt_%s_test.cxx" % namespace_klass):
         print ":::  INFO Creating test/gt_%s_test.cxx" % namespace_klass
         loadFile = open("test/gt_%s_test.cxx" % namespace_klass,'w')
         loadFile.writelines(testcppg%d)
         loadFile.flush()
         loadFile.close()
    else:
      if not os.path.isfile("test/ut_%s_test.cxx" % namespace_klass):
         print ":::  INFO Creating test/ut_%s_test.cxx" % namespace_klass
         loadFile = open("test/ut_%s_test.cxx" % namespace_klass,'w')
         loadFile.writelines(testcpp%d)
         loadFile.flush()
         loadFile.close()


    #now add the tool to the _entries.cxx file in the components folder 
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
          d["namespace"] = args.toolname.split("::")[0]
          loadFile.writelines("""
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "%(pkg)s/%(namespace_klass)s.h"

DECLARE_NAMESPACE_TOOL_FACTORY(%(namespace)s, %(klass)s )

DECLARE_FACTORY_ENTRIES( %(pkg)s ) 
{
  DECLARE_NAMESPACE_TOOL(%(namespace)s, %(klass)s );
}
"""%d)
       else:
          loadFile.writelines("""
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "%(pkg)s/%(namespace_klass)s.h"

DECLARE_TOOL_FACTORY( %(klass)s )

DECLARE_FACTORY_ENTRIES( %(pkg)s ) 
{
  DECLARE_TOOL( %(klass)s );
}
"""%d)
       loadFile.flush()
       loadFile.close()
    else:
       #first check tool not already in _entries file 
       inFile=False
       for line in open("src/components/%s_entries.cxx"%full_pkg_name):
          if len(namespace_begin)==0 and "DECLARE_TOOL" in line and d["klass"] in line: inFile=True
          if len(namespace_begin)>0 and "DECLARE_NAMESPACE_TOOL" in line and d["klass"] in line and d["namespace"]: inFile=True
          
       if not inFile:
         print ":::  INFO Adding %s to src/components/%s_entries.cxx"% (args.toolname,full_pkg_name)
         nextAdd=False
         for line in fileinput.input("src/components/%s_entries.cxx"%full_pkg_name, inplace=1):
            if nextAdd and not "{" in line:
               nextAdd=False
               if len(namespace_begin)>0:
                  print """  DECLARE_NAMESPACE_TOOL(%(namespace)s, %(klass)s );"""%d
               else:
                  print """  DECLARE_TOOL( %(klass)s );"""%d
            if line.startswith("DECLARE_FACTORY_ENTRIES"):
               nextAdd=True
               if len(namespace_begin)>0:
                  
                  print """
#include "%(pkg)s/%(namespace_klass)s.h"
DECLARE_NAMESPACE_TOOL_FACTORY( %(namespace)s, %(klass)s )
"""%d
               else:
                  print """
#include "%(pkg)s/%(namespace_klass)s.h"
DECLARE_TOOL_FACTORY( %(klass)s )
"""%d
            print line,
          
    #to finish up, call cmt config so that the new tool will be captured and genconf run on it
    cwd = os.getcwd()
    try:
        os.chdir('cmt')
        _ = commands.getstatusoutput('cmt config')
    finally:
        os.chdir(cwd)
