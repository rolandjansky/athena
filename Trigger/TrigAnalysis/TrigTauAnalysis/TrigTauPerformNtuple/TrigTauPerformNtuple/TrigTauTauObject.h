/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUTAUOBJECT_H
#define TRIGTAUTAUOBJECT_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauTauObject
/// Package : TauTrigPerformance
/// Author  : Olya Igonkina (University of Oregon)
/// Created : April 2006
///
/// DESCRIPTION:
///           basic class for objects related to tau (reonstructed or triggered)
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "TObject.h"
#include "TLorentzVector.h"
#include "TrigTauPerformNtuple/TrigTauTrack.h"
#include "TrigTauPerformNtuple/TrigTauCell.h"
#include "TrigTauPerformNtuple/TrigTauCaloCluster.h"

#include <vector>

class TrigTauTauObject : public TLorentzVector
{
 public :
  enum CONELEVELS{ NCONELEVELS=3};
  enum SEEDTYPES { UNDEFINED = 0, CALOOBJECT = 1, TRACK = 2, BOTH = 3 };

  TrigTauTauObject();
  TrigTauTauObject(const TrigTauTauObject & other);
  //TrigTauTauObject & operator = (const TrigTauTauObject & other);

  virtual ~TrigTauTauObject();
  virtual void reset();

  virtual int charge() const { return 0;}
  const TrigTauTrack * leadingTrack() const;


  //----------- public data members
  SEEDTYPES        m_seedType;   //   -- seedType --

  //------- cluster information
  float            m_caloPhi;
  float            m_caloEta;

  float            m_EMRadius;
  int              m_nStrips;
  float            m_stripWidth2;
  float            m_isoFrac;
  
  //------- tracking information
  float                    ptTotTracks() const ;
  int                      chargeTotTracks() const; //

  virtual unsigned int             nTracks() const = 0;
  virtual const  TrigTauTrack&     track(unsigned int i ) const = 0;
  virtual void                     resetTracks() = 0 ; // remove all tracks

  // moved in derived classes : std::vector<TrigTauTrack> tracks;


  //------- cell information
  unsigned int             nCells() const {return cells.size();} ;
  unsigned int             nCells(int sampID) const;  // slow!!
  const  TrigTauCell&      cell(unsigned int i ) const { return *cells[i]; };

  std::vector<TrigTauCell*> cells;

  //------- CaloCluster information
  unsigned int nCaloClusters() const {return caloClusters.size();};
  const TrigTauCaloCluster &
    caloCluster(unsigned int i ) const { return *caloClusters[i]; };

  std::vector<TrigTauCaloCluster*> caloClusters;


 private:
  ClassDef(TrigTauTauObject, 10)  ;
};

#endif //TRIGTAUTAUOBJECT_H
