include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

fields = runArgs.jobConfig[0].replace(".py","").split("_")

gentype = fields[2]
decaytype = fields[3]

if gentype == 'DD' :
    process = '''
    import model RPVMSSM_UFO
    generate p p > dr dr QCD=2 QED=0 @1
    add process p p > dr dr j QCD=3 QED=0 @2
    add process p p > dr dr j j QCD=4 QED=0 @3
    '''
    njets = 2
    masses['1000021'] = float(fields[4]) #go
    masses['2000001'] = float(fields[5]) #sd4 (dr) 
elif gentype == 'GG' :
    process = '''
    import model RPVMSSM_UFO
    generate p p > go go $ susysq susysq~ @1
    add process p p > go go j $ susysq susysq~ @2
    '''
    njets = 1
    masses['1000021'] = float(fields[4]) #go
    masses['1000002'] = float(fields[5]) #su1 (t1)
    masses['1000004'] = float(fields[5]) #su2 (t2)
    if  masses['1000021'] < (masses['1000002']+175) :
        decays = {'1000021':"""DECAY  1000021   1.000000e+00 # Wgo
##  BR         NDA      ID1       ID2
   5.000000e-01   3    1  5  6 # BR( go -> t d b )
   5.000000e-01   3    -6  -5  -1 #  BR( go -> t~ d~ b~ )"""}
    elif masses['1000021'] > (masses['1000002']+175) :
        decays['1000021'] = """DECAY  1000021   1.000000e+00 # Wgo
## BR         NDA      ID1       ID2
   5.000000e-01   2    -6  1000002 # BR( go -> t~ t1 )
   5.000000e-01   2    -1000002  6 # BR( go -> t t1~ )"""
        decays['1000002'] = """DECAY  1000002   1.000000e+00 # Wt1
## BR         NDA      ID1       ID2
   1.000000e+00   2    -5  -1 #  BR( t1 -> d~ b~ )"""        
else:
    raise RunTimeError("ERROR: Do not understand gentype: DD or GG?")


evgenConfig.contact  = ["simon.paul.berlendis@cern.ch"]
evgenConfig.keywords += [ 'SUSY', 'RPV', 'top' ]

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if gentype == 'DD' :
    evgenConfig.description = 'd-squark pair production and decay to top pair and extra parton via RPV lampp331 coupling, m_glu = %s GeV, m_dsquark = %s GeV'%(masses['1000021'],masses['2000001'])
elif gentype == 'GG' :
    evgenConfig.description = 'gluino pair production and decay to top pair and extra parton via RPV lampp331 coupling, m_glu = %s GeV, m_stop = %s GeV'%(masses['1000021'],masses['1000002'])

if njets>0:
    if gentype == 'DD' :
        genSeq.Pythia8.Commands += ["Merging:Process = pp>{dr,2000001}{dr,2000001}"]
    elif gentype == 'GG' :
        genSeq.Pythia8.Commands += ["Merging:Process = pp>{go,1000021}{go,1000021}"]
