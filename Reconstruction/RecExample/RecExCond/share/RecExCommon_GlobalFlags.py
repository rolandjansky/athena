include.block("RecExCond/RecExCommon_GlobalFlags.py")

# this is now the sole reference for Luminosity
from AthenaCommon.BeamFlags import jobproperties


# GlobalFlags : Author Tadashi Maeno, David Rousseau
# This is class for main jobs configuration flags
#
# Category : DetGeo has states : atlas ctbh6 ctbh8    (ctbh6 and ctbh8 grouped in ctb)
# Category : DataSource has states : data geant3 geant4
# Category : InputFormat has states : zebra pool bytestream
# Category : Luminosity has states :  zero, verylow (10^33), low (2 10^33),
#                                              high (10^34)
#
# Each category can be set once and only once by the main topOptions.
# states are mutually exclusive
#
# from AthenaCommon.GlobalFlags import GlobalFlags
# GlobalFlags.DetGeo.set_atlas()  # set DetGeo to be atlas
#  (trying to set again the same category will throw an error)
#
# state can be interrogated as follows (by construction, one category
#   is only in one state. Testing a category that has not been set
#          throws an error):
# if GlobalFlags.DetGeo.is_atlas():
#
# Groupings canbe used to interrogate (no set methods available):
# if GlobalFlags.DetGeo.is_ctb():  (or of is_ctbh6() and is_ctbh8())


from AthenaCommon.Logging import logging
logRecExCommon_GlobalFlags = logging.getLogger( 'RecExCommon_GlobalFlags' )


if 'GlobalFlags' in dir():
   logRecExCommon_GlobalFlags.info("WARNING GlobalFlags obsolete, please use import globalflags instead. Deleting now")
   del GlobalFlags
#else:
#   from AthenaCommon.GlobalFlags import GlobalFlags

from AthenaCommon.GlobalFlags import globalflags


# default is atlas geometry
#try:
#   GlobalFlags.DetGeo.set_atlas()
#except Exception:
#   logRecExCommon_GlobalFlags.warning("GlobalFlags.DetGeo already set")    




#try:
#   GlobalFlags.DataSource.set_geant4()
#except Exception:
#   logRecExCommon_GlobalFlags.warning("GlobalFlags.DataSource already set")

#try:
#   GlobalFlags.InputFormat.set_pool()
#except Exception:
#   logRecExCommon_GlobalFlags.warning("GlobalFlags.InputFormat already set")    



## try:
##    # if jobproperties.BeamFlags.
##    # default is zero luminosity
##    if jobproperties.Beam.zeroLuminosity():
##       GlobalFlags.Luminosity.set_zero()
##       #automatic compatibility
##       #globalflags.luminosity="zero"
##    else:
##       GlobalFlags.Luminosity.set_verylow()      
##       #automatic compatibility
##       # globalflags.luminosity="verylow"
      

##    # for high lumi
##    # GlobalFlags.Luminosity.set_high()
## except Exception:
##    logRecExCommon_GlobalFlags.warning("GlobalFlags.Luminosity already set")    



# set here the geometry version
#
#if (not "DetDescrVersion" in dir()) or (DetDescrVersion == "" or DetDescrVersion == "Default"):
#    DetDescrVersion = "ATLAS-CSC-01-02-00"
#    if globalflags.DetGeo=="ctb":
#       DetDescrVersion = "CTB"


########
# fill new GlobalFlags

from RecExConfig.RecFlags import rec
if 'DetDescrVersion' in dir() and rec.oldFlagTopSteering:
   globalflags.DetDescrVersion=DetDescrVersion
# Other case is that we have left it the default.  The default is sensible, or should be.  Don't touch it.

# delee DetDescrVersion if required
if 'DetDescrVersion' in dir():
   if 'oldFlagCompatibility' in dir() and not oldFlagCompatibility:
      del DetDescrVersion
   else:
      if  not rec.oldFlagCompatibility():
         del DetDescrVersion

