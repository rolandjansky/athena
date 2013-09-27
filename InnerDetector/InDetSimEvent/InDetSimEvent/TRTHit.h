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
      {return firstWord;}

    unsigned GetSecondWord()
      {return secondWord;}

    unsigned GetThirdWord()
      {return thirdWord;}

    unsigned GetFourthWord()
      {return fourthWord;}

    unsigned GetFifthWord()
      {return fifthWord;}

  private:
    unsigned firstWord;
    unsigned secondWord;
    unsigned thirdWord;
    unsigned fourthWord;
    unsigned fifthWord;
};

typedef AtlasHitsVector<TRTHit> TRTHitCollection;
typedef AtlasHitsVector<TRTHit>::iterator TRTHitIterator;
typedef AtlasHitsVector<TRTHit>::const_iterator TRTHitConstIterator;

#ifndef __CINT__
  CLASS_DEF(TRTHitCollection, 1120337553 , 1 )
#endif

#endif
