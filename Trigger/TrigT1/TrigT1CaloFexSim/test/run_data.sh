Reco_tf.py --inputESDFile "/eos/atlas/user/b/bcarlson/Run3Tmp/data18_13TeV.00360026.physics_EnhancedBias.recon.ESD.r10978_r11179_r11185/ESD.16781883._001043.pool.root.1" \
    --outputAODFile "out.AOD.root" \
    --maxEvents=10 \
    --preExec \
    "from TrigT1CaloFexSim.L1SimulationControlFlags import L1Phase1SimFlags as simflags; simflags.CTP.RunCTPEmulation=False; simflags.Calo.RunFexAlgorithms=False;simflags.Calo.ApplySCQual=False; simflags.Calo.SCellType=\"Emulated\"" \
    --postInclude \
    "default:PyJobTransforms/UseFrontier.py" \
    "TrigT1CaloFexSim/createL1SimulationSequence.py" \
    --autoConfiguration="everything" \