/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1CTPFiles.h"

const std::map<TrigConf::L1CTPFiles::MuctpiAccess, std::string> TrigConf::L1CTPFiles::s_keyMap = {
   { RoiMaskA, "roimasking_lut_A" },
   { RoiMaskC, "roimasking_lut_C" },
   { PtLutBarrel, "pt_lut_BA" },
   { PtLutEndcap, "pt_lut_EF" }
};

TrigConf::L1CTPFiles::L1CTPFiles() {
   for(auto x : s_keyMap) {
      m_muctpi[x.second];
   }
   m_muctpi[s_keyMap.at(RoiMaskA)];
   m_muctpi[s_keyMap.at(RoiMaskC)];
   m_muctpi[s_keyMap.at(PtLutBarrel)];
   m_muctpi[s_keyMap.at(PtLutEndcap)];
}

TrigConf::L1CTPFiles::~L1CTPFiles() = default;

void
TrigConf::L1CTPFiles::print() const
{
   std::cout << "CTP Files (" << (m_hasCompleteCtpData? "complete" : "incomplete") << ")" << std::endl;
   std::cout << "   LUT: " << m_Ctpcore_LUT.size() << std::endl;
   std::cout << "   CAM: " << m_Ctpcore_CAM.size() << std::endl;
   std::cout << "   CTPIN Mon Sel 7: " << m_Ctpin_MonSelector_Slot7.size() << std::endl;
   std::cout << "   CTPIN Mon Sel 8: " << m_Ctpin_MonSelector_Slot8.size() << std::endl;
   std::cout << "   CTPIN Mon Sel 9: " << m_Ctpin_MonSelector_Slot9.size() << std::endl;
   std::cout << "   CTPIN Mon Dec 7: " << m_Ctpin_MonDecoder_Slot7.size() << std::endl;
   std::cout << "   CTPIN Mon Dec 8: " << m_Ctpin_MonDecoder_Slot8.size() << std::endl;
   std::cout << "   CTPIN Mon Dec 9: " << m_Ctpin_MonDecoder_Slot9.size() << std::endl;
   std::cout << "   CTPMON Mon Sel: " << m_Ctpmon_MonSelector.size() << std::endl;
   std::cout << "   CTPMON Mon Dec: " << m_Ctpmon_MonDecoder.size() << std::endl;
   std::cout << "   CTPMON DMX: " << m_Ctpmon_DMX.size() << std::endl;
   std::cout << "CTP SMX (" << (m_hasCompleteSmxData? "complete" : "incomplete") << ")" << std::endl;
   std::cout << "   Output: " << m_Smx_Output.size() << std::endl;
   std::cout << "   VHDL 7: " << m_Smx_Vhdl_Slot7.size() << std::endl;
   std::cout << "   VHDL 8: " << m_Smx_Vhdl_Slot8.size() << std::endl;
   std::cout << "   VHDL 9: " << m_Smx_Vhdl_Slot9.size() << std::endl;
   std::cout << "   SVFI 7: " << m_Smx_Svfi_Slot7.size() << std::endl;
   std::cout << "   SVFI 8: " << m_Smx_Svfi_Slot8.size() << std::endl;
   std::cout << "   SVFI 9: " << m_Smx_Svfi_Slot9.size() << std::endl;
   std::cout << "MUCTPI Files (" << (m_hasCompleteMuctpiData? "complete" : "incomplete") << ")" << std::endl;
   std::cout << "   required:" << std::endl;
   std::cout << "      RoiMask A: " << m_muctpi.at("roimasking_lut_A").size() << std::endl;
   std::cout << "      RoiMask C: " << m_muctpi.at("roimasking_lut_C").size() << std::endl;
   std::cout << "      pt Barrel: " << m_muctpi.at("pt_lut_BA").size() << std::endl;
   std::cout << "      pt EC/FW : " << m_muctpi.at("pt_lut_EF").size() << std::endl;
   std::cout << "   extra pt lut:" << std::endl;
   for(auto x: m_muctpi_Extra_Ptlut) {
      std::cout << "     " << x.first << ": " << x.second.size() << std::endl;
   }
   std::cout << "   nbits: " << m_muctpi_Nbits.size() << std::endl;
   std::cout << "TMC information (" << (m_hasCompleteTmcData? "complete" : "incomplete") << ")" << std::endl;
   std::cout << "   CTPCore inputs    : " << m_Tmc_CtpcoreInputs.size() << std::endl;
   std::cout << "   CTPIN map entries : " << m_Tmc_CtpinCounters.size() << std::endl;
   std::cout << "   CTPMON map entries: " << m_Tmc_CtpmonCounters.size() << std::endl;
}

/**
 * Accessors
 **/
