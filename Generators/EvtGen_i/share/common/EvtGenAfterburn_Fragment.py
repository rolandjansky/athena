if runArgs.trfSubstepName == 'afterburn':
   include("EvtGen_i/EvtGen_Fragment.py")
   evgenConfig.auxfiles += ['inclusiveP8DsDPlus.pdt']
   genSeq.EvtInclusiveDecay.pdtFile = "inclusiveP8DsDPlus.pdt"
   genSeq.EvtInclusiveDecay.readExisting = True
#   genSeq.EvtInclusiveDecay.McEventKey = "GEN_EVENT_EVTGEN"
#   genSeq.EvtInclusiveDecay.McEventsR = "StoreGateSvc/GEN_EVENT_EVTGEN"
#   genSeq.EvtInclusiveDecay.McEventsRW = "StoreGateSvc/GEN_EVENT_EVTGEN"

   if hasattr(testSeq, "FixHepMC"):
      fixSeq.remove(FixHepMC)

   if hasattr(testSeq, "TestHepMC"):
      testSeq.remove(TestHepMC())

   if "Sherpa" in evgenConfig.generators:
      genSeq.EvtInclusiveDecay.whiteList.remove(10411)
      genSeq.EvtInclusiveDecay.whiteList.remove(-10411)
      genSeq.EvtInclusiveDecay.whiteList.remove(10421)
      genSeq.EvtInclusiveDecay.whiteList.remove(-10421)
      genSeq.EvtInclusiveDecay.whiteList.remove(-10423)
      genSeq.EvtInclusiveDecay.whiteList.remove(10423)
      genSeq.EvtInclusiveDecay.whiteList.remove(10431)
      genSeq.EvtInclusiveDecay.whiteList.remove(-10431)
      genSeq.EvtInclusiveDecay.whiteList.remove(-10433)
      genSeq.EvtInclusiveDecay.whiteList.remove(10433)
      genSeq.EvtInclusiveDecay.whiteList.remove(-20433)
      genSeq.EvtInclusiveDecay.whiteList.remove(20433)

   postSeq.CountHepMC.CorrectRunNumber = True



