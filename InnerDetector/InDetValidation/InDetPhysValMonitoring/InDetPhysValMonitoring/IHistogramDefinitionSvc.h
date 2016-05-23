/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  IHistogramDefinitionSvc.h
//
//  Created by sroe on 07/07/2015.
//

#ifndef IHistogramDefinitionSvc_h
#define IHistogramDefinitionSvc_h
#include <limits>
#include <string>
#include <utility>
#include "GaudiKernel/IInterface.h"

class SingleHistogramDefinition;
///Interface class to get the histogram defintion for a named histogram in a given directory
class IHistogramDefinitionSvc:virtual public IInterface{
public:
	///reimplemented from IInterface
	static const InterfaceID & interfaceID();

	enum Formats{UNKNOWN,TEXT_XML,TEXT_PLAIN,NFORMATS};
	virtual ~IHistogramDefinitionSvc(){}
	typedef std::pair<float, float> axesLimits_t ;
	virtual SingleHistogramDefinition definition(const std::string &name, const std::string & dirName="") const =0;
	virtual std::string histoType(const std::string &name, const std::string &dirName="") const = 0;
	virtual std::string title(const std::string &name, const std::string &dirName="") const =0;
	virtual unsigned int nBinsX(const std::string &name, const std::string &dirName="") const = 0;
	virtual unsigned int nBinsY(const std::string &/*name*/, const std::string &/*dirName*/="") const { return 0; }
	virtual axesLimits_t xLimits(const std::string &name, const std::string &dirName="") const = 0;
	virtual axesLimits_t yLimits(const std::string & /*name*/, const std::string &/*dirName*/="") const {return std::make_pair(std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::quiet_NaN());}
	virtual std::string xTitle(const std::string &name, const std::string &dirName="") const = 0;
	virtual std::string yTitle(const std::string &name, const std::string &dirName="") const = 0;
	//virtual bool initialise()=0;
	
};
inline const InterfaceID & IHistogramDefinitionSvc::interfaceID(){
  static const InterfaceID IID("IHistogramDefinitionSvc",1,0);
  return IID;
}

#endif
