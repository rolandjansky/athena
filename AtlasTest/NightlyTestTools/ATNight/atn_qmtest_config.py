#!/usr/bin/env python2

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# 
# Script for setting QMTest environment for ATLAS purposes 
#
########################################################################
import string,os,getopt,sys,re,glob
import os.path, shutil
#import xml.parsers.expat
import atnTestProcessing

########################################################################
#    DEFINITIONS:                                                      #
########################################################################
# ---- Debugging level print out ----
dbgl=10          # for the developer in the defined functions 
                # dbgl=10 --> all functions

# ---- Presentation ----
bar='__________________________________________________________________'

# ---- Error report message ----
MyError = 'Error'

# ---- Defined functions ----
def help_message():
  ''' help_message function '''
  print '''atlas-qmtest-config.py :  

               QMTest configuration for ATLAS releases
		  
            Options: -h               --> displays this help message
                     -r release       --> release name (default latest)
                     -d directory     --> work directory for QMTest
                                          (default ./qmtest)    
                     -v version       --> qmtest version
            Usage:
                  nicos_qmtest_config.py [-h] '''
  sys.exit(0)

# looks for the TestDescription files  (dbgl=11)
# in this case --> nicos_inttest_files
#tdict=[]
#def find_TestDescription_files(names,tdict=tdict):
#  ''' Find test description files with extensions nicos_inttest_files '''
#  for n in filter(lambda n: n[-3:] == nicos_inttest_files, names):
#        tdict.append(n)
#  return tdict
#

