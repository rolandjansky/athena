/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPHYSVALDECORATORALG_H
#define INDETPHYSVALMONITORING_INDETPHYSVALDECORATORALG_H
/**
 * @file InDetPhysValDecoratorAlg.h
 * header file for class of same name
 * @author shaun roe
 * @date 20 January 2015
**/
//STL includes
#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

//fwd declaration
class IInDetPhysValDecoratorTool;

/** @class InDetPhysValDecoratorAlg
 *	Class to decorate track or truth particles with additional info 
 *  required for track validation and/or plotting
 */
class InDetPhysValDecoratorAlg:public AthAlgorithm{
public:

	/** Standard Athena-Algorithm Constructor */
	InDetPhysValDecoratorAlg(const std::string& name, ISvcLocator* pSvcLocator);
	/** Default Destructor */
	virtual ~InDetPhysValDecoratorAlg();

	/** standard Athena-Algorithm method */
	StatusCode          initialize();
	/** standard Athena-Algorithm method */
	StatusCode          execute();
	/** standard Athena-Algorithm method */
	StatusCode          finalize();
	
private:
  ///@name container names
  ///@{ 
	///TrackParticle container's name
	std::string m_trkParticleName;
	///TruthParticle container's name
	std::string m_truthParticleName;
  ///@}
  ///@name decorators
  ///@{ 
  ToolHandle<IInDetPhysValDecoratorTool> m_truthDecoratorTool;
  ToolHandle<IInDetPhysValDecoratorTool> m_hitDecoratorTool;
  ToolHandle<IInDetPhysValDecoratorTool> m_errDecoratorTool;
  ToolHandle<IInDetPhysValDecoratorTool> m_truthClassDecoratorTool;
  ///@}
  ///@name member functions
  ///@{ 
  template<class T>
	const T* getContainer( const std::string & containerName);
  ///@}
};

///implementation for templated getContainer function
template<class T>
const T* InDetPhysValDecoratorAlg::getContainer(const std::string & containerName){
const T * ptr = evtStore()->retrieve< const T >( containerName );
  if (!ptr) {
		ATH_MSG_ERROR("Container '"<<containerName<<"' could not be retrieved");
	}
	return ptr;
}


#endif
