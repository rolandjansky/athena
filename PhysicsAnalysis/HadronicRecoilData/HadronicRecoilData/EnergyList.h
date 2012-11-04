/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ENERGYLIST_H
#define ENERGYLIST_H

#include "HadronicRecoilData/TransverseEnergy.h" 
#include "DataModel/DataVector.h"
#include <vector>
#include "CaloEvent/CaloCluster.h"
#include "Particle/TrackParticle.h"

template <class T> class EnergyList : public TransverseEnergy
{
 public:
  /** default constructor*/
 EnergyList()
   :TransverseEnergy()
    { 
    };

  /** simple constructor*/
 EnergyList(double Etx, double Ety)
   :TransverseEnergy(Etx, Ety)
    { 
    };

  /** destructor */
  ~EnergyList()
    {
    };

  /** adds a element to the group and updates relevant quantities (e.g. the energy sum) */
  void AddElement( const T* element )
  {
    T* myelement = new T(*element);
    m_list.push_back( myelement );
    //std::cout << "Adding T" << std::endl;
    TransverseEnergy::Add( myelement->pt()*std::cos( myelement->phi() ),
			   myelement->pt()*std::sin( myelement->phi() ) );
  };
  
  /** returns the number of elements */
  unsigned int Size() const { return m_list.size(); };

  /** returns the list of elements */
  DataVector<const T> GetList() const { return m_list; };

  /** returns i-th element of the list */
  const T* At(unsigned int i) const { return m_list.at(i); };

 private:
  /** list of the elements in the group */
  DataVector<const T> m_list;

};

/** Overload default class for CaloCluster */
template <> class EnergyList<CaloCluster> : public TransverseEnergy
{
 public:
  /** default constructor*/
 EnergyList()
   :TransverseEnergy()
    { 
    };

  /** simple constructor*/
 EnergyList(double Etx, double Ety)
   :TransverseEnergy(Etx, Ety)
    { 
    };

  /** destructor */
  ~EnergyList()
    {
    };

  void AddElement( const CaloCluster* element )
  {
    CaloCluster* myelement = new CaloCluster(*element);
    m_list.push_back( myelement );
    //std::cout << "Adding Cluster" << std::endl;
    if( myelement->pt() > 0 ){
      TransverseEnergy::Add( myelement->pt()*std::cos( myelement->phi() ),
			     myelement->pt()*std::sin( myelement->phi() ) );
    }
  };

  /** returns the number of elements */
  unsigned int Size() const { return m_list.size(); };

  /** returns the list of elements */
  DataVector<const CaloCluster> GetList() const { return m_list; };

  /** returns i-th element of the list */
  const CaloCluster* At(unsigned int i) const { return m_list.at(i); };

 private:
  /** list of the elements in the group */
  DataVector<const CaloCluster> m_list;

};

/** Overload default class for TrackParticles */
template <> class EnergyList<Rec::TrackParticle> : public TransverseEnergy
{
 public:
  /** default constructor*/
 EnergyList()
   :TransverseEnergy()
    {
    };

  /** simple constructor*/
 EnergyList(double Etx, double Ety)
   :TransverseEnergy(Etx, Ety)
    {
    };

  /** destructor */
  ~EnergyList()
    {
    };

  void AddElement( const Rec::TrackParticle* element )
  {
    Rec::TrackParticle* myelement = new Rec::TrackParticle(*element);
    m_list.push_back( myelement );
    //std::cout << "Adding TrackParticle" << std::endl;
    TransverseEnergy::Add( TMath::Abs(myelement->pt())*std::cos( myelement->phi() ),
			   TMath::Abs(myelement->pt())*std::sin( myelement->phi() ) );
    
    
  };

  /** returns the number of elements */
  unsigned int Size() const { return m_list.size(); };

  /** returns the list of elements */
  DataVector<const Rec::TrackParticle> GetList() const { return m_list; };

  /** returns i-th element of the list */
  const Rec::TrackParticle* At(unsigned int i) const { return m_list.at(i); };

 private:
  /** list of the elements in the group */
  DataVector<const Rec::TrackParticle> m_list;

};

#endif //ENERGYLIST_H