def write_qmtFile(qmtestLocalPath,tName,tName_new,ttName,ttName_new,tDir,tDir_new,atlasbr,ttlimit):
  ''' Defines and writes the contents of the xml test-case template '''
  atn_home=os.environ['ATN_HOME']
  nicos_relhome=os.environ['NICOS_RELHOME']
  nicos_inttests_dir_os=os.environ['NICOS_INTTESTS_DIR']
  pathAndFile=qmtestLocalPath+os.sep+tName.lower()+'.qmt'
  atnWorkArea=tName.lower()+'_work'
  atnWorkAreaFull=nicos_relhome+os.sep+nicos_inttests_dir_os+os.sep+atnWorkArea
  if(os.path.exists(atnWorkAreaFull)!=1):
      os.mkdir(atnWorkAreaFull)
  else:
      shutil.rmtree(atnWorkAreaFull)
      os.mkdir(atnWorkAreaFull)
  exeLine='echo str test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;source '+atn_home+os.sep+'setup_cmt_qmtest.sh 2>/dev/null; export ATN_WORK_AREA='+atnWorkAreaFull+'; cd '+atnWorkAreaFull+'; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; echo cnt test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_testfile_modifier '+nicos_relhome+os.sep+tDir_new+ttName_new+'; '+atn_home+'/atn_comm_wrapper.sh '+ttlimit+' '+nicos_relhome+os.sep+tDir_new+ttName_new+' 2>&amp;1 | tee -a $NICOS_LOG ; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_comm_termin.sh >> $NICOS_LOG 2>&amp;1; echo end test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;'
  APTests  = re.compile("^.*(AthenaPool).*$",re.IGNORECASE)
  if APTests.match(ttname):
     exeLine='echo str test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;source '+atn_home+os.sep+'setup_cmt_qmtest.sh 2>/dev/null; export ATN_WORK_AREA='+atnWorkAreaFull+'; cd '+atnWorkAreaFull+'; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; echo cnt test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_testfile_modifier '+nicos_relhome+os.sep+tDir_new+ttName_new+'; '+atn_home+'/atn_comm_wrapper.sh 10 '+nicos_relhome+os.sep+tDir_new+ttName_new+' 2>&amp;1 | tee -a $NICOS_LOG ; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_comm_termin.sh >> $NICOS_LOG 2>&amp;1; echo end test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;'
  DTests  = re.compile("^.*(TestDigitization).*$",re.IGNORECASE)
  if DTests.match(ttname):
         exeLine='echo str test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;source '+atn_home+os.sep+'setup_cmt_qmtest.sh 2>/dev/null; export ATN_WORK_AREA='+atnWorkAreaFull+'; cd '+atnWorkAreaFull+'; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; echo cnt test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_testfile_modifier '+nicos_relhome+os.sep+tDir_new+ttName_new+'; '+atn_home+'/atn_comm_wrapper.sh 20 '+nicos_relhome+os.sep+tDir_new+ttName_new+' 2>&amp;1 | tee -a $NICOS_LOG ; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_comm_termin.sh >> $NICOS_LOG 2>&amp;1; echo end test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;'
  DABTests  = re.compile("^.*(AthenaPoolMultiTest).*$",re.IGNORECASE)
  if DABTests.match(ttname):
         exeLine='echo str test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;source '+atn_home+os.sep+'setup_cmt_qmtest.sh 2>/dev/null; export ATN_WORK_AREA='+atnWorkAreaFull+'; cd '+atnWorkAreaFull+'; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; echo cnt test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_testfile_modifier '+nicos_relhome+os.sep+tDir_new+ttName_new+'; '+atn_home+'/atn_comm_wrapper.sh 20 '+nicos_relhome+os.sep+tDir_new+ttName_new+' 2>&amp;1 | tee -a $NICOS_LOG ; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_comm_termin.sh >> $NICOS_LOG 2>&amp;1; echo end test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;'
  PUTests  = re.compile("^.*(TestPileUp).*$",re.IGNORECASE)
  if PUTests.match(ttname):
         exeLine='echo str test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;source '+atn_home+os.sep+'setup_cmt_qmtest.sh 2>/dev/null; export ATN_WORK_AREA='+atnWorkAreaFull+'; cd '+atnWorkAreaFull+'; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; echo cnt test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_testfile_modifier '+nicos_relhome+os.sep+tDir_new+ttName_new+'; '+atn_home+'/atn_comm_wrapper.sh 20 '+nicos_relhome+os.sep+tDir_new+ttName_new+' 2>&amp;1 | tee -a $NICOS_LOG ; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_comm_termin.sh >> $NICOS_LOG 2>&amp;1; echo end test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;'
  APPUTests  = re.compile("^.*(TestAthenaPoolTest).*$",re.IGNORECASE)
  if APPUTests.match(ttname):
         exeLine='echo str test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;source '+atn_home+os.sep+'setup_cmt_qmtest.sh 2>/dev/null; export ATN_WORK_AREA='+atnWorkAreaFull+'; cd '+atnWorkAreaFull+'; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; echo cnt test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_testfile_modifier '+nicos_relhome+os.sep+tDir_new+ttName_new+'; '+atn_home+'/atn_comm_wrapper.sh 20 '+nicos_relhome+os.sep+tDir_new+ttName_new+' 2>&amp;1 | tee -a $NICOS_LOG ; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_comm_termin.sh >> $NICOS_LOG 2>&amp;1; echo end test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;'
  TCTests  = re.compile("^.*(TestControl).*$",re.IGNORECASE)
  if TCTests.match(ttname):
             exeLine='echo str test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;source '+atn_home+os.sep+'setup_cmt_qmtest.sh 2>/dev/null; export ATN_WORK_AREA='+atnWorkAreaFull+'; cd '+atnWorkAreaFull+'; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; echo cnt test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_testfile_modifier '+nicos_relhome+os.sep+tDir_new+ttName_new+'; '+atn_home+'/atn_comm_wrapper.sh 10 '+nicos_relhome+os.sep+tDir_new+ttName_new+' 2>&amp;1 | tee -a $NICOS_LOG ; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_comm_termin.sh >> $NICOS_LOG 2>&amp;1; echo end test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;'
  TRITests  = re.compile("^.*(ransBS|iteBS).*$",re.IGNORECASE)
  if TRITests.match(ttname):
                 exeLine='echo str test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;source '+atn_home+os.sep+'setup_cmt_qmtest.sh 2>/dev/null; export ATN_WORK_AREA='+atnWorkAreaFull+'; cd '+atnWorkAreaFull+'; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; echo cnt test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_testfile_modifier '+nicos_relhome+os.sep+tDir_new+ttName_new+'; '+atn_home+'/atn_comm_wrapper.sh 15 '+nicos_relhome+os.sep+tDir_new+ttName_new+' 2>&amp;1 | tee -a $NICOS_LOG ; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_comm_termin.sh >> $NICOS_LOG 2>&amp;1; echo end test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;'
  RomeTests  = re.compile("^.*(Rome).*$",re.IGNORECASE)
  if RomeTests.match(ttname):
                exeLine='echo str test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;source '+atn_home+os.sep+'setup_cmt_qmtest.sh 2>/dev/null; export ATN_WORK_AREA='+atnWorkAreaFull+'; cd '+atnWorkAreaFull+'; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; echo cnt test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_testfile_modifier '+nicos_relhome+os.sep+tDir_new+ttName_new+'; '+atn_home+'/atn_comm_wrapper.sh 15 '+nicos_relhome+os.sep+tDir_new+ttName_new+' 2>&amp;1 | tee -a $NICOS_LOG ; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_comm_termin.sh >> $NICOS_LOG 2>&amp;1; echo end test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;'
  RECTests  = re.compile("^.*(RecEx).*$",re.IGNORECASE)
  if RECTests.match(ttname):
                exeLine='echo str test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;source '+atn_home+os.sep+'setup_cmt_qmtest.sh 2>/dev/null; export ATN_WORK_AREA='+atnWorkAreaFull+'; cd '+atnWorkAreaFull+'; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; echo cnt test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_testfile_modifier '+nicos_relhome+os.sep+tDir_new+ttName_new+'; '+atn_home+'/atn_comm_wrapper.sh 15 '+nicos_relhome+os.sep+tDir_new+ttName_new+' 2>&amp;1 | tee -a $NICOS_LOG ; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_comm_termin.sh >> $NICOS_LOG 2>&amp;1; echo end test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;'
  SIMTests  = re.compile("^.*(G4AtlasApp).*$",re.IGNORECASE)
  if SIMTests.match(ttname):
                exeLine='echo str test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;source '+atn_home+os.sep+'setup_cmt_qmtest.sh 2>/dev/null; export ATN_WORK_AREA='+atnWorkAreaFull+'; cd '+atnWorkAreaFull+'; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; echo cnt test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_testfile_modifier '+nicos_relhome+os.sep+tDir_new+ttName_new+'; '+atn_home+'/atn_comm_wrapper.sh 15 '+nicos_relhome+os.sep+tDir_new+ttName_new+' 2>&amp;1 | tee -a $NICOS_LOG ; ps -ef | grep atnight >>$NICOS_LOG 2>&amp;1; date >>$NICOS_LOG 2>&amp;1; '+atn_home+'/atn_comm_termin.sh >> $NICOS_LOG 2>&amp;1; echo end test '+tDir+ttName+'>>$NICOS_LOG 2>&amp;1;'
  exeLineFull='<argument name="command"><text>'+exeLine+'</text></argument>'
  envLine='<argument name="environment"><set><text>ATLAS_BASE_RELEASE='+atlasbr+'</text></set></argument>'
  f=open(pathAndFile, 'w')
  f.write('<?xml version="1.0" ?>')
  f.write('<extension class="command.ShellCommandTest" kind="test">')
  f.write(envLine)
  f.write('<argument name="target_group"><text>.*</text></argument>')
  f.write(exeLineFull)
  f.write('<argument name="stderr"><text/></argument>')
  f.write('<argument name="stdout"><text/></argument>')
  f.write('<argument name="prerequisites"><set/></argument>')
  f.write('<argument name="stdin"><text/></argument>')
  f.write('<argument name="exit_code"><integer>0</integer></argument>')
  f.write('<argument name="resources"><set/></argument></extension>')
  f.close()

