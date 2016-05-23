/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_ToolTestMonitoringPlots_H
#define INDETPHYSVALMONITORING_ToolTestMonitoringPlots_H
/**
 * @file ToolTestMonitoringPlots.h
 * header file for class of same name
 * @author shaun roe
 * @date 14 September 2015
**/
//STL includes
#include <string>

//#gaudi includes
#include "GaudiKernel/ToolHandle.h"

//local include
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

//fwd declaration
class InDetTestPlot;


/**
 * @brief Tool to book and fill test histograms in monitoring framework
 */
class ToolTestMonitoringPlots:public ManagedMonitorToolBase{
public:
	///Constructor with parameters
	ToolTestMonitoringPlots(const std::string & type, const std::string & name, const IInterface* parent);
	///Destructor
	virtual ~ToolTestMonitoringPlots();
	/** \name BaseclassMethods Baseclass methods reimplemented 
	}**/
	//@{
    virtual StatusCode initialize();
    virtual StatusCode bookHistograms();
    virtual StatusCode fillHistograms();
    virtual StatusCode procHistograms();
	//@}
private:
	///prevent default construction
	ToolTestMonitoringPlots();

	///histograms
	std::unique_ptr< InDetTestPlot > m_monPlots;
	
	template<class T>
	const T* getContainer( const std::string & containerName);
};

template<class T>
	const T* ToolTestMonitoringPlots::getContainer(const std::string & containerName){
		const T * ptr = evtStore()->retrieve< const T >( containerName );
    	if (!ptr) {
        	ATH_MSG_ERROR("Container '"<<containerName<<"' could not be retrieved");
    	}
    	return ptr;
	}

#endif
