/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// STL include files

#include <vector>
#include <string>

// TDAQ include files

#include "ers/ers.h"
#include "ers/SampleIssues.h"
#include "EventStorage/pickDataReader.h"
#include "eformat/eformat.h"

#include "MuonNSWCommonDecode/NSWCommonDecoder.h"
#include "MuonNSWCommonDecode/NSWElink.h"
#include "MuonNSWCommonDecode/VMMChannel.h"
#include "MuonNSWCommonDecode/NSWResourceId.h"

// Error parameters

#define ERR_NOERR      0
#define ERR_ARGS      -1
#define ERR_GENERIC   -2

// Global parameters

bool g_printout = false;
bool g_tree_view = true;
bool g_flat_view = true;
unsigned int g_printout_level = 0;
std::vector <std::string> g_file_names;

// Global variables

DataReader *reader;

void test_nsw_common_decoder_help (char *progname)
{
  std::cout << "Usage: " << progname << " [-v] [-t] [-f] [-h] file1, file2, ..." << std::endl;
  std::cout << "\t\tMultiple [-v] options increase printout detail level" << std::endl;
  std::cout << "\t\t[-t] only shows hits taken from tree view of decoded data" << std::endl;
  std::cout << "\t\t[-f] only shows hits taken from flat view of decoded data" << std::endl;
}

int test_nsw_common_decoder_opt (int argc, char **argv)
{
  int errcode = ERR_NOERR;
  int i;

  for (i=1; i < argc; ++i)
  {
    if (argv[i][0] == '-')
      switch (argv[i][1])
      {
        case 'v':
	  g_printout = true;
          ++g_printout_level;
	  break;
        case 't':
          g_flat_view = false;
          break;
        case 'f':
          g_tree_view = false;
          break;
        case 'h':
	  test_nsw_common_decoder_help (argv[0]);
	  exit (ERR_NOERR);
        default:
	  test_nsw_common_decoder_help (argv[0]);
	  exit (ERR_ARGS);
      }
    else
    {
      std::string data_file_name (argv[i]);
      g_file_names.push_back (data_file_name);
    }
  }

  if (g_file_names.size () == 0)
  {
    test_nsw_common_decoder_help (argv[0]);
    return (ERR_ARGS);
  }

  return errcode;
}

int test_nsw_common_decoder_init ()
{
  int errcode = ERR_NOERR;
  return errcode;
}

int test_nsw_common_decoder_end ()
{
  int errcode = ERR_NOERR;
  return errcode;
}