# creates the xml suite files 

def xml_suite_files(qmtestLocalPath,dictTests,atlasbr):
  if (dbgl>9):
    print 'test dictionary: '
    print dictTests
    print '----------' 
  suites=dictTests.items()
  for suite, testList in suites:
    if (dbgl>9):
      print '+++++++++++++++++++++++++++'
      print 'suite: ', suite, 'testList: ', testList
      print '+++++++++++++++++++++++++++'
    g=qmtestLocalPath+suite+'.qms'
    f=open(g, 'w')
    f.write('<?xml version="1.0" ?>')
    f.write('<suite>')
    for j in range(len(testList)):
      if (dbgl>9):
        print 'testsuite_files:',g,j,'in range ', len(testList),suite,testList[j]
      line='<test_id>'+testList[j]+'</test_id>'
      f.write(line)
    f.write('</suite>')
    f.close()

def xml_suite_files_221(qmtestLocalPath,dictTests,atlasbr):
  if (dbgl>9):
    print 'test dictionary: '
    print dictTests
    print '----------'
  suites=dictTests.items()
  for suite, testList in suites:
    if (dbgl>9):
      print '+++++++++++++++++++++++++++'
      print 'suite: ', suite, 'testList: ', testList
      print '+++++++++++++++++++++++++++'
    g=qmtestLocalPath+suite+'.qms'
    f=open(g, 'w')
    f.write('<?xml version="1.0" ?>')
    f.write('<extension class="explicit_suite.ExplicitSuite" kind="suite"><argument name="suite_ids"><set/></argument><argument name="test_ids"><set>')
    for j in range(len(testList)):
      if (dbgl>9):
        print 'testsuite_files:',g,j,'in range ', len(testList),suite,testList[j]
      line='<text>'+testList[j]+'</text>'
      f.write(line)
    f.write('</set></argument></extension>')
    f.close()

