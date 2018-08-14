/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZdcRecNoiseTool.h
 *
 *  This extracts the pedestal and noise figures from every channel. This should
 *  process "pedestal runs" where no activity on the channels are present.
 *      Author: leite
 */

#ifndef ZDCRECNOISETOOL_H_
#define ZDCRECNOISETOOL_H_

#include <string>
#include <map>
//#include <boost/accumulators/accumulators.hpp>
//#include <boost/accumulators/statistics/min.hpp>
//#include <boost/accumulators/statistics/max.hpp>
//#include <boost/accumulators/statistics/mean.hpp>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "ZdcEvent/ZdcRawChannelCollection.h"

class IInterface;
class InterfaceID;
class StatusCode;
//class Identifier;
class ZdcDigitsCollection;


//Interface Id for retrieving the tool
static const InterfaceID IID_IZdcRecNoiseTool("ZdcRecNoiseTool", 1, 1);

class ZdcRecNoiseTool: public AthAlgTool
{
public:
	 ZdcRecNoiseTool(const std::string& type,
					   const std::string& name,
					   const IInterface* parent);

	virtual ~ZdcRecNoiseTool();

	static const InterfaceID& interfaceID();

	virtual StatusCode initialize();
	virtual StatusCode finalize();

	int readPedestals();
	int writePedestals();

private:

	//ZdcRawChannelCollection      m_ChannelCollection;
	//unsigned int     m_nsamples;

	std::string  m_pedestalDir;
	std::string  m_pedestalFile;

	ZdcDigitsCollection* m_pedestalData;

	//Boost accumulator
	//boost::accumulators::accumulator_set< double, boost::accumulators::features<
	//															boost::accumulators::tag::min,
	//															boost::accumulators::tag::max,
	//															boost::accumulators::tag::mean > > m_acc;
};

#endif /* ZDCRECNOISETOOL_H_ */
