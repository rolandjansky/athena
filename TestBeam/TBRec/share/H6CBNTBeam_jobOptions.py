# CBNT_BeamInstrumentation
from TBRec.TBRecConf import CBNTAA_BeamInstrumentation
CBNT_BeamInstrumentation = CBNTAA_BeamInstrumentation()
CBNT_BeamInstrumentation.BPCnames=["BPC0","BPC1","BPC2","BPC3","BPC4","BPC5"]
CBNT_BeamInstrumentation.Scintnames=["S1","S2","S3","Veto","Halo","B","muon1","muon2","muon3","muon4","muon5","muon6","muon7","muon8"]

# BPC RAW info
#CBNT_BeamInstrumentation.BPCRAW = TRUE

# Trigger Array H6
#CBNT_BeamInstrumentation.TrigArray = TRUE
CBNT_BeamInstrumentation.TrigPattern = [0x00000001,  # S1 accept
                                        0x00000002,  # S2 accept
                                        0x00000004,  # S3 accept
                                        0x00000008,  # B2 accept
                                        0x00000010,  # VetoLeft
                                        0x00000020,  # VetoRight
                                        0x00000040,  # VetoDown
                                        0x00000080,  # VetoUp
                                        0x00000100,  # Mu 1 Left
                                        0x00000200,  # Mu 1 Right
                                        0x00000400,  # Mu 2 Left
                                        0x00000800,  # Early PileUp
                                        0x00001000,  # Late PileUp
                                        0x00002000,  # In Spill Tag
                                        0x00004000,  # Off Spill Tag
                                        0x00008000,  # Cedar 6 of 8 segments
                                        0x00010000,  # Mu 2 Right
                                        0x00020000,  # Hole Counter
                                        0x00040000,  # W1*W2 coincidence
                                        0x00080000,  # B1 accept
                                        0x00100000,  # Scaler Reset Tag
                                        0x08000000,  # Cedar 7 of 8 segments
                                        0x10000000,  # Cedar 8 of 8 segments
                                        ]                                        
CBNT_BeamInstrumentation.NeverReturnFailure = True
CBNT_AthenaAware += CBNT_BeamInstrumentation

from TBRec.TBRecConf import CBNTAA_TBTrack
CBNT_TBTrack = CBNTAA_TBTrack()
if doSim and 'RunNumber' in dir():
     CBNT_TBTrack.runNumberForSimulation = RunNumber
CBNT_AthenaAware += CBNT_TBTrack

