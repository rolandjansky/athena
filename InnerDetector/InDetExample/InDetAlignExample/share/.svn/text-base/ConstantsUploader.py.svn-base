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
Def_Do_Silicon = 1
User_Do_Silicon = -1
#
Def_Do_TRT = 1
User_Do_TRT = -1
#
Def_ConstantsFile = "IterX_AlignmentConstants"
User_ConstantsFile = ""
#
Def_Mode = "Test"
User_Mode = ""
#
Def_Comment = "This is a TEST" 
User_Comment =""
#
Def_Tag = "new_tag"
User_Tag = ""
#
Def_UPD4 = ""
User_UPD4 = ""
#
Def_DBName = "COMP200"
User_DBName = ""
#
Def_Conditions = "cond10_data.gen.COND"
User_Conditions = ""
#
# =====================================================================
#  Handling of run options
# =====================================================================
if len(sys.argv) > 1:
    argNum = 1
    argMax = len(sys.argv)-1
    argCurr = -1 # jut to make that first time it points to 0 
    #print ' >>> user input has %s arguments' % argMax
    for arg in sys.argv:
        argCurr += 1
        #print ".. %s .." % arg
        if arg in ("-h", "--help"):    
            print ' >>> This is the help message of RunIterator.py'       
            print ' NAME'
            print '     RunIterator.py'
            print ' '
            print ' SYNOPSIS'
            print '     python RunIterator.py [-r RunNumber] [-f ConstantsFile]' 
            print ' '
            print ' OPTIONS'
            print '     The following options are recognized by RunIterator.py'
            print ' '
            print '     -db DBName'
            print '          DB where the constants will be uploaded. The posible names are COMP200 and OFLP200. The default \n name is: %s' %Def_DBName
            print ' '
            print '     -f ConstantsFile'
            print '          The constants .root and .db files taken are: ConstantsFile.root and ConstantsFile.db. The default \n\
            files are {0}.root and {0}.db'.format(Def_ConstantsFile)
            print ' '
            print '     -exec'
            print '           The constants will be effectively uploaded to the DB. Default mode is test mode when commands are just\n\
            displayed but no executed, therefore no constants are uploaded.'
            print ' '
            print '     -c Comment'
            print '           Set a comment (between quotes) in the DB entry next to the tag name. Default comment: \"{0}\"'.format(Def_Comment)
            print ' '
            print '     -silicon doSilicon'
            print '           If doSilicon = 1 then silicon constants are uploaded (default).'           
            print '           If doSilicon = 0 then silicon constants are not uploaded.' 
            print '           default value: doSilicon = %d' %Def_Do_Silicon
            print ' '
            print '     -trt doTRT'
            print '           If doTRT = 1 then TRT constants are uploaded.'           
            print '           If doTRT = 0 then TRT constants are not uploaded (default)'           
            print '           default value: doTRT = %d' %Def_Do_TRT
            print ' '
            print '     -tag Tag_Id'
            print '           The extensions of the tag identifier are set for InDetAlign and TRTAlign. The default tag is: \"%s\"' %Def_Tag
            print ' '
            print '     -upd4'
            print ' ' 
            print ' \n 09/February/2011'
            sys.exit()
        elif arg in ("-f", "--file"):    
            User_ConstantsFile = str(sys.argv[argCurr+1])
            # 
        elif arg in ("-t", "-tag", "--tag"):    
            User_Tag = str(sys.argv[argCurr+1])
            # 
        elif arg in ("-db", "--db", "--DB"):    
            User_DBName = str(sys.argv[argCurr+1])
            #
        elif arg in ("-cond", "--cond", "--COND"):    
            User_Conditions = str(sys.argv[argCurr+1])
            #
        elif arg in ("--exec"):    
            # This option activates the execution of the commands
            User_Mode = "Exec"
            #
        elif arg in ("-silicon", "--silicon"):    
            User_Do_Silicon = int(sys.argv[argCurr+1])
            if (User_Do_Silicon > 1): 
                User_Do_Silicon = 1;
            elif (User_Do_Silicon <0):
                User_Do_Silicon = 0
            #
        elif arg in ("-trt", "--trt"):    
            User_Do_TRT = int(sys.argv[argCurr+1])
            if (User_Do_TRT > 1): 
                User_Do_TRT = 1;
            elif (User_Do_TRT <0):
                User_Do_TRT = 0
            #
        elif arg in ("-c", "--comment"):    
            User_Comment = str(sys.argv[argCurr+1])
            #
        elif arg in ("-upd4","--upd4","--UPD4"):    
            # This option activates the execution of the commands
            User_UPD4 = "--ignoremode BackDoor"
            #

    if (1):
        print '>>> ConstantsUploader >>> List of user input values:'
        print '> User File Name = %s' % User_ConstantsFile 
        print '> User Do_Silicon = %d' % User_Do_Silicon
        print '> User Do_TRT = %d' % User_Do_TRT
        print '> User Mode = %s' % User_Mode
        print '> User tag = %s' %User_Tag
        print '> User Comment = %s' %User_Comment
        print '> User upd4 = %s' %User_UPD4
        print '> User DB name = %s' %User_DBName
        print '> User conditions file = %s' %User_Conditions
        print '>>> End of input options processing <<< \n' 

