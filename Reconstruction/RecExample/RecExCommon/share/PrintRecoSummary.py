###############################################
# Print summary or reconstruction top options #
###############################################


if rec.OutputLevel() <= INFO:
    printfunc ("***************** Summary of Reconstruction Top Options  ******************")

    printfunc ("\n * * * * * * * * * * * * * * * * *")
    printfunc ("Python files pattern excluded from traceing:")
    from AthenaCommon.Include import excludeTracePattern
    printfunc (excludeTracePattern)

    if rec.doWriteRDO():
        printfunc ("\n * * * * * * * * * * * * * * * * *")
        printfunc ("StreamRDO Itemlist:")
        try:
            printfunc (StreamRDO.ItemList)
        except Exception:
            treatException("Could not print StreamRDO!")        

    if rec.doWriteESD():
        printfunc ("\n * * * * * * * * * * * * * * * * *")
        printfunc ("StreamESD Itemlist:")
        try:
            printfunc (StreamESD.ItemList)
        except Exception:
            treatException("Could not print StreamESD!")        

    if rec.doWriteAOD():
        printfunc ("\n * * * * * * * * * * * * * * * * *")
        printfunc ("StreamAOD Itemlist:")
        try:
            printfunc (StreamAOD.ItemList)
        except Exception:
            treatException("Could not print StreamAOD!")        

    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    if MSMgr.nStream>0:
        MSMgr.Print()

    printfunc ("\n * * * * * * * * * * * * * * * * *")
    if rec.doDumpProperties:
        jobproperties.print_JobProperties('tree&value')
    else:
        printfunc ("JobProperties tree&value (only non default one):")
        try:
            jobproperties.print_JobProperties('tree&valuenondefault')
        except Exception:
            printfunc ("JobProperties tree&value (all): (failed to print only non default)"        )
            jobproperties.print_JobProperties('tree&value')

    printfunc ("\n * * * * * * * * * * * * * * * * *")
    printfunc ("ExtSvc:")
    printfunc (theApp.ExtSvc)

    printfunc ("\n * * * * * * * * * * * * * * * * *")
    printfunc ("Top algorithms:")
    printfunc (theApp.TopAlg)

    try:
        import PerfMonComps.DomainsRegistry as pdr
        printfunc ("PerfMon domains : ",pdr.domain_names())
        for d in pdr.domain_names():
            printfunc ("::: domain [%s]"% d)
            algs=pdr.domain_algs(d)
            printfunc ("   algs:",algs)
    except Exception:
        printfunc ("WARNING Could not print PerfMonDomain")
        
    printfunc ("\n * * * * * * * * * * * * * * * * *")
    printfunc ("Detector flags:")
    try:
        DetFlags.Print()
    except Exception:
        treatException("Could not print DetFlags!")        

    printfunc ("\n * * * * * * * * * * * * * * * * *")

    if rec.OutputLevel() <= DEBUG:
        printfunc ("objKeyStore:")
        try:
            if 'objKeyStore' in dir():
                printfunc (objKeyStore)
        except Exception:
            treatException("Could not print objKeyStore!")        


    # dump all known python properties of algorithms and services (including tools)
    from RecExConfig.RecoFunctions import OutputFileName
    OutFileName=OutputFileName(rec.OutputSuffix())

    if rec.doDumpProperties():
        AthenaCommon.Configurable.log.setLevel( INFO ) 

        jobproperties.print_JobProperties('tree&value')

        printfunc (topSequence)
        printfunc (ServiceMgr)

        try:
            # dump in a separate ascii file
            from AthenaCommon.ConfigurationShelve import saveToAscii
            saveToAscii(OutFileName+'_config.txt')
        except Exception:
            treatException ("Could not saveToAscii configuration!")

        try:    
            from AthenaCommon.ConfigurationShelve import saveToPickle
            saveToPickle ('OutFileName_config.pkl')
        except Exception:
            printfunc ("WARNING could not saveToPickle configuration! Continue")
            # treatException ("Could not saveToPickle configuration!")            
        # dump objKeyStore content in a readable way
        objKeyStore.writeInputFile('OKS_InputFile_'+OutFileName+'.py')
        objKeyStore.writeStreamRDO('OKS_streamRDO_'+OutFileName+'.py')
        objKeyStore.writeStreamESD('OKS_streamESD_'+OutFileName+'.py')
        objKeyStore.writeStreamAOD('OKS_streamAOD_'+OutFileName+'.py')
        objKeyStore.writeStreamTAG('OKS_streamTAG_'+OutFileName+'.py')
    # switch on auditor on specific alg or service
    # to measure CPU time and memory spend in initialisation
    # alg.AuditInitialize = True
    # svc.AuditInitialize = True


# objKeyStore sanity check
try:
    if rec.doWriteAOD() and rec.readESD():
        persOutput=cfgRecExPers['streamAOD'].list()
        allInput=objKeyStore['inputFile'].list()
        diff=[]
        for i in persOutput:
            if not i in allInput:
                diff+=[i]
        if len(diff)>0:
            printfunc (" (experimental) the following objects should be written in AOD according to RecExPers/share/OKS_streamAOD_fromESD.py")
            printfunc ("but are not in input ESD file.File probably need to be updated, or maybe this is not full reco",diff)

    if rec.doWriteAOD() and rec.doESD():
        persOutput=cfgRecExPers['streamAOD'].list()
        schedOutput=objKeyStore['streamAOD'].list()
        diff=[]
        for i in persOutput:
            if not i in schedOutput:
                diff+=[i]
        if len(diff)>0:
            printfunc (" (experimental) the following objects should be written in AOD according to RecExPers/share/OKS_streamAOD_fromESD.py")
            printfunc ("but are not in new streamAOD. File probably need to be updated, or maybe this is non full reco",diff)
except Exception:
    printfunc ("Could not check cfgRecExpers")

        
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties

printfunc ("TrfJobReport metaData_geometryVersion = %s" %globalflags.DetDescrVersion())
printfunc ("TrfJobReport metaData_conditionsTag = %s" %globalflags.ConditionsTag())
printfunc ("TrfJobReport metaData_BeamType = %s" %jobproperties.Beam.beamType())
