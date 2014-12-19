/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// specification of the COOL folder format
// author: Joerg Stelzer stelzer@cern.ch
// author: Karsten Koenecke karsten.koenecke@cern.ch

#include "HLTRates2COOL/HLTCoolFolderSpec.h"
#include "HLTRates2COOL/HLTCounter.h"
#include "HLTRates2COOL/HLTCountCoolWriter.h"

#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <iterator>
#include <functional>
#include <algorithm>

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Blob.h"

#include "CoolKernel/FolderVersioning.h"
#include "CoolKernel/FolderSpecification.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/StorageType.h"
#include "CoolKernel/Record.h"



// ------------------------------------------------------------
// createLvl1MenuFolderSpecification
// ------------------------------------------------------------
cool::RecordSpecification
hltca::HLTCoolFolderSpec::getHLTCountersFolderSpecification()
{
  cool::RecordSpecification spec;
  spec.extend( "Format", cool::StorageType::UChar );  
  spec.extend( "Data" ,  cool::StorageType::Blob64k );
  return spec;
}


cool::IFolderPtr
hltca::HLTCoolFolderSpec::getL2CountersFolder(cool::IDatabasePtr db)
{
  return db->getFolder( "/TRIGGER/LUMI/LVL2COUNTERS");
}


cool::IFolderPtr
hltca::HLTCoolFolderSpec::getEFCountersFolder(cool::IDatabasePtr db)
{
  return db->getFolder( "/TRIGGER/LUMI/EFCOUNTERS");
}





// ------------------------------------------------------------
// createFolderStructure()
// ------------------------------------------------------------
cool::IFolderSetPtr
hltca::HLTCoolFolderSpec::createFolderStructure(cool::IDatabasePtr db)
{
  
   //std::string singleChannelDesc = "<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"40774348\" /></addrHeader><typeName>AthenaAttributeList</typeName>";
   std::string multiChannelDesc  = "<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>";

   cool::IFolderSetPtr topFolder;
   if( !db->existsFolderSet( "/TRIGGER" ) ) {
      topFolder = db->createFolderSet( "/TRIGGER" );
   }

   if( !db->existsFolderSet( "/TRIGGER/LUMI" ) ) {
      db->createFolderSet( "/TRIGGER/LUMI" );
   }

   if( !db->existsFolder( "/TRIGGER/LUMI/EFCOUNTERS" ) ) {
      cool::IFolderPtr ptr = db->createFolder( "/TRIGGER/LUMI/EFCOUNTERS", cool::FolderSpecification(getHLTCountersFolderSpecification()));
      ptr->setDescription(multiChannelDesc);
   }

   if( !db->existsFolder( "/TRIGGER/LUMI/LVL2COUNTERS" ) ) {
      cool::IFolderPtr ptr = db->createFolder( "/TRIGGER/LUMI/LVL2COUNTERS", cool::FolderSpecification(getHLTCountersFolderSpecification()));
      ptr->setDescription(multiChannelDesc);
   }

   return topFolder;
}


// ------------------------------------------------------------
// printFolderStructure(cool::IDatabasePtr db)
// ------------------------------------------------------------
void
hltca::HLTCoolFolderSpec::printFolderStructure(cool::IDatabasePtr db, std::ostream & o)
{

   std::vector<std::string> nodes = db->listAllNodes();

   std::vector<std::string>::const_iterator fIt = nodes.begin();
   for(;fIt != nodes.end();fIt++) {
      bool isFolderSet = db->existsFolderSet(*fIt);
      o << "        " << *fIt;
      if(isFolderSet) {
         o << "/" << std::endl;
      }
      else {
         cool::IFolderPtr folder = db->getFolder(*fIt);
         bool isMultiVersion = (folder->versioningMode() == cool::FolderVersioning::MULTI_VERSION);
         // const cool::IRecord & att = folder->folderAttributes();
         // att.print(o);
         const cool::IRecordSpecification & rspec = folder->payloadSpecification();


         o << " (" << (isMultiVersion?"Multiversion":"Singleversion") << ")" << std::endl;

         o.setf(std::ios::left);
         for(uint i=0;i<rspec.size(); i++)
            {
               o << "            " << std::setw(30) << rspec[i].name() << "         (" << rspec[i].storageType().name() << ")" << std::endl;
            }
      } // End if(isFolderSet)/else

   }
   return;
}



