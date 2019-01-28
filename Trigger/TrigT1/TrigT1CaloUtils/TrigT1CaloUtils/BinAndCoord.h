/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          BinAndCoord.h  -  description
                             -------------------
    begin                : 7/03/2002
    email                : e.moyse@qmul.ac.uk
 ***************************************************************************/




#ifndef _BinAndCoord_H_
#define _BinAndCoord_H_

#ifndef  TRIGGERSPACE
// running in Athena
#include "TrigT1Interfaces/Coordinate.h"
#else
// running in TRIGGERSPACE
#include "Coordinate.h"
#endif

/**LVL1 namespace.
This belongs to the TrigT1Calo  simulation.
*/
namespace LVL1 {

/**
Used to pass data between the methods of the Key Classes:  Returns the Eta
and Phi bins, and the centre of the bins. 
*/

class BinAndCoord {

public:
  BinAndCoord(int phiBin, int etaBin, Coordinate* centralCoords):
	m_phiBin(phiBin), m_etaBin(etaBin), m_centralCoords(centralCoords){} ;
  virtual ~BinAndCoord(){ delete m_centralCoords;};

	
public:	
	int phiBin() const {return m_phiBin;};
	int etaBin() const {return m_etaBin;};
	Coordinate* coords() const {return m_centralCoords;};

private:
 	
	int m_phiBin;
	int m_etaBin; 
	Coordinate* m_centralCoords;
	
};

} // end of namespace bracket


#endif











