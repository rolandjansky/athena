##Check the --rs option## - Promemoria

#! /usr/bin/python
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
Def_ConstantsFile = "TempConstants"
User_ConstantsFile = ""
#
Def_Mode = "Test"
User_Mode = ""
#
Def_Comment = "Calibration Loop constants derived from run " 
User_Comment =""
#
Def_Run = 0
User_Run = 0
#
Def_DBName = "CONDBR2"
User_DBName = ""
#
Def_Conditions = "condR2_data.gen.COND"
User_Conditions = ""

Def_InDetUPD4 = "InDetAlign-RUN2-BLK-UPD4-16"
User_InDetUPD4 = ""

Def_InDetUPD1 = "InDetAlign-RUN2-ES1-UPD1-06"
User_InDetUPD1 = ""

Def_TRTUPD4 = "TRTAlign-RUN2-BLK-UPD4-13"
User_TRTUPD4 = ""
Def_TRTUPD1 =  "TRTAlign-RUN2-ES1-UPD1-05"
User_TRTUPD1 = "" 

#
# =====================================================================
#  Handling of run options
# =====================================================================
if len(sys.argv) > 1:
    argNum = 1
    argMax = len(sys.argv)-1
    argCurr = -1 # jut to make that first time it points to 0 
    print ' >>> user input has %s arguments' % argMax
    for arg in sys.argv:
        argCurr += 1
        #print ".. %s .." % arg
        if arg in ("-h", "--help"):    
            print ' >>> This is the help message of ConstantsUploaderSimple.py'       
            print ' NAME'
            print '     ConstantsUploaderSimple.py'
            print ' '
            print ' SYNOPSIS'
            print '     python ConstantsUploaderSimple.py [-r RunNumber] [-f ConstantsFile]' 
            print ' '
            print ' OPTIONS'
            print '     The following options are recognized by ConstantsUploaderSimple.py'
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
            print '     -InDetUPD1 tagName '
            print '          ES UPD1 tag that the constants will be copied into. The default \n name is: %s' %Def_InDetUPD1
            print ' '
            print '     -TRTUPD1 tagName '
            print '          ES UPD1 tag that the constants will be copied into. The default \n name is: %s' %Def_TRTUPD1
            print ' '
            print '     -InDetUPD4 tagName '
            print '          BLK UPD4 tag that the constants will be copied into. The default \n name is: %s' %Def_InDetUPD4
            print ' '
            print '     -TRTUPD4 tagName '
            print '          BLK UPD4 tag that the constants will be copied into. The default \n name is: %s' %Def_TRTUPD4   
            print ' '
            print '     -r Run'
            print '           Run number to look at the default is %i' %Def_Run
            print ' '            
            print ' \n 09/February/2011'
            sys.exit()
        elif arg in ("-f", "--file"):    
            User_ConstantsFile = str(sys.argv[argCurr+1])
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
        
        elif arg in ("-InDetUPD1","--InDetUPD1"):    
            # This option activates the execution of the commands
            User_InDetUPD1 = str(sys.argv[argCurr+1])
            #
        elif arg in ("-InDetUPD4","--InDetUPD4"):    
            # This option activates the execution of the commands
            User_InDetUPD4 = str(sys.argv[argCurr+1])
            #
        elif arg in ("-TRTUPD1","--TRTUPD1"):    
            # This option activates the execution of the commands
            User_TRTUPD1 = str(sys.argv[argCurr+1])
            #
        elif arg in ("-TRTUPD4","--TRTUPD4"):
            # This option activates the execution of the commands
            User_TRTUPD4 = str(sys.argv[argCurr+1])
            #
        elif arg in ("-r", "--run", "-r ", "--run "):
            # This option activates the execution of the commands
            User_Run = int(sys.argv[argCurr+1])



    if (1):
        print '>>> ConstantsUploader >>> List of user input values:'
        print '> User File Name = %s' % User_ConstantsFile 
        print '> User Run number = %i' %User_Run 
        print '> User Do_Silicon = %d' % User_Do_Silicon
        print '> User Do_TRT = %d' % User_Do_TRT
        print '> User Mode = %s' % User_Mode
        print '> User Comment = %s' %User_Comment
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

