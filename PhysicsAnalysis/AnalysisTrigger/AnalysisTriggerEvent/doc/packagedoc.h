/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page AnalysisTriggerEvent_page AnalysisTriggerEvent

   @author Tadashi.Maeno <Tadashi.Maeno@cern.ch>
   @author Attila Kraznahorkay Jr. <Attila.Krasznahorkay@cern.ch>
   @author Alan Watson <Alan.Watson@cern.ch>
   @author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>

@section AnalysisTriggerEvent_introductionAnalysisTriggerEvent Introduction

   AnalysisTriggerEvent contains ESD/AOD classes to store LVL1 trigger information.

@section AnalysisTriggerEvent_packagecontentAnalysisTriggerEvent Package Contents

   AnalysisTriggerEvent contains the following files/classes:
   - CTP_Decision ... LVL1 result decided by Central Trigger Processor (legacy object)
   - LVL1_ROI ... LVL1 Region Of Interest, which contains the following XYZ_ROI objects
   - EmTau_ROI ... ROI from Em/Tau part of Calorimeter trigger
   - EnergySum_ROI ... ROI from Energy-sum trigger 
   - JetET_ROI ... Jet transverse-energy ROI 
   - Jet_ROI ... ROI from Jet trigger
   - Muon_ROI ... ROI from Muon trigger
   - FakeHLTWord ... result of HLT system, which is used for DC2 and should retire near future. This is needed for streaming.

@section AnalysisTriggerEvent_AnalysisTriggerEvent Extra Pages

*/
