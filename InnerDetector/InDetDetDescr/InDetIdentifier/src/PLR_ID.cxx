/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

/***************************************************************************
   Inner Detector identifier package
   -------------------------------------------
***************************************************************************/


#include "GaudiKernel/MsgStream.h"

#include "InDetIdentifier/PLR_ID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictDefs.h"
#include <set>
#include <algorithm>
#include <iostream>


PLR_ID::PLR_ID(void)
  : PixelID()
{
  // changes compared to PixelID:
  m_BARREL_EC_INDEX = 3;
  m_LAYER_DISK_INDEX = 4;
  m_PHI_MODULE_INDEX = 5;
  m_ETA_MODULE_INDEX = 6;
  m_PHI_INDEX_INDEX = 7;
  m_ETA_INDEX_INDEX = 8;
}

int
PLR_ID::initialize_from_dictionary(const IdDictMgr& dict_mgr) {
  MsgStream log(m_msgSvc, "PLR_ID");

  if (m_msgSvc) log << MSG::INFO << "Initialize from dictionary" << endmsg;
  else std::cout << " INFO Initialize from dictionary" << std::endl;

  // Check whether this helper should be reinitialized
  if (!reinitialize(dict_mgr)) {
    if (m_msgSvc) log << MSG::INFO << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
    else std::cout << " INFO Request to reinitialize not satisfied - tags have not changed" << std::endl;
    return(0);
  } else {
    if (m_msgSvc) log << MSG::DEBUG << "(Re)initialize" << endmsg;
    else std::cout << " DEBUG (Re)initialize" << std::endl;
  }

  // init base object
  if (AtlasDetectorID::initialize_from_dictionary(dict_mgr)) return(1);

  // Register version of InnerDetector dictionary
  if (register_dict_tag(dict_mgr, "InnerDetector")) return(1);

  m_dict = dict_mgr.find_dictionary("InnerDetector");
  if (!m_dict) {
    if (m_msgSvc) log << MSG::FATAL << " PLR_ID::initialize_from_dict - cannot access InnerDetector dictionary "
                      << endmsg;
    else std::cout << " FATAL PLR_ID::initialize_from_dict - cannot access InnerDetector dictionary "
                   << std::endl;
    return(1);
  }

  AtlasDetectorID::setDictVersion(dict_mgr, "InnerDetector");

  // Initialize the field indices
  if (initLevelsFromDict()) return(1);

  // save indet id
  m_pixel_id = lumi();
  if (!is_lumi(m_pixel_id)) {
    if (m_msgSvc) log << MSG::FATAL << " PLR_ID::initialize_from_dict - cannot get plr id dictionary "
                      << endmsg;
    else std::cout << " FATAL PLR_ID::initialize_from_dict - cannot get plr id dictionary "
                   << std::endl;
    return(1);
  }

  //
  // Set barrel field for testing is_barrel
  //
  int barrel_value;
  m_barrel_field.clear();
  //  barrel
  if (m_dict->get_label_value("barrel_endcap", "barrel", barrel_value)) {
    if (m_msgSvc) log << MSG::FATAL << "Could not get value for label 'barrel' of field 'barrel_endcap' in dictionary "
                      << m_dict->m_name
                      << endmsg;
    else std::cout << " FATAL Could not get value for label 'barrel' of field 'barrel_endcap' in dictionary "
                   << m_dict->m_name
                   << std::endl;
    return(1);
  }
  m_barrel_field.add_value(barrel_value);
  m_barrel_field.add_value(barrel_value);
  if (m_msgSvc) log << MSG::DEBUG << " PLR_ID::initialize_from_dict "
                    << "Set barrel field values: "
                    << (std::string) m_barrel_field
                    << endmsg;
  else std::cout << " DEBUG PLR_ID::initialize_from_dict "
                 << "Set barrel field values: "
                 << (std::string) m_barrel_field
                 << std::endl;

  //DBM
  //Set dbm field for testing is_dbm
  //
  // WARNING:
  //   modified to skip DBM when aprorpiate dictionary is not present
  //   by adding +999 or -999 to the field
  //

  int dbm_value;
  m_dbm_field.clear();
  if (m_dict->get_label_value("barrel_endcap", "negative_dbm", dbm_value)) {
    if (m_dict->m_version.find("DBM") != std::string::npos) {
      if (m_msgSvc) log << MSG::WARNING <<
        "Could not get value for label 'negative_dbm' of field 'barrel_endcap' in dictionary "
                        << m_dict->m_name
                        << endmsg;
      else std::cout << " WARNING Could not get value for label 'negative_dbm' of field 'barrel_endcap' in dictionary "
                     << m_dict->m_name
                     << std::endl;
    }
    //return (1);
    m_dbm_field.add_value(-999);
  } else {
    m_dbm_field.add_value(dbm_value);
  }
  if (m_dict->get_label_value("barrel_endcap", "positive_dbm", dbm_value)) {
    if (m_dict->m_version.find("DBM") != std::string::npos) {
      if (m_msgSvc) log << MSG::WARNING <<
        "Could not get value for label 'positive_dbm' of field 'barrel_endcap' in dictionary "
                        << m_dict->m_name
                        << endmsg;
      else std::cout << " WARNING Could not get value for label 'positive_dbm' of field 'barrel_endcap' in dictionary "
                     << m_dict->m_name
                     << std::endl;
    }
    //return (1);
    m_dbm_field.add_value(999);
  } else {
    m_dbm_field.add_value(dbm_value);
  }
  if (m_msgSvc) log << MSG::DEBUG << " PLR_ID::initialize_from_dict "
                    << "Set dbm field values: "
                    << (std::string) m_dbm_field
                    << endmsg;
  else std::cout << " DEBUG PLR_ID::initialize_from_dict "
                 << "Set dbm field values: "
                 << (std::string) m_dbm_field
                 << std::endl;


  //
  // Build multirange for the valid set of identifiers
  //


  // Find value for the field InnerDetector
  const IdDictDictionary* atlasDict = dict_mgr.find_dictionary("ATLAS");
  int inDetField = -1;
  if (atlasDict->get_label_value("subdet", "InnerDetector", inDetField)) {
    if (m_msgSvc) log << MSG::FATAL << "Could not get value for label 'InnerDetector' of field 'subdet' in dictionary "
                      << atlasDict->m_name
                      << endmsg;
    else std::cout << " FATAL Could not get value for label 'InnerDetector' of field 'subdet' in dictionary "
                   << atlasDict->m_name
                   << std::endl;
    return(1);
  }

  // Find value for the field LuminosityDetectors
  int lumiField = -1;
  if (m_dict->get_label_value("part", "LuminosityDetectors", lumiField)) {
    if (m_msgSvc) log << MSG::FATAL << "Could not get value for label 'LuminosityDetectors' of field 'part' in dictionary "
                      << m_dict->m_name
                      << endmsg;
    else std::cout << " FATAL Could not get value for label 'LuminosityDetectors' of field 'part' in dictionary "
                   << m_dict->m_name
                   << std::endl;
    return(1);
  }

  // Find value for the field PLR
  int plrField = -1;
  if (m_dict->get_label_value("PLR_or_BCM", "PLR", plrField)) {
    if (m_msgSvc) log << MSG::FATAL << "Could not get value for label 'PLR' of field 'PLR_or_BCM' in dictionary "
                      << m_dict->m_name
                      << endmsg;
    else std::cout << " FATAL Could not get value for label 'PLR' of field 'PLR_or_BCM' in dictionary "
                   << m_dict->m_name
                   << std::endl;
    return(1);
  }

  if (m_msgSvc) log << MSG::DEBUG << " PLR_ID::initialize_from_dict "
                    << "Found field values: InDet/LuminosityDetectors/PLR "
                    << inDetField << "/"
                    << lumiField << "/"
                    << plrField
                    << endmsg;
  else std::cout << " DEBUG PLR_ID::initialize_from_dict "
                 << "Found field values: InDet/LuminosityDetectors/PLR "
                 << inDetField << "/"
                 << lumiField << "/"
                 << plrField
                 << std::endl;

  // Set up id for region and range prefix
  ExpandedIdentifier region_id;
  region_id.add(inDetField);
  region_id.add(lumiField);
  region_id.add(plrField);
  Range prefix;
  m_full_wafer_range = m_dict->build_multirange(region_id, prefix, "eta_module");
  m_full_pixel_range = m_dict->build_multirange(region_id, prefix);

  // Set the base identifier for PLR
  m_baseIdentifier = ((Identifier::value_type) 0);
  m_indet_impl.pack(indet_field_value(), m_baseIdentifier);
  m_lumi_impl.pack(lumi_field_value(), m_baseIdentifier);
  m_plr_impl.pack(plr_field_value(), m_baseIdentifier);

  // Set the base expanded identifier for PLR
  m_baseExpandedIdentifier << indet_field_value() << lumi_field_value() << plr_field_value();

  // Setup the hash tables
  if (init_hashes()) return(1);

  // Setup hash tables for finding neighbors
  if (init_neighbors()) return(1);

  if (m_msgSvc) {
    log << MSG::DEBUG << " PLR_ID::initialize_from_dict "
        << endmsg;
    log << MSG::DEBUG
        << "Wafer range -> " << (std::string) m_full_wafer_range
        << endmsg;
    log << MSG::DEBUG
        << "Pixel range -> " << (std::string) m_full_pixel_range
        << endmsg;
  } else {
    std::cout << " DEBUG PLR_ID::initialize_from_dict "
              << std::endl;
    std::cout << " DEBUG Wafer range -> " << (std::string) m_full_wafer_range
              << std::endl;
    std::cout << " DEBUG Pixel range -> " << (std::string) m_full_pixel_range
              << std::endl;
  }
  return 0;
}