int test_nsw_common_decoder_event (eformat::read::FullEventFragment &f)
{
  int errcode = ERR_NOERR;
  std::vector <eformat::read::ROBFragment> robs;

  f.robs (robs);

  for (auto r = robs.begin (); r != robs.end (); ++r)
  {
    bool is_nsw = false, is_mmg = false, is_stg = false;

    // check fragment for errors

    try
    {
      r->check ();
    }
    catch (eformat::Issue &ex)
    {
      ers::warning (ex);
      continue;
    }

    uint32_t sid = r->rob_source_id ();
    eformat::helper::SourceIdentifier source_id (sid);
    eformat::SubDetector s = source_id.subdetector_id ();

    if (s == 0)    // NSW data written before end of March 2021 have wrong source Id
      s = static_cast <eformat::SubDetector> ((sid >> 24) & 0xff);

    if (s == eformat::MUON_MMEGA_ENDCAP_A_SIDE || s == eformat::MUON_MMEGA_ENDCAP_C_SIDE)
      is_nsw = is_mmg = true;
    else if (s == eformat::MUON_STGC_ENDCAP_A_SIDE  || s == eformat::MUON_STGC_ENDCAP_C_SIDE)
      is_nsw = is_stg = true;

    if (is_nsw)
    {
      // Decode the ROB fragment (including sanity check)

      Muon::nsw::NSWCommonDecoder nsw_decoder (*r);

      // Check direct access to channels and tree access to elinks and channels

      // Check the number of channels by accessing in both ways

      unsigned int nchan = 0;
      const std::vector <Muon::nsw::NSWElink *>& links = nsw_decoder.get_elinks ();
      for (auto i = links.begin (); i != links.end (); ++i)
	nchan += (*i)->get_channels ().size ();

      if (nchan != nsw_decoder.get_channels ().size ())
	std::cerr << "Inconsistent channel number" << std::endl;

      // How to access information about detector elements and channels using the tree view

      if (g_tree_view)
      {
        for (auto i = links.begin (); i != links.end (); ++i)
        {
          if (! (*i)->isNull ())
          {
            uint16_t l1Id  = (*i)->l1Id ();
            uint16_t bcId  = (*i)->bcId ();

            uint8_t sector = (*i)->elinkId ()->sector (); // (*i)->elinkId () returns a pointer to a Muon::nsw::ResourceId object
            uint8_t layer  = (*i)->elinkId ()->layer ();
	    uint8_t radius = (*i)->elinkId ()->radius ();
            uint8_t elink  = (*i)->elinkId ()->elink ();  // elink number is not needed to decode channel position

            // Offline ID components

            std::string station_name;

            if (is_mmg)
              station_name = (*i)->elinkId ()->is_large_station () ? "MML" : "MMS";
            else
              station_name = (*i)->elinkId ()->is_large_station () ? "STL" : "STS";

            int8_t   station_eta    = (*i)->elinkId ()->station_eta ();
            uint8_t  station_phi    = (*i)->elinkId ()->station_phi ();
            uint8_t  multi_layer    = (*i)->elinkId ()->multi_layer ();
            uint8_t  gas_gap        = (*i)->elinkId ()->gas_gap ();

            const std::vector <Muon::nsw::VMMChannel *> channels = (*i)->get_channels ();
            for (auto j = channels.begin (); j != channels.end (); ++j)
            {
              uint16_t vmm_number  = (*j)->vmm ();
              uint16_t vmm_channel = (*j)->vmm_channel ();
              uint16_t rel_bcid    = (*j)->rel_bcid ();
              uint16_t pdo         = (*j)->pdo ();
              uint16_t tdo         = (*j)->tdo ();
              bool     parity      = (*j)->parity ();
              bool     neighbor    = (*j)->neighbor ();

              // Get offline information

              uint8_t  channel_type   = (*j)->channel_type ();
              uint16_t channel_number = (*j)->channel_number ();

              if (g_printout)
              {
                if (g_printout_level > 1)
                {
                  std::cout << "ROD header:" << std::endl;
                  std::cout << "\t\tROD fragment size (words)  " << r->rod_fragment_size_word () << std::endl;
                  std::cout << "\t\tROD header size (words)    " << r->rod_header_size_word () << std::endl;
                  std::cout << "\t\tROD trailer size (words)   " << r->rod_trailer_size_word () << std::endl;
                  std::cout << "\t\tROD format version          0x" << std::hex << r->rod_version () << std::dec << std::endl;
                  std::cout << "\t\tROD source ID               0x" << std::hex << r->rod_source_id () << std::dec << std::endl;
                  std::cout << "\t\tROD L1ID                   " << r->rod_lvl1_id () << std::endl;
                  std::cout << "\t\tROD BCID                   " << r->rod_bc_id () << std::endl;
                  std::cout << "\t\tROD trigger type           " << r->rod_lvl1_trigger_type () << std::endl;
                  std::cout << "\t\tROD run number             " << r->rod_run_no () << std::endl;
                  std::cout << "\t\tROD detector event type    " << r->rod_detev_type () << std::endl;
                  std::cout << "\t\tROD number of status words " << r->rod_nstatus () << std::endl;
                }

                std::cout << "Online decoding of hit word 0x" << std::hex << (*j)->vmm_word ()
                          << " on link 0x" << (*i)->elinkWord () << std::dec << std::endl;
                std::cout << "Parity " << parity << " Calculated parity " << (*j)->calculate_parity () << std::endl;
                std::cout << "L1ID " << l1Id << " BCID " << bcId << " Sector " << static_cast <unsigned int> (sector)
                          << " Layer " << static_cast <unsigned int> (layer) << " Radius " << static_cast <unsigned int> (radius)
                          << " Elink " << static_cast <unsigned int> (elink) << std::endl;
                std::cout << "VMM " << vmm_number << " Channel " << vmm_channel << " Relative BCID " << rel_bcid 
                          << " Pdo " << pdo << " Tdo " << tdo << " Parity " << parity << " Neighbor " << neighbor << std::endl;
                std::cout << "Offline decoding of hit word 0x" << std::hex << (*j)->vmm_word ()
                          << " on link 0x" << (*i)->elinkWord () << std::dec << std::endl;
                std::cout << "Station name " << station_name << " Station eta " << static_cast <int> (station_eta)
                          << " Station phi " << static_cast <unsigned int> (station_phi) << std::endl;
                std::cout << "Multilayer " << static_cast <unsigned int> (multi_layer) << " Gas gap " << static_cast <unsigned int> (gas_gap)
                          << " Channel type " << static_cast <unsigned int> (channel_type)
                          << " Channel Number " << channel_number << std::endl;
              }
            }
          }
        }
      }

      // The same information can be accessed through the list of all the channels for that ROB as follows

      if (g_flat_view)
      {
        const std::vector <Muon::nsw::VMMChannel *>& channels = nsw_decoder.get_channels ();
        for (auto j = channels.begin (); j != channels.end (); ++j)
        {
          Muon::nsw::NSWElink *link = const_cast <Muon::nsw::NSWElink *> ((*j)->elink ());

          uint16_t l1Id  = link->l1Id ();
          uint16_t bcId  = link->bcId ();

          uint8_t sector = link->elinkId ()->sector (); // (*i)->elinkId () returns a pointer to a Muon::nsw::ResourceId object
          uint8_t layer  = link->elinkId ()->layer ();
	  uint8_t radius = link->elinkId ()->radius ();
          uint8_t elink  = link->elinkId ()->elink ();  // elink number is not needed to decode channel position

          uint16_t vmm_number  = (*j)->vmm ();
          uint16_t vmm_channel = (*j)->vmm_channel ();
          uint16_t rel_bcid    = (*j)->rel_bcid ();
          uint16_t pdo         = (*j)->pdo ();
          uint16_t tdo         = (*j)->tdo ();
          bool     parity      = (*j)->parity ();
          bool     neighbor    = (*j)->neighbor ();

          // Offline ID components

          std::string station_name;
          if (is_mmg)
            station_name = (*j)->is_large_station () ? "MML" : "MMS";
          else
            station_name = (*j)->is_large_station () ? "STL" : "STS";

          int8_t   station_eta    = (*j)->station_eta ();
          uint8_t  station_phi    = (*j)->station_phi ();
          uint8_t  multi_layer    = (*j)->multi_layer ();
          uint8_t  gas_gap        = (*j)->gas_gap ();
          uint8_t  channel_type   = (*j)->channel_type ();
          uint16_t channel_number = (*j)->channel_number ();

          if (g_printout)
          {
            if (g_printout_level > 1)
            {
              std::cout << "ROD header:" << std::endl;
              std::cout << "\t\tROD fragment size (words)  " << r->rod_fragment_size_word () << std::endl;
              std::cout << "\t\tROD header size (words)    " << r->rod_header_size_word () << std::endl;
              std::cout << "\t\tROD trailer size (words)   " << r->rod_trailer_size_word () << std::endl;
              std::cout << "\t\tROD format version          0x" << std::hex << r->rod_version () << std::dec << std::endl;
              std::cout << "\t\tROD source ID               0x" << std::hex << r->rod_source_id () << std::dec << std::endl;
              std::cout << "\t\tROD L1ID                   " << r->rod_lvl1_id () << std::endl;
              std::cout << "\t\tROD BCID                   " << r->rod_bc_id () << std::endl;
              std::cout << "\t\tROD trigger type           " << r->rod_lvl1_trigger_type () << std::endl;
              std::cout << "\t\tROD run number             " << r->rod_run_no () << std::endl;
              std::cout << "\t\tROD detector event type    " << r->rod_detev_type () << std::endl;
              std::cout << "\t\tROD number of status words " << r->rod_nstatus () << std::endl;
            }

            std::cout << "Online decoding of hit word 0x" << std::hex << (*j)->vmm_word () << " on link 0x" << link->elinkWord () << std::dec << std::endl;
            std::cout << "Parity " << parity << " Calculated parity " << (*j)->calculate_parity () << std::endl;
            std::cout << "L1ID " << l1Id << " BCID " << bcId << " Sector " << static_cast <unsigned int> (sector)
                      << " Layer " << static_cast <unsigned int> (layer) << " Radius " << static_cast <unsigned int> (radius)
                      << " Elink " << static_cast <unsigned int> (elink) << std::endl;
            std::cout << "VMM " << vmm_number << " Channel " << vmm_channel << " Relative BCID " << rel_bcid 
                      << " Pdo " << pdo << " Tdo " << tdo << " Parity " << parity << " Neighbor " << neighbor << std::endl;
            std::cout << "Offline decoding of hit word 0x" << std::hex << (*j)->vmm_word () << " on link 0x" << link->elinkWord () << std::dec << std::endl;
            std::cout << "Station name " << station_name << " Station eta " << static_cast <int> (station_eta)
                      << " Station phi " << static_cast <unsigned int> (station_phi) << std::endl;
            std::cout << "Multilayer " << static_cast <unsigned int> (multi_layer) << " Gas gap " << static_cast <unsigned int> (gas_gap)
                      << " Channel type " << static_cast <unsigned int> (channel_type)
                      << " Channel Number " << channel_number << std::endl;
          }
        }
      }
    }
  }

  return errcode;
}

