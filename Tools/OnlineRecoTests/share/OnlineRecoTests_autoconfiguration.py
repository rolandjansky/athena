import pickle

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import jobproperties,athenaCommonFlags

onlinegoodies = {}

onlinegoodies['DataSource'] = globalflags.DataSource()
onlinegoodies['InputFormat'] = globalflags.InputFormat()
onlinegoodies['ConditionsTag'] = globalflags.ConditionsTag()
onlinegoodies['beamType'] = jobproperties.Beam.beamType()
onlinegoodies['DetDescrVersion'] = globalflags.DetDescrVersion()

file = open("globalflags.pickle",'wb')
pickle.dump(onlinegoodies,file)
file.close()