Tag_InDetUPD1 = Def_InDetUPD1
if (User_InDetUPD1):
    Tag_InDetUPD1 = User_InDetUPD1
Tag_InDetUPD4 = Def_InDetUPD4
if (User_InDetUPD4):
    Tag_InDetUPD4 = User_InDetUPD4


Tag_TRTUPD1 = Def_TRTUPD1
if (User_TRTUPD1):
    Tag_TRTUPD1 = User_TRTUPD1

Tag_TRTUPD4 = Def_TRTUPD4
if (User_TRTUPD4):
    Tag_TRTUPD4 = User_TRTUPD4

Run = Def_Run
if (User_Run != 0 ):
  Run = User_Run

if  Run == 0 :
  print '****************************************************'
  print '** Run number is set to zero and will not execute **'
  print '**             Switching to test modes            **'   
  print '****************************************************\n'

  Upload_Mode = 'Test'


#
DotDbFile = ConstantsFileName+".db"
DotRootFile = ConstantsFileName+".root"
OutputDbFile = ConstantsFileName+"_output.db"
#
CommentOnConstants = Def_Comment + str(Run)
if (User_Comment):
 CommentOnConstants = User_Comment + str(Run)
# 



if (1):
    print '>>>>>>>>>>>> List of variables >>>>>>>>>>>>>>> '
    print '> DotDbFile    = %s' %DotDbFile
    print '> DotRootFile  = %s' %DotRootFile
    print '> OutputDbFile = %s' %OutputDbFile
    print '> DB Name      = %s' %DBName
    print '> Conditions   = %s' %ConditionsFile
    print '> Upload_Mode  = %s' %Upload_Mode
    print '> Run number   = %s' %Run
    print '> InDet UPD1 tag    = %s' %Tag_InDetUPD1
    print '> TRT UPD1 tag      = %s' %Tag_TRTUPD1
    print '> InDet UPD4 tag    = %s' %Tag_InDetUPD4
    print '> TRT UPD4 tag      = %s' %Tag_TRTUPD4
    print '> Do_Silicon   = %d' %Do_Silicon
    print '> Do_TRT       = %d' %Do_TRT
    print '> Comment      = %s' %CommentOnConstants
    print '<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n '


if Upload_Mode in ("Exec")  and  (os.system('which AtlCoolCopy.exe') != 0 ):
  print ' ** no AtlCoolCopy.exe  -- Script will setup athena now **'
  os.system('export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup')
  os.system('source $AtlasSetup/scripts/asetup.sh --input /afs/cern.ch/user/a/atlidali/IDAlignDBTools/asetupConfig')



# ===============================
# Common step
# ===============================
# this command line is the same for silicon and TRT
CommandLine0 = "~atlcond/utils/registerFiles2 --wait " + ConditionsFile + " " + DotRootFile

# ===============================
# 1st step
# ===============================

# -- prepare the command 
CommandLine1_Silicon = "AtlCoolCopy.exe \"sqlite://schema="+DotDbFile+";dbname="+DBName+"\" \"sqlite://;schema="+OutputDbFile+";dbname="+DBName+"\" -create -folder /Indet/Align -outtag " + Tag_InDetUPD4  + " -rs "+ str(Run)  +" -truncate -tl \""+ CommentOnConstants +"\""

CommandLine1_TRT = "AtlCoolCopy.exe \"sqlite://schema="+DotDbFile+";dbname="+DBName+"\" \"sqlite://;schema="+OutputDbFile+";dbname="+DBName+"\" -create -folder /TRT/Align -outtag " + Tag_TRTUPD4 + " -rs "+ str(Run) +" -truncate  -tl \""+ CommentOnConstants +"\""


