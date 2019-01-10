include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

fields = runArgs.jobConfig[0].replace(".py","").split("_")

gentype = "TT" #use same param_card for TT and higgsino production
realgentype = fields[2]
decaytype = fields[3]

if realgentype == 'C1N1' :
    process = '''
    define c1 = x1+ x1-
    generate    p p > c1 n1     $ susystrong
    add process p p > c1 n1 j   $ susystrong
    add process p p > c1 n1 j j $ susystrong
    '''
    njets = 2
    masses['1000006'] = float(fields[4]) #t1, we use gentype "TT" and the matching scale is read as m_t1/4
    masses['1000022'] = float(fields[4]) #N1
    masses['1000023'] = float(fields[4]) #N2
    masses['1000024'] = float(fields[4]) #C1
elif realgentype == 'N1N2' :
    process = '''
    generate    p p > n1 n2     $ susystrong
    add process p p > n1 n2 j   $ susystrong
    add process p p > n1 n2 j j $ susystrong
    '''
    njets = 2
    masses['1000006'] = float(fields[4]) #t1, we use gentype "TT" and the matching scale is read as m_t1/4
    masses['1000022'] = float(fields[4]) #N1
    masses['1000023'] = float(fields[4]) #N2
    masses['1000024'] = float(fields[4]) #C1
elif realgentype == 'TT' :
    process = '''
    generate    p p > t1 t1~     $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @1
    add process p p > t1 t1~ j   $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @2
    add process p p > t1 t1~ j j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @3
    '''
    njets = 2 
    masses['1000006'] = float(fields[4]) #t1
    masses['1000022'] = float(fields[5]) #N1
    masses['1000023'] = float(fields[5]) #N2
    masses['1000024'] = float(fields[5]) #C1
else:
    raise RunTimeError("ERROR: Do not understand gentype: TT, C1N1 or N1N2?")


evgenConfig.contact  = ["javier.montejo@cern.ch"]
evgenConfig.keywords += [ 'SUSY', 'RPV', 'stop','Higgsino']
if realgentype == 'C1N1' :
    evgenConfig.description = 'chargino-neutralino production and decay via RPV lampp332 coupling, m_N1 = %s GeV, m_C1 = %s GeV, m_N2 = %s GeV'%(masses['1000022'],masses['1000024'],masses['1000023'])
elif realgentype == 'N1N2' :
    evgenConfig.description = 'neutralino1-neutralino2 production and decay via RPV lampp332 coupling, m_N1 = %s GeV, m_C1 = %s GeV, m_N2 = %s GeV'%(masses['1000022'],masses['1000024'],masses['1000023'])
elif realgentype == 'TT' :
    evgenConfig.description = 'stop pair production and decay to chargino/neutralino, which decay via RPV lampp332 coupling, m_stop = %s GeV, m_N1 = %s GeV, m_C1 = %s GeV, m_N2 = %s GeV'%(masses['1000006'],masses['1000022'],masses['1000024'],masses['1000023'])


if '1L20' in runArgs.jobConfig[0].split('_')[-1]:
    evgenLog.info('1Lepton20 filter is applied')
    include ( 'GeneratorFilters/MultiElecMuTauFilter.py' )
    filtSeq.MultiElecMuTauFilter.MinPt  = 20000.
    filtSeq.MultiElecMuTauFilter.MaxEta = 2.8
    filtSeq.MultiElecMuTauFilter.NLeptons = 1
    filtSeq.MultiElecMuTauFilter.IncludeHadTaus = 0
    filtSeq.Expression = "MultiElecMuTauFilter"
    evt_multiplier = 5

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    if realgentype == 'C1N1' :
        genSeq.Pythia8.Commands += ["Merging:Process = pp>{x1+,1000024}{x1-,-1000024}{n1,1000022}", 
                                    "1000024:spinType = 1",
                                    "1000022:spinType = 1",
                                    "1000023:spinType = 1" ]
    elif realgentype == 'N1N2' :
        genSeq.Pythia8.Commands += ["Merging:Process = pp>{n1,1000022}{n2,1000024}",
                                    "1000024:spinType = 1",
                                    "1000022:spinType = 1",
                                    "1000023:spinType = 1" ]
    elif realgentype == 'TT' :
        genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,-1000006}"]
