/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTHit_h
#define TRTHit_h

// Data members classes
#include "HitManagement/AtlasHitsVector.h"
#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif



class TRTHit
{
  public:
    TRTHit(unsigned, unsigned, unsigned, unsigned, unsigned);

    unsigned GetFirstWord()
      {return m_firstWord;}

    unsigned GetSecondWord()
      {return m_secondWord;}

    unsigned GetThirdWord()
      {return m_thirdWord;}

    unsigned GetFourthWord()
      {return m_fourthWord;}

    unsigned GetFifthWord()
      {return m_fifthWord;}

  private:
    unsigned m_firstWord;
    unsigned m_secondWord;
    unsigned m_thirdWord;
    unsigned m_fourthWord;
    unsigned m_fifthWord;
};

typedef AtlasHitsVector<TRTHit> TRTHitCollection;
typedef AtlasHitsVector<TRTHit>::iterator TRTHitIterator;
typedef AtlasHitsVector<TRTHit>::const_iterator TRTHitConstIterator;

#ifndef __CINT__
  CLASS_DEF(TRTHitCollection, 1120337553 , 1 )
#endif

#endif
