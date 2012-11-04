/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ENERGYCONE_H
#define ENERGYCONE_H

#include "HadronicRecoilData/Cone.h"
#include "HadronicRecoilData/EnergyList.h"
#include "HadronicRecoilData/TransverseEnergy.h"
#include "HadronicRecoilData/ConeBuilder.h"
#include "HadronicRecoilData/General.h"

#include <vector>

/** @class EnergyCone defines a @class EnergyList contained in a @class Cone. */ 
template <class T> class EnergyCone : public Cone, public EnergyList<T>//, public TransverseEnergy
{
 public:
  /** default constructor */
  EnergyCone()
    :Cone(),
    EnergyList<T>()
    {
    };
  
 EnergyCone(double eta, double phi, float size, double Etx, double Ety)
   :Cone( eta, phi, size ),
    EnergyList<T>(Etx, Ety)
    {
    };

 EnergyCone(DataVector<T> inputContainer, double eta, double phi, double radius)
   :Cone( eta, phi, radius )
   {
     Fill(inputContainer);
   };
   
   /** destructor*/
  ~EnergyCone(){};

  /** constructor */
  EnergyCone(double eta, double phi, float size)
    :Cone( eta, phi, size ),
    EnergyList<T>()
    {
    };

    void Fill( DataVector<T> inputContainer )
    {
      double en_eta = 0.;
      double en_phi = 0.;
      
      for(unsigned int i=0; i < inputContainer.size(); ++i){
	
	en_eta = inputContainer.at(i)->eta();
	en_phi = inputContainer.at(i)->phi();
	
	if( HadronicRecoil::DeltaR( this->Eta(), this->Phi(), en_eta, en_phi ) < Radius() ){
	  this->EnergyList<T>::AddElement( inputContainer.at(i) );
	}
      }
      return;
    }

    void Fill( double etx, double ety )
    {
      this->TransverseEnergy::Add( etx, ety ); 
      return;
    }
    
};
#endif //ENERGYCONE_H
