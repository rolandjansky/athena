/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DocumentationRecord_H
#define DocumentationRecord_H

// DocumentationRecord : light-weight class to keep on-line documentation for the parent object
//

#include <string>
#include <vector>

namespace FADS {

class DocumentationRecord {
private:
	std::vector<std::string> m_theDoc;
public:
	inline void AddDocumentationLine(std::string);
	inline void PrintDocumentation();
};

}	// end namespace

#endif
