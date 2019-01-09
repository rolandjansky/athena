


mass_points = ["0.5","1","2","5","10","15","20","25","30","35","40","45","50","55","60"] #mZd in GeV. maps onto 302073 - 302087

if runArgs.runNumber>=302073 and runArgs.runNumber<=302087:
   mzd = mass_points[runArgs.runNumber-302073];
   evgenConfig.description="MadGraph Hidden Abelian Higgs Model (HAHM): gg -> H -> 4l (l=e,mu) , with mZd=%sGeV" % mzd
   proc_card = """
import model HAHM_variableMW_v3_UFO
define l+ = e+ mu+
define l- = e- mu-
generate g g > h HIG=1 HIW=0 QED=0 QCD=0, (h > Zp Zp, Zp > l+ l-)"""

elif runArgs.runNumber>=302088 and runArgs.runNumber<=302090:
   if runArgs.runNumber==302088: mzd="0.5"
   elif runArgs.runNumber==302089: mzd="20"
   elif runArgs.runNumber==302090: mzd="50"
   evgenConfig.description="MadGraph Hidden Abelian Higgs Model (HAHM): gg -> H -> 4l (l=e,mu,tau) , with mZd=%sGeV" % mzd
   proc_card = """
import model HAHM_variableMW_v3_UFO
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
generate g g > h HIG=1 HIW=0 QED=0 QCD=0, (h > Zp Zp, Zp > l+ l-)"""
else:
   raise RuntimeError("Unrecognised runNumber: %d" % runArgs.runNumber)


proc_name = "HAHM_ggf_ZdZd_4l_mZd%s" % mzd


#modifications to the param_card.dat (generated from the proc_card i.e. the specific model)
#if you want to see the resulting param_card, run Generate_tf with this jobo, and look at the param_card.dat in the cwd
#If you want to see the auto-calculated values of the decay widths, look at the one in <proc_name>/Cards/param_card.dat (again, after running a Generate_tf)
param_card_extras = { "HIDDEN": { 'epsilon': '1e-4', #kinetic mixing parameter
                                 'kap': '1e-4', #higgs mixing parameter
                                 'mzdinput': mzd, #Zd mass
                                 'mhsinput':'200.0' }, #dark higgs mass
                     "HIGGS": { 'mhinput':'125.0'}, #higgs mass
                     "DECAY": { 'wzp':'Auto', 'wh':'Auto', 'wt':'Auto' } #auto-calculate decay widths and BR of Zp, H, t
                  }


run_card_extras = { 'lhe_version':'2.0',
                   'cut_decays':'F',
                   'ptj':'0',
                   'ptb':'0',
                   'pta':'0',
                   'ptl':'0',
                   'etaj':'-1',
                   'etab':'-1',
                   'etaa':'-1',
                   'etal':'-1',
                   'drjj':'0',
                   'drbb':'0',
                   'drll':'0',
                   'draa':'0',
                   'drbj':'0',
                   'draj':'0',
                   'drjl':'0',
                   'drab':'0',
                   'drbl':'0',
                   'dral':'0' }



evgenConfig.keywords+=['exotic','BSMHiggs']
evgenConfig.contact = ['will@cern.ch']
evgenConfig.process = "HAHM_H_ZdZd_4l"

include("MadGraphControl/MadGraphControl_Pythia8_A14_NNPDF23LO_EvtGen_Common.py")






