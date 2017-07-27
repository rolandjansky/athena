#include "MuonEventTPCnv/CreateTransientTemplates.h"
#include <iostream>
#include <signal.h>

using namespace TPCnv;

std::unique_ptr<Muon::MuonCoinDataContainer<Muon::MuonCoinDataCollection<Muon::TgcCoinData> >>
  CreateTransient<Muon::MuonCoinDataContainer<Muon::MuonCoinDataCollection<Muon::TgcCoinData> >>::create() {
  std::cout << "Override CreateTransient in the calling class InDetEventAthenaPool/" << __FILE__ << " " << __LINE__ << std::endl;
  raise(SIGSEGV);
 return nullptr; }

std::unique_ptr<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::TgcPrepData> >>
  CreateTransient<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::TgcPrepData> >>::create() {
  std::cout << "Override CreateTransient in the calling class InDetEventAthenaPool/" << __FILE__ << " " << __LINE__ << std::endl;
  raise(SIGSEGV);
 return nullptr; }

std::unique_ptr<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::CscPrepData> >>
  CreateTransient<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::CscPrepData> >>::create() { 
  std::cout << "Override CreateTransient in the calling class InDetEventAthenaPool/" << __FILE__ << " " << __LINE__ << std::endl;
  raise(SIGSEGV);
return nullptr; }

std::unique_ptr<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::CscStripPrepData> >>
  CreateTransient<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::CscStripPrepData> >>::create() {  std::cout << "Override CreateTransient in the calling class InDetEventAthenaPool/" << __FILE__ << " " << __LINE__ << std::endl;
  raise(SIGSEGV);
 return nullptr; }

std::unique_ptr<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::MdtPrepData> >>
  CreateTransient<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::MdtPrepData> >>::create() {   std::cout << "Override CreateTransient in the calling class InDetEventAthenaPool/" << __FILE__ << " " << __LINE__ << std::endl;
  raise(SIGSEGV);
return nullptr; }

std::unique_ptr<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::RpcPrepData> >>
  CreateTransient<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::RpcPrepData> >>::create() {
  std::cout << "Override CreateTransient in the calling class InDetEventAthenaPool/" << __FILE__ << " " << __LINE__ << std::endl;
  raise(SIGSEGV);
 return nullptr; }

std::unique_ptr<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::MMPrepData> >>
  CreateTransient<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::MMPrepData> >>::create() {   std::cout << "Override CreateTransient in the calling class InDetEventAthenaPool/" << __FILE__ << " " << __LINE__ << std::endl;
  raise(SIGSEGV);
return nullptr; }

std::unique_ptr<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::RpcCoinData> >>
  CreateTransient<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::RpcCoinData> >>::create() {  std::cout << "Override CreateTransient in the calling class InDetEventAthenaPool/" << __FILE__ << " " << __LINE__ << std::endl;
  raise(SIGSEGV);
 return nullptr; }

std::unique_ptr<MmDigitContainer>
  CreateTransient<MmDigitContainer>::create() {  std::cout << "Override CreateTransient in the calling class InDetEventAthenaPool/" << __FILE__ << " " << __LINE__ << std::endl;
  raise(SIGSEGV);
 return nullptr; }

std::unique_ptr<sTgcDigitContainer>
  CreateTransient<sTgcDigitContainer>::create() {   std::cout << "Override CreateTransient in the calling class InDetEventAthenaPool/" << __FILE__ << " " << __LINE__ << std::endl;
  raise(SIGSEGV);
return nullptr; }

std::unique_ptr<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::sTgcPrepData> >>
  CreateTransient<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::sTgcPrepData> >>::create() {
  std::cout << "Override CreateTransient in the calling class InDetEventAthenaPool/" << __FILE__ << " " << __LINE__ << std::endl;
  raise(SIGSEGV);
  return nullptr; }

std::unique_ptr<Muon::MuonCoinDataContainer<::Muon::MuonCoinDataCollection<::Muon::RpcCoinData> >>
  CreateTransient<Muon::MuonCoinDataContainer<::Muon::MuonCoinDataCollection<::Muon::RpcCoinData> >>::create() { 
  std::cout << "Override CreateTransient in the calling class InDetEventAthenaPool/" << __FILE__ << " " << __LINE__ << std::endl;
  raise(SIGSEGV);
  return nullptr; }

