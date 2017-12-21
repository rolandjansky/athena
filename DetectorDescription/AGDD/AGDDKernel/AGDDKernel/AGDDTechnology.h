/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDTechnology_H
#define AGDDTechnology_H

#include <string>

// abstract base class for detector technology classes

//namespace AGDDModel {

class AGDDTechnology {
protected:
	std::string m_name;
public:

	AGDDTechnology(std::string s);
	virtual ~AGDDTechnology();
	std::string GetName();
};
//} // namespace AGDDModel

#endif
