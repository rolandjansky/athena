
tagList = ['LARConfigurationDSPThresholdTemplates-Qt0-samp0',
           'LARConfigurationDSPThresholdTemplates-Qt250-samp0',
           'LARConfigurationDSPThresholdTemplates-Qt250-samp1000',
           'LARConfigurationDSPThresholdTemplates-Qt250HECFCAL500-samp1000',
           'LARConfigurationDSPThresholdTemplates-Qt10000-samp10000',
           'LARConfigurationDSPThresholdTemplates-Qt0sigma-samp50sigma',
           'LARConfigurationDSPThresholdTemplates-Qt50sigma-samp0sigma',
           'LARConfigurationDSPThresholdTemplates-Qt0.5sigma-samp0.5sigma',
           'LARConfigurationDSPThresholdTemplates-Qt1sigma-samp1sigma',
##           'LARConfigurationDSPThresholdTemplates-Qt1sigma-samp10sigma',
           'LARConfigurationDSPThresholdTemplates-Qt1.5sigma-samp1.5sigma',
           'LARConfigurationDSPThresholdTemplates-Qt2sigma-samp2sigma',
           'LARConfigurationDSPThresholdTemplates-Qt3sigma-samp1sigma',
##           'LARConfigurationDSPThresholdTemplates-Qt3sigma-samp1.5sigma',
           'LARConfigurationDSPThresholdTemplates-Qt3sigma-samp2sigma',
           'LARConfigurationDSPThresholdTemplates-Qt3sigma-samp3sigma',
           'LARConfigurationDSPThresholdTemplates-Qt3sigma-samp5sigma',
           'LARConfigurationDSPThresholdTemplates-Qt5sigma-samp5sigma',
           'LARConfigurationDSPThresholdTemplates-Qt1sigmaPileup-samp1sigmaPileup',
           'LARConfigurationDSPThresholdTemplates-Qt2sigmaPileup-samp2sigmaPileup',
           'LARConfigurationDSPThresholdTemplates-Qt2sigmaPileup-samp3sigmaPileup',
           'LARConfigurationDSPThresholdTemplates-Qt3sigmaPileup-samp4sigmaPileup',
           'LARConfigurationDSPThresholdTemplates-Qt3sigmaPileup-samp3sigmaPileup',
           'LARConfigurationDSPThresholdTemplates-Qt4sigmaPileup-samp4sigmaPileup',
           'LARConfigurationDSPThresholdTemplates-Qt5sigmaPileup-samp5sigmaPileup']

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
    
