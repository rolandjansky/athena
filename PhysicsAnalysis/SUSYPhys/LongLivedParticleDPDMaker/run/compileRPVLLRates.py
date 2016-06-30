# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#the input log file should have the output of:
#grep -E "RAWtoESD.*RPVLL.*Events|BSESOutputSvcStreamDRAW_RPVLL.*events" 00300784/data16_13TeV.00300784.physics_Main.daq.RAW.f708.recon.task.LOG/* > ~/rpvllLog_00300784.out &
#the folder with log files is on eos: eos/eosatlas/atlastier0/tzero/prod/physics_Main/ 
#if grep complains about the long argument, just cd into the folder with the log files. The command does take quite a while to run.
#The log files only stay around for a few weeks, or the folder is empty and only LOGARC remain. the logs can likely be recovered from the LOGARC files, if necessary
#39 lines should be recovered from each file!

#run file with python compileRates.py

List = open("/afs/cern.ch/user/h/hrussell/rpvllLog_00300908.out").readlines()

#the filter names could change run-to-run, with filter updates. If so, need to update the number of items in the list
#and the hardcoded numbers 39 (2*number of filters + 1 line for the total accepted into RPVLL) , 19 (number of filters), 38 (2*number of filters)

filterNames = ['DiLep_SiElectronFilterKernel','DiLep_SiPhotonXFilterKernel','DiLep_SiMuonFilterKernel','DiLep_DiElectronFilterKernel','DiLep_DiPhotonFilterKernel',
'DiLep_DiElPhFilterKernel','DiLep_DiLoElPhFilterKernel','DVAugmentationKernel','DVMuonFilterKernel','DV_PhotonFilterKernel',
'DV_PhotonPlusTLJetFilterKernel','DV_MultiJetFilterKernel','DV_METFilterKernel','DV_MeffFilterKernel','KinkedTrackJetFilterKernel',
'KinkedTrackZeeFilterKernel','KinkedTrackZmumuFilterKernel','HnlFilterKernel','HVTriggerFilterKernel']

eventList = [0] * 39
lineNo = 0

for i,line in enumerate(List):
    if(i < len(List) - 39):
        if(line.split()[0][:-10] == List[i+39].split()[0][:-10]):
		continue
    [int(s) for s in line.split() if s.isdigit()]
    eventList[lineNo%39] += int(s)
    lineNo += 1

#print eventList

TotalEvents = eventList[0]
TotalRPVLLPass = eventList[38]
print 'Out of ',TotalEvents,' events, ',TotalRPVLLPass,' events passed RPVLL filters. Efficiency: ',float(TotalRPVLLPass)/float(TotalEvents)*100.,'%.'

TotalFracList = [0]*19
RPVLLFracList = [0]*19
filterEvents = [0]*19
closureTest = 0

for filterNo in range(0,19):	
	closureTest += eventList[filterNo*2+1]
	TotalFracList[filterNo] = float(eventList[filterNo*2+1])/float(TotalEvents)
	RPVLLFracList[filterNo] = float(eventList[filterNo*2+1])/float(TotalRPVLLPass)
	filterEvents[filterNo] = eventList[filterNo*2+1]
	print filterNames[filterNo],' (rel, total): (',RPVLLFracList[filterNo],', ',TotalFracList[filterNo],')'
	
#subtracting away the results of the DVAugmentationKernel, which isn't a filter.
#the first number should always be greater than the second - because of overlaps.
print 'Total number of events passing filters / total number passing RPVLL: ',closureTest-eventList[15],'/',TotalRPVLLPass,'. Rate: ',float(closureTest-eventList[15])/float(TotalRPVLLPass)*100.,'%.'

#printout for easy copy-paste into google spreasheet:
for n in filterNames:
	print n,
print ''
for n in filterEvents:
	print n,

