#example of personal configuration file for flags definition (optional)
# (just an example)
# do NOT put there property modifiers, they will be ignored !
#
# to use it (myconfig.py and myproperties.py are optional):
# athena -s myconfig.py RecExCommon_topOptions.py myproperties.py >! athena.log
# (in fact easier to use myTopOptions.py)
#
# 
# see RecExCommon/RecExCommon_flags.py for more available flags
#
# AllAlgs, DetDescrVersion needs be set before the include, since several

# secondary flags are configured according to that one
#
# DetDescrVersion="Rome-Initial" # mandatory for rome data
# doESD = False # if false, all algorithms are switched off by defaults 
# doiPatRec = False
include ("RecExCommon/RecExCommon_flags.py")

# all other flags are better set after the include
# switch off the muons
# DetFlags.Calo_setOff()

# switch off xkalman
# InDetFlags.doxKalman=False


# number of event to process

# jp.AthenaCommonFlags.EvtMax=2

#no jet
# doJetRec=False

# etc...
