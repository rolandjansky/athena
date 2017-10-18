#--------------------------------
# Heavy Ion main switch settings
#--------------------------------
include.block ('HIRecExample/HIRec_jobOptions.py')

print 'IN HEAVY ION REC'

# # General adjustments
# topSequence.TimeOut=3600* Units.s

# # Heavy Ion specific
# include("HIRecExample/HICalibrationsCoolFolders.py")

# from AthenaCommon.DetFlags import DetFlags
# from HIRecExample.HIRecExampleFlags import jobproperties

#   if jobproperties.HIRecExampleFlags.doHIEventView:
#       include( "HIRecExample/HIEventView_jobOptions.py" )

# include reconstruction algorithms
# only in ESD step
if rec.doESD:
    print "Including HI algorithms in ESD step"
    if jobproperties.HIRecExampleFlags.doHIGlobal:
        include( "HIGlobal/HIGlobal_jobOptions.py" )
    
    if (jobproperties.HIRecExampleFlags.doHIJetRec and DetFlags.haveRIO.Calo_on() and   rec.triggerStream() != 'CosmicCalo') :
        include( "HIJetRec/HIJetRec_jobOptions.py" )
        if jobproperties.HIRecExampleFlags.doHIegamma : 
            include ("HIJetRec/HIegamma_jobOptions.py");
# else :
#     jobproperties.HIRecExampleFlags.doHIJetRec=False

# if jobproperties.HIRecExampleFlags.doValidation:
#       include( "HIValidation/HIValidation_jobOptions.py")


if not rec.doESD and not rec.doWriteTAG and rec.doWritexAOD :
    include( "HIJetRec/HIJetRec_AODFix_207.py" );
