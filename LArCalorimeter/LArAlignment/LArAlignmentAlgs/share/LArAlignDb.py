#########################################################
#
# Job options file
#
#########################################################

# Set db name
TESTCOOL = "TESTABC"

#include jobOpt you want to run :

# step 1
#include ( "LArAlignmentAlgs/LArAlignDbWriteCoolNoReg.py")
# check step 1
include ( "LArAlignmentAlgs/LArAlignDbReadNoReg.py")

# step 2
#include ( "LArAlignmentAlgs/LArAlignDbReadCoolAndReg.py")

# ======= Customization ==============================

# for the H8 table position overwrite default :
# LArAlignDbAlg.CTBGeometry = True

# change Root file name ( default is LArAlignmentFile.root ) :
ToolSvc = Service( "ToolSvc" )
ToolSvc.CondStream1.OutputFile = "LArFile-1.root"