CommandLine15_Silicon = "AtlCoolCopy.exe \"sqlite://schema="+DotDbFile+";dbname="+DBName+"\" \"sqlite://;schema="+OutputDbFile+";dbname="+DBName+"\" -create -folder /Indet/Align -outtag " + Tag_InDetUPD1 + " -go "  + " -tl \""+ CommentOnConstants +"\""

CommandLine15_TRT = "AtlCoolCopy.exe \"sqlite://schema="+DotDbFile+";dbname="+DBName+"\" \"sqlite://;schema="+OutputDbFile+";dbname="+DBName+"\" -create -folder /TRT/Align -outtag " + Tag_TRTUPD1 +" -go "+" -tl \""+ CommentOnConstants +"\""


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
CommandLine2_Silicon = "/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --debug --batch --nobackup --comment \""+CommentOnConstants+"\" --tag " + Tag_InDetUPD1  +" --passopt='-cti' --folder /Indet/Align "+ OutputDbFile +" "+DBName+" ATLAS_COOLWRITE ATLAS_COOLOFL_INDET_W WCOOLOFL4INDET1"

CommandLine2_TRT = "/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --debug --batch --nobackup --comment \""+CommentOnConstants+"\" --tag " + Tag_TRTUPD1 + " --passopt='-cti' --folder /TRT/Align "+ OutputDbFile +" "+DBName+" ATLAS_COOLWRITE ATLAS_COOLOFL_TRT_W WCOOLOFL4TRT2"


# ===============================
# 3rd step
# ===============================
CommandLine3_Silicon = "/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --debug --batch --nobackup --comment \""+CommentOnConstants+"\" --tag " + Tag_InDetUPD4 + " --rs "+ str(Run) + " --passopt='-cti' --folder /Indet/Align "+ OutputDbFile +" "+DBName+" ATLAS_COOLWRITE ATLAS_COOLOFL_INDET_W WCOOLOFL4INDET1"

CommandLine3_TRT = "/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --debug --batch --nobackup --comment \""+CommentOnConstants+"\" --tag " + Tag_TRTUPD4 +  " --rs "+ str(Run) + " --passopt='-cti' --folder /TRT/Align "+ OutputDbFile +" "+DBName+" ATLAS_COOLWRITE ATLAS_COOLOFL_TRT_W WCOOLOFL4TRT2"