int test_nsw_common_decoder_loop ()
{
  int errcode = ERR_NOERR;
  unsigned int tot_nev = 0;

  for (auto file_iter = g_file_names.begin (); file_iter != g_file_names.end (); ++file_iter)
  {
    char *buf = 0;
    unsigned int size = 0;

    std::string data_file_name (*file_iter);

    std::cout << "Reading file " << data_file_name << std::endl;
    reader = pickDataReader (data_file_name);

    if (!reader || !reader->good ())
    {
      ers::fatal (ers::CantOpenFile (ERS_HERE, data_file_name.c_str ()));
      return ERR_GENERIC;
    }

    while (!reader->endOfFile ())
    {
      try
      {
        DRError err = reader->getData (size, &buf);

        if (err != EventStorage::DROK)
        {
          ers::fatal (ers::File (ERS_HERE, data_file_name.c_str ()));
          errcode = ERR_GENERIC;
          if (buf) delete buf;
          break;
        }

        eformat::read::FullEventFragment f ((unsigned int *) buf);
        f.check ();

	if ((errcode = test_nsw_common_decoder_event (f)) != ERR_NOERR)
        {
          ers::error (ers::File (ERS_HERE, data_file_name.c_str ()));
          if (buf) delete buf;
          continue;
        }

        ++tot_nev;
      }
      catch (ers::Issue &ex)
      {
        ers::error (ers::File (ERS_HERE, data_file_name.c_str (), ex));
        errcode = ERR_GENERIC;
        if (buf) delete buf;
        break;
      }

      if (buf) delete buf;
    }

    if (reader)
      delete reader;
  }

  std::cout << "Total event number: " << tot_nev << std::endl;

  return errcode;
}

int main (int argc, char **argv)
{
  int err = ERR_NOERR;

  if ((err = test_nsw_common_decoder_opt (argc, argv)) != ERR_NOERR)
    return err;

  if ((err = test_nsw_common_decoder_init ()) != ERR_NOERR)
    return err;

  if ((err = test_nsw_common_decoder_loop ()) != ERR_NOERR)
    return err;

  if ((err = test_nsw_common_decoder_end ()) != ERR_NOERR)
    return err;

  return err;
}