# ===============================
# Handling of variables
# ===============================
#
ConstantsFileName = Def_ConstantsFile 
if (User_ConstantsFile): 
    ConstantsFileName = User_ConstantsFile 
#
Upload_Mode = Def_Mode
if (User_Mode):
    Upload_Mode = "Exec"
#
TheNewTag = Def_Tag 
if (User_Tag): 
    TheNewTag = User_Tag 
#
DBName = Def_DBName 
if (User_DBName): 
    DBName = User_DBName 
#
ConditionsFile = Def_Conditions 
if (User_Conditions): 
    ConditionsFile = User_Conditions
#
Do_Silicon = Def_Do_Silicon
if (User_Do_Silicon != -1): 
    Do_Silicon = User_Do_Silicon
#
Do_TRT = Def_Do_TRT
if (User_Do_TRT != -1):
    Do_TRT = User_Do_TRT
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
CommandUPD4 = Def_UPD4
if (User_UPD4):
 CommandUPD4 = User_UPD4



if (1):
    print '>>>>>>>>>>>> List of variables >>>>>>>>>>>>>>> '
    print '> DotDbFile    = %s' %DotDbFile
    print '> DotRootFile  = %s' %DotRootFile
    print '> OutputDbFile = %s' %OutputDbFile
    print '> DB Name      = %s' %DBName
    print '> Conditions   = %s' %ConditionsFile
    print '> Upload_Mode  = %s' %Upload_Mode
    print '> InDet tag    = %s' %New_InDet_TagName
    print '> TRT tag      = %s' %New_TRT_TagName
    print '> Do_Silicon   = %d' %Do_Silicon
    print '> Do_TRT       = %d' %Do_TRT
    print '> Comment      = %s' %CommentOnConstants
    print '> CommandUPD4  = %s' %CommandUPD4
    print '<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n '

# ===============================
# Common step
# ===============================
# this command line is the same for silicon and TRT
CommandLine0 = "~atlcond/utils/registerFiles2 --wait " + ConditionsFile + " " + DotRootFile

# ===============================
# 1st step
# ===============================

# -- prepare the command 
CommandLine1_Silicon = "AtlCoolCopy.exe \"sqlite://schema="+DotDbFile+";dbname="+DBName+"\" \"sqlite://;schema="+OutputDbFile+";dbname="+DBName+"\" -create -folder /Indet/Align -outtag " + New_InDet_TagName + " -tl \""+ CommentOnConstants +"\" -alliov "+CommandUPD4

CommandLine1_TRT = "AtlCoolCopy.exe \"sqlite://schema="+DotDbFile+";dbname="+DBName+"\" \"sqlite://;schema="+OutputDbFile+";dbname="+DBName+"\" -create -folder /TRT/Align -outtag " + New_TRT_TagName +" -tl \""+ CommentOnConstants +"\" -alliov "+CommandUPD4

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

   if (Do_Silicon == 1):
       print ' =============> Uploading silicon constants ============'
       print " == step1 ==> " + CommandLine1_Silicon + "\n"
       os.system(CommandLine1_Silicon)
       print " \n == step2 ==> " + CommandLine2_Silicon + "\n"
       os.system(CommandLine2_Silicon)
   if (Do_TRT == 1):
       print ' =============> Uploading TRT constants ============'
       print " == step1 ==> " + CommandLine1_TRT + "\n"
       print os.system(CommandLine1_TRT)
       print " \n == step2 ==> " + CommandLine2_TRT + "\n" 
       print os.system(CommandLine2_TRT)
   
elif Upload_Mode in ("Test"):
   print " \n == Test Mode !!!! == \n"
   print " =========== Common ============= " 
   print " == step0 ==> " + CommandLine0 + "\n\n"
   print "   WARNING       first check the condXX_data_gen.COND to point with the following command:"
   print "                 > ~atlcond/utils/registerFiles2 --list \n"
   print " =========== Silicon ============= " 
   print " == step1 ==> " + CommandLine1_Silicon + "\n"
   print " == step2 ==> " + CommandLine2_Silicon + "\n\n"
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
