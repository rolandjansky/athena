/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ALFA_EventTPCnv/test/ALFA_CLinkEventCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ALFA_CLinkEventCnv_p1.
 */


#undef NDEBUG
#include "ALFA_EventTPCnv/ALFA_CLinkEventCnv_p1.h"
#include <cassert>
#include <iostream>


void compare (const DCSID& p1,
              const DCSID& p2)
{
  assert (p1.ullBlmID == p2.ullBlmID);
  assert (p1.ullHVChannelID == p2.ullHVChannelID);
  assert (p1.ullLocalMonitoringID == p2.ullLocalMonitoringID);
  assert (p1.ullMovementID == p2.ullMovementID);
  assert (p1.ullRadMonID == p2.ullRadMonID);
  assert (p1.ullTriggerRatesID == p2.ullTriggerRatesID);
  assert (p1.ullFEConfigurationID == p2.ullFEConfigurationID);
  assert (p1.ullTriggerSettingsID == p2.ullTriggerSettingsID);
}


void compare (const ALFA_CLinkEvent& p1,
              const ALFA_CLinkEvent& p2)
{
  compare (p1.DCSId(), p2.DCSId());
  assert (p1.rawDataEvCollLink() == p2.rawDataEvCollLink());
  assert (p1.digitEvCollLink() == p2.digitEvCollLink());
  assert (p1.ODDigitEvCollLink() == p2.ODDigitEvCollLink());
  assert (p1.locRecEvCollLink() == p2.locRecEvCollLink());
  assert (p1.locRecODEvCollLink() == p2.locRecODEvCollLink());
  assert (p1.locRecCorrEvCollLink() == p2.locRecCorrEvCollLink());
  assert (p1.locRecCorrODEvCollLink() == p2.locRecCorrODEvCollLink());
}


void testit (const ALFA_CLinkEvent& trans1)
{
  MsgStream log (0, "test");
  ALFA_CLinkEventCnv_p1 cnv;
  ALFA_CLinkEvent_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ALFA_CLinkEvent trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  ALFA_CLinkEvent trans1;

  trans1.setRawDataEvCollLink (DataLink<ALFA_RawDataContainer> ("rawdata"));
  trans1.setDigitEvCollLink (DataLink<ALFA_DigitCollection> ("digit"));
  trans1.setODDigitEvCollLink (DataLink<ALFA_ODDigitCollection> ("oddigit"));
  trans1.setLocRecEvCollLink (DataLink<ALFA_LocRecEvCollection> ("locrec"));
  trans1.setLocRecODEvCollLink (DataLink<ALFA_LocRecODEvCollection> ("locrecod"));
  trans1.setLocRecCorrEvCollLink (DataLink<ALFA_LocRecCorrEvCollection> ("locreccorr"));
  trans1.setLocRecCorrODEvCollLink (DataLink<ALFA_LocRecCorrODEvCollection> ("locreccorrod"));

  trans1.SetDCSFolderID (EDCSI_BLM, 123);
  trans1.SetDCSFolderID (EDCSI_HVCHANNEL, 234);
  trans1.SetDCSFolderID (EDCSI_LOCALMONITORING, 345);
  trans1.SetDCSFolderID (EDCSI_MOVEMENT, 456);
  trans1.SetDCSFolderID (EDCSI_RADMON, 567);
  trans1.SetDCSFolderID (EDCSI_TRIGGERRATES, 678);
  trans1.SetDCSFolderID (EDCSI_FECONFIGURATION, 789);
  trans1.SetDCSFolderID (EDCSI_TRIGGERSETTINGS, 890);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
