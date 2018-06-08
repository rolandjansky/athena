/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          ICoordinate.h  -  description
                             -------------------
    begin                : 28/02/2002
    email                : e.moyse@qmul.ac.uk
 ***************************************************************************/




#ifndef ICOORDINATE_H
#define ICOORDINATE_H
/**LVL1 namespace.
This belongs to the TrigT1Calo atrig simulation.
*/
namespace LVL1 {

/**
                 Used by Key Classes, returns and integer coorginate for the bin Eta-Phi.

*/

class ICoordinate {

 public:

  ICoordinate(int phi, int eta, double dEta) ;
  virtual ~ICoordinate();
	void setCoords(int phi, int eta);
	int eta() const;
	int phi() const;
  double dEta() const;

 private:
 	void checkBounds();
	int m_phi;
	int m_eta;
  double m_dEta;
	
};

} // end of namespace bracket


#endif











