# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.scripts.cmt_newanalysisalg
# @purpose streamline and ease the creation of new athena algs
# @author Will Buttinger
# @date September 2014

#Note - this code could use a serious rewrite, I just hacked it together to get something working

from __future__ import with_statement

__version__ = "$Revision: 801634 $"
__author__ = "Will Buttinger"
__doc__ = "streamline and ease the creation of new AthAnalysisAlgorithm"

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

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

//Example ROOT Includes
//#include "TTree.h"
//#include "TH1D.h"

%(namespace_begin)s

class %(klass)s: public ::AthAnalysisAlgorithm { 
 public: 
  %(klass)s( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~%(klass)s(); 

  ///uncomment and implement methods as required

                                        //IS EXECUTED:
  virtual StatusCode  initialize();     //once, before any input is loaded
  virtual StatusCode  beginInputFile(); //start of each input file, only metadata loaded
  //virtual StatusCode  firstExecute();   //once, after first eventdata is loaded (not per file)
  virtual StatusCode  execute();        //per event
  //virtual StatusCode  endInputFile();   //end of each input file
  //virtual StatusCode  metaDataStop();   //when outputMetaStore is populated by MetaDataTools
  virtual StatusCode  finalize();       //once, after all events processed
  

  ///Other useful methods provided by base class are:
  ///evtStore()        : ServiceHandle to main event data storegate
  ///inputMetaStore()  : ServiceHandle to input metadata storegate
  ///outputMetaStore() : ServiceHandle to output metadata storegate
  ///histSvc()         : ServiceHandle to output ROOT service (writing TObjects)
  ///currentFile()     : TFile* to the currently open input file
  ///retrieveMetadata(...): See twiki.cern.ch/twiki/bin/view/AtlasProtected/AthAnalysisBase#ReadingMetaDataInCpp


 private: 

   //Example algorithm property, see constructor for declaration:
   //int m_nProperty = 0;

   //Example histogram, see initialize method for registration to output histSvc
   //TH1D* m_myHist = 0;
   //TTree* m_myTree = 0;

}; 
%(namespace_end)s
#endif //> !%(guard)s
"""

    alg_cxx_template = """\
// %(pkg)s includes
#include "%(namespace_klass)s.h"

//#include "xAODEventInfo/EventInfo.h"


%(namespace_begin)s

%(klass)s::%(klass)s( const std::string& name, ISvcLocator* pSvcLocator ) : AthAnalysisAlgorithm( name, pSvcLocator ){

  //declareProperty( "Property", m_nProperty = 0, "My Example Integer Property" ); //example property declaration

}


%(klass)s::~%(klass)s() {}


StatusCode %(klass)s::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  //
  //This is called once, before the start of the event loop
  //Retrieves of tools you have configured in the joboptions go here
  //

  //HERE IS AN EXAMPLE
  //We will create a histogram and a ttree and register them to the histsvc
  //Remember to configure the histsvc stream in the joboptions
  //
  //m_myHist = new TH1D("myHist","myHist",100,0,100);
  //CHECK( histSvc()->regHist("/MYSTREAM/myHist", m_myHist) ); //registers histogram to output stream
  //m_myTree = new TTree("myTree","myTree");
  //CHECK( histSvc()->regTree("/MYSTREAM/SubDirectory/myTree", m_myTree) ); //registers tree to output stream inside a sub-directory


  return StatusCode::SUCCESS;
}

StatusCode %(klass)s::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  //
  //Things that happen once at the end of the event loop go here
  //


  return StatusCode::SUCCESS;
}

StatusCode %(klass)s::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  setFilterPassed(false); //optional: start with algorithm not passed



  //
  //Your main analysis code goes here
  //If you will use this algorithm to perform event skimming, you
  //should ensure the setFilterPassed method is called
  //If never called, the algorithm is assumed to have 'passed' by default
  //


  //HERE IS AN EXAMPLE
  //const xAOD::EventInfo* ei = 0;
  //CHECK( evtStore()->retrieve( ei , "EventInfo" ) );
  //ATH_MSG_INFO("eventNumber=" << ei->eventNumber() );
  //m_myHist->Fill( ei->averageInteractionsPerCrossing() ); //fill mu into histogram


  setFilterPassed(true); //if got here, assume that means algorithm passed
  return StatusCode::SUCCESS;
}

StatusCode %(klass)s::beginInputFile() { 
  //
  //This method is called at the start of each input file, even if
  //the input file contains no events. Accumulate metadata information here
  //

  //example of retrieval of CutBookkeepers: (remember you will need to include the necessary header files and use statements in requirements file)
  // const xAOD::CutBookkeeperContainer* bks = 0;
  // CHECK( inputMetaStore()->retrieve(bks, "CutBookkeepers") );

  //example of IOVMetaData retrieval (see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_access_file_metadata_in_C)
  //float beamEnergy(0); CHECK( retrieveMetadata("/TagInfo","beam_energy",beamEnergy) );
  //std::vector<float> bunchPattern; CHECK( retrieveMetadata("/Digitiation/Parameters","BeamIntensityPattern",bunchPattern) );



  return StatusCode::SUCCESS;
}


%(namespace_end)s
"""
    testxml_template = """\
   <TEST name="%(namespace_klass)s" type="athena" suite="ASGTests">
      <options_atn>%(pkg)s/%(namespace_klass)sJobOptions.py</options_atn>
      <timelimit>5</timelimit>
      <author> PLEASE ENTER A NAME </author>
      <mailto> PLEASEENTER@cern.ch </mailto>
      <expectations>
         <errorMessage> Athena exited abnormally </errorMessage>
         <errorMessage>FAILURE (ERROR)</errorMessage>
         <returnValue>0</returnValue>
      </expectations>
   </TEST>