def suite_guess(name):
  CoreTests  = re.compile("^.*(Athena|Control).*$",re.IGNORECASE)
  ExampleTests  = re.compile("^.*(Example).*$",re.IGNORECASE)
  BasicTests  = re.compile("^.*(Hello|RunEvent).*$",re.IGNORECASE)
  suite='othertests'
  if CoreTests.match(name) and not ExampleTests.match(name):
     suite='coretests'
  elif ExampleTests.match(name):
     suite='examples'
  elif BasicTests.match(name):
     suite='basictests'
  else:
     suite='othertests'
  return suite
                                                                    
testdir=[]
testfile=[]
testsuite=[]
testauthor=[]
testtype=[]
def test_lister_restr(currdir):
#    print "AAAAAA",'[' + currdir + ']'
    for file in os.listdir(currdir):
      path = os.path.join(currdir, file)
      if os.path.isdir(path) and not re.match('InstallArea', file) and not re.match('cmt', file):
         if os.path.basename(path) == 'test':
#             print 'path of test_l: ', path 
             if os.path.isdir(path):
               for file1 in os.listdir(path):
                 path_tot=os.path.join(path, file1)
                 if not os.path.isdir(path_tot):
                   if re.search(nicos_inttests_files, file1):
                     if not re.match('^.*pre_check.*$',file1):
                       if not re.match('^.*post_check.*$',file1):
                         pathh=path+os.sep
                         sign1=1; sign2=1
                         if re.search('.xml$', file1):
                           sign1=0; sign2=0
                           ff=open(path_tot)
                           while 1: 
                             line = ff.readline()
                             if not line: break
                             if sign1 == 2 and sign2 == 2: break
                             if re.search("<atn", line):sign1=2
                             if re.search("<TEST", line):sign2=2
                           ff.close()
                         if sign1 > 0 and sign2 >0:
