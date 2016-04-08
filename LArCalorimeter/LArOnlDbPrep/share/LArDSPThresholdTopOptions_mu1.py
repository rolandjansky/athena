
tagList = [
           'LARConfigurationDSPThresholdFlatTemplates-Qt1sigmaSmallPileup-samp1sigmaSmallPileup',
           'LARConfigurationDSPThresholdFlatTemplates-Qt2sigmaSmallPileup-samp2sigmaSmallPileup',
           'LARConfigurationDSPThresholdFlatTemplates-Qt2sigmaSmallPileup-samp3sigmaSmallPileup',
           'LARConfigurationDSPThresholdFlatTemplates-Qt3sigmaSmallPileup-samp4sigmaSmallPileup',
           'LARConfigurationDSPThresholdFlatTemplates-Qt3sigmaSmallPileup-samp3sigmaSmallPileup',
           'LARConfigurationDSPThresholdFlatTemplates-Qt4sigmaSmallPileup-samp4sigmaSmallPileup',
           'LARConfigurationDSPThresholdFlatTemplates-Qt5sigmaSmallPileup-samp5sigmaSmallPileup'
           ]

tag=tagList[tagNum]

print tag

Qtstr=tag.split("Qt")[1]
Sampstr=tag.split("samp")[1]

#print Qtstr,Sampstr

if "sigma" in tag:
    ModeType = "noise"

    Qtpileup = False
    Samppileup = False
        
    if len(Qtstr.split("sigma")) > 1:
        print Qtstr.split("sigma")[1]
        if Qtstr.split("sigma")[1].find("Pileup") > -1:
            Qtpileup = True

    if len(Sampstr.split("sigma")) > 1:
        print Sampstr.split("sigma")[1]
        if Sampstr.split("sigma")[1].find("Pileup") > -1:
            Samppileup = True

    print "Setting pileup noise to (Qt,Samp) = ",Qtpileup,Samppileup

    Qtstr=Qtstr.split("sigma")[0]
    Sampstr=Sampstr.split("sigma")[0]
    
elif "HECFCAL" in tag:
    ModeType = "group"
    Qtstr="0" ## done manually
    Sampstr="0" ## done manually

else:
    ModeType = "fixed"
    Qtstr=Qtstr.split("-")[0]

Qtval=float(Qtstr)
Sampval=float(Sampstr)
print Qtval,Sampval

include("LArOnlDbPrep/LArDSPThresholdFillAlg.py")
    
