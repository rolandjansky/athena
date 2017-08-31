#include "CreateTransientTemplate.h"
#include <iostream>
#include <signal.h>

using namespace TPCnv;

std::unique_ptr<InDetRawDataContainer<InDetRawDataCollection<PixelRDORawData> >>
  CreateTransient<InDetRawDataContainer<InDetRawDataCollection<PixelRDORawData> >>::create() { 
  std::cout << "Override CreateTransient in the calling class InDetEventAthenaPool/" << __FILE__ << " " << __LINE__ << std::endl;
  raise(SIGSEGV);
return nullptr; }


std::unique_ptr<InDetRawDataContainer<InDetRawDataCollection<TRT_RDORawData> >>
  CreateTransient<InDetRawDataContainer<InDetRawDataCollection<TRT_RDORawData> >>::create() { 
  std::cout << "Override CreateTransient in the calling class InDetEventAthenaPool/" << __FILE__ << " " << __LINE__ << std::endl;
  raise(SIGSEGV);
  return nullptr; }

std::unique_ptr<InDetRawDataContainer<InDetRawDataCollection<SCT_RDORawData> >>
  CreateTransient<InDetRawDataContainer<InDetRawDataCollection<SCT_RDORawData> >>::create() { 
  std::cout << "Override CreateTransient in the calling class InDetEventAthenaPool/" << __FILE__ << " " << __LINE__ << std::endl;
  raise(SIGSEGV);
  return nullptr; }