cool::Record
hltca::HLTCoolFolderSpec::createHLTCountersPayload(unsigned char format, const std::vector<hltca::HLTCounter>& counters, hltca::HLTCounter::TriggerLevel level)
{

   cool::Record payload = getHLTCountersFolderSpecification();

   // fill the payload
   payload["Format"].setValue<cool::UChar>(format);
   cool::Blob64k blob;

   // pack the counters
   pack(format, counters, blob, level);

   payload["Data"].setValue<cool::Blob64k>(blob);


   return payload;
}

void
hltca::HLTCoolFolderSpec::readHLTCountersPayload(const cool::IRecord & payload,
                                                 std::vector<hltca::HLTCounter>& counters, 
                                                 hltca::HLTCounter::TriggerLevel level)
{

   unsigned char packingFormat = payload["Format"].data<cool::UChar>();
   cool::Blob64k blob = payload["Data"].data<cool::Blob64k>();
   unpack(packingFormat, blob, counters, level);
}

    
namespace {
   struct levelfilter {
      typedef hltca::HLTCounter               first_argument_type;
      typedef hltca::HLTCounter::TriggerLevel second_argument_type;
      typedef bool                            result_type;
      bool operator()(const hltca::HLTCounter& x, const hltca::HLTCounter::TriggerLevel& level) const {
         return x.trlevel()==level;
      }
   };
}

void
hltca::HLTCoolFolderSpec::pack(unsigned char format,
                               const std::vector<hltca::HLTCounter>& counters,
                               cool::Blob64k& blob,
                               hltca::HLTCounter::TriggerLevel level)
{

   int n_lvl = std::count_if(counters.begin(), counters.end(), std::bind2nd(levelfilter(),level));

   std::cout << "Found " << n_lvl << " entries for level " << level << std::endl;

   if(format==1) {
      
      const unsigned int NData = 5;

      // required space
      blob.resize( NData * sizeof(unsigned int) * n_lvl ); // NData unsigned integers per chain in this level

      // starting point
      unsigned int* p = static_cast<unsigned int*>(blob.startingAddress());

      levelfilter lf;
      for (std::vector<HLTCounter>::const_iterator c = counters.begin(); c != counters.end(); c++ ) {
         
         if (! lf(*c,level)) continue;
         *(p++) = (*c).chain_counter();
         *(p++) = (*c).input();
         *(p++) = (*c).after_prescale();
         *(p++) = (*c).raw();
         *(p++) = (*c).total();
      }
   }
}

void
hltca::HLTCoolFolderSpec::unpack(unsigned char format,
                                 const cool::Blob64k& blob,
                                 std::vector<hltca::HLTCounter>& counters,
                                 hltca::HLTCounter::TriggerLevel level)
{

   std::cout << "format " << (unsigned int)format << std::endl;
   std::cout << "blob size " << blob.size() << std::endl;


   // don't clear the counters vector, new HLTCounters will be appended
   if(format==1) {

      const unsigned int NData = 5;

      int n_lvl = blob.size() / ( NData * sizeof(unsigned int) ); // 4 unsigned integers per chain in this level
      const unsigned int* p = static_cast<const unsigned int*>(blob.startingAddress());

      for(int i=0; i<n_lvl; i++) {
         unsigned int chain_counter  = *(p++);
         unsigned int input          = *(p++);       
         unsigned int after_prescale = *(p++);
         unsigned int raw            = *(p++);
         unsigned int total          = *(p++);
         counters.push_back(HLTCounter(chain_counter, input, after_prescale, raw, total, level));
      }
   }
}
