/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "MuonNSWCommonDecode/NSWResourceId.h"
#include "MuonNSWCommonDecode/NSWOfflineHelper.h"

// Error parameters

#define ERR_NOERR      0
#define ERR_ARGS      -1
#define ERR_GENERIC   -2

// Global parameters

uint32_t detector_id;
uint16_t vmm_number;
uint16_t vmm_channel;

void test_nsw_online2offline_help (char *progname)
{
  std::cout << "Usage: " << progname << " detector_id vmm_number vmm_channel" << std::endl;
}

int test_nsw_online2offline_opt (int argc, char **argv)
{
  int errcode = ERR_NOERR;

  if (argc < 4)
  {
    test_nsw_online2offline_help (argv[0]);
    exit (ERR_ARGS);
  }
  else if (argv[1][0] == '-')
  {
    switch (argv[1][1])
    {
      case 'h':
	test_nsw_online2offline_help (argv[0]);
	exit (ERR_NOERR);
      default:
	test_nsw_online2offline_help (argv[0]);
	exit (ERR_ARGS);
    }
  }
  else
  {
      detector_id = atoi (argv[1]);
      vmm_number  = atoi (argv[2]);
      vmm_channel = atoi (argv[3]);
  }

  return errcode;
}

int test_nsw_online2offline_init ()
{
  int errcode = ERR_NOERR;
  return errcode;
}

int test_nsw_online2offline_end ()
{
  int errcode = ERR_NOERR;
  return errcode;
}

int test_nsw_online2offline_loop ()
{
  int errcode = ERR_NOERR;

  Muon::nsw::NSWResourceId res_id (detector_id);
  Muon::nsw::helper::NSWOfflineHelper helper (&res_id, vmm_number, vmm_channel);

  std::string station_name;

  if (res_id.detId () < eformat::MUON_STGC_ENDCAP_C_SIDE)
    station_name = res_id.is_large_station () ? "MML" : "MMS";
  else
    station_name = res_id.is_large_station () ? "STL" : "STS";

  int8_t   station_eta    = res_id.station_eta ();
  uint8_t  station_phi    = res_id.station_phi ();
  uint8_t  multi_layer    = res_id.multi_layer ();
  uint8_t  gas_gap        = res_id.gas_gap ();

  uint8_t  channel_type   = helper.channel_type ();
  uint16_t channel_number = helper.channel_number ();

  std::cout << "Station name " << station_name << " Station eta " << static_cast <int> (station_eta)
	    << " Station phi " << static_cast <unsigned int> (station_phi) << std::endl;
  std::cout << "Multilayer " << static_cast <unsigned int> (multi_layer) << " Gas gap " << static_cast <unsigned int> (gas_gap)
	    << " Channel type " << static_cast <unsigned int> (channel_type)
	    << " Channel Number " << channel_number << std::endl;

  return errcode;
}

int main (int argc, char **argv)
{
  int err = ERR_NOERR;

  if ((err = test_nsw_online2offline_opt (argc, argv)) != ERR_NOERR)
    return err;

  if ((err = test_nsw_online2offline_init ()) != ERR_NOERR)
    return err;

  if ((err = test_nsw_online2offline_loop ()) != ERR_NOERR)
    return err;

  if ((err = test_nsw_online2offline_end ()) != ERR_NOERR)
    return err;

  return err;
}
