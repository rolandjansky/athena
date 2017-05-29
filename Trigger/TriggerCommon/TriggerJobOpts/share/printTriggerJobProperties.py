from TriggerJobOpts.TriggerFlags import TriggerFlags
#example 
TriggerFlags.TauSlice.doL2ID=False

TriggerFlags.print_JobProperties('tree&value')   # dump full container of flags
raise Exception("Finished, do not worry all was OK, just do not kow how to stop this job other way")


