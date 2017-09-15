#include "MuonEventAthenaPool/CreateTransientTemplates.h"
#include <iostream>
#include <signal.h>

using namespace TPCnv;

std::unique_ptr<RpcPadContainer>
  CreateTransient<RpcPadContainer>::create() { 
  std::cout << "Override CreateTransient in the calling class MuonEventAthenaPool/" << __FILE__ << " " << __LINE__ << std::endl;
  raise(SIGSEGV);
return nullptr; }



std::unique_ptr<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::TgcCoinData> >>
  CreateTransient<Muon::MuonPrepDataContainer<Muon::MuonPrepDataCollection<Muon::TgcCoinData> >>::create() {
  std::cout << "Override CreateTransient in the calling class MuonEventAthenaPool/" << __FILE__ << " " << __LINE__ << std::endl;
  raise(SIGSEGV);
 return nullptr; }


