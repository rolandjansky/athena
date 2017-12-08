/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsUtilities/DocumentationRecord.h"
#include <iostream>

namespace FADS {

void DocumentationRecord::AddDocumentationLine(std::string record)
{
	m_theDoc.push_back(record);
}
void DocumentationRecord::PrintDocumentation()
{
	if (m_theDoc.size()) 
		for (unsigned int i=0;i<m_theDoc.size();i++)
			std::cout<<"--- "<<m_theDoc[i]<<std::endl;
	else
		std::cout<<"Sorry - no documentation is available "<<std::endl;
}

}	// end namespace
