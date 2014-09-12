/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FileCatalog/FCException.h"
namespace pool{
  FCduplicatePFNException::FCduplicatePFNException(const std::string& from):pool::Exception(  std::string("PFN is not unique"), std::string("pool::FC::")+from,
	      std::string("FileCatalog")){}

  FCduplicateLFNException::FCduplicateLFNException(const std::string& from):pool::Exception(
		std::string("LFN is not unique"),
                std::string("pool::FC::")+from,
		std::string("FileCatalog")){}

   FCnonexistentFileException::FCnonexistentFileException(const std::string& from, const std::string& fileID):pool::Exception(
		std::string("non-existent file: "+fileID),
                std::string("pool::FC::")+from,
		std::string("FileCatalog")){}
  
  FCconnectionException:: FCconnectionException(const std::string& from , const std::string& backendmessage):pool::Exception( std::string(backendmessage), std::string("pool::FC::")+from, std::string("FileCatalog")){}

  FCbackendException::FCbackendException(const std::string& from, const std::string& backendmessage):pool::Exception(
		std::string(backendmessage),
                std::string("pool::FC::")+from,
		std::string("FileCatalog")){
    }

  FCillegalContactStringException::FCillegalContactStringException(const std::string& from, const std::string& message):pool::Exception(
                                        std::string( message),
					std::string("pool::FC::")+from,
					std::string("FileCatalog")){}

  FCinconsistentSchemaException::FCinconsistentSchemaException(const std::string& from):pool::Exception( std::string("FC inconsistent schema "),std::string("pool::FC::")+from, std::string("FileCatalog")){}

  FCTransactionException::FCTransactionException(const std::string& from, const std::string& message):pool::Exception( std::string("FC transaction error")+message,std::string("pool::FC::")+from, std::string("FileCatalog")){}

  FCstringLimitException::FCstringLimitException(const std::string& from, const std::string& str):pool::Exception(  std::string("string Limit exceeded 250 ")+str, std::string("pool::FC::")+from, std::string("FileCatalog")) {}
  
  FCduplicatemetadataspecException::FCduplicatemetadataspecException(const std::string& from):pool::Exception(  std::string("metadata definition already exists "), std::string("pool::FC::")+from, std::string("FileCatalog")) {}
  FCduplicatemetadataException::FCduplicatemetadataException(const std::string& from):pool::Exception(  std::string("guid-metadata entry already exists "), std::string("pool::FC::")+from, std::string("FileCatalog")) {}
  FCnonexistentMetaDataException::FCnonexistentMetaDataException(const std::string& from):pool::Exception(  std::string("metadata catalog doesnot exist "), std::string("pool::FC::")+from, std::string("FileCatalog")) {}
}//ns pool