bool
TrigConf::L1CTPFiles::hasCompleteCtpData() const { 
   return m_hasCompleteCtpData;
}

bool
TrigConf::L1CTPFiles::hasCompleteSmxData() const { 
   return m_hasCompleteSmxData;
}

bool
TrigConf::L1CTPFiles::hasCompleteMuctpiData() const { 
   return m_hasCompleteMuctpiData;
}

bool
TrigConf::L1CTPFiles::hasCompleteTmcData() const { 
   return m_hasCompleteTmcData;
}

const std::vector<uint32_t> &
TrigConf::L1CTPFiles::ctpcore_LUT() const {
   return m_Ctpcore_LUT;
}

const std::vector<uint32_t> &
TrigConf::L1CTPFiles::ctpcore_CAM() const {
   return m_Ctpcore_CAM;
}

const std::vector<uint32_t> &
TrigConf::L1CTPFiles::ctpin_MonSelector_Slot7() const {
   return m_Ctpin_MonSelector_Slot7;

}

const std::vector<uint32_t> &
TrigConf::L1CTPFiles::ctpin_MonSelector_Slot8() const {
   return m_Ctpin_MonSelector_Slot8;
}

const std::vector<uint32_t> &
TrigConf::L1CTPFiles::ctpin_MonSelector_Slot9() const {
   return m_Ctpin_MonSelector_Slot9;
}

const std::vector<uint32_t> &
TrigConf::L1CTPFiles::ctpin_MonDecoder_Slot7() const {
   return m_Ctpin_MonDecoder_Slot7;
}

const std::vector<uint32_t> &
TrigConf::L1CTPFiles::ctpin_MonDecoder_Slot8() const {
   return m_Ctpin_MonDecoder_Slot8;
}

const std::vector<uint32_t> &
TrigConf::L1CTPFiles::ctpin_MonDecoder_Slot9() const {
   return m_Ctpin_MonDecoder_Slot9;
}

const std::vector<uint32_t> &
TrigConf::L1CTPFiles::ctpmon_MonSelector() const {
   return m_Ctpmon_MonSelector;
}

const std::vector<uint32_t> &
TrigConf::L1CTPFiles::ctpmon_MonDecoder() const {
   return m_Ctpmon_MonDecoder;
}

const std::vector<uint32_t> &
TrigConf::L1CTPFiles::ctpmon_DMX() const {
   return m_Ctpmon_DMX;
}

const std::string &
TrigConf::L1CTPFiles::smx_Output() const {
   return m_Smx_Output;
}

const std::string &
TrigConf::L1CTPFiles::smx_Vhdl_Slot7() const {
   return m_Smx_Vhdl_Slot7;
}

const std::string &
TrigConf::L1CTPFiles::smx_Vhdl_Slot8() const {
   return m_Smx_Vhdl_Slot8;
}

const std::string &
TrigConf::L1CTPFiles::smx_Vhdl_Slot9() const {
   return m_Smx_Vhdl_Slot9;
}

const std::string &
TrigConf::L1CTPFiles::smx_Svfi_Slot7() const {
   return m_Smx_Svfi_Slot7;
}

const std::string &
TrigConf::L1CTPFiles::smx_Svfi_Slot8() const {
   return m_Smx_Svfi_Slot8;
}

const std::string &
TrigConf::L1CTPFiles::smx_Svfi_Slot9() const {
   return m_Smx_Svfi_Slot9;
}

const std::vector<uint32_t> &
TrigConf::L1CTPFiles::muctpiRoi(MuctpiAccess key) const {
   return m_muctpi.at(s_keyMap.at(key));
}

const std::vector<uint32_t> &
TrigConf::L1CTPFiles::muctpi_Extra_Ptlut(const std::string & sector) const {
   return m_muctpi_Extra_Ptlut.at(sector);
}

const std::vector<uint32_t> &
TrigConf::L1CTPFiles::muctpi_Nbits() const {
   return m_muctpi_Nbits;
}

const std::vector<TrigConf::L1CTPFiles::CTPCoreInput> &
TrigConf::L1CTPFiles::tmc_CtpcoreInputs() const {
   return m_Tmc_CtpcoreInputs;
}

const std::map<std::string, size_t> &
TrigConf::L1CTPFiles::tmc_CtpinCounters() const {
   return m_Tmc_CtpinCounters;
}

const std::map<std::string, size_t> &
TrigConf::L1CTPFiles::tmc_CtpmonCounters() const {
   return m_Tmc_CtpmonCounters;
}


/**
 * Setters
 **/
void
TrigConf::L1CTPFiles::set_HasCompleteCtpData(bool flag) {
   m_hasCompleteCtpData = flag;
}

void
TrigConf::L1CTPFiles::set_HasCompleteSmxData(bool flag) {
   m_hasCompleteSmxData = flag;
}

