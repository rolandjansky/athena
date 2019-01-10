from MadGraphControl.MadGraphUtils import *
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

top_mass = 173. # GeV

def get_widths(lambda_coupling, coupling_value, neutralino_mass, intermediate_mass):
  if not (lambda_coupling == 'lamp231' and coupling_value == 1.):
    raise NotImplementedError('I do not know what to do for coupling %s at value %f.' % (lambda_coupling, coupling_value))
  width_dict = {  #GeV
	'1_1':		[0.0,0.0],
	'0_250':	[0.0,4.304781e+00],
	'0_500':	[0.0,2.362933e+01],
	'0_800':	[0.0,4.419344e+01],
	'0_1200':	[0.0,6.988584e+01],
	'0_1500':	[0.0,8.867416e+01],
	'0_1750':	[0.0,1.041819e+02],
	'0_2000':	[0.0,1.195659e+02],
	'0_2250':	[0.0,1.349769e+02],
	'0_2500':	[0.0,1.503077e+02],
	'0_2750':	[0.0,1.656101e+02],
	'0_3000':	[0.0,1.808911e+02],
	'0_3250':	[0.0,1.961556e+02],
	'0_3500':	[0.0,2.114072e+02],
	'50_50':	[0.0,0.0],
	'50_250':	[0.0,4.284981e+00],
	'50_500':	[0.0,2.361928e+01],
	'50_800':	[0.0,4.418714e+01],
	'50_1200':	[0.0,6.988163e+01],
	'50_1500':	[0.0,8.867079e+01],
	'50_1750':	[0.0,1.041790e+02],
	'50_2000':	[0.0,1.196046e+02],
	'50_2250':	[0.0,1.349747e+02],
	'50_2500':	[0.0,1.503057e+02],
	'50_2750':	[0.0,1.656083e+02],
	'50_3000':	[0.0,1.808894e+02],
	'50_3250':	[0.0,1.961541e+02],
	'50_3500':	[0.0,2.114057e+02],
	'100_100':	[0.0,0.0],
	'100_250':	[0.0,4.230429e+00],
	'100_500':	[0.0,2.358973e+01],
	'100_800':	[0.0,4.416838e+01],
	'100_1200':	[0.0,6.986906e+01],
	'100_1500':	[0.0,8.866072e+01],
	'100_1750':	[0.0,1.041703e+02],
	'100_2000':	[0.0,1.195970e+02],
	'100_2250':	[0.0,1.349679e+02],
	'100_2500':	[0.0,1.502997e+02],
	'100_2750':	[0.0,1.656028e+02],
	'100_3000':	[0.0,1.808844e+02],
	'100_3250':	[0.0,1.961494e+02],
	'100_3500':	[0.0,2.114014e+02],
	'150_150':	[0.0,0.0],
	'150_250':	[0.0,4.155672e+00],
	'150_500':	[0.0,2.354250e+01],
	'150_800':	[0.0,4.413761e+01],
	'150_1200':	[0.0,6.984825e+01],
	'150_1500':	[0.0,8.864400e+01],
	'150_1750':	[0.0,1.041560e+02],
	'150_2000':	[0.0,1.195844e+02],
	'150_2250':	[0.0,1.349567e+02],
	'150_2500':	[0.0,1.502896e+02],
	'150_2750':	[0.0,1.655936e+02],
	'150_3000':	[0.0,1.808760e+02],
	'150_3250':	[0.0,1.961417e+02],
	'150_3500':	[0.0,2.113942e+02],
	'175_250':	[6.201e-13,4.117915e+00],
	'175_500':	[1.334e-14,2.351316e+01],
	'175_800':	[1.732e-15,4.411794e+01],
	'175_1200':	[3.24e-16,6.983482e+01],
	'175_1500':	[1.307e-16,8.863319e+01],
	'175_1750':	[7.003e-17,1.041467e+02],
	'175_2000':	[4.086e-17,1.195763e+02],
	'175_2250':	[2.543e-17,1.349495e+02],
	'200_250':	[3.158e-07,4.084956e+00],
	'200_500':	[5.137e-09,2.348062e+01],
	'200_800':	[6.491e-10,4.409557e+01],
	'200_1200':	[1.204e-10,6.981943e+01],
	'200_1500':	[4.844e-11,8.862076e+01],
	'200_1750':	[2.593e-11,1.041360e+02],
	'200_2000':	[1.512e-11,1.195669e+02],
	'200_2250':	[9.404e-12,1.349411e+02],
	'200_2500':	[6.154e-12,1.502755e+02],
	'200_2750':	[4.195e-12,1.655808e+02],
	'200_3000':	[2.957e-12,1.808642e+02],
	'200_3250':	[2.145e-12,1.961308e+02],
	'200_3500':	[1.593e-12,2.113841e+02],
		}
  this_key = '_'.join( ['%d' % i for i in [neutralino_mass, intermediate_mass]] )
  if this_key in width_dict:
    return width_dict[this_key]
  else: raise NotImplementedError('Expect neutralino decay but no decay width given.')

# Sample job options should have name such as MC15.999900.MGPy8EG_A14N23LO_RPVsinglelamp_50_500_lamp231_1p0.py
jo_name_list = runArgs.jobConfig[0].rstrip('.py').split('.')
mcid        = jo_name_list[1]
sample_name = jo_name_list[2]

