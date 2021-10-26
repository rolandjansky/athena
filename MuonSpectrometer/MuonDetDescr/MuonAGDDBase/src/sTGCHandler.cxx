/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/sTGCHandler.h"
#include "MuonAGDDBase/AGDDsTGC.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>


sTGCHandler::sTGCHandler(const std::string& s,
                         AGDDController& c)
  : XMLHandler(s, c)
{
}

void sTGCHandler::ElementHandle(AGDDController& c,
                                xercesc::DOMNode *t)
{
    
//    std::cout<<" this is sTGCHandler::Handle"<<std::endl;

	bool ret=true;
	std::string name=getAttributeAsString(c, t, "type",ret);
	std::string sType=getAttributeAsString(c, t, "subType",ret);
	std::vector<double> vvv;
	vvv.push_back(getAttributeAsDouble(c, t, "sWidth",ret));
	vvv.push_back(getAttributeAsDouble(c, t, "lWidth",ret));
	vvv.push_back(getAttributeAsDouble(c, t, "Length",ret));
	vvv.push_back(getAttributeAsDouble(c, t, "Tck",ret));
	vvv.push_back(getAttributeAsDouble(c, t, "yCutout",0.));
	vvv.push_back(getAttributeAsDouble(c, t, "stripPitch",0.));
	vvv.push_back(getAttributeAsDouble(c, t, "wirePitch",0.));
	vvv.push_back(getAttributeAsDouble(c, t, "stripWidth",0.));
	vvv.push_back(getAttributeAsDouble(c, t, "yCutoutCathode",0.));
	
	//double ttt=getAttributeAsDouble(c, t, "yCutout",0.);
	//std::cout<<"+++++++++++++++++++++++> yCutout "<<ttt<<std::endl;
	
	std::string technology=getAttributeAsString(c, t, "tech",ret);
	
	AGDDsTGC *b=new AGDDsTGC(name,
                                 c.GetDetectorStore(),
                                 c.GetVolumeStore(),
                                 c.GetSectionStore());
        m_sTGCCurrent = b;
	b->SetXYZ(vvv);
	b->subType(sType);
	b->tech=technology;
	
	double xf=getAttributeAsDouble(c, t, "xFrame",0.);
	b->xFrame(xf);
	double ysf=getAttributeAsDouble(c, t, "ysFrame",0.);
	b->ysFrame(ysf);
	double ylf=getAttributeAsDouble(c, t, "ylFrame",0.);
	b->ylFrame(ylf);
	
}

sTGCDetectorDescription* sTGCHandler::GetsTGCCurrent()
{
  return m_sTGCCurrent;
}

