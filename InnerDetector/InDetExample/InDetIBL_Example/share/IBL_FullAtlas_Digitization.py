##########################################
# Comments
# Requested for IBL Digitization studies
##########################################
try:
    from SimuJobTransforms.DigitConfig import digitConfig
    ## Most of the configuration is done in the postOptions
    digitConfig.postOptions = "InDetIBL_Example/postOptions.IBL_Digitization.py"

except:
    print "Digitization WARNING From 16.0.0 using DigitConfig.postOptions is deprecated.\n Use the command line option postInclude=InDetIBL_Example/postOptions.IBL_Digitization.py instead"
    
