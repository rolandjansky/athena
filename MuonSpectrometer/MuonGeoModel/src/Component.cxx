/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/Component.h"
#include "MuonGeoModel/Technology.h"

namespace MuonGM {

Component::Component()
{
    name = "XXXY";
    dx1 = 0.;
    dx2 = 0.;
    dy = 0.;
}

Component::Component(const Component& c)
{
//	cout<<" Component c-tor"<<endl;
	name=c.name;
	dx1=c.dx1;
	dx2=c.dx2;
	dy=c.dy;
        //	name="temp";
}

double Component::GetThickness() const
{
	return 0;
}

Component& Component::operator=(const Component& c) {
	if (this!=&c) {
		name=c.name;
		dx1=c.dx1;
		dx2=c.dx2;
		dy=c.dy;     
	}
	return *this;
}

} // namespace MuonGM
