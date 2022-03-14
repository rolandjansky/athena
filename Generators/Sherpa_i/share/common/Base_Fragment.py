## Base config for Sherpa
from Sherpa_i.Sherpa_iConf import Sherpa_i
genSeq += Sherpa_i()
evgenConfig.generators = ["Sherpa"]
genSeq.Sherpa_i.PluginCode = ""

import os
if os.environ["SHERPAVER"].startswith('3.'):
  ## General ATLAS parameters
  genSeq.Sherpa_i.BaseFragment = """
  BEAMS: 2212
  BEAM_ENERGIES: $(EBEAMSETBYATHENA)

  MAX_PROPER_LIFETIME: 10.0
  HEPMC_TREE_LIKE: 1
  PRETTY_PRINT: Off
  EXTERNAL_RNG: Atlas_RNG
  """

  ## Cap event weights at |w|<10 in unweighted evgen; set H-event shower mode
  ## in the soft region to avoid instable weights 
  genSeq.Sherpa_i.BaseFragment += """
  OVERWEIGHT_THRESHOLD: 10
  MC@NLO:
    HPSMODE: 0
  """

  particledata = {
      '5': {'mass': '4.95', 'width': '0.'}, # mb consistent with McProductionCommonParametersMC15 and https://cds.cern.ch/record/2047636
      '6': {'mass': '1.725E+02', 'width': '1.32E+00'},
      '15': {'mass': '1.777', 'width': '2.26735e-12'},
      '23': {'mass': '91.1876', 'width': '2.4952'},
      '24': {'mass': '80.399', 'width': '2.085'},
  }
  sin2thetaw='0.23113'
  try:
      USE_PDG_VALUES
  except NameError:
      USE_PDG_VALUES = False
  if USE_PDG_VALUES:
      from EvgenProdTools.offline_dict import parameters
      for id, values in particledata:
          particledata[i] = parameters['particles'][id]
      sin2thetaw = parameters['EW_parameters']['SIN2THETHAW']
  genSeq.Sherpa_i.BaseFragment += """
  PARTICLE_DATA:
  """
  for id, values in particledata.items():
      genSeq.Sherpa_i.BaseFragment += """
    {id}:
      Mass: {mass}
      Width: {width}
  """.format(id=id, mass=values['mass'], width=values['width'])
  ## ToDo: Include partial widths for H/W/Z in EvgenProdTools/python/offline_dict.py ?

  ## Switch to EW_SCHEME=0 to be able to set PDG value of thetaW
  genSeq.Sherpa_i.BaseFragment += """
  EW_SCHEME: UserDefined
  SIN2THETAW: {}
  """.format(sin2thetaw)


  ## SM decay branching ratios
  genSeq.Sherpa_i.BaseFragment += """
  HARD_DECAYS:
    Enabled: true
    Channels:
      "6 -> 24 5":  { Width: 1.32 }
      "-6 -> -24 -5":  { Width: 1.32 }
      "25 -> 5 -5":  { Width: 2.35e-3 }
      "25 -> 15 -15":  { Width: 2.57e-4 }
      "25 -> 13 -13":  { Width: 8.91e-7 }
      "25 -> 4 -4":  { Width: 1.18e-4 }
      "25 -> 3 -3":  { Width: 1.00e-6 }
      "25 -> 21 21":  { Width: 3.49e-4 }
      "25 -> 22 22":  { Width: 9.28e-6 }
      "24 -> 2 -1":  { Width: 0.7041 }
      "24 -> 4 -3":  { Width: 0.7041 }
      "24 -> 12 -11":  { Width: 0.2256 }
      "24 -> 14 -13":  { Width: 0.2256 }
      "24 -> 16 -15":  { Width: 0.2256 }
      "-24 -> -2 1":  { Width: 0.7041 }
      "-24 -> -4 3":  { Width: 0.7041 }
      "-24 -> -12 11":  { Width: 0.2256 }
      "-24 -> -14 13":  { Width: 0.2256 }
      "-24 -> -16 15":  { Width: 0.2256 }
      "23 -> 1 -1":  { Width: 0.3828 }
      "23 -> 2 -2":  { Width: 0.2980 }
      "23 -> 3 -3":  { Width: 0.3828 }
      "23 -> 4 -4":  { Width: 0.2980 }
      "23 -> 5 -5":  { Width: 0.3828 }
      "23 -> 11 -11":  { Width: 0.0840 }
      "23 -> 12 -12":  { Width: 0.1663 }
      "23 -> 13 -13":  { Width: 0.0840 }
      "23 -> 14 -14":  { Width: 0.1663 }
      "23 -> 15 -15":  { Width: 0.0840 }
      "23 -> 16 -16":  { Width: 0.1663 }
  """

  ## OpenLoops parameters
  import os
  genSeq.Sherpa_i.BaseFragment += """
  OL_PARAMETERS:
    preset: 2
    write_parameters: 1
  OL_PREFIX: {}
  """.format(str(os.environ['OPENLOOPSPATH']))
