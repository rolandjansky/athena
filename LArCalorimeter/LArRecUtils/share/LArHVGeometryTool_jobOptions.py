# JobOption for the LArHVGeometryTool
#
# Warning : this fragment assumes that the GlobalFlags have been set
#           It cannot be run in standalone !!!
#
# author C. Bourdarios-Adam , january 2005.

if GlobalFlags.DetGeo.is_ctbh8() :
    include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )

else :
    if  GlobalFlags.DetGeo.is_ctbh6() :
        include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H6_joboptions.py" )
    else :
        include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )

include( "LArRecUtils/LArHVScaleTool_jobOptions.py" ) 
