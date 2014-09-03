###############################################
# Print summary or reconstruction top options #
###############################################


if rec.OutputLevel() <= INFO:
    print "***************** Summary of Reconstruction Top Options  ******************"

    print "\n * * * * * * * * * * * * * * * * *"
    print "Python files pattern excluded from traceing:"
    from AthenaCommon.Include import excludeTracePattern
    print excludeTracePattern

    if rec.doWriteRDO():
        print "\n * * * * * * * * * * * * * * * * *"
        print "StreamRDO Itemlist:"
        try:
            print StreamRDO.ItemList
        except Exception:
            treatException("Could not print StreamRDO!")        

    if rec.doWriteESD():
        print "\n * * * * * * * * * * * * * * * * *"
        print "StreamESD Itemlist:"
        try:
            print StreamESD.ItemList
        except Exception:
            treatException("Could not print StreamESD!")        

    if rec.doWriteAOD():
        print "\n * * * * * * * * * * * * * * * * *"
        print "StreamAOD Itemlist:"
        try:
            print StreamAOD.ItemList
        except Exception:
            treatException("Could not print StreamAOD!")        

    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    if MSMgr.nStream>0:
        MSMgr.Print()

    print "\n * * * * * * * * * * * * * * * * *"
    if rec.doDumpProperties:
        jobproperties.print_JobProperties('tree&value')
    else:
        print "JobProperties tree&value (only non default one):"
        try:
            jobproperties.print_JobProperties('tree&valuenondefault')
        except Exception:
            print "JobProperties tree&value (all): (failed to print only non default)"        
            jobproperties.print_JobProperties('tree&value')

    print "\n * * * * * * * * * * * * * * * * *"
    print "ExtSvc:"
    print theApp.ExtSvc

    print "\n * * * * * * * * * * * * * * * * *"
    print "Top algorithms:"
    print theApp.TopAlg

    try:
        import PerfMonComps.DomainsRegistry as pdr
        print "PerfMon domains : ",pdr.domain_names()
        for d in pdr.domain_names():
            print "::: domain [%s]"% d
            algs=pdr.domain_algs(d)
            print "   algs:",algs
    except Exception:
        print "WARNING Could not print PerfMonDomain"
        
    print "\n * * * * * * * * * * * * * * * * *"
    print "Detector flags:"
    try:
        DetFlags.Print()
    except Exception:
        treatException("Could not print DetFlags!")        

    print "\n * * * * * * * * * * * * * * * * *"

    if rec.OutputLevel() <= DEBUG:
        print "objKeyStore:"
        try:
            if 'objKeyStore' in dir():
                print objKeyStore
        except Exception:
            treatException("Could not print objKeyStore!")        


    

    #FIXME
    if 'EventTagFlags' in dir():
        print "\n * * * * * * * * * * * * * * * * *"
        print "EventTagFlags:"
        try:
            EventTagFlags.Print()
        except Exception:
            treatException("Could not print EventTagFlags!")        

    # dump all known python properties of algorithms and services (including tools)
    from RecExConfig.RecoFunctions import OutputFileName
    OutFileName=OutputFileName(rec.OutputSuffix())

    if rec.doDumpProperties():
        AthenaCommon.Configurable.log.setLevel( INFO ) 

        jobproperties.print_JobProperties('tree&value')

        print topSequence
        print ServiceMgr

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
            print "WARNING could not saveToPickle configuration! Continue"
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
            print " (experimental) the following objects should be written in AOD according to RecExPers/share/OKS_streamAOD_fromESD.py"
            print "but are not in input ESD file.File probably need to be updated, or maybe this is not full reco",diff

    if rec.doWriteAOD() and rec.doESD():
        persOutput=cfgRecExPers['streamAOD'].list()
        schedOutput=objKeyStore['streamAOD'].list()
        diff=[]
        for i in persOutput:
            if not i in schedOutput:
                diff+=[i]
        if len(diff)>0:
            print " (experimental) the following objects should be written in AOD according to RecExPers/share/OKS_streamAOD_fromESD.py"
            print "but are not in new streamAOD. File probably need to be updated, or maybe this is non full reco",diff
except Exception:
    print "Could not check cfgRecExpers"

        
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties

print "TrfJobReport metaData_geometryVersion = %s" %globalflags.DetDescrVersion()
print "TrfJobReport metaData_conditionsTag = %s" %globalflags.ConditionsTag()
print "TrfJobReport metaData_BeamType = %s" %jobproperties.Beam.beamType()