#                           print "SIGN1 2 ",sign1,sign2,file1 
                           testdir.append(pathh)
                           testfile.append(file1)
                           sui=suite_guess(file1)
                           testsuite.append(sui)
                           testauthor.append("")
                           if sign1 == 2 and sign2 == 2:
                             testtype.append('xml')
                           else:
                             testtype.append('scr')
         test_lister_restr(path)

def test_lister(currdir):
          path=currdir
          if os.path.isdir(path) and not re.match('^.*InstallArea.*$', path):
#                     print 'path of test_l: ', path
                     if os.path.isdir(path):
                       for file1 in os.listdir(path):
                         path_tot=os.path.join(path, file1) 
                         if not os.path.isdir(path_tot):
                           if re.search(nicos_inttests_files, file1):
                             if not re.match('^.*pre_check.*$',file1):
                               if not re.match('^.*post_check.*$',file1):
                                  sign1=1; sign2=1 
                                  if re.search('.xml$', file1):
                                    sign1=0; sign2=0
                                    ff=open(path_tot)
                                    while 1:
                                      line = ff.readline()
                                      if not line: break
                                      if sign1 == 2 and sign2 == 2: break
                                      if re.search("<atn", line):sign1=2
                                      if re.search("<TEST", line):sign2=2
                                    ff.close()     
                                  if sign1 > 0 and sign2 > 0: 
                                      testdir.append(path)
                                      testfile.append(file1)
                                      sui=suite_guess(file1)
                                      testsuite.append(sui)
                                      testauthor.append("") 
                                      if sign1 == 2 and sign2 == 2:
                                        testtype.append('xml')
                                      else:
                                         testtype.append('scr')
                                        
#                     test_lister(path)                     

def dirextractor(dname,base):
    bas=re.split('/',base)
    i=len(bas)
    dnam=re.split('/',dname)
    a=string.join(dnam[i:],'/')
    return a
  
def containerextractor(dname):
    bas=re.split('/',dname)
    mm = re.compile("^.*[0-9][0-9]-[0-9][0-9]-[0-9][0-9]$",re.IGNORECASE)
    i=len(bas)
    icoun=-1
    for i in range(len(bas)):
      icoun=icoun+1
      if mm.match(bas[i]):
         break       
    a=string.join(bas[:icoun],'/') 
    return a+os.sep
  
########################################################################
#    MAIN SECTION                                                      #
########################################################################
# ---- Global definitions ----
# default qmtest work directory
dirName='./qmtest'
# default release name
releaseName='latest'
# search (1) or not search (0) xml test descriptors
xmlSearch=1

# ---- Process command line arguments ----
try:
  optionslist, args = getopt.getopt(sys.argv[1:],'hr:d:v:')  
except getopt.error:
  print '''Error: You tried to use an unknown option or the
                 argument for an option that requires it was missing
                 Try "atlas-qmtest-config.py -h" for more information.'''
  sys.exit(0)

qmVersion=221
for a in optionslist[:]:
  if a[0] == '-h':
    help_message()
  if a[0] == '-r' and a[1] != '':
    releaseName=a[1]
  if a[0] == '-d' and a[1] != '':
    dirName=a[1] 
  if a[0] == '-v' and a[1] != '':
    qmVersion=a[1]

#if re.search(r'atlrel_', releaseName):
#  releasePathA='/afs/cern.ch/atlas/software/dist/nightlies/rel/'
#else:
#  releasePathA='/afs/cern.ch/atlas/software/dist/'

print "qmVersion", qmVersion
print "ATN_HOME", os.environ['ATN_HOME']

try:
  nicos_inttests_files=os.environ['NICOS_INTTESTS_FILES']
except:
  nicos_inttests_files='.sh$'
if xmlSearch:
  nicos_inttests_files=nicos_inttests_files+'|.xml$'  

test_script_patt=['TEST_SUITE','MAILTO','ERROR_MESSAGE','SUCCESS_MESSAGE','WARNING_MESSAGE']

