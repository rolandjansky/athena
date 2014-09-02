/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/TgcComponent.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/Technology.h"
#include "MuonGeoModel/Tgc.h"
#include "MuonGeoModel/TGC_Technology.h"

namespace MuonGM {


TgcComponent:: TgcComponent():StandardComponent()
{
}


std::ostream& operator<<(std::ostream& os,const TgcComponent& c)
{
	os<<"Component "<<c.name<<std::endl;
// 	os<<c.posx<<" "<<c.posy<<" "<<c.posz<<" "<<c.index<<" "<<c.name;
// 	os<<" "<<c.dx1<<" "<<c.dx2<<" "<<c.dy<<" "<<c.excent<<" "<<c.deadx;
// 	os<<" "<<c.deady<<endl;
	std::cout<<" Component name "<<c.name<<std::endl;
	return os;
}

// double TgcComponent::GetThickness() const
// {
// 	MYSQL *mysql=MYSQL::GetPointer();
// 	TGC *t=(TGC *)mysql->GetTechnology(name);
//         std::cout<<"Tgc Component:: TGC found for name "<<name<<" its thickness is "<<t->thickness<<std::endl;
//         return t->thickness;
	
// }
} // namespace MuonGM
