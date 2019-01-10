include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )
 
JOName = os.path.basename(runArgs.jobConfig[0])
 
gentype = JOName.split('_')[2]
decaytype = JOName.split('_')[3]
 
masses['1000006'] = float(JOName.split('_')[4])
masses['1000022'] = float(JOName.split('_')[5])
if masses['1000022']<0.5: masses['1000022']=0.5
 
process = '''
generate p p > t1 t1~ $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @1
add process p p > t1 t1~ j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @2
add process p p > t1 t1~ j j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @2
'''
    
njets = 2

extras['event_norm']='sum'
extras['use_syst']='F'
extras['pdgs_for_merging_cut']='1, 2, 3, 4, 21'
    
#extras['drjl']="0.5"
#extras['asrwgtflavor']="5"
#extras['auto_ptj_mjj']="F" 

if 'MET' in JOName.split('_')[-1]:
    include ( 'GeneratorFilters/MissingEtFilter.py' )

    metFilter = JOName.split('_')[-1]
    metFilter = int(metFilter.split("MET")[1].split(".")[0])
   
    print "Using MET Filter: " + str(metFilter)
    filtSeq.MissingEtFilter.METCut = metFilter*GeV
    evt_multiplier = metFilter / 10

else:
    print "No MET Filter applied"
               
       
evgenLog.info('Registered generation of stop pair production, stop to c+LSP; grid point '+str(runArgs.runNumber)+' decoded into mass point mstop=' + str(masses['1000006']) + ', mlsp='+str(masses['1000022']))
 
evgenConfig.contact  = [ "jan.schaeffer@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel','charm']
evgenConfig.description = 'stop direct pair production, st->c+LSP in simplified model'

# evgenConfig.minevents = 2000
 
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )
 
if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = guess"]
    if "UserHooks" in genSeq.Pythia8.__slots__.keys():
        genSeq.Pythia8.UserHooks += ['JetMergingaMCatNLO']
    else:
        genSeq.Pythia8.UserHook = 'JetMergingaMCatNLO'
    #genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,-1000006}"]
