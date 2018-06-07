/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/SupComponent.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/Technology.h"
#include "MuonGeoModel/Sup.h"
#include "MuonGeoModel/SUP_Technology.h"

namespace MuonGM {

std::ostream& operator<<(std::ostream& os,const SupComponent& c)
{
	os<<"Component "<<c.name<<std::endl;
// 	os<<c.posx<<" "<<c.posy<<" "<<c.posz<<" "<<c.index<<" "<<c.name;
// 	os<<" "<<c.dx1<<" "<<c.dx2<<" "<<c.dy<<" "<<c.excent<<" "<<c.deadx;
// 	os<<" "<<c.deady<<endl;
	std::cout<<" Component name "<<c.name<<std::endl;
	return os;
}

double SupComponent::xAMDB0() const
{
	MYSQL *mysql=MYSQL::GetPointer();
	SUP *t=(SUP *)mysql->GetTechnology(name);
 	return t->xAMDB0;
}
double SupComponent::yAMDB0() const
{
	MYSQL *mysql=MYSQL::GetPointer();
	SUP *t=(SUP *)mysql->GetTechnology(name);
 	return t->yAMDB0;
}
double SupComponent::zAMDB0() const
{
	MYSQL *mysql=MYSQL::GetPointer();
	SUP *t=(SUP *)mysql->GetTechnology(name);
 	return t->zAMDB0;
}
double SupComponent::bottomsizewrtAMDB0() const
{
	MYSQL *mysql=MYSQL::GetPointer();
	SUP *t=(SUP *)mysql->GetTechnology(name);
 	return t->bottomsizewrtAMDB0;
}
double SupComponent::topsizewrtAMDB0() const
{
	MYSQL *mysql=MYSQL::GetPointer();
	SUP *t=(SUP *)mysql->GetTechnology(name);
 	return t->topsizewrtAMDB0;
}
} // namespace MuonGM
