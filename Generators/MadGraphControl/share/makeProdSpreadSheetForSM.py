#!/bin/env python

def getNeventsPriority( a ):
    if 'SM_SG_' not in a:
        light=0

        offset = 0
        if '_qup' in a or '_qdown' in a or '_rad' in a: offset=1

        if 'direct' in a:
            if int(a.split('_')[-2-offset])<400: light=1
        elif int(a.split('_')[-3-offset])<400: light=1
        lowms=0
        if 'direct' in a:
            if int(a.split('_')[-2-offset])-int(a.split('_')[-1-offset])<200: lowms=1
        elif int(a.split('_')[-3-offset])-int(a.split('_')[-1-offset])<200: lowms=1

        if 'SS_direct' in a: return [ 10000+5000*lowms+5000*light , 2 ]
        if 'GG_direct' in a: return [ 10000+5000*lowms+5000*light , 2 ]

        # "X-grids"
        if (148302<int(a.split('.')[0]) and 148589>int(a.split('.')[0]) or\
            152776<int(a.split('.')[0]) and 152858>int(a.split('.')[0])):
            if 'GG_onestep' in a: return [ 60000 , 1 ]
            if 'SS_onestep' in a: return [ 30000 , 1 ]
        else:
            if 'GG_onestep' in a and not 'CN' in a: return [ 30000 , 2 ]
            if 'SS_onestep' in a and not 'CN' in a: return [ 60000 , 2 ]
            if 'GG_onestep' in a and 'CN' in a: return [ 10000+5000*lowms+5000*light , 2 ]
            if 'SS_onestep' in a and 'CN' in a: return [ 10000+5000*lowms+5000*light , 2 ]

        if 'SS_twostep' in a: return [ 20000 , 2 ]
        if 'GG_twostep' in a: return [ 20000 , 2 ]

    if 'SM_SG_' in a:
        mGluino = int(a.split('_')[-2])
        mSquark = int(a.split('_')[-3])
        mLSP    = int(a.split('_')[-1])
        mcstats = { }

        deltam=1000
        for proc in ['SS','SSB','SG','GG']:
            if proc=='SS' or proc=='SSB':
                deltam = mSquark-mLSP
            elif proc == 'GG':
                deltam = mGluino-mLSP
            elif proc == 'SG':
                deltam = (mSquark+mGluino-2*mLSP)/2

            if deltam>800:
                mcstats[proc] = 2500
            elif deltam>500:
                mcstats[proc] = 5000
            else:
                mcstats[proc] = 10000

        if a.split('_')[2]=='ALL' : return [ mcstats['SS']+mcstats['SSB']+mcstats['SG']+mcstats['GG'] , 3 ]
        else : return [ mcstats[a.split('_')[2]] , 3 ]

    # Default: bad run, return junk
    return [-1,4]
"""

Slepton grids will be 20k per point (one at priority 1? the remainder priority 2)

"""

import os

if not 'listOfRuns' in dir():
    listOfRuns = [164656,164891]
if not 'campaign' in dir():
    campaign = 'MC12'
if not 'ecm' in dir():
    if campaign == 'MC12': ecm = 8000
    else: ecm = 7000
if not 'prodTag' in dir():
    if campaign == 'MC12': prodTag = '17.6.0.2'
    else: prodTag = '16.6.8.5-Prod'
print 'Compiling job options for runs in',listOfRuns

f = open(os.getcwd()+'/prodSpreadSheet.txt','w')
f.write('brief\tdatasetNumber\tESD,RDO,DESD\tJobOptions\tEvent (Fullsim)\tEvent (ATLFAST-II)\tPriority\tEvgen\tSimul\tMerge\tDigi\tReco\tRec Merge\tRec Tag\tAtlfast\tAtlfast Merge\tAtlf TAG\tcross-section\tfiltering efficiency\tNLO cross-section\tGenerator\tECM\tEF\tcomment\tResponse\tTrigger\tFrozen shower\tFiltered pileup truth\n')

f2 = open(os.getcwd()+'/evgeninputfiles.csv','w')
f2.write('# DSID, energy, inputeventfile, (inputconffile)\n')

from MadGraphControl.SetupMadGraphEventsForSM import setupMadGraphEventsForSM
for run in xrange(listOfRuns[0],listOfRuns[1]):
    partialString = setupMadGraphEventsForSM(run_madgraph=False,runNumber=run)
    if partialString is None or partialString=='': continue
    #[Nevents,Priority]=getNeventsPriority( partialString )

    #if Nevents<0: continue

    # hack configuration for grid 5
    if run >= 148152 and run <= 148302:
        string = setupMadGraphEventsForSM(False,run)
        lsp = int(string.split("_")[-1])
        gluino = int(string.split("_")[-3])
        c = lsp - gluino
        if c == -80:
            Nevents = 80000
        if c == -25:
            Nevents = 80000
        if c == -50:
            continue
        if c < -80:
            Nevents = 20000

    # hack configuration for twostep slepton grids
    if run >= 153044 and run <= 153583:
        string = setupMadGraphEventsForSM(False,run)
        lsp = int(string.split("_")[-1])
        gluino = int(string.split("_")[-3])
        c = lsp - gluino
        if c == -80:
            Nevents = 40000
        if c == -25:
            Nevents = 40000
        if c == -50:
            continue
        if c < -80:
            Nevents = 10000


    # configuration for direct sbottom grid
    if run>=164656 and run<164891:
        Priority = 1
        string = setupMadGraphEventsForSM(False,run)
        m_LSP = int(string.split("_")[-2])
        m_sbottom = int(string.split("_")[-3])
        deltaM = m_sbottom - m_LSP
        if m_sbottom<=250:                                                                                                                                    
            if deltaM<=50:
                Nevents = 100000
            else:
                Nevents = 50000
        elif m_sbottom<=350:
            Nevents = 50000
        elif m_sbottom<=400:
            Nevents = 30000
        elif m_sbottom==450:
            Nevents = 15000   

    if run>152646 and run<152647+261:
        f.write(partialString.split('.')[1]+'\t'+str(run)+'\tNone\t'+campaign.rstrip('abc')+'.'+partialString+'.py\t0\t'+str(Nevents)+'\t'+str(Priority)+'\t'+campaign+'\t-\t-\t-\t-\t-\t-\t-\t-\t-\t-\t100%\t-\tMadGraph+Pythia\t'+str(ecm)+'\t-\t'+prodTag+', NTUP_TRUTH\n')
    else:
        f.write(partialString.split('.')[1]+'\t'+str(run)+'\tAOD\t'+campaign.rstrip('abc')+'.'+partialString+'.py\t0\t'+str(Nevents)+'\t'+str(Priority)+'\t'+campaign+'\t'+campaign+'\t'+campaign+'\t'+campaign+'\t-\t-\t-\t'+campaign+'\t'+campaign+'\t'+campaign+'\t-\t100%\t-\tMadGraph+Pythia\t'+str(ecm)+'\t-\t'+prodTag+' \n')

    f2.write('{0}, {1},\n'.format(run,ecm))




f.close()
f2.close()
theApp.exit()
