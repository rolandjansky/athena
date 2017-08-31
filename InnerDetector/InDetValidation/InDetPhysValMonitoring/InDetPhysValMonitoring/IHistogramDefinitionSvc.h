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
///Interface class to get the histogram definition for a named histogram in a given directory
class IHistogramDefinitionSvc:virtual public IInterface{
public:
	///reimplemented from IInterface
	static const InterfaceID & interfaceID();
  ///Format of the data source holding the histogram definition
	enum Formats{UNKNOWN,TEXT_XML,TEXT_PLAIN,NFORMATS};
	///Virtual destructor does nothing
	virtual ~IHistogramDefinitionSvc(){}
	///typedef for axes limits, (lower bound, upper bound)
	typedef std::pair<float, float> axesLimits_t ;
	///Return a histogram definition, retrieved by histogram identifier (and directory name, if supplied)
	virtual SingleHistogramDefinition definition(const std::string &name, const std::string & dirName="") const =0;
	///Return Histogram type (TH1, TH2 etc) by histogram identifier (and directory name, if supplied)
	virtual std::string histoType(const std::string &name, const std::string &dirName="") const = 0;
	///Return Histogram title by histogram identifier (and directory name, if supplied)
	virtual std::string title(const std::string &name, const std::string &dirName="") const =0;
	///Return number of x bins by histogram identifier (and directory name, if supplied)
	virtual unsigned int nBinsX(const std::string &name, const std::string &dirName="") const = 0;
	///Return number of y bins by histogram identifier (and directory name, if supplied); default returns 0 for 1-D histos
	virtual unsigned int nBinsY(const std::string &/*name*/, const std::string &/*dirName*/="") const { return 0; }
	///Return number of z bins by histogram identifier (and directory name, if supplied); default returns 0 for 1-D histos
	virtual unsigned int nBinsZ(const std::string &/*name*/, const std::string &/*dirName*/="") const { return 0; }
	///Return x axes (lo,hi) by histogram identifier (and directory name, if supplied)
	virtual axesLimits_t xLimits(const std::string &name, const std::string &dirName="") const = 0;
	///Return y axes (lo,hi) by histogram identifier (and directory name, if supplied). Default returns (nan,nan).
	virtual axesLimits_t yLimits(const std::string & /*name*/, const std::string &/*dirName*/="") const {return std::make_pair(std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::quiet_NaN());}
	///Return z axes (lo,hi) by histogram identifier (and directory name, if supplied)
	virtual axesLimits_t zLimits(const std::string &name, const std::string &dirName="") const = 0;
	///Return x-axis title by histogram identifier (and directory name, if supplied)
	virtual std::string xTitle(const std::string &name, const std::string &dirName="") const = 0;
	///Return y-axis title by histogram identifier (and directory name, if supplied)
	virtual std::string yTitle(const std::string &name, const std::string &dirName="") const = 0;
	///Return z-axis title by histogram identifier (and directory name, if supplied)
	virtual std::string zTitle(const std::string &name, const std::string &dirName="") const = 0;
	//virtual bool initialise()=0;
	
};
inline const InterfaceID & IHistogramDefinitionSvc::interfaceID(){
  static const InterfaceID IID("IHistogramDefinitionSvc",1,0);
  return IID;
}

#endif