#- 
# ===============================
# Upload
# ===============================
if Upload_Mode in ("Exec"):
   print " \n == Excute upload !!!! == \n"

   print " \n == step0 ==> " + CommandLine0 + "\n"
   ret = os.system(CommandLine0)
   if ( ret != 0 ):
       sys.exit(1)
   
   """
   doThis = raw_input("Do you want to execute the command? <yes/no>: ")
   if doThis == 'yes':
      print 'Will attempt to upload POOL file'
      os.system(CommandLine0)
   else :
      print 'Will not execute commnand -- your responce was: ' + doThis
   """
   #print "   WARNING       first check the that POOL file has been registered eg:"
   #print " \n > " + CommandLine0 + "\n"

   if (Do_Silicon == 1):
       print ' =============> Uploading silicon constants ============'
       print " == step1   ==> " + CommandLine1_Silicon + "\n"
       print " == step1.5 ==> " + CommandLine15_Silicon + "\n"
       print " == step2   ==> " + CommandLine2_Silicon + "\n"
       print " == step3   ==> " + CommandLine3_Silicon + "\n"
       
       print " == step1 ==> " + CommandLine1_Silicon + "\n"
       ret = os.system(CommandLine1_Silicon)
       if( ret != 0 ):
           sys.exit(2)
       print " == step1.5 ==> " + CommandLine15_Silicon + "\n"
       ret = os.system(CommandLine15_Silicon)
       if( ret != 0 ):
           sys.exit(3)
       print " \n == step2 ==> " + CommandLine2_Silicon + "\n"
       ret = os.system(CommandLine2_Silicon)
       if( ret != 0 ):
           sys.exit(4)
       print " \n == step3 ==> " + CommandLine3_Silicon + "\n"
       ret = os.system(CommandLine3_Silicon)
       if( ret != 0 ):
           sys.exit(5)
       
       """
       doThis = raw_input("Do you want to upload the silicon constants with the above commands? <yes/no>: ")
       if doThis == 'yes':
         print 'Will attempt to upload Si constants to the db'

         print " == step1 ==> " + CommandLine1_Silicon + "\n"
         os.system(CommandLine1_Silicon)
         print " == step1.5 ==> " + CommandLine15_Silicon + "\n"
         os.system(CommandLine15_Silicon)
         print " \n == step2 ==> " + CommandLine2_Silicon + "\n"
         os.system(CommandLine2_Silicon)
         print " \n == step3 ==> " + CommandLine3_Silicon + "\n"
         os.system(CommandLine3_Silicon)
       else :
         print 'Will not upload Si constants -- your responce was: ' + doThis
       """


   if (Do_TRT == 1):
       print ' =============> Uploading TRT constants ============'
       print " == step1   ==> " + CommandLine1_TRT + "\n"
       print " == step1.5 ==> " + CommandLine15_TRT + "\n"
       print " == step2   ==> " + CommandLine2_TRT + "\n"
       print " == step3   ==> " + CommandLine3_TRT + "\n"
       
       print " == step1 ==> " + CommandLine1_TRT + "\n"
       ret = os.system(CommandLine1_TRT)
       if( ret != 0 ):
           sys.exit(6)
       print " == step1.5 ==> " + CommandLine15_TRT + "\n"
       ret = os.system(CommandLine15_TRT)
       if( ret != 0 ):
           sys.exit(7)
       print " \n == step2 ==> " + CommandLine2_TRT + "\n" 
       ret = os.system(CommandLine2_TRT)
       if( ret != 0 ):
           sys.exit(8)
       print " \n == step3 ==> " + CommandLine3_TRT + "\n"
       ret = os.system(CommandLine3_TRT)
       if( ret != 0 ):
           sys.exit(9)
       
       """
       doThis = raw_input("Do you want to upload the TRT constants with the above commands? <yes/no>: ")
       if doThis == 'yes':       
          print " == step1 ==> " + CommandLine1_TRT + "\n"
          print os.system(CommandLine1_TRT)
          print " == step1.5 ==> " + CommandLine15_TRT + "\n"
          print os.system(CommandLine15_TRT)
          print " \n == step2 ==> " + CommandLine2_TRT + "\n" 
          print os.system(CommandLine2_TRT)
          print " \n == step3 ==> " + CommandLine3_TRT + "\n"
          print os.system(CommandLine3_TRT)
       else :
         print 'Will not upload TRT constants -- your responce was: ' + doThis
       """

   
elif Upload_Mode in ("Test"):
   print " \n == Test Mode !!!! == \n"
   print " =========== Common ============= " 
   print " == step0 ==> " + CommandLine0 + "\n\n"
   print "   WARNING       first check the condXX_data_gen.COND to point with the following command:"
   print "                 > ~atlcond/utils/registerFiles2 --list \n"
   print " =========== Silicon ============= " 
   print " == step1 ==> " + CommandLine1_Silicon + "\n"
   print " == step1.5 ==> " + CommandLine15_Silicon + "\n"
   print " == step2 ==> " + CommandLine2_Silicon + "\n"
   print " == step3 ==> " + CommandLine3_Silicon + "\n\n"
   
   print " ===========  TRT  ============= " 
   print " == step1 ==> " + CommandLine1_TRT + "\n"
   print " == step1.5 ==> " + CommandLine15_TRT + "\n"
   print " == step2 ==> " + CommandLine2_TRT + "\n" 
   print " == step3 ==> " + CommandLine3_TRT + "\n\n"

   
# ===============================
# completed
# ===============================
print "\n ============================================="
print "    GAME OVER.  INSERT COIN"
print " =============================================\n"
sys.exit()
