/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDDBase/micromegasHandler.h"
#include "MuonAGDDBase/AGDDMicromegas.h"
#include "AGDDControl/AGDDController.h"
#include <iostream>


micromegasHandler::micromegasHandler(const std::string& s,
                                     AGDDController& c)
  : XMLHandler(s, c),
    m_mmCurrent (nullptr)
{
}

void micromegasHandler::ElementHandle(AGDDController& c,
                                      xercesc::DOMNode *t)
{
    
//    std::cout<<" this is micromegasHandler::Handle"<<std::endl;

	bool ret=true;
	std::string name=getAttributeAsString(c, t, "type",ret);
	std::string sType=getAttributeAsString(c, t, "subType",ret);
	std::vector<double> vvv;
	vvv.push_back(getAttributeAsDouble(c, t, "sWidth",ret));
	vvv.push_back(getAttributeAsDouble(c, t, "lWidth",ret));
	vvv.push_back(getAttributeAsDouble(c, t, "Length",ret));
	vvv.push_back(getAttributeAsDouble(c, t, "Tck",ret));
	
	std::string technology=getAttributeAsString(c, t, "tech",ret);
	
	AGDDMicromegas *b=new AGDDMicromegas(name,
                                             c.GetDetectorStore(),
                                             c.GetVolumeStore(),
                                             c.GetSectionStore());
        m_mmCurrent = b;
	b->SetXYZ(vvv);
	b->subType(sType);
	b->tech=technology;

	double xf=getAttributeAsDouble(c, t, "xFrame",0.);
	b->xFrame(xf);
	double ysf=getAttributeAsDouble(c, t, "ysFrame",0.);
	b->ysFrame(ysf);
	double ylf=getAttributeAsDouble(c, t, "ylFrame",0.);
	b->ylFrame(ylf);
	
//	std::string col=getAttributeAsString(c, t, "color",ret);
//	if (ret)
//		b->SetColor(col);
}


MMDetectorDescription* micromegasHandler::GetMMCurrent()
{
  return m_mmCurrent;
}

