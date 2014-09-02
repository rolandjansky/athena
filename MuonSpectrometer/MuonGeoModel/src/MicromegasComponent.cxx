/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/MicromegasComponent.h"
#include "MuonGeoModel/MYSQL.h"

namespace MuonGM {


MicromegasComponent:: MicromegasComponent():StandardComponent(),subType("")
{
}


std::ostream& operator<<(std::ostream& os,const MicromegasComponent& c)
{
	os<<"Component "<<c.name<<std::endl;
	return os;
}

} // namespace MuonGM
