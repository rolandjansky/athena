/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDigitTest/MuonDigitTestAlg.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/StatusCode.h"

#include "FadsUtilities/Tokenizer.h"

void MuonDigitTestAlg::createSubAlg(std::string typeName, std::string assignedName)
{
	Algorithm* temp;
	StatusCode sc;
	sc=createSubAlgorithm(typeName,assignedName, temp);
	if (sc.isFailure()) 
		log << MSG::INFO << " Error creating the algorithm"<< 
			   typeName<< " " << assignedName <<endreq;
	else 
		algoSuite.push_back(temp);
}

MuonDigitTestAlg::MuonDigitTestAlg(const std::string& name, ISvcLocator* pSvcLocator) :
			Algorithm(name, pSvcLocator), log( msgSvc(), name )
{
	log << MSG::INFO << "++++++++++++  MuonDigitTestAlg created  ++++++++++++"
            << endreq;
			
	declareProperty("SubAlgorithms",algoNames);
	
}

MuonDigitTestAlg::~MuonDigitTestAlg()
{
	log << MSG::INFO << " deleting MuonDigitTestAlg " << endreq;
}

StatusCode MuonDigitTestAlg::initialize()
{
	log << MSG::INFO << " initialize " << endreq;
	
	
	std::string typ;
	std::string nam;
	
	for (unsigned int i=0;i<algoNames.size();i++)
	{
		FADS::Tokenizer tokens("/",algoNames[i]);
		typ=tokens[0];
		if (tokens.size()==1) nam=typ;
		else nam=tokens[1];
		log << MSG::INFO << " Registering Algorithm "<< typ << " " << nam <<endreq;
		createSubAlg(typ,nam);
	}
	return StatusCode::SUCCESS;
}

StatusCode MuonDigitTestAlg::finalize()
{
	log << MSG::INFO << " finalize " << endreq;
	return StatusCode::SUCCESS;
}

StatusCode MuonDigitTestAlg::execute()
{
	log << MSG::INFO << " execute " << endreq;
	
	int nAlgs=algoSuite.size();
	
	for (int i=0;i<nAlgs;i++) if( (algoSuite[i]->execute()).isFailure() ) return StatusCode::FAILURE;
	
	return StatusCode::SUCCESS;
}


