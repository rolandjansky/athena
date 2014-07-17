##########################################
# Comments
# Requested for IBL Digitization studies
##########################################
try:
    from SimuJobTransforms.DigitConfig import digitConfig
    ## Assume we're only interested in digitizing the ID.
    digitConfig.detectorsOff = ['Muon','Calo','LVL1']
    ## Most of the configuration is done in the postOptions
    digitConfig.postOptions = "InDetIBL_Example/postOptions.IBL_Digitization.py"

except:
    if not 'DetFlags' in dir():
        #if you configure one detflag, you're responsible for configuring them all!
        from AthenaCommon.DetFlags import DetFlags
        DetFlags.all_setOn()
    DetFlags.Muon_setOff()
    DetFlags.Calo_setOff()
    DetFlags.LVL1_setOff()
    print "Digitization WARNING From 16.0.0 using DigitConfig.postOptions is deprecated.\n Use the command line option postInclude=InDetIBL_Example/postOptions.IBL_Digitization.py instead"
    
