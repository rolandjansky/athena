print "now initializing the LAr readout geometry : standard Atlas flavour"

# ------------------------------------------------
#  FIXME : setup online identifier & mapping
#    ( should be done somewhere else )
# ------------------------------------------------
include( "CaloConditions/CaloConditions_jobOptions.py" )

from AthenaCommon.GlobalFlags import globalflags

if globalflags.DataSource() == 'geant4' :
  include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
  include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
elif globalflags.DataSource() == 'data'  :
  include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py" )
  include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
else :
  raise RunTimeError, " from LArDetDescr_joboptions.py: DataSource not supported" 

# must be included once only :
include.block ("LArDetDescr/LArDetDescr_joboptions.py")

