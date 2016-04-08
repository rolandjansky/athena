#! /usr/bin/env python
# =====================================================================
# Main script to run the upload constants to DB
# =====================================================================
import os
import sys
# =====================================================================
#  Welcome print
# =====================================================================
print "\n ============================================="
print "    ID Alignment constants uploader"
print " =============================================\n"
# =====================================================================
#  Default values
# =====================================================================
Do_Silicon = 1
#
Do_TRT = 1
#
Def_Mode = "Test" #Def_Mode = "Exec"
User_Mode = ""
#
Def_Comment = "Constants derived using 2010 data and 2012 baseline  d0, z0 and p have been corrected using data from Z->mumu events" 
User_Comment =""
#
TheNewTag = "Repro2010_d0z0p_constrained"

constantsDefault= "155073"

# =======================================
# constantsGenRun is a list containing each folder that contains the 
# Iter*/Iter*_AlignmentConstants.root & Iter*/mycool.db files to upload
# =======================================
constantsGenRun = [ "155073",  "155112",  "155160",  "155569",  "155634", "155697",  "156682",
"158116", "158269", "158392",  "158548",  "158582",  "158632",  "158801", "158975",  "159041",
"159086", "159113", "159179",  "159202",  "159203",  "159224",  "160472", "160530",  "160801",
"160879", "160954", "160958",  "161118",  "161379",  "161407",  "161520", "161562",  "161948",
"162347", "162526", "162576",  "162623",  "162690",  "162764",  "162843", "162882",  "165591",
"165632", "165732", "165767",  "165818",  "165821",  "166097",  "166142", "166198",  "166383",
"166466", "166658", "166786",  "166924",  "166927",  "167576",  "167607", "167680",  "167776",  "167844"]
# =======================================
# constantsValidRun is a list containing a run number for each element in constantsGenRun. This run
# number is the started run where the corrections stored in constantsGenRun[i] are valid, untill the
# the next run number
# =======================================
constantsValidRun = list(constantsGenRun)

iteration = 1 # iteration number-> Iter**, must be an integer
constantsPath = "/afs/cern.ch/user/a/atlidali/w1/users/spedraza/repro/"

ConstantsFileName = "2010-Test2"
#
DBName = "COMP200"
#
ConditionsFile = "cond12_data.gen.COND"
#
# =====================================================================
#  Handling of run options
# =====================================================================
if len(sys.argv) > 1:
    argNum = 1
    argMax = len(sys.argv)-1
    argCurr = -1 # just to make that first time it points to 0 
    #print ' >>> user input has %s arguments' % argMax
    for arg in sys.argv:
        argCurr += 1
        #print ".. %s .." % arg
        if arg in ("-h", "--help"):    
            print ' >>> This is the help message of RunIterator.py'       
            print '     -exec'
            print '           The constants will be effectively uploaded to the DB. Default mode is test mode when commands are just\n\
            displayed but no executed, therefore no constants are uploaded.'
            sys.exit()
            #
        elif arg in ("--exec"):    
            # This option activates the execution of the commands
            User_Mode = "Exec"
            #
            #

    if (1):
        print '>>> ConstantsUploader >>> List of user input values:'
        print '> User Mode = %s' %User_Mode
        print '>>> End of input options processing <<< \n' 

# ===============================
# Handling of variables
# ===============================
#
#
Upload_Mode = Def_Mode
if (User_Mode):
    Upload_Mode = "Exec"
#
#
DotDbFile = ConstantsFileName+".db"
DotRootFile = ConstantsFileName+".root"
OutputDbFile = ConstantsFileName+"_output.db"
#
CommentOnConstants = Def_Comment
if (User_Comment):
 CommentOnConstants = User_Comment
# 
New_InDet_TagName = "InDetAlign_" + TheNewTag
New_TRT_TagName = "TRTAlign_" + TheNewTag
#



if (1):
    print '>>>>>>>>>>>> List of variables >>>>>>>>>>>>>>> '
    print '> Upload_Mode  = %s' %Upload_Mode
    print '<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n '



baselinePath =  constantsPath + constantsDefault + "/Iter%i/" %(iteration)
DotRootFile = baselinePath + "Iter%i_AlignmentConstants.root" %(iteration)
DotDbFile   = baselinePath + "mycool.db"

# ===============================
# To understand next steps see https://twiki.cern.ch/twiki/bin/viewauth/Atlas/AtlCoolCopy
# ===============================
# ===============================
# Common step
# ===============================
# this command line is the same for silicon and TRT
CommandLine0 = "~atlcond/utils/registerFiles2 --wait " + ConditionsFile + " " + DotRootFile

# ===============================
# 1st step
# ===============================

# -- prepare the command 
CommandLine1_Silicon = "AtlCoolCopy.exe \"sqlite://schema="+DotDbFile+";dbname="+DBName+"\" \"sqlite://;schema="+OutputDbFile+";dbname="+DBName+"\" -create -folder /Indet/Align -outtag " + New_InDet_TagName + " -tl \""+ CommentOnConstants +"\" -alliov "

CommandLine1_TRT = "AtlCoolCopy.exe \"sqlite://schema="+DotDbFile+";dbname="+DBName+"\" \"sqlite://;schema="+OutputDbFile+";dbname="+DBName+"\" -create -folder /TRT/Align -outtag " + New_TRT_TagName +" -tl \""+ CommentOnConstants +"\" -alliov "

TestCommand = "ls -l %s" %DotDbFile
ExistsDotDb = os.system(TestCommand)
if (ExistsDotDb !=0):
    print '\n ** ERROR ** file %s does not exist ' % DotDbFile 
    sys.exit()

