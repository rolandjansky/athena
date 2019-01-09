from MadGraphControl.MadGraphUtils import *
import math


nameModel=None
nameZp=None
nameMassParamZp=None
nameWidthParamZp=None
nameMassParamhD=None
nameWidthParamhD=None
nameMassParamN2=None
nameWidthParamN2=None
codeZp=None
mg_proc=None
process=None


if model=='LightVector':
    nameZp='vx'
    nameMassParamZp='MVdark'
    nameWidthParamZp='WVdark'
    nameMassParamN2='MX2'
    nameWidthParamN2='Wchip'
    nameModel=model
    codeZp=56
elif model=='InelasticVectorEFT':
    nameZp='zp'
    nameMassParamZp='MZp'
    nameWidthParamZp='MZp'
    nameMassParamN2='Mn2'
    nameWidthParamN2='Wn2'
    nameModel=model
    codeZp=57
elif model=='darkHiggs':
    nameZp='zp'
    nameMassParamZp='MZp'
    nameWidthParamZp='MZp'
    nameMassParamhD='MHD'
    nameWidthParamhD='WhD'
    nameModel=model
    codeZp=56
else:
    raise RuntimeError("Unknown model.")

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")


if model=='LightVector' or model=='InelasticVectorEFT':
    mg_proc=["define j = j b b~",
             "define p = p b b~",
             "generate p p > %s n1 n1, %s > j j"%(nameZp,nameZp)]
    process='pp>(%s>jj)n1n1'%nameZp
elif model=='darkHiggs':
    mg_proc=["define j = j b b~",
             "define p = p b b~",
             "generate p p > hd %s, hd > n1 n1, %s > j j"%(nameZp,nameZp)]
    process='pp>(hd>n1n1)(%s>jj)'%nameZp

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model %s

%s

output -f
"""%(nameModel,'\n'.join(mg_proc)))
fcard.close()

process_dir = new_process()

extras = {
    }
extras['drjj']=0.

build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               xqcut=0,
               nevts=int(runArgs.maxEvents*2./filteff),
               rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)
if model=='LightVector' or model=='InelasticVectorEFT':
    build_param_card(param_card_old='MadGraph_param_card_%s.dat'%nameModel,param_card_new='param_card.dat',
                     masses={'1000022':mDM1,
                             '1000023':mDM2,
                             str(codeZp):mZp},
                     decays={str(codeZp):'DECAY %d %f # W%s'%(codeZp,widthZp,nameWidthParamZp), '1000023':'DECAY %d %f # W%s'%(1000023,widthN2,nameWidthParamN2)}
                     )

elif model=='darkHiggs':
    build_param_card(param_card_old='MadGraph_param_card_%s.dat'%nameModel,param_card_new='param_card.dat',
                     masses={'1000022':mDM1,
                             str(codeZp):mZp,
                             '26':mHD},
                     decays={str(codeZp):'DECAY %d %f # W%s'%(codeZp,widthZp,nameWidthParamZp), '26':'DECAY %d %f # W%s'%(26,widthhd,nameWidthParamhD)}
                     )



print_cards()

runName='run_01'

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name=runName,proc_dir=process_dir)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')

evgenConfig.description = "Mono Zprime sample - model %s"%nameModel
evgenConfig.keywords = ["exotic","BSM","WIMP"]
evgenConfig.process = process
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'
evgenConfig.contact = ["Kevin Bauer <kevin.thomas.bauer@cern.ch>"]

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

genSeq.Pythia8.Commands += ["1000022:all = chid chid~ 2 0 0 %d 0" %(mDM1),
                            "1000022:isVisible = false"]



include("GeneratorFilters/MissingEtFilter.py")
filtSeq.MissingEtFilter.METCut = 50*GeV
