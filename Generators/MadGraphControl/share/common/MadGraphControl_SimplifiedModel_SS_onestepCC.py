include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

def LHE_Filter( runArgs ):
    # Get the list of LHE files
    # Largely taken from code in the generate skeleton
    import os,glob
    files = []
    for afile in runArgs.inputGeneratorFile.split(','):
        if ".tar" in runArgs.inputGeneratorFile:
            inputroot = os.path.basename(afile).split(".tar.")[0]
        else:
            input0 = os.path.basename(afile).split("._")[0]
            input1 = (os.path.basename(afile).split("._")[1]).split(".")[0]
            inputroot = input0+"._"+input1
        # Look for the event file
        realEventsFile = glob.glob('*%s.*ev*ts' % inputroot)
        if len(realEventsFile)!=1:
            raise RuntimeError('Multiple files matching '+('*%s.*ev*ts'%inputroot)+' found')
        files += realEventsFile

    # Loop through all of the input LHE files
    for genFile in files:
        # Move the old file out of the way
        os.rename(genFile, genFile+'_OLD')
        # Set up a new file, read the old file
        old_file = open( genFile+'_OLD' , 'r' )
        new_file = open( genFile , 'w' )
        event = ''
        keepEvent = True
        # Run through the old file and populate the new
        print("Starting to read file:")
        with open( genFile+'_OLD' , 'r' ) as old_file:
            for l in old_file:
                # If we aren't in an event, then populate the new
                if event=='' and not '<event>' in l:
                    new_file.write(l)
                # Check for the start of a new event
                elif '<event>' in l:
                    event += l
                # Check for the end of an event
                elif '</event>' in l:
                    event += l
                    if keepEvent: new_file.write( event )
                    event = ''
                    keepEvent = True
                # If we are in the midst of an event, add and check for a keeper
                elif event!='':
                    event += l
                    # File has a very special format -- lines we want are 13 numbers in a row
                    if not '<' in l and not '>' in l and len(l.split('#')[0].split())==13:
                        # Look for the PDG ID; should be the first field
                        pid = int(l.split('#')[0].split()[0].strip())
                        if 2000001<=pid<=2000006: keepEvent=False
        # Done with looping over file; close up

        old_file.close()
        new_file.close()
        # Clean up
        os.remove( genFile+'_OLD' )

    # Done with the LHE filter

LHE_Filter(runArgs)

for q in squarksl: masses[q] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000024'] = float(runArgs.jobConfig[0].split('_')[5])
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[6].split('.')[0])
if masses['1000022']<0.5: masses['1000022']=0.5
genSeq.Pythia8.Commands += ["24:mMin = 0.2"]
genSeq.Pythia8.Commands += ["23:mMin = 0.2"]


gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
process = '''
define susylqL = ul dl cl sl
define susylqL~ = ul~ dl~ cl~ sl~
define susylqR = ur dr cr sr
define susylqR~ = ur~ dr~ cr~ sr~
generate p p > susylqL susylqL~ $ go susyweak susylqR susylqR~ @1
add process p p > susylqL susylqL~ j $ go susyweak susylqR susylqR~ @2
add process p p > susylqL susylqL~ j j $ go susyweak susylqR susylqR~ @3
'''

njets = 2
evt_multiplier = 4
evgenLog.info('Registered generation of squark grid '+str(runArgs.runNumber))



evgenConfig.contact  = [ "ljiljana.morvaj@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel', 'squark']
evgenConfig.description = 'squark production in simplified model, one-step decays through chargino, m_sq = %s GeV, m_C1 = %s GeV, m_N1 = %s GeV'%(masses[squarks[0]],masses['1000024'],masses['1000022'])



include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )
print("Mephisto!")

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = guess"]
    if "UserHooks" in genSeq.Pythia8.__slots__.keys():
      genSeq.Pythia8.UserHooks += ['JetMergingaMCatNLO']
    else:
      genSeq.Pythia8.UserHook = 'JetMergingaMCatNLO' 
