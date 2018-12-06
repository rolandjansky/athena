// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_SLINKWORD_H
#define TRIGT1INTERFACES_SLINKWORD_H

namespace LVL1CTP {

   /**
    *  @short Simple wrapper around a 32-bit integer
    *
    *         This class contains a 32bit Slink word - it is a minimal
    *         wrapping to allow storage in Athena ObjectVectors
    *
    * @author Edward Moyse
    * @author Thomas Schoerner-Sadenius
    *
    * $Revision: 187728 $
    * $Date: 2009-05-27 18:18:06 +0200 (Wed, 27 May 2009) $
    */
   class SlinkWord {

   public:
      SlinkWord( unsigned int word );
      ~SlinkWord();

      /** returns Slink word */
      unsigned int word() const;

   private:
      /** this is the 32bit Slink word */
      const unsigned int m_slinkWord;

   }; // class SlinkWord

} // namespace LVL1CTP

#endif // TRIGT1INTERFACES_SLINKWORD_H