nicos_testlog=os.environ['NICOS_TESTLOG']
nicos_test_dir=os.path.dirname(nicos_testlog)
nicos_test_dir_temp=nicos_test_dir+os.sep+'temp'+os.sep  
if(os.path.exists(nicos_test_dir_temp)!=1):
    os.mkdir(nicos_test_dir_temp)
else:
    shutil.rmtree(nicos_test_dir_temp)
    os.mkdir(nicos_test_dir_temp)

releasePathX=os.environ['NICOS_RELHOME']
releaseName=os.path.basename(releasePathX)
releasePath=releasePathX+os.sep
BaseReleaseName=releaseName
ProjectRelname=os.environ['NICOS_PROJECT_RELNAME']
if ProjectRelname == "WORK_RELEASE" :
  cmtpath=os.environ['CMTPATH']
  cmtpath2=string.split(cmtpath, ':')[1]
  BaseReleaseName=os.path.basename(cmtpath2)
#  print 'DDDDDDDDDDDDD:', BaseReleaseName
    
#finding test files
test_lister_restr(releasePath)

try:
    nicos_inttests_dir=os.environ['NICOS_INTTESTS_DIR']
    full_path=os.path.join(releasePath,nicos_inttests_dir) 
    dirWithTests=glob.glob(full_path + os.sep)[0]
    test_lister(dirWithTests) 
except:
    print 'Error: NICOS_INTTESTS_DIR does not exist:  '
    print nicos_inttests_dir 

try:
      nicos_inttests_dir=os.environ['NICOS_INTTESTS_LOCATION']
      full_path=os.path.join(releasePath,nicos_inttests_dir)
      dirWithTests=glob.glob(full_path + os.sep)[0]
      test_lister(dirWithTests)
except:
      print 'Error: NICOS_INTTESTS_LOCATION does not exist:  '
      print nicos_inttests_dir

#print 'SSS :', testdir
#print 'SAS :', testfile


# ---- INFO print  ----
print bar
print '  ---------------------------'
print 'Script Name: atlas-qmtest-config.py'
print 'performs QMTest setup for ATLAS release'
print releasePathX
print '  ---------------------------'
print '                                 '

# ---- Checks if local qmtest directory exists
# append the os.sep to the end of the dirName if it is not there
if(string.rfind(dirName,os.sep)==len(dirName)-1):
  dirName=dirName
else:
  dirName=dirName+os.sep

qmtestLocalPath=os.getcwd()+os.sep+dirName
if(os.path.exists(qmtestLocalPath)!=1):
  os.mkdir(dirName)
else:
  shutil.rmtree(dirName)
  os.mkdir(dirName)
# ---- Checks if the QMtest database is in place 
qmtestLocalPathDB=qmtestLocalPath+os.sep+'QMTest/'
if (os.path.exists(qmtestLocalPathDB)!=1):
  commandLine1='qmtest -D '+qmtestLocalPath+' create-tdb'
  try:
    os.system(commandLine1) 
  except:
    print '''Error: qmtest is not in your path !!!''' 
    print '''                                     '''
    print '''                                     '''
    print bar
    sys.exit(0)

# checking for test db file
lines_db=[]
nicos_test_dbfile_gen=os.environ['NICOS_TEST_DBFILE_GEN']
if os.path.exists(nicos_test_dbfile_gen) : os.unlink(nicos_test_dbfile_gen)

try:
    nicos_test_dbfile=os.environ['NICOS_TEST_DBFILE'] 
    mf=open(nicos_test_dbfile)
    lines_db = mf.readlines()
    mf.close()
except:
    lines_db=[]

# generate db files
mf=open(nicos_test_dbfile_gen,'a')
cmtstruct="with_version_directory"
if os.environ.has_key('CMTSTRUCTURINGSTYLE') :
    cmtstruct=os.environ['CMTSTRUCTURINGSTYLE']