def sample_name_to_parameters(sample_name):
  sample_name_list = sample_name.split('_')
  neutralino_mass = float( sample_name_list[-4] )
  smuon_mass      = float( sample_name_list[-3] )
  lambda_coupling = sample_name_list[-2]
  coupling_value  = float( sample_name_list[-1].replace('p','.') )
  return neutralino_mass, smuon_mass, lambda_coupling, coupling_value

neutralino_mass, smuon_mass, lambda_coupling, coupling_value = sample_name_to_parameters(sample_name)

gentype = 'RPVsinglelamp'
decaytype = lambda_coupling

param_card_old = None
if lambda_coupling == 'lamp231':
  param_card_old = 'MadGraph_param_card_SM.'+gentype+"."+lambda_coupling+'.dat' #old convention: 'param_card.SM.'+gentype+'.'+lambda_coupling+'.dat'
  masses['1000022'] = neutralino_mass
  masses['1000013'] = smuon_mass
else:
  raise NotImplementedError('I do not recognise the coupling requested: %s' % lambda_coupling)

if lambda_coupling == 'lamp231':
  width = get_widths(lambda_coupling, coupling_value, neutralino_mass, smuon_mass)
  neutralino_width = width[0]
  smuon_width = width[1]
  if neutralino_width == 0.0:
    decays = {'1000022':"""DECAY   1000022  0.000000 # neutralino decay"""}
  else:
    decays = {'1000022':"""DECAY   1000022  %g # neutralino decay
                ##           BR         NDA      ID1       ID2       ID3
                5.0000000E-01    3     13        6         -1   # BR(N1 -> mu tdbar)
                5.0000000E-01    3    -13       -6          1   # BR(N1 -> mubar tbard) """
              % neutralino_width
		 }
  decays.update({'1000013':"""DECAY 1000013  %g #smuon decay""" % smuon_width})

process = '''
  import model RPVMSSM_UFO
  define lepton = l+ l-
  define top = t t~
'''
if lambda_coupling == 'lamp231':
  process += '''
    define decoupled go ul cl t1 ur cr t2 dl sl b1 dr sr b2 ul~ cl~ t1~ ur~ cr~ t2~ dl~ sl~ b1~ dr~ sr~ b2~ h2 h3 h+ sve svm svt el- ta1- er- ta2- h- sve~ svm~ svt~ el+ ta1+ er+ ta2+ n2 n3 n4 x1+ x2+ ta- x1- x2- ta+
    generate    p p > lepton n1 top   / decoupled @ 1
    add process p p > lepton n1 top j / decoupled @ 2
  '''

  # Set the matching scale. By default this is MG ktdurham cut and Pythia8 CKKW-L TMS.
  xqcut = .25*( masses['1000022']+top_mass )
else:
  raise NotImplementedError('I do not recognise the coupling requested: %s' % lambda_coupling)

njets =1 

evgenLog.info('Registered generation of run number '+str(runArgs.runNumber))

build_param_card(param_card_old=param_card_old, param_card_new='MadGraph_param_card_SM.'+sample_name+'.dat', masses=masses, decays=decays)

# Make sure we generate enough events to compensate for filter.
# Always expect 1mu from smuon. Select 1e/mu from top decay. This has branching fraction ~22%, so multiplier 1/.22 = 4.5
evt_multiplier =20 

evgenLog.info('Using LeptonPairFilter accepting OS/SS lepton pairs')
from GeneratorFilters.GeneratorFiltersConf import LeptonPairFilter
filtSeq += LeptonPairFilter('DiLepFilter')
twolep_filter = filtSeq.DiLepFilter
twolep_filter.NLeptons_Min = 0
twolep_filter.NLeptons_Max = -1
twolep_filter.Ptcut = 10000
twolep_filter.Etacut = 2.8
twolep_filter.NSFOS_Min = -1
twolep_filter.NSFOS_Max = -1
twolep_filter.NOFOS_Min = -1
twolep_filter.NOFOS_Max = -1
twolep_filter.NSFSS_Min = -1
twolep_filter.NSFSS_Max = -1
twolep_filter.NOFSS_Min = -1
twolep_filter.NOFSS_Max = -1
twolep_filter.NPairSum_Max = -1
twolep_filter.NPairSum_Min = 1
twolep_filter.UseSFOSInSum = True
twolep_filter.UseSFSSInSum = True
twolep_filter.UseOFOSInSum = True
twolep_filter.UseOFSSInSum = True
twolep_filter.OnlyMassiveParents = False

evgenConfig.description = 'Single RPV coupling %s with value %f. m_N1 = %s GeV, m_smuon = %s GeV' % (lambda_coupling, coupling_value, neutralino_mass , smuon_mass)
evgenConfig.contact  = [ "holly.ann.pacey@cern.ch" ]
evgenConfig.keywords += ['SUSY', 'simplifiedModel', 'RPV']

include ( 'MadGraphControl/MadGraphControl_SimplifiedModel_RPVsinglelamp_dilep_PostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += [ "Merging:Process = pp>LEPTONS{n1,1000022}t" ]

