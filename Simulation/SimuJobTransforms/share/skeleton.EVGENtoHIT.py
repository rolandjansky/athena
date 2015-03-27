include("SimuJobTransforms/CommonSkeletonJobOptions.py")

if hasattr(runArgs, "jobNumber"): 
    if runArgs.jobNumber < 1: 
        raise ValueError('jobNumber must be a postive integer. %s lies outside this range', str(runArgs.jobNumber)) 

