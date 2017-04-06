#!/python
import os,re,sys

InputData = sys.argv[1]#e.g.: 'Run_periodA.txt'
DataSets = [line.rstrip('\n') for line in open(InputData)]

RegEx = re.compile( '(\w*).(\w*)\.(\d*)\.(\w*)\.(\w*)\.(\w*)\.(\w*)' )
UserName = 'sthenkel'
Version = 'maps2016-A-I-DESDMt'
nJobs = 200000
FilesPerJob = 5
Nfiles=90
official = True
UseConstants = False
officialGroup = "perf-idtracking"
officialTag= "--official --voms=atlas:/atlas/"+officialGroup+"/Role=production"
excludeSites = ""
#athenaVersion = '19.3.0.1'

#athenaVersion = 'AtlasProduction,20.7.7.7'
athenaVersion = 'AtlasProduction,20.7.7.9'

GBperJob = 'MAX'
#JOs = os.path.expandvars('$TestArea/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/runZmumuValidation_data_OnGrid.py')

#JOs = os.path.expandvars('$TestArea/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/runzmumu_runallmonitoring_grid.py')
JOs = os.path.expandvars('$TestArea/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/runzmumu_maps_grid.py')
AlignmentConst = "Iter0_AlignmentConstants_Mat_ZmumuSagitta_d0_onTopOfWillsEPconstr_2iterations.root"#"./Iter1_AlignmentConstants.root"
alignmentFile = os.path.expandvars(AlignmentConst)

outputFile="_ZmumuValidation/"

downloadFile = open("Download_"+str(os.path.splitext(InputData)[0])+".sh",'w')
downloadFile.write("#!/bin/bash \n")
for DataSet in DataSets:
    Match = RegEx.match( DataSet )
    if official == True:
        OutputDataSet = 'group.' + officialGroup + '.' + Version + '.' + Match.group( 2 ) + '.' + Match.group( 3 ) +'.' + Match.group( 5 ) + '.' + Match.group( 6 )
        print(OutputDataSet)
    else:
        OutputDataSet = 'user.' + UserName + '.' + Version + '.' + Match.group( 2 ) + '.' + Match.group( 3 ) +'.' + Match.group( 5 ) + '.' + Match.group( 6 )
        print(OutputDataSet)

    proc_file_RDO=DataSet
#    print 'DataSet : '+proc_file_RDO

    downloadFile.write("rucio download "+str(OutputDataSet)+str(outputFile)+" \n")


    if UseConstants == False and official == True:

        os.system( 'pathena %s %s --long --inDS=%s --outDS=%s --nJobs=%d --nFilesPerJob=%d --nGBPerJob=%s --athenaTag=%s --useShortLivedReplicas --excludedSite=%s' % ( JOs, officialTag, DataSet, OutputDataSet, nJobs, FilesPerJob, GBperJob, athenaVersion,excludeSites ) )

    if UseConstants == False and official == False:
        os.system( 'pathena %s --long --inDS=%s --outDS=%s --nJobs=%d --nFilesPerJob=%d --nGBPerJob=%s --athenaTag=%s  --useShortLivedReplicas --excludedSite=%s' % ( JOs, DataSet, OutputDataSet, nJobs, FilesPerJob, GBperJob, athenaVersion, excludeSites) )

    if UseConstants == True and official == True:
        os.system( 'pathena %s %s  --long --inDS=%s --outDS=%s --nJobs=%d --nFilesPerJob=%d --nGBPerJob=%s --athenaTag=%s --extFile=%s --addPoolFC=%s --useShortLivedReplicas --excludedSite=%s' % ( JOs, officialTag, DataSet, OutputDataSet, nJobs, FilesPerJob, GBperJob, athenaVersion,alignmentFile,alignmentFile,excludeSites ) )

    if UseConstants == True and official == False:
        os.system( 'pathena %s  --long --inDS=%s --outDS=%s --nJobs=%d --nFilesPerJob=%d --nGBPerJob=%s --athenaTag=%s --extFile=%s --addPoolFC=%s --useShortLivedReplicas --excludedSite=%s' % ( JOs, DataSet, OutputDataSet, nJobs, FilesPerJob, GBperJob, athenaVersion,alignmentFile,alignmentFile,excludeSites ) )

#command = "pathena runZmumuValidation.py --official --voms=atlas:/atlas/det-indet/Role=production --inDS mc14_13TeV.147406.PowhegPythia8_AZNLO_Zee.recon.ESD.e3059_s1982_s2008_r5862/  --outDS group.det-indet.mc14_13TeV.147406.PowhegPythia8_AZNLO_Zee.recon.ESD.e3059_s1982_s2008_r5862 --nGBPerJob=MAX --excludedSite='ANALY_GOEGRID' --useShortLivedReplicas --cmtConfig=x86_64-slc5-gcc43-opt"


#file = "mc14_13TeV.147407.PowhegPythia8_AZNLO_Zmumu.recon.ESD.e3059_s2046_s2008_r5862/"#mc14_13TeV.147406.PowhegPythia8_AZNLO_Zee.recon.ESD.e3059_s1982_s2008_r5862/"
#path = "/afs/cern.ch/user/s/sthenkel/work/ProjectArea/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/"
#command2 = "pathena "+path+"share/runZmumuValidation.py -c \"inputFiles = "+file+"\" --inDS "+file+"  --outDS user.sthenkel.test1234."+file+" --nGBPerJob=MAX --athenaTag=19.3.0.1 --excludedSite='ANALY_GOEGRID' --useShortLivedReplicas --cmtConfig=x86_64-slc5-gcc43-opt"

#command3 = "pathena "+path+"share/runZmumuValidation.py --inDS "+file+"  --outDS user.sthenkel.test1234."+file+" --nGBPerJob=MAX --athenaTag=19.3.0.1 --excludedSite='ANALY_GOEGRID' --useShortLivedReplicas --nJobs=10 --nFilesPerJob=10"# --cmtConfig=x86_64-slc5-gcc43-opt"
#os.system(setup)
#os.system(command3)
