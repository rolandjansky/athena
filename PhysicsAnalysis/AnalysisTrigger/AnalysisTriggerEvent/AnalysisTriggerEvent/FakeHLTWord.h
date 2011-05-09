// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FakeHLTWord.h,v 1.3 2008-05-08 15:00:11 krasznaa Exp $
#ifndef ANALYSISTRIGGEREVENT_FAKEHLTWORD_H
#define ANALYSISTRIGGEREVENT_FAKEHLTWORD_H

// System include(s):
#include <inttypes.h>

// Gaudi/Athena include(s):
#include "CLIDSvc/CLASS_DEF.h"

/**
 *   @short Fake HLT word for streaming (deprecated)
 *
 *          This is a deprecated class!!! It is only left here to avoid
 *          compilation problems in some packages. But no new code should
 *          even think about trying to use it. It's not part of the EDM
 *          anymore.
 */
class FakeHLTWord {

public:
   FakeHLTWord () : m_word(0) {}
   FakeHLTWord( uint32_t word ) : m_word( word ) {}  
   virtual ~FakeHLTWord() {}

   uint32_t word() const { return m_word; }

private:
   uint32_t m_word;

}; // class FakeHLTWord

CLASS_DEF( FakeHLTWord, 238803446, 1 )

#endif // ANALYSISTRIGGEREVENT_FAKEHLTWORD_H
