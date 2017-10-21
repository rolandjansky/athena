from CosmicGenerator.CosmicGeneratorConf import CosmicGenerator
cg = CosmicGenerator()

if simFlags.CosmicPtSlice == 'slice1':
    cg.emin =  10.*1000. #  10 GeV
    cg.emax = 100.*1000. # 100 GeV
    cg.xvert_low =  -1000.*200. # 200 m
    cg.xvert_hig =   1000.*200. # 200 m
    cg.zvert_low =  -1000.*200. # 200 m
    cg.zvert_hig =   1000.*200. # 200 m
elif simFlags.CosmicPtSlice == 'slice2':
    cg.emin = 100.*1000. # 100 GeV
    cg.emax = 300.*1000. # 300 GeV
    cg.xvert_low =  -1000.*600. # 600 m
    cg.xvert_hig =   1000.*600. # 600 m
    cg.zvert_low =  -1000.*600. # 600 m
    cg.zvert_hig =   1000.*600. # 600 m
elif simFlags.CosmicPtSlice == 'slice3':
    cg.emin =  300.*1000.  #  300 GeV
    cg.emax = 1000.*1000.  # 1000 GeV
    cg.xvert_low =  -1000.*1000. # 1 km
    cg.xvert_hig =   1000.*1000. # 1 km
    cg.zvert_low =  -1000.*1000. # 1 km
    cg.zvert_hig =   1000.*1000. # 1 km
elif simFlags.CosmicPtSlice == 'slice4':
    cg.emin = 1000.*1000. # 1 TeV
    cg.emax = 5000.*1000. # 5 TeV
    cg.xvert_low =  -1000.*3000. # 3 km
    cg.xvert_hig =   1000.*3000. # 3 km
    cg.zvert_low =  -1000.*3000. # 3 km
    cg.zvert_hig =   1000.*3000. # 3 km
elif simFlags.CosmicPtSlice != 'NONE':
    print 'Slice name incorrect!'
    # TODO: theApp.exit(1)?
    import sys
    sys.exit(1)

bedrockDX = (cg.xvert_hig - cg.xvert_low)/2.
bedrockDZ = (cg.zvert_hig - cg.zvert_low)/2.

if (bedrockDX > 350000 or bedrockDZ > 350000) :
    newSize = max( bedrockDX , bedrockDZ )
    print "Resizing bedrock (mm) to fit cosmic generator:",newSize
    from AthenaCommon.Configurable import Configurable
    if Configurable.allConfigurables.get('GeoModelSvc'):
        GeoModelSvc = Configurable.allConfigurables.get('GeoModelSvc')
    else:
        GeoModelSvc = theApp.service('GeoModelSvc')
    if (newSize <=  500000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-04-Bedrock500'
    elif (newSize <= 1000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-04-Bedrock1000'
    elif (newSize <= 1500000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-04-Bedrock1500'
    elif (newSize <= 2000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-04-Bedrock2000'
    elif (newSize <= 3000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-04-Bedrock3000'
    elif (newSize <= 4000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-04-Bedrock4000'
    elif (newSize <= 5000000) : GeoModelSvc.CavernInfraVersionOverride = 'CavernInfra-04-Bedrock5000'
else :
    print "No need to resize the bedrock for cosmic generation"