for line in lines_db:
    line11=string.strip(line)
    line1=string.replace(line11,'\n','')
    if len(re.sub(r'\s+','',line1)) == 0 : continue
    fil=re.split(r'\s+',line1)
    signat=fil[0]
    tnam=fil[1] 
    tdir=fil[2]
    tdir_base=os.path.basename(tdir)
    full_path=os.path.join(releasePath,tdir,tdir_base)
    full_path_1=os.path.join(releasePath,tdir)
    try:
       dirWTests=glob.glob(full_path + '*' + os.sep)[0]
       if cmtstruct == "without_version_directory" :
         dirWTests=glob.glob(full_path + os.sep)[0]
       else :
         dirWTests=glob.glob(full_path + '*' + os.sep)[0]
       dirWTests_s=dirextractor(dirWTests,releasePathX)
       tsui=string.lower(fil[3])
       taut=string.join(fil[4:])
#       print 'line_db - fil:',fil
#       print 'XXX : ',signat,tnam,dirWTests_s,releasePathX,tsui,taut
       if signat != "-" :
         aaaa=tnam+' '+tnam+' '+dirWTests_s+' '+dirWTests_s+' '+tsui+' '+taut+'\n'
         mf.write(aaaa)
    except:
       print 'nicos_qmtest_config.py: can not find dir', full_path, 'indicated in test db file'
    try:
       dirWTests=glob.glob(full_path_1 + os.sep)[0]      
       dirWTests_s=dirextractor(dirWTests,releasePathX)
       tsui=string.lower(fil[3])
       taut=string.join(fil[4:])
       if signat != "-" :
         aaaa=tnam+' '+tnam+' '+dirWTests_s+' '+dirWTests_s+' '+tsui+' '+taut+'\n'
         mf.write(aaaa)
    except:
         print 'nicos_qmtest_config.py: can not find dir', full_path_1, 'indicated in test db file'

#print "SSS", testfile
#print "DFF",testtype
    
icoun=-1 
for tnam in testfile:
    icoun=icoun+1
#    print "OOOOO", icoun
#
    tnam_base=re.split(r'\.',tnam)[0]
    variabl=0
    autlist=[]
    author=testauthor[icoun]
    suite=testsuite[icoun]
    suite=string.lower(suite)
    autlist.append(author)
    testtimelimit="8"
    for line in lines_db:
        line1=string.strip(line)
        if len(re.sub(r'\s+','',line1)) == 0 : continue
        fil=re.split(r'\s+',line1)
        signat=fil[0]
        ttnam=fil[1]
        ttnam_base=re.split(r'\.',ttnam)[0]
#        print 'DDDDD ',ttnam,'DDDDD ',ttnam_base
        if ttnam_base == tnam_base :
           variabl=1
           break
    if variabl : continue
#
    if testtype[icoun] == 'xml': 
      testdir_prev=testdir[icoun]
      testfile_prev=testfile[icoun]
      path_to_script, TD = atnTestProcessing.atnTestProc(testdir_prev, testfile_prev)
      if path_to_script != None:
        testdir_new=os.path.dirname(path_to_script)
        testfile_new=os.path.basename(path_to_script)
#
        fpath=testdir[icoun]+os.sep+tnam
        autlist.append(TD.properties['mailto'])
        testsuite1=TD.testAttr['suite']
        suite=string.lower(testsuite1)
        testtimelimit=TD.properties['timelimit']
#
        key_patt={'errorMessage' : 'ERROR_MESSAGE', 'warningMessage' : 'WARNING_MESSAGE', 'successMessage' : 'SUCCESS_MESSAGE'} 
        dname11=testdir[icoun]
        dname1=dirextractor(dname11,releasePathX)
        dname2=containerextractor(dname1)
        dname=re.sub(r'/','_',dname2)
        for key in key_patt.keys():
          for ptn in TD.expectations[key]:
              nameend=key_patt[key]
              fname=nicos_test_dir_temp+dname+tnam_base+"_"+nameend
              if ptn != "":
                    mj=open(fname,'a')
                    ptn1=ptn+'\n'
                    mj.write(ptn1)
                    mj.close()
        autstring=string.join(autlist)
        dirWTests_s=dirextractor(testdir[icoun],releasePathX)
        aaaa=tnam+' '+testfile_new+' '+dirWTests_s+' '+testdir_new+os.sep+' '+suite+' '+testtimelimit+' '+autstring+'\n'
        mf.write(aaaa)
