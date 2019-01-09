from MadGraphControl.MadGraphUtils import *
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

joparts = runArgs.jobConfig[0].rstrip('.py').split('_')
for q in squarks: masses[q] = float(joparts[4])
masses['1000022'] = float(joparts[5])
if masses['1000022']<0.5: masses['1000022']=0.5
gentype = joparts[2]
decaytype = '_'.join([joparts[3],joparts[6],joparts[7]]) 
short_decaytype = joparts[3]
lambda_coupling = joparts[6]
neutralino_ctau = float(joparts[7].replace('p','.')) 
neutralino_width = 3.*1e11*6.581e-25/neutralino_ctau
if lambda_coupling == "lamp211":
    decays = {'1000022':"""DECAY   1000022  """   +str(neutralino_width)+  """ # neutralino decay
    ##           BR         NDA      ID1       ID2       ID3
    2.5000000E-01    3     13        2         -1   # BR(~chi_10 -> mu udbar)
    2.5000000E-01    3    -13       -2          1   # BR(~chi_10 -> mubar ubard)
    2.5000000E-01    3     14        1         -1   # BR(~chi_10 -> nu ddbar)
    2.5000000E-01    3    -14       -1          1   # BR(~chi_10 -> nubar dbard) """}
elif lambda_coupling == "lamp111":
    decays = {'1000022':"""DECAY   1000022  """   +str(neutralino_width)+  """ # neutralino decay
    ##           BR         NDA      ID1       ID2       ID3
    2.5000000E-01    3     11        2         -1   # BR(~chi_10 -> e udbar)
    2.5000000E-01    3    -11       -2          1   # BR(~chi_10 -> ebar ubard)
    2.5000000E-01    3     12        1         -1   # BR(~chi_10 -> nu ddbar)
    2.5000000E-01    3    -12       -1          1   # BR(~chi_10 -> nubar dbard) """}
elif lambda_coupling == "lamp223":
    decays = {'1000022':"""DECAY   1000022  """   +str(neutralino_width)+  """ # neutralino decay
    ##           BR         NDA      ID1       ID2       ID3
    2.5000000E-01    3     13        4         -5   # BR(~chi_10 -> mu cbbar)
    2.5000000E-01    3    -13       -4          5   # BR(~chi_10 -> mubar cbarb)
    2.5000000E-01    3     14        4         -5   # BR(~chi_10 -> nu bbarb)
    2.5000000E-01    3    -14       -4          5   # BR(~chi_10 -> nubar bbbar) """}
elif lambda_coupling == "lamp123":
    decays = {'1000022':"""DECAY   1000022  """   +str(neutralino_width)+  """ # neutralino decay
    ##           BR         NDA      ID1       ID2       ID3
    2.5000000E-01    3     11        4         -5   # BR(~chi_10 -> e cbbar)
    2.5000000E-01    3    -11       -4          5   # BR(~chi_10 -> ebar cbarb)
    2.5000000E-01    3     12        4         -5   # BR(~chi_10 -> nu bbarb)
    2.5000000E-01    3    -12       -4          5   # BR(~chi_10 -> nubar bbbar) """}
elif lambda_coupling == "lamp213":
    decays = {'1000022':"""DECAY   1000022  """   +str(neutralino_width)+  """ # neutralino decay
    ##           BR         NDA      ID1       ID2       ID3
    2.5000000E-01    3     13        2         -5   # BR(~chi_10 -> mu ubbar)
    2.5000000E-01    3    -13       -2          5   # BR(~chi_10 -> mubar ubarb)
    2.5000000E-01    3     14        1         -5   # BR(~chi_10 -> nu dbarb)
    2.5000000E-01    3    -14       -1          5   # BR(~chi_10 -> nubar dbbar) """}    
elif lambda_coupling == "lamp113":
    decays = {'1000022':"""DECAY   1000022  """   +str(neutralino_width)+  """ # neutralino decay
    ##           BR         NDA      ID1       ID2       ID3
    2.5000000E-01    3     11        2         -5   # BR(~chi_10 -> e ubbar)
    2.5000000E-01    3    -11       -2          5   # BR(~chi_10 -> ebar ubarb)
    2.5000000E-01    3     12        1         -5   # BR(~chi_10 -> nu dbarb)
    2.5000000E-01    3    -12       -1          5   # BR(~chi_10 -> nubar dbbar) """}
elif lambda_coupling == "lam12k":
    decays = {'1000022':"""DECAY   1000022  """   +str(neutralino_width)+  """ # neutralino decay
    ##           BR         NDA      ID1       ID2       ID3
    8.333333E-02    3      12       -13        11   # BR(~chi_10 -> nue mubar e)
    8.333333E-02    3     -12        13       -11   # BR(~chi_10 -> nuebar mu ebar)
    8.333333E-02    3      14       -13        11   # BR(~chi_10 -> numu mubar e)
    8.333333E-02    3     -14        13       -11   # BR(~chi_10 -> numubar mu ebar)
    1.666667E-01    3      14       -11        11   # BR(~chi_10 -> numu ebar e)
    1.666667E-01    3     -14        11       -11   # BR(~chi_10 -> numubar e ebar)
    1.666667E-01    3      12       -13        13   # BR(~chi_10 -> nue mubar mu)
    1.666667E-01    3     -12        13       -13   # BR(~chi_10 -> nuebar mu mubar) """}
process = '''
generate p p > susylq susylq~ $ go susyweak @1
add process p p > susylq susylq~ j $ go susyweak @2
add process p p > susylq susylq~ j j $ go susyweak @3
'''
njets = 2
evgenLog.info('Registered generation of squark grid '+str(runArgs.runNumber))

build_param_card(param_card_old='param_card.SM.SS.'+short_decaytype+'.dat', param_card_new='param_card.SM.SS.'+decaytype+'.dat',      masses=masses,decays=decays)

evgenConfig.contact  = [ "giovanna.cottin@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel', 'squark']
evgenConfig.description = 'squark production, sq->q+LSP, RPV scenarios with displaced neutralino decay in simplified model, m_sq = %s GeV, m_N1 = %s GeV, ctau(N1) = %s mm'%(masses[squarks[0]],masses['1000022'],neutralino_ctau)
evgenConfig.inputfilecheck = "SS_direct_"+str(masses[squarks[0]]) # Same production process as the SS_direct grid (RPC with sq -> q LSP)

randomSeed_original = 1234
if hasattr(runArgs, "randomSeed"):
    randomSeed_original = runArgs.randomSeed
# Giving a unique seed number (not exceeding the limit of ~30081^2)
runArgs.randomSeed = 1000 * int(str(runArgs.runNumber)[1:6]) + randomSeed_original
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )
runArgs.randomSeed = randomSeed_original

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{ul,1000002}{ul~,-1000002}{ur,2000002}{ur~,-2000002}{dl,1000001}{dl~,-1000001}{dr,2000001}{dr~,-2000001}{sl,1000003}{sl~,-1000003}{sr,2000003}{sr~,-2000003}{cl,1000004}{cl~,-1000004}{cr,2000004}{cr~,-2000004}"]

# Turn off checks for displaced vertices. 
# Other checks are fine.
testSeq.TestHepMC.MaxVtxDisp = 1000*1000 #In mm
testSeq.TestHepMC.MaxTransVtxDisp = 1000*1000
