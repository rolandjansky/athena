/***************************************************************************
                          ICoordinate.h  -  description
                             -------------------
    begin                : 28/02/2002
    copyright            : (C) 2002 by moyse
    email                : e.moyse@qmul.ac.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
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