void
TrigConf::L1CTPFiles::set_HasCompleteMuctpiData(bool flag) {
   m_hasCompleteMuctpiData = flag;
}

void
TrigConf::L1CTPFiles::set_HasCompleteTmcData(bool flag) {
   m_hasCompleteTmcData = flag;
}

void
TrigConf::L1CTPFiles::set_Ctpcore_LUT(std::vector<uint32_t> data) {
   m_Ctpcore_LUT = std::move(data);
}

void
TrigConf::L1CTPFiles::set_Ctpcore_CAM(std::vector<uint32_t> data) {
   m_Ctpcore_CAM = std::move(data);
}

void
TrigConf::L1CTPFiles::set_Ctpin_MonSelector_Slot7(std::vector<uint32_t> data) {
   m_Ctpin_MonSelector_Slot7 = std::move(data);
}

void
TrigConf::L1CTPFiles::set_Ctpin_MonSelector_Slot8(std::vector<uint32_t> data) {
   m_Ctpin_MonSelector_Slot8 = std::move(data);
}

void
TrigConf::L1CTPFiles::set_Ctpin_MonSelector_Slot9(std::vector<uint32_t> data) {
   m_Ctpin_MonSelector_Slot9 = std::move(data);
}

void
TrigConf::L1CTPFiles::set_Ctpin_MonDecoder_Slot7(std::vector<uint32_t> data) {
   m_Ctpin_MonDecoder_Slot7 = std::move(data);
}

void
TrigConf::L1CTPFiles::set_Ctpin_MonDecoder_Slot8(std::vector<uint32_t> data) {
   m_Ctpin_MonDecoder_Slot8 = std::move(data);
}

void
TrigConf::L1CTPFiles::set_Ctpin_MonDecoder_Slot9(std::vector<uint32_t> data) {
   m_Ctpin_MonDecoder_Slot9 = std::move(data);
}

void
TrigConf::L1CTPFiles::set_Ctpmon_MonSelector(std::vector<uint32_t> data) {
   m_Ctpmon_MonSelector = std::move(data);
}

void
TrigConf::L1CTPFiles::set_Ctpmon_MonDecoder(std::vector<uint32_t> data) {
   m_Ctpmon_MonDecoder = std::move(data);
}

void
TrigConf::L1CTPFiles::set_Ctpmon_DMX(std::vector<uint32_t> data) {
   m_Ctpmon_DMX = std::move(data);
}

void
TrigConf::L1CTPFiles::set_Smx_Output(const std::string & data) { 
   m_Smx_Output = data;
}

void
TrigConf::L1CTPFiles::set_Smx_Vhdl_Slot7(const std::string & data) { 
   m_Smx_Vhdl_Slot7 = data;
}

void
TrigConf::L1CTPFiles::set_Smx_Vhdl_Slot8(const std::string & data) { 
   m_Smx_Vhdl_Slot8 = data;
}

void
TrigConf::L1CTPFiles::set_Smx_Vhdl_Slot9(const std::string & data) { 
   m_Smx_Vhdl_Slot9 = data;
}

void
TrigConf::L1CTPFiles::set_Smx_Svfi_Slot7(const std::string & data) { 
   m_Smx_Svfi_Slot7 = data;
}

void
TrigConf::L1CTPFiles::set_Smx_Svfi_Slot8(const std::string & data) { 
   m_Smx_Svfi_Slot8 = data;
}

void
TrigConf::L1CTPFiles::set_Smx_Svfi_Slot9(const std::string & data) { 
   m_Smx_Svfi_Slot9 = data;
}

void
TrigConf::L1CTPFiles::set_Muctpi(MuctpiAccess key, std::vector<uint32_t> data) {
   m_muctpi[s_keyMap.at(key)] = std::move(data);
}

void
TrigConf::L1CTPFiles::set_Muctpi_Extra_Ptlut(const std::string & key, std::vector<uint32_t> data) {
   m_muctpi_Extra_Ptlut[key] = std::move(data);
}

void
TrigConf::L1CTPFiles::set_Muctpi_Nbits(std::vector<uint32_t> data) {
   m_muctpi_Nbits = std::move(data);
}

void
TrigConf::L1CTPFiles::set_Tmc_CtpcoreInputs(std::vector<TrigConf::L1CTPFiles::CTPCoreInput> data) {
   m_Tmc_CtpcoreInputs = std::move(data);
}

void
TrigConf::L1CTPFiles::set_Tmc_CtpinCounters(std::map<std::string, size_t> data) {
   m_Tmc_CtpinCounters = std::move(data);
}

void
TrigConf::L1CTPFiles::set_Tmc_CtpmonCounters(std::map<std::string, size_t> data) {
   m_Tmc_CtpmonCounters = std::move(data);
}
