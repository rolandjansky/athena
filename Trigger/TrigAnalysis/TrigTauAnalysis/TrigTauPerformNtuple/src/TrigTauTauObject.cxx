/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauPerformNtuple/TrigTauTauObject.h"
#include <iostream>
using namespace std;

ClassImp(TrigTauTauObject)  

TrigTauTauObject::TrigTauTauObject()
{
  reset();
}

TrigTauTauObject::TrigTauTauObject(const TrigTauTauObject & other) :
  TLorentzVector(other),
  m_seedType    (other.m_seedType   ),
  m_caloPhi     (other.m_caloPhi    ),
  m_caloEta     (other.m_caloEta    ),
		                    
  m_EMRadius    (other.m_EMRadius   ),
  m_nStrips     (other.m_nStrips    ),
  m_stripWidth2 (other.m_stripWidth2),
  m_isoFrac     (other.m_isoFrac    )
  //   cells         (other.cells),
  //  caloClusters  (other.caloClusters)
{
  for(std::vector<TrigTauCell*>::const_iterator itr = other.cells.begin(); itr != other.cells.end(); ++itr)
    cells.push_back(new TrigTauCell(**itr));

  for(std::vector<TrigTauCaloCluster*>::const_iterator itr = other.caloClusters.begin(); itr != other.caloClusters.end(); ++itr)
    caloClusters.push_back(new TrigTauCaloCluster(**itr));


}



TrigTauTauObject::~TrigTauTauObject()
{
  reset();
}

void TrigTauTauObject::reset()
{
  m_seedType            = UNDEFINED;

  m_caloPhi = 0;
  m_caloEta = 0;

  m_EMRadius = 0;
  m_nStrips = 0;
  m_stripWidth2 = 0;
  m_isoFrac = 0;
  

  for(std::vector<TrigTauCell*>::iterator itr = cells.begin(); itr != cells.end(); ++itr)
    delete *itr;
  cells.erase( cells.begin(), cells.end() );
  for(std::vector<TrigTauCaloCluster*>::iterator itr = caloClusters.begin(); itr != caloClusters.end(); ++itr)
    delete *itr;
  caloClusters.erase( caloClusters.begin(), caloClusters.end() );
  return ;
}

float TrigTauTauObject::ptTotTracks() const
{
  float px = 0;
  float py = 0;
  unsigned int nTrk = nTracks();
  for(unsigned int i = 0; i < nTrk ; ++i )
    {
      const TrigTauTrack * itr = &track(i);
      px += itr->Px();
      py += itr->Py();
    }
  return sqrt(px*px + py*py) ; 
}

int   TrigTauTauObject::chargeTotTracks() const
{
  int charge = 0;
  unsigned int nTrk = nTracks();
  for(unsigned int i = 0; i < nTrk ; ++i )
    {
      const TrigTauTrack * itr = &track(i);
      charge += itr->charge();
    }
  return charge;
}
const TrigTauTrack *  TrigTauTauObject::leadingTrack() const
{
  const TrigTauTrack * pTrack(0);
  float maxPt = -1;
  
  unsigned int nTrk = nTracks();
  for(unsigned int i = 0; i < nTrk ; ++i )
    {
      const TrigTauTrack * itr = &track(i);
      //cout << " point to track is " << itr << endl;
      if( itr == 0 ){
	cout << " Something very strange!! Number of tracks is "<< nTrk << ", but can not access trk "<< i<<" Eta/Phi ~ "<< Eta() << " " << Phi() << "\n";
	continue;
      }

      float pt = itr->Pt();
      if( pt > maxPt ) { maxPt = pt; pTrack = &(*itr);}
    }
  return pTrack;
}

unsigned int             TrigTauTauObject::nCells(int sampID) const
{
  unsigned int nC = 0;
  for( std::vector<TrigTauCell*>::const_iterator ic = cells.begin();
       ic != cells.end() ; ++ic )
    {
      if( (*ic)->m_samp == sampID ) nC++;
    }
  return nC;
}
