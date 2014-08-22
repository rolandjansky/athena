# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


class InDetCosmicFlags:
  # ---- data type (if simulated doSim=True)
  doSim=True
  RunNumber = 2102103
  FilePrefix = "daq_SFI-51_combined"
  
  # ---- make truth
  doTruth=True

  # ---- pattern to run
  SiTBLineFitter=True

  # ---- refitting to run
  Refit=False
  KalmanFitter=True
  DistributedKalmanFitter=False
  TBStraightLineFitter=False
  # ---- post processing
  # ---- compute TrackParticle
  ParticleCreation=False
  # --- write CBNT ntuple
  CBNT=True
  # --- write ESD
  doWriteESD = False
  # ---- write alignment ntuple 
  makeAlignNtp=False
  # --- write Atlantis xml file  
  JiveXML=True
  # --- write Atlantis geometry xml file (JiveXML needs to be set True)
  AtlantisGeometry=False
  # --- Monitoring at the EF
  doMonitoring=True
