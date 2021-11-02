/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDTechnology_H
#define AGDDTechnology_H

#include <string>
class AGDDDetectorStore;

// abstract base class for detector technology classes

//namespace AGDDModel {

class AGDDTechnology {
protected:
	std::string m_name;
public:

	AGDDTechnology(const std::string& s,
                       AGDDDetectorStore& ds);
	virtual ~AGDDTechnology();
	std::string GetName() const;
};
//} // namespace AGDDModel

#endif