int
PLR_ID::initLevelsFromDict(void) {
  MsgStream log(m_msgSvc, "PLR_ID");

  if (!m_dict) {
    if (m_msgSvc) log << MSG::FATAL << " PLR_ID::initLevelsFromDict - dictionary NOT initialized " << endmsg;
    else std::cout << " FATAL PLR_ID::initLevelsFromDict - dictionary NOT initialized " << std::endl;
    return(1);
  }

  // Find out which identifier field corresponds to each level. Use
  // names to find each field/leve.

  m_INDET_INDEX = 999;
  m_LUMI_INDEX = 999;
  m_PLR_INDEX = 999;
  m_BARREL_EC_INDEX = 999;
  m_LAYER_DISK_INDEX = 999;
  m_PHI_MODULE_INDEX = 999;
  m_ETA_MODULE_INDEX = 999;
  m_PHI_INDEX_INDEX = 999;
  m_ETA_INDEX_INDEX = 999;

  // Save index to a PIXEL region for unpacking
  ExpandedIdentifier id;
  id << indet_field_value() << lumi_field_value() << plr_field_value();
  if (m_dict->find_region(id, m_pixel_region_index)) {
    if (m_msgSvc) log << MSG::FATAL << "PLR_ID::initLevelsFromDict - unable to find pixel region index: id, reg "
                      << (std::string) id << " " << m_pixel_region_index
                      << endmsg;
    else std::cout << " FATAL PLR_ID::initLevelsFromDict - unable to find pixel region index: id, reg "
                   << (std::string) id << " " << m_pixel_region_index
                   << std::endl;
    return(1);
  }

  // Get levels
  IdDictField* field = m_dict->find_field("subdet");
  if (field) {
    m_INDET_INDEX = field->m_index;
  } else {
    if (m_msgSvc) log << MSG::FATAL << "PLR_ID::initLevelsFromDict - unable to find 'subdet' field "
                      << endmsg;
    else std::cout << " FATAL PLR_ID::initLevelsFromDict - unable to find 'subdet' field "
                   << std::endl;
    return(1);
  }

  field = m_dict->find_field("part");
  if (field) {
    m_LUMI_INDEX = field->m_index;
  } else {
    if (m_msgSvc) log << MSG::FATAL << "PLR_ID::initLevelsFromDict - unable to find 'part' field " << endmsg;
    else std::cout << " FATAL PLR_ID::initLevelsFromDict - unable to find 'part' field " << std::endl;
    return(1);
  }

  field = m_dict->find_field("PLR_or_BCM");
  if (field) {
    m_PLR_INDEX = field->m_index;
  } else {
    if (m_msgSvc) log << MSG::FATAL << "PLR_ID::initLevelsFromDict - unable to find 'PLR_or_BCM' field " << endmsg;
    else std::cout << " FATAL PLR_ID::initLevelsFromDict - unable to find 'PLR_or_BCM' field " << std::endl;
    return(1);
  }

  field = m_dict->find_field("barrel_endcap");
  if (field) {
    m_BARREL_EC_INDEX = field->m_index;
    m_BARREL_EC_INDEX = 3;
  } else {
    if (m_msgSvc) log << MSG::FATAL << "PLR_ID::initLevelsFromDict - unable to find 'barrel_endcap' field " << endmsg;
    else std::cout << " FATAL PLR_ID::initLevelsFromDict - unable to find 'barrel_endcap' field " << std::endl;
    return(1);
  }

  field = m_dict->find_field("layer");
  if (field) {
    m_LAYER_DISK_INDEX = field->m_index;
    m_LAYER_DISK_INDEX = 4;
  } else {
    if (m_msgSvc) log << MSG::FATAL << "PLR_ID::initLevelsFromDict - unable to find 'layer' field " << endmsg;
    else std::cout << " FATAL PLR_ID::initLevelsFromDict - unable to find 'layer' field " << std::endl;
    return(1);
  }

  field = m_dict->find_field("phi_module");
  if (field) {
    m_PHI_MODULE_INDEX = field->m_index;
    m_PHI_MODULE_INDEX = 5;
  } else {
    if (m_msgSvc) log << MSG::FATAL << "PLR_ID::initLevelsFromDict - unable to find 'phi_module' field " << endmsg;
    else std::cout << " FATAL PLR_ID::initLevelsFromDict - unable to find 'phi_module' field " << std::endl;
    return(1);
  }
  field = m_dict->find_field("eta_module");
  if (field) {
    m_ETA_MODULE_INDEX = field->m_index;
    m_ETA_MODULE_INDEX = 6;
  } else {
    if (m_msgSvc) log << MSG::FATAL << "PLR_ID::initLevelsFromDict - unable to find 'eta_module' field " << endmsg;
    else std::cout << " FATAL PLR_ID::initLevelsFromDict - unable to find 'eta_module' field " << std::endl;
    return(1);
  }
  field = m_dict->find_field("phi_index");
  if (field) {
    m_PHI_INDEX_INDEX = field->m_index;
    m_PHI_INDEX_INDEX = 7;
  } else {
    if (m_msgSvc) log << MSG::FATAL << "PLR_ID::initLevelsFromDict - unable to find 'phi_index' field " << endmsg;
    else std::cout << " FATAL PLR_ID::initLevelsFromDict - unable to find 'phi_index' field " << std::endl;
    return(1);
  }
  field = m_dict->find_field("eta_index");
  if (field) {
    m_ETA_INDEX_INDEX = field->m_index;
    m_ETA_INDEX_INDEX = 8;
  } else {
    if (m_msgSvc) log << MSG::FATAL << "PLR_ID::initLevelsFromDict - unable to find 'eta_index' field " << endmsg;
    else std::cout << " FATAL PLR_ID::initLevelsFromDict - unable to find 'eta_index' field " << std::endl;
    return(1);
  }

  // Set the field implementations: for bec, lay/disk, eta/phi mod
  // there are two kinds - shifted and non-shifted

  const IdDictRegion& region = *m_dict->m_regions[m_pixel_region_index];

  m_indet_impl = region.m_implementation[m_INDET_INDEX];
  m_lumi_impl = region.m_implementation[m_LUMI_INDEX];
  m_plr_impl = region.m_implementation[m_PLR_INDEX];
  m_bec_impl = region.m_implementation[m_BARREL_EC_INDEX];
  m_lay_disk_impl = region.m_implementation[m_LAYER_DISK_INDEX];
  m_phi_mod_impl = region.m_implementation[m_PHI_MODULE_INDEX];
  m_eta_mod_impl = region.m_implementation[m_ETA_MODULE_INDEX];
  m_phi_index_impl = region.m_implementation[m_PHI_INDEX_INDEX];
  m_eta_index_impl = region.m_implementation[m_ETA_INDEX_INDEX];

  if (m_msgSvc) {
    log << MSG::DEBUG << "decode index and bit fields for each level: " << endmsg;
    log << MSG::DEBUG << "indet          " << m_indet_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "lumi           " << m_lumi_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "plr            " << m_plr_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "bec            " << m_bec_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "bec_shift      " << m_bec_shift_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "lay_disk       " << m_lay_disk_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "lay_disk_shift " << m_lay_disk_shift_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "phi_mod        " << m_phi_mod_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "phi_mod_shift  " << m_phi_mod_shift_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "eta_mod        " << m_eta_mod_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "eta_mod_shift  " << m_eta_mod_shift_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "phi_index      " << m_phi_index_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "eta_index      " << m_eta_index_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "bec_eta_mod    " << m_bec_eta_mod_impl.show_to_string() << endmsg;
  } else {
    std::cout << " DEBUG decode index and bit fields for each level: " << std::endl;
    std::cout << " DEBUG indet          " << m_indet_impl.show_to_string() << std::endl;
    std::cout << " DEBUG lumi           " << m_lumi_impl.show_to_string() << std::endl;
    std::cout << " DEBUG plr            " << m_plr_impl.show_to_string() << std::endl;
    std::cout << " DEBUG bec            " << m_bec_impl.show_to_string() << std::endl;
    std::cout << " DEBUG bec_shift      " << m_bec_shift_impl.show_to_string() << std::endl;
    std::cout << " DEBUG lay_disk       " << m_lay_disk_impl.show_to_string() << std::endl;
    std::cout << " DEBUG lay_disk_shift " << m_lay_disk_shift_impl.show_to_string() << std::endl;
    std::cout << " DEBUG phi_mod        " << m_phi_mod_impl.show_to_string() << std::endl;
    std::cout << " DEBUG phi_mod_shift  " << m_phi_mod_shift_impl.show_to_string() << std::endl;
    std::cout << " DEBUG eta_mod        " << m_eta_mod_impl.show_to_string() << std::endl;
    std::cout << " DEBUG eta_mod_shift  " << m_eta_mod_shift_impl.show_to_string() << std::endl;
    std::cout << " DEBUG phi_index      " << m_phi_index_impl.show_to_string() << std::endl;
    std::cout << " DEBUG eta_index      " << m_eta_index_impl.show_to_string() << std::endl;
    std::cout << " DEBUG bec_eta_mod    " << m_bec_eta_mod_impl.show_to_string() << std::endl;
  }



  std::cout << "indet " << m_indet_impl.decode_index() << " "
            << (std::string) m_indet_impl.ored_field() << " "
            << std::hex << m_indet_impl.mask() << " "
            << m_indet_impl.zeroing_mask() << " "
            << std::dec << m_indet_impl.shift()
            << " " << m_indet_impl.bits() << " " << m_indet_impl.bits_offset() << " ";
  m_indet_impl.ored_field().show();
  std::cout << "lumi " << m_lumi_impl.decode_index() << " "
            << (std::string) m_lumi_impl.ored_field() << " "
            << std::hex << m_lumi_impl.mask() << " "
            << m_lumi_impl.zeroing_mask() << " "
            << std::dec << m_lumi_impl.shift()
            << " " << m_lumi_impl.bits() << " " << m_lumi_impl.bits_offset() << " ";
  m_lumi_impl.ored_field().show();
  std::cout << "plr " << m_plr_impl.decode_index() << " "
            << (std::string) m_plr_impl.ored_field() << " "
            << std::hex << m_plr_impl.mask() << " "
            << m_plr_impl.zeroing_mask() << " "
            << std::dec << m_plr_impl.shift()
            << " " << m_plr_impl.bits() << " " << m_plr_impl.bits_offset() << " ";
  m_plr_impl.ored_field().show();
  std::cout << "bec " << m_bec_impl.decode_index() << " "
            << (std::string) m_bec_impl.ored_field() << " "
            << std::hex << m_bec_impl.mask() << " "
            << m_bec_impl.zeroing_mask() << " "
            << std::dec << m_bec_impl.shift()
            << " " << m_bec_impl.bits() << " " << m_bec_impl.bits_offset() << " ";
  m_bec_impl.ored_field().show();
  std::cout << "bec_shift " << m_bec_shift_impl.decode_index() << " "
            << (std::string) m_bec_shift_impl.ored_field() << " "
            << std::hex << m_bec_shift_impl.mask() << " "
            << m_bec_shift_impl.zeroing_mask() << " "
            << std::dec << m_bec_shift_impl.shift()
            << " " << m_bec_shift_impl.bits() << " " << m_bec_shift_impl.bits_offset() << " ";
  m_bec_shift_impl.ored_field().show();
  std::cout << "lay_disk " << m_lay_disk_impl.decode_index() << " "
            << (std::string) m_lay_disk_impl.ored_field() << " "
            << std::hex << m_lay_disk_impl.mask() << " "
            << m_lay_disk_impl.zeroing_mask() << " "
            << std::dec << m_lay_disk_impl.shift()
            << " " << m_lay_disk_impl.bits() << " " << m_lay_disk_impl.bits_offset() << " ";
  m_lay_disk_impl.ored_field().show();
  std::cout << "lay_disk_shift " << m_lay_disk_shift_impl.decode_index() << " "
            << (std::string) m_lay_disk_shift_impl.ored_field() << " "
            << std::hex << m_lay_disk_shift_impl.mask() << " "
            << m_lay_disk_shift_impl.zeroing_mask() << " "
            << std::dec << m_lay_disk_shift_impl.shift()
            << " " << m_lay_disk_shift_impl.bits() << " " << m_lay_disk_shift_impl.bits_offset() << " ";
  m_lay_disk_shift_impl.ored_field().show();
  std::cout << "phi_mod " << m_phi_mod_impl.decode_index() << " "
            << (std::string) m_phi_mod_impl.ored_field() << " "
            << std::hex << m_phi_mod_impl.mask() << " "
            << m_phi_mod_impl.zeroing_mask() << " "
            << std::dec << m_phi_mod_impl.shift()
            << " " << m_phi_mod_impl.bits() << " " << m_phi_mod_impl.bits_offset() << " ";
  m_phi_mod_impl.ored_field().show();
  std::cout << "phi_mod_shift " << m_phi_mod_shift_impl.decode_index() << " "
            << (std::string) m_phi_mod_shift_impl.ored_field() << " "
            << std::hex << m_phi_mod_shift_impl.mask() << " "
            << m_phi_mod_shift_impl.zeroing_mask() << " "
            << std::dec << m_phi_mod_shift_impl.shift()
            << " " << m_phi_mod_shift_impl.bits() << " " << m_phi_mod_shift_impl.bits_offset() << " ";
  m_phi_mod_shift_impl.ored_field().show();
  std::cout << "eta_mod " << m_eta_mod_impl.decode_index() << " "
            << (std::string) m_eta_mod_impl.ored_field() << " "
            << std::hex << m_eta_mod_impl.mask() << " "
            << m_eta_mod_impl.zeroing_mask() << " "
            << std::dec << m_eta_mod_impl.shift()
            << " " << m_eta_mod_impl.bits() << " " << m_eta_mod_impl.bits_offset() << " ";
  m_eta_mod_impl.ored_field().show();
  std::cout << "eta_mod_shift " << m_eta_mod_shift_impl.decode_index() << " "
            << (std::string) m_eta_mod_shift_impl.ored_field() << " "
            << std::hex << m_eta_mod_shift_impl.mask() << " "
            << m_eta_mod_shift_impl.zeroing_mask() << " "
            << std::dec << m_eta_mod_shift_impl.shift()
            << " " << m_eta_mod_shift_impl.bits() << " " << m_eta_mod_shift_impl.bits_offset() << " ";
  m_eta_mod_shift_impl.ored_field().show();
  std::cout << "phi_index " << m_phi_index_impl.decode_index() << " "
            << (std::string) m_phi_index_impl.ored_field() << " "
            << std::hex << m_phi_index_impl.mask() << " "
            << m_phi_index_impl.zeroing_mask() << " "
            << std::dec << m_phi_index_impl.shift()
            << " " << m_phi_index_impl.bits() << " " << m_phi_index_impl.bits_offset() << " ";
  m_phi_index_impl.ored_field().show();
  std::cout << "eta_index " << m_eta_index_impl.decode_index() << " "
            << (std::string) m_eta_index_impl.ored_field() << " "
            << std::hex << m_eta_index_impl.mask() << " "
            << m_eta_index_impl.zeroing_mask() << " "
            << std::dec << m_eta_index_impl.shift()
            << " " << m_eta_index_impl.bits() << " " << m_eta_index_impl.bits_offset() << " ";
  m_eta_index_impl.ored_field().show();
  std::cout << "bec_eta_mod " << m_bec_eta_mod_impl.decode_index() << " "
            << (std::string) m_bec_eta_mod_impl.ored_field() << " "
            << std::hex << m_bec_eta_mod_impl.mask() << " "
            << m_bec_eta_mod_impl.zeroing_mask() << " "
            << std::dec << m_bec_eta_mod_impl.shift()
            << " " << m_bec_eta_mod_impl.bits() << " " << m_bec_eta_mod_impl.bits_offset() << " ";
  m_bec_eta_mod_impl.ored_field().show();


  std::cout << "PLR_ID::initLevelsFromDict - found levels " << std::endl;
  std::cout << "subdet        " << m_INDET_INDEX << std::endl;
  std::cout << "part          " << m_LUMI_INDEX << std::endl;
  std::cout << "plr           " << m_PLR_INDEX << std::endl;
  std::cout << "barrel_endcap " << m_BARREL_EC_INDEX << std::endl;
  std::cout << "layer or disk " << m_LAYER_DISK_INDEX << std::endl;
  std::cout << "phi_module    " << m_PHI_MODULE_INDEX << std::endl;
  std::cout << "eta_module    " << m_ETA_MODULE_INDEX << std::endl;
  std::cout << "phi_index     " << m_PHI_INDEX_INDEX << std::endl;
  std::cout << "eta_index     " << m_ETA_INDEX_INDEX << std::endl;

  return(0);
}
