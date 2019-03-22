Reco_tf.py --inputESDFile "/eos/atlas/user/b/bcarlson/Run3Tmp/mc16_13TeV.361020.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ0W.recon.ESD.e3569_s3126_r10684/ESD.15453868._000040.pool.root.1" \
    --outputAODFile "out.AOD.root" \
    --maxEvents=10 \
     --preExec \
        "from TrigT1CaloFexSim.L1SimulationControlFlags import L1Phase1SimFlags as simflags; simflags.CTP.RunCTPEmulation=False; simflags.Calo.QualBitMask=0x40; simflags.Calo.SCellType=\"Pulse\"; simflags.Calo.ApplySCQual=True" \
    --postInclude \
    "default:PyJobTransforms/UseFrontier.py" \
    "TrigT1CaloFexSim/createL1SimulationSequence.py" \
    "LArROD/LArConfigureCablingSCFolder.py" \
    --autoConfiguration="everything" \