#
    else:
#
      fpath=testdir[icoun]+os.sep+tnam
      mg=open(fpath)
      for x in range(30):
        line = mg.readline()
        if not line: break
        for patt in test_script_patt :
          if re.search(patt,line) :  
              line1=string.strip(line)
              fil=re.split(r':',line1)
              if len(fil) > 1 :
                 patter=' '+fil[1]
                 patter1=string.join(fil[1:],':')
                 if patt == 'MAILTO' :
                    autlist.append(patter)
                 elif patt == 'TEST_SUITE' :
                    testsuite1=re.sub(r'\s','',patter)
                    suite=string.lower(testsuite1)
                 else:
                    dname11=testdir[icoun]
                    dname1=dirextractor(dname11,releasePathX)
                    dname2=containerextractor(dname1)
                    dname=re.sub(r'/','_',dname2)
                    fname=nicos_test_dir_temp+dname+tnam_base+"_"+patt
#                    print 'GGG : ',fname
                    mj=open(fname,'a')
                    aaaa=patter1+'\n'
                    mj.write(aaaa)
                    mj.close()
      autstring=string.join(autlist)
      dirWTests_s=dirextractor(testdir[icoun],releasePathX)
      aaaa=tnam+' '+tnam+' '+dirWTests_s+' '+dirWTests_s+' '+suite+' '+testtimelimit+' '+autstring+'\n'
      mg.close() 
      mf.write(aaaa)
mf.close()
                

# ---- Generating the xml test-case files ----
mf=open(nicos_test_dbfile_gen)
lines_db_gen = mf.readlines()
mf.close()

dictTests={}
for x in lines_db_gen:
  line1=string.strip(x)
  if len(re.sub(r'\s+','',line1)) == 0 : continue
  fil=re.split(r'\s+',line1)
  tttname=fil[0]
  ttname=string.split(x,'.')[0]
  tname=string.lower(ttname)
  tttname_new=fil[1]
  ttname_new=string.split(tttname_new,'.')[0]
  tname_new=string.lower(ttname_new)
  tdir=fil[2]
  tdir_new=fil[3]
#  ttdir=dirextractor(tdir,releasePathX)
#  print " FFFF ",tdir,ttdir,releasePathX
  tsui=fil[4]
  tlimit=fil[5]
  taut=string.join(fil[6:])
  if not dictTests.has_key(tsui) : dictTests[tsui]=[]
  dictTests[tsui].append(tname)
  write_qmtFile(qmtestLocalPath,ttname,ttname_new,tttname,tttname_new,tdir,tdir_new,BaseReleaseName,tlimit)
# ---- Generating the xml test-suite files ----
if (dbgl>9):
        print bar
        print 'call xml_suite_files with args: '
        print qmtestLocalPath
        print dictTests
        print BaseReleaseName
        print bar
if (string.atoi(qmVersion)<220):
   print "qm version less 220", qmVersion
   xml_suite_files(qmtestLocalPath,dictTests,releaseName)
else:
   xml_suite_files_221(qmtestLocalPath,dictTests,releaseName)
   print "qm version greater 220", qmVersion
########################################################################
# ---- End ----                                                        #
########################################################################
print bar
print '                                                 '
print 'ATLAS Qmtest setup is completed                  '
print '-------------------------------------------------'
print '--> Use now Qmtest                               '
print '   $> qmtest gui                                 '
print ' in the directory:                              ' 
print qmtestLocalPath
print bar