else:
  ## Tell Sherpa to read its run card sections from the jO
  ## TODO: write out Run.dat from genSeq.Sherpa_i.RunCard and read from it
  ## instead of the JO
  genSeq.Sherpa_i.Parameters += [ 'RUNDATA=%s' % runArgs.jobConfig[0] ]

  ## Tell Sherpa to write logs into a separate file
  ## (need for production, looping job detection, Wolfgang Ehrenfeld)
  genSeq.Sherpa_i.Parameters += [ 'LOG_FILE=sherpa.log' ]

  ## General ATLAS parameters
  genSeq.Sherpa_i.Parameters += [
      "BEAM_1=2212",
      "BEAM_2=2212",
      "MAX_PROPER_LIFETIME=10.0",
      "HEPMC_TREE_LIKE=1",
      "PRETTY_PRINT=Off",
      ]

  ## Cap event weights at |w|<10 in unweighted evgen; set H-event shower mode
  ## in the soft region to avoid instable weights 
  genSeq.Sherpa_i.Parameters += [
      "OVERWEIGHT_THRESHOLD=10",
      "PP_HPSMODE=0"
  ]

  ## Enable scale variations by default
  genSeq.Sherpa_i.Parameters += [
      "HEPMC_USE_NAMED_WEIGHTS=1",
      "CSS_REWEIGHT=1",
      "REWEIGHT_SPLITTING_PDF_SCALES=1",
      "REWEIGHT_SPLITTING_ALPHAS_SCALES=1",
      "CSS_REWEIGHT_SCALE_CUTOFF=5.0",
      "HEPMC_INCLUDE_ME_ONLY_VARIATIONS=1",
      "SCALE_VARIATIONS=0.25,0.25 0.25,1. 1.,0.25 1.,1. 1.,4. 4.,1. 4.,4.",
  ]

  ## Switch to EW_SCHEME=0 to be able to set PDG value of thetaW
  genSeq.Sherpa_i.Parameters += [ "EW_SCHEME=0" ]

  try:
      USE_PDG_VALUES
  except NameError:
      USE_PDG_VALUES = False
  ## if USE_PDG_VALUES = True, load PDG value 
  ## of sin2thetaW and particle masses/widths 
  ## from parameter dictionary located in 
  ## EvgenProdTools/python/offline_dict.py 
  ## ToDo: Include partial widths for H/W/Z?
  recorded = []
  if USE_PDG_VALUES:
      from EvgenProdTools.offline_dict import parameters
      for k,v in parameters.items():
          if k == 'particles':
              for key,value in v.items():
                  if 5 < int(key) and int(key) < 26:
                      ## This includes now the top quark, 
                      ## the leptons and the bosons
                      genSeq.Sherpa_i.Parameters += [ 
                          'MASS['+key+']='+ value['mass'],
                          'WIDTH['+key+']='+ value['width'],
                      ]
                      recorded.append(key)
          elif k == 'EW_parameters':
              for key,value in v.items():
                  if key[0] == 'SIN2THETAW':
                      genSeq.Sherpa_i.Parameters += [
                          str(key[0])+'='+str(value),
                      ]
                      recorded.append(str(key[0]))
                      break

  # Fall-back to the MC15 default values if need be
  defaults = {
      '6'  : [ "MASS[6]=172.5",    "WIDTH[6]=1.32"         ],
      '15' : [ "MASS[15]=1.777",   "WIDTH[15]=2.26735e-12" ],
      '23' : [ "MASS[23]=91.1876", "WIDTH[23]=2.4952"      ],
      '24' : [ "MASS[24]=80.399",  "WIDTH[24]=2.085"       ],
      'SIN2THETAW' : [ "SIN2THETAW=0.23113" ],
  }
  for key, value in defaults.items():
    if key not in recorded:
      genSeq.Sherpa_i.Parameters += value



  ## set/add partial widths for H, W, Z to PDG values
  ## cf. https://sherpa.hepforge.org/doc/SHERPA-MC-2.2.4.html#HDH_005fWIDTH
  genSeq.Sherpa_i.Parameters += [
      "HDH_WIDTH[6,24,5]=1.32",    #updated to comply latest updated top results
      "HDH_WIDTH[-6,-24,-5]=1.32",  #updated to comply latest updated top results
      "HDH_WIDTH[25,5,-5]=2.35e-3",
      "HDH_WIDTH[25,15,-15]=2.57e-4",
      "HDH_WIDTH[25,13,-13]=8.91e-7",
      "HDH_WIDTH[25,4,-4]=1.18e-4",
      "HDH_WIDTH[25,3,-3]=1.00e-6",
      "HDH_WIDTH[25,21,21]=3.49e-4",
      "HDH_WIDTH[25,22,22]=9.28e-6",
      "HDH_WIDTH[24,2,-1]=0.7041",
      "HDH_WIDTH[24,4,-3]=0.7041",
      "HDH_WIDTH[24,12,-11]=0.2256",
      "HDH_WIDTH[24,14,-13]=0.2256",
      "HDH_WIDTH[24,16,-15]=0.2256",
      "HDH_WIDTH[-24,-2,1]=0.7041",
      "HDH_WIDTH[-24,-4,3]=0.7041",
      "HDH_WIDTH[-24,-12,11]=0.2256",
      "HDH_WIDTH[-24,-14,13]=0.2256",
      "HDH_WIDTH[-24,-16,15]=0.2256",
      "HDH_WIDTH[23,1,-1]=0.3828",
      "HDH_WIDTH[23,2,-2]=0.2980",
      "HDH_WIDTH[23,3,-3]=0.3828",
      "HDH_WIDTH[23,4,-4]=0.2980",
      "HDH_WIDTH[23,5,-5]=0.3828",
      "HDH_WIDTH[23,11,-11]=0.0840",
      "HDH_WIDTH[23,12,-12]=0.1663",
      "HDH_WIDTH[23,13,-13]=0.0840",
      "HDH_WIDTH[23,14,-14]=0.1663",
      "HDH_WIDTH[23,15,-15]=0.0840",
      "HDH_WIDTH[23,16,-16]=0.1663",
      ]

  ## OpenLoops parameters
  _sh_par="OL_PREFIX="+str(os.environ['OPENLOOPSPATH'])
  genSeq.Sherpa_i.Parameters += [
      "OL_PARAMETERS=preset=2 write_parameters=1",
  ]
  genSeq.Sherpa_i.Parameters.append(_sh_par)

  if hasattr(runArgs,'ecmEnergy') and runArgs.ecmEnergy > 13001.:
    genSeq.Sherpa_i.Parameters += [
      "STRANGE_FRACTION=0.535",
      "BARYON_FRACTION=1.48",
      "DECAY_OFFSET=1.29",
      "DECAY_EXPONENT=3.03"
      "P_qs_by_P_qq=0.26",
      "P_ss_by_P_qq=0.012",
      "P_di_1_by_P_di_0=0.93",
      "G2QQ_EXPONENT=0.60",
      "PT_MAX=1.48",
      "PT_MAX_FACTOR=1.34",
      "SPLIT_EXPONENT=0.24",
      "SPLIT_LEADEXPONENT=1.49",
      "SPECT_EXPONENT=1.49",
      "SPECT_LEADEXPONENT=10.32",
      "MASS[413]=2.01022",
      "WIDTH[413]=0.000083", # cf. https://gitlab.com/sherpa-team/sherpa/-/issues/349
      "DECAYPATH=/cvmfs/atlas.cern.ch/repo/sw/Generators/MCJobOptions/common/Sherpa_i/Decaydata/2.2.12-fix-D_Kstar_K", # cf. https://gitlab.com/sherpa-team/decaydata/-/commit/af49fe3b
    ]
