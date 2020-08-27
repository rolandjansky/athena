## Base config for Sherpa
from Sherpa_i.Sherpa_iConf import Sherpa_i
genSeq += Sherpa_i()
evgenConfig.generators = ["Sherpa"]
genSeq.Sherpa_i.PluginCode = ""

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
genSeq.Sherpa_i.Parameters += [
    "OL_PREFIX=/cvmfs/sft.cern.ch/lcg/releases/LCG_88/MCGenerators/openloops/2.0.0/x86_64-slc6-gcc62-opt",
    "OL_PARAMETERS=preset=2 write_parameters=1",
]
