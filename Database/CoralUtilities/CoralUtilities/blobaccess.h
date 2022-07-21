/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CORALUTILITIES_BLOBACCESS_H
#define CORALUTILITIES_BLOBACCESS_H

#include <string>
#include <memory>
#include "nlohmann/json.hpp"

class TTree;
class TMemFile;

namespace coral {
	class Blob;
}

namespace CoralUtilities {

	bool compressBlob        (const char*           , coral::Blob&);
	bool writeBlobFromString (const std::string_view, coral::Blob&);
	bool writeBlobFromJson   (const nlohmann::json& , coral::Blob&);
	bool writeBlobFromTTree  (      TTree*          , coral::Blob&);

	bool uncompressBlob      (const coral::Blob&, std::unique_ptr<unsigned char[]>&, unsigned long&);
	bool readBlobAsString    (const coral::Blob&, std::string&                   );
	bool readBlobAsJson      (const coral::Blob&, nlohmann::json&                );
	bool readBlobAsTTree     (const coral::Blob&, std::unique_ptr<TTree>& );

}

#endif