TestCommand = "ls -l %s" %DotRootFile
ExistsDotRoot = os.system(TestCommand)
if (ExistsDotRoot !=0):
     print '\n ** ERROR ** file %s does not exist ' % DotRootFile 
     sys.exit()

UpdateDB_Si = []
UpdateDB_TRT = []
LoadFiles = []
for i in range( len(constantsGenRun)):

  baselinePath =  constantsPath +constantsGenRun[i]+"/Iter%i/" %(iteration)
  DotRootFile = baselinePath + "Iter%i_AlignmentConstants.root" %(iteration)
  DotDbFile   = baselinePath + "mycool.db"

  TestCommand = "ls -l %s" %DotDbFile
  ExistsDotDb = os.system(TestCommand)
  if (ExistsDotDb !=0):
    print '\n ** ERROR ** file %s does not exist ' % DotDbFile
    sys.exit()

  TestCommand = "ls -l %s" %DotRootFile
  ExistsDotRoot = os.system(TestCommand)
  if (ExistsDotRoot !=0):
     print '\n ** ERROR ** file %s does not exist ' % DotRootFile
     sys.exit()



  LoadFiles.append( "~atlcond/utils/registerFiles2 --wait " + ConditionsFile + " " + DotRootFile)
  
  UpdateDB_Si.append( "AtlCoolCopy.exe \"sqlite://schema="+DotDbFile+";dbname="+DBName+"\" \"sqlite://;schema="+OutputDbFile+";dbname="+DBName+"\" -create -folder /Indet/Align -outtag " + New_InDet_TagName + " -tl \""+ CommentOnConstants +"\" -rs "+ constantsValidRun[i] +  " -truncate")
  UpdateDB_TRT.append( "AtlCoolCopy.exe \"sqlite://schema="+DotDbFile+";dbname="+DBName+"\" \"sqlite://;schema="+OutputDbFile+";dbname="+DBName+"\" -create -folder /TRT/Align -outtag " + New_TRT_TagName + " -tl \""+ CommentOnConstants +"\" -rs "+ constantsValidRun[i] + " -truncate")





# ===============================
# 2nd step
# ===============================
CommandLine2_Silicon = "/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --debug  --batch --comment \""+CommentOnConstants+"\" --tag " + New_InDet_TagName + " --passopt='-ctl' --folder /Indet/Align "+ OutputDbFile +" "+DBName+" ATLAS_COOLWRITE ATLAS_COOLOFL_INDET_W WCOOLOFL4INDET1"

CommandLine2_TRT = "/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --debug --batch --comment \""+CommentOnConstants+"\" --tag " + New_TRT_TagName + " --passopt='-ctl' --folder /TRT/Align "+ OutputDbFile +" "+DBName+" ATLAS_COOLWRITE ATLAS_COOLOFL_TRT_W WCOOLOFL4TRT2"

#- 
# ===============================
# Upload
# ===============================
if Upload_Mode in ("Exec"):
   print " \n == Excute upload !!!! == \n"

   print " \n == step0 ==> " + CommandLine0 + "\n"
   os.system(CommandLine0)
   for i in range( len(LoadFiles)):
     print "== step0.X ==> " +LoadFiles[i]
     os.system( LoadFiles[i] )



   if (Do_Silicon == 1):
       print ' =============> Uploading silicon constants ============'
       print " == step1 ==> " + CommandLine1_Silicon + "\n"
       os.system(CommandLine1_Silicon)
       for i in range( len(constantsGenRun)):
         #print "== step1.X ==> " +UpdateDB_Si[i]
         os.system( UpdateDB_Si[i])  
       #print " \n == step2 ==> " + CommandLine2_Silicon + "\n" 
       #print os.system(CommandLine2_Silicon)

   if (Do_TRT == 1):
       print ' =============> Uploading TRT constants ============'
       print " == step1 ==> " + CommandLine1_TRT + "\n"
       print os.system(CommandLine1_TRT)
       for i in range( len(constantsGenRun)):
         #print "== step1.X ==> " +UpdateDB_TRT[i]
         os.system( UpdateDB_TRT[i]) 
       #print " \n == step2 ==> " + CommandLine2_TRT + "\n" 
       #print os.system(CommandLine2_TRT)
 
   print "<===>Final step<===>"
   print " \n == Silicon step2 ==> \n" + CommandLine2_Silicon + "\n"
   print " \n == TRT step2     ==> \n" + CommandLine2_TRT + "\n"

elif Upload_Mode in ("Test"):
   print " \n == Test Mode !!!! == \n"
   print " =========== Common ============= " 
   print " == step0 ==> " + CommandLine0 + "\n\n"
   print "   WARNING       first check the condXX_data_gen.COND to point with the following command:"
   print "                 > ~atlcond/utils/registerFiles2 --list \n"
   print " =========== Silicon ============= " 
   print " == step1 ==> " + CommandLine1_Silicon + "\n"
   print " == step2 ==> " + CommandLine2_Silicon + "\n\n"
   #for i in range( len(constantsGenRun)):
     #print LoadFiles[i]
     #print UpdateDB_Si[i]
     #print UpdateDB_TRT[i] 

   print " ===========  TRT  ============= " 
   print " == step1 ==> " + CommandLine1_TRT + "\n"
   print " == step2 ==> " + CommandLine2_TRT + "\n" 

   
# ===============================
# completed
# ===============================
print "\n ============================================="
print "    GAME OVER.  INSERT COIN"
print " =============================================\n"
sys.exit()
