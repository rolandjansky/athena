from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'commis'
globalflags.DataSource.set_Value_and_Lock('data')

from RecExConfig.RecFlags import rec
rec.Commissioning.set_Value_and_Lock(True)