"""


### functions -----------------------------------------------------------------
@acmdlib.command(
    name='cmt.new-analysisalg'
    )
@acmdlib.argument(
    'algname',
    help="name of the new alg"
    )
@acmdlib.argument(
    '--newJobo',
    action='store_true',
    default=False,
    help='Create a skeleton joboption for execution of the new algorithm'
    )
@acmdlib.argument(
    '--atnTest',
    action='store_true',
    default=False,
    help='Register the skeleton joboption as an ATN test'
    )
def main(args):
    """create a new AthAnalysisAlgorithm inside the current package. Call from within the package directory

    ex:
     $ acmd cmt new-analysisalg MyAlg
    """
    sc = 0
    
    full_alg_name = args.algname

    #determine the package from the cwd 
    cwd = os.getcwd()
    #check that cmt dir exists (i.e. this is a package)
    if not os.path.isdir(cwd+"/cmt"):
        print "ERROR you must call new-analysisalg from within the package you want to add the algorithm to"
        return -1
   
    if args.atnTest and not full_alg_name.startswith("Test::"):
       print "::: INFO  Requested --atnTest option, so adding 'Test::' namespace to the alg"
       args.algname = "Test::%s"%args.algname
       full_alg_name = args.algname
   
    full_pkg_name = os.path.basename(cwd)
    print textwrap.dedent("""\
    ::: create alg [%(full_alg_name)s] in pkg [%(full_pkg_name)s]""" %locals())

    
    #first we must check that requirements file has the AthenaBaseComps use statement in it
    foundBaseComps=False
    hasxAODEventInfo=False
    hasAtlasROOT=False
    hasAsgTools=False
    lastUse=0 
    lineCount=0
    hasLibraryLine=False
    for line in open('cmt/requirements'):
        lineCount +=1 
        if line.startswith("library") or line.startswith("apply_pattern dual_use_library"): hasLibraryLine=True
        if not line.startswith("use "): continue
        lastUse=lineCount
        uu = line.split(" ")
        if uu[1].startswith("AthAnalysisBaseComps"): foundBaseComps=True
        if uu[1].startswith("xAODEventInfo"): hasxAODEventInfo=True
        if uu[1].startswith("AsgTools"): hasAsgTools=True
        if uu[1].startswith("AtlasROOT"): hasAtlasROOT=True;
        
        
    if not foundBaseComps:
        print ":::  INFO Adding AthAnalysisBaseComps to requirements file"
        #must add a use statement to the requirements file 
        #put inside private blocks
        lineCount=0
        inPrivate=False
        for line in fileinput.input('cmt/requirements', inplace=1):
            lineCount+=1
            if lineCount==lastUse+1:
                if not inPrivate: print "private"
                print ""
                print "use AthAnalysisBaseComps AthAnalysisBaseComps-* Control"
                print ""
                if not hasAtlasROOT:
                  print "#uncomment the next line to use ROOT libraries in your package"
                  print "#use AtlasROOT AtlasROOT-* External"
                  print ""
                if not hasxAODEventInfo:
                  print "#use xAODEventInfo xAODEventInfo-* Event/xAOD"
                  print ""
                if not hasAsgTools:
                  print "#use AsgTools AsgTools-* Control/AthToolSupport"
                  print ""
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
   
   
    if args.atnTest or args.newJobo:
      #make the joboptions file too
      full_jobo_name = namespace_klass + "JobOptions"
      full_alg_name = namespace_klass
   
      print textwrap.dedent("""\
      ::: create jobo [%(full_jobo_name)s] for alg [%(full_alg_name)s]""" %locals())
   
      #following code borrowed from gen_klass
      from cmt_newjobo import Templates as joboTemplates
      jobo = getattr(joboTemplates, 'jobo_template')
   
      e = dict( klass=full_alg_name,
               inFile=os.environ['ASG_TEST_FILE_MC'],
               )
      fname = 'share/%s.py' % full_jobo_name
      #first check doesn't exist 
      if os.path.isfile(fname):
         print ":::  WARNING %s already exists .. will not overwrite" % fname
      else:
         o_hdr = open(fname, 'w')
         o_hdr.writelines(jobo%e)
         o_hdr.flush()
         o_hdr.close()

    if args.atnTest:
      testxml = getattr(Templates, 'testxml_template')
      #add the test joboptions to the atn test
      #check we have a test directory with appropriate xml file in it
      if not os.path.isdir("test"):
         os.mkdir("test")
      if not os.path.isfile("test/%s.xml"%full_pkg_name):
         print ":::  INFO Creating test/%s.xml"%full_pkg_name
         loadFile = open("test/%s.xml"%full_pkg_name,'w')
         loadFile.writelines("<?xml version=\"1.0\"?>\n<atn>\n")
         loadFile.writelines(testxml%d)
         loadFile.writelines("</atn>\n")
         loadFile.flush()
         loadFile.close()
      else:
         print ":::  INFO Adding %s to test/%s.xml"%(namespace_klass + "JobOptions.py",full_pkg_name)
         nextAdd=False
         for line in fileinput.input("test/%s.xml"%full_pkg_name, inplace=1):
            if nextAdd:
               print(testxml%d)
               nextAdd=False
            if "<atn>" in line: nextAdd=True
            print line,

   
    #to finish up, call cmt config so that the new algorithm will be captured and genconf run on it
    cwd = os.getcwd()
    try:
        os.chdir('cmt')
        _ = commands.getstatusoutput('cmt config')
    finally:
        os.chdir(cwd)
