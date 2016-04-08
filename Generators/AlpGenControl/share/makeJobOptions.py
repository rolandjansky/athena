#!/bin/env python

def GetSampleLongName( run_num ):
    HorP='Herwig'
    stringy = ''
    Inclusive = ['Incl','Excl']
    events_athena=5000

    # W+jets samples obo L. Mijovic
    if run_num>=147001 and run_num<=147048:
        jets = [(run_num-147001)%8,1] # Exclusive matching to the last number of partons
        if jets[0]>5: jets[1]=0
        if jets[0]==7: jets[0]=5

        if jets[0]==4: events_athena=750
        if jets[0]==5: events_athena=200 
        if jets[0]==6: events_athena=50
        if jets[0]==7: events_athena=100

        if (run_num-147001)>=24: HorP='Pythia' # Move to Pythia for half the samples

        W_strings = ['enu','munu','taunu']
        W_mode = W_strings[ int((run_num-147001)/8)%3 ]

        stringy = 'AutoAlpgen'+HorP+'_W'+W_mode+'_Np'+str(jets[0])+Inclusive[jets[1]]

    # TTbar samples obo J. Ferrando
    if run_num>=147049 and run_num<=147072:
        jets = [(run_num-147049)%6,1] # Exclusive matching for Np0-Np4
        if jets[0]==5: jets[1]=0 # Inclusive matching for Np5

        if jets[0]==2: events_athena=2500
        elif jets[0]>2: events_athena=500

        if run_num-147049>=12: HorP='Pythia' # Move to Pythia for half the samples
        if (run_num-147049)%12<6:
            decay = 'lnulnu'
        else:
            decay = 'qqlnu'
        stringy = 'AutoAlpgen'+HorP+'_ttbarNp'+str(jets[0])+Inclusive[jets[1]]+'_'+decay

    # DY Filtered sample obo O. Rosenthal and B. Di Micco
    if run_num>=146830 and run_num<=146886:
        jets = [(run_num-146830)%10,1]
        if jets[0]==5: jets[1]=0
        if jets[0]==6: jets=[5,1]
        elif jets[0]==7: jets=[6,1]
        elif jets[0]>7:  jets=[5,1]

        if jets[0]==2: events_athena=1000
        elif jets[0]==3: events_athena=200
        elif jets[0]==4: events_athena=10
        elif jets[0]==5: events_athena=50
        if (run_num>=146880 and run_num<=146886):
           if jets[0]==0: events_athena=10
           elif jets[0]==1: events_athena=1000
           elif jets[0]==2: events_athena=10
           elif jets[0]==3: events_athena=20
        elif run_num>=146830 and run_num<=146856:
            if jets[0]==2: events_athena=5000
            elif jets[0]==3: events_athena=500
            elif jets[0]==4: events_athena=100
            elif jets[0]==5: events_athena=20

        HorP='AlpgenJimmy_Auto_AUET2CTEQ6L1'
        if run_num>=146860: HorP = 'AlpgenJimmy_Auto_AUET2CTEQ6L1_Filtered' 
        decays = ['ee','mumu','tautau']
        decay = decays[ int((run_num-146830)/10)%3 ]
        stringy = HorP+'_Z'+decay+'Np'+str(jets[0])+Inclusive[jets[1]]+'_Mll10to60'

    # Z+jets samples obo O. Rosenthal and B. Di Micco
    if run_num>=147073 and run_num<=147136:
        jets = [(run_num-147073)%8,1] # Exclusive matching to the last number of partons
        if jets[0]==5: jets[1]=0
        if jets[0]==6: jets=[5,1]
        elif jets[0]==7: jets=[6,1]
        elif jets[0]>7:  jets=[5,1]

        if jets[0]==4: events_athena=2000
        if jets[0]==5: events_athena=20

        HorP='AlpgenPythia_Auto_AUET2CTEQ6L1'
        if (run_num-147073)>=32: HorP='AlpgenJimmy_Auto_AUET2CTEQ6L1'
        decays = [ 'ee','mumu','tautau','nunu' ]
        decay = decays[ int((run_num-147073)/8)%4 ]
        stringy = HorP+'_Z'+decay+'Np'+str(jets[0])+Inclusive[jets[1]]

    return (stringy,events_athena)

import sys
if not 'runs' in dir():
    print 'Usage:  athena.py -c "runs=[a,b]" AlpGenControl/makeJobOptions.py'
    print '    a and b are the two end runs (job options will be generated for them)'
    sys.exit()

for run in xrange(runs[0],runs[1]+1):
    (nom,evts) = GetSampleLongName( run )
    if nom is '':
        print 'No run',run,'found.'
        continue
    fout = open( 'MC12.'+str(run)+'.'+nom+'.py' , 'w' )
    fout.write( 'include(\'AlpGenControl/MC.GenAlpGen.py\') \n' )
    fout.write( 'evgenConfig.minevents = %i \n'%(evts) )
    fout.close()

theApp.exit()
