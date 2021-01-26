# Use EW scheme which allows specifying sinThetaW and mZ simultaneously at the expense of mW.
# Three sub-options for specifying the QED coupling are introduced:
#
#    EW_SCHEME=20 for alpha(0), mZ, sinThetaW
#    EW_SCHEME=21 for Gmu, mZ, sinThetaW
#    EW_SCHEME=22 for alpha(mZ), mZ, sinThetaW

genSeq.Sherpa_i.Parameters += [ "EW_SCHEME=22" ]

try:
    USE_PDG_VALUES
except NameError:
    USE_PDG_VALUES = False
## if USE_PDG_VALUES = True, load PDG value of 
## sin2thetaW from parameter dictionary located
## in EvgenProdTools/python/offline_dict.py
if USE_PDG_VALUES:
    from EvgenProdTools.offline_dict import parameters
    for k,v in parameters.items():
        if k == 'EW_parameters':
            for key,value in v.items():
                if key[0] == 'SIN2THETAW':
                    genSeq.Sherpa_i.Parameters += [
                        str(key[0])+'='+str(value),
                    ]
                    break
            break
if 'SIN2THETAW' not in genSeq.Sherpa_i.Parameters[-1]:
    genSeq.Sherpa_i.Parameters += [ "SIN2THETAW=0.23113" ]


## Overwrite OL_PREFIX to not use it from cvmfs (which doesn't have this scheme implemented yet)
## Also adding OL_IGNORE_MODEL (default to SM) since the scheme names aren't synchronised yet
genSeq.Sherpa_i.Parameters += [
    "OL_PREFIX=./Process/OpenLoops",
    "OL_PARAMETERS=ew_renorm_scheme 22 write_parameters 1",
    "OL_IGNORE_MODEL=1",
]
