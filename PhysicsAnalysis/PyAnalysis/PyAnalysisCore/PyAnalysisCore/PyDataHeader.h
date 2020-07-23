/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PYANALYSISCORE_PYDATAHEADER_H
#define PYANALYSISCORE_PYDATAHEADER_H

/**
   A wrapper for DataHeader

    @author Tadashi Maeno
*/
 
#include "PersistentDataModel/DataHeader.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <vector>

class PyDataHeader
{
public:
  PyDataHeader () : m_DataHeaderElementV(0)
  {
    MsgStream log(Athena::getMessageSvc(), "PyDataHeader");

    // get StoreGate
    StoreGateSvc *pSvc = 0;
    StatusCode sc = Gaudi::svcLocator()->service("StoreGateSvc", pSvc);
    if (sc.isFailure())
      log << MSG::ERROR << "could not get StoreGateSvc" << endmsg;
    else
      {
	// retrieve DataHeader
	const DataHandle<DataHeader> beg; 
	const DataHandle<DataHeader> ending; 
	sc = pSvc->retrieve(beg,ending);
	if (sc.isFailure() || beg==ending)
	  log << MSG::ERROR << "could not get DataHeader" << endmsg;
	else
	  {
	    // cache DataHeaderElement
	    for (; beg != ending; ++beg)
	      {
		DataHeader &dh = const_cast<DataHeader &>(*beg);
		std::vector<DataHeaderElement>::const_iterator it = dh.begin();
		for (; it != dh.end(); ++it)
		  m_DataHeaderElementV.push_back(const_cast<DataHeaderElement *>(&*it));
	      }
	  }
      }

    m_classIDSvc = 0;
    sc = Gaudi::svcLocator()->service("ClassIDSvc", m_classIDSvc);
    if (sc.isFailure())
      log << MSG::ERROR << "could not get ClassIDSvc" << endmsg;
  }

  virtual ~PyDataHeader () {}

  //! get number of elements
  int size() { return m_DataHeaderElementV.size(); }

  //! get class name of Nth element
  std::string getName (int index)
  {
    if (index >= size()) return "";
    std::vector<DataHeaderElement *>::iterator it = m_DataHeaderElementV.begin();
    it += index;

    // look for non-symlink name
    std::string name="";
    for (CLID clid : (*it)->getClassIDs())
      {
	// convert CLID to class name
	std::string localName;
	StatusCode sc = m_classIDSvc->getTypeNameOfID (clid, localName);
	if (sc.isFailure()) 
	  {
	    MsgStream log(Athena::getMessageSvc(), "PyDataHeader");
	    log << MSG::ERROR << "could not get TypeName for " << clid << endmsg;
	    return "";
	  }
	// select non-symlink
	if ((name == "") || 
	    ((localName != "ParticleBaseContainer") && 
	     (localName != "IParticleContainer") &&
	     (localName != "INavigable4MomentumCollection")))
	  name = localName;
      }
    return name;
  }

  //! get key of Nth element
  std::string getKey (int index)
  {
    if (index >= size()) return "";
    std::vector<DataHeaderElement *>::iterator it = m_DataHeaderElementV.begin();
    it += index;
    return (*it)->getKey();
  }

private:
  //! cash for DataHeaderElements
  std::vector<DataHeaderElement *> m_DataHeaderElementV;

  //!  class ID service
  IClassIDSvc *m_classIDSvc;
};

#endif

  
