# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TBEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( TBEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES TBEvent/TBADCRawCont.h TBEvent/TBBPCCont.h TBEvent/TBBPCRawCont.h TBEvent/TBEventInfo.h TBEvent/TBHitCont.h TBEvent/TBHitPlaneCont.h TBEvent/TBIdentifiedParticle.h TBEvent/TBLArCalibDigitContainer.h TBEvent/TBLArDigitContainer.h TBEvent/TBMWPCCont.h TBEvent/TBMWPCRawCont.h TBEvent/TBPhase.h TBEvent/TBScintillatorCont.h TBEvent/TBScintillatorRawCont.h TBEvent/TBTDC.h TBEvent/TBTDCRawCont.h TBEvent/TBTailCatcher.h TBEvent/TBTailCatcherRaw.h TBEvent/TBTrack.h TBEvent/TBTriggerPatternUnit.h TBEvent/TBTrackInfo.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaKernel AthenaPoolCnvSvcLib GaudiKernel TBEvent TBTPCnv )

