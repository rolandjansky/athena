/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1CTP_CTPUTIL_H
#define TRIGT1CTP_CTPUTIL_H

// standard include(s):
#include <stdint.h>

// STL include(s):
#include <vector>
#include <string>
#include <bitset>

// forward declarations
namespace ROIB {
   class CTPRoI;
}

namespace LVL1CTP {

   /**
    *
    *   @short Class grouping useful functions together
    *
    *          I use this class mainly as a replacement of the former TMUtil class.
    *          I hope it will be much-much more efficient than TMUtil was. (That class
    *          was a horror...)
    *
    *  @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *  @author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>
    *
    * @version \$Id: CTPUtil.h,v 1.8 2009-01-29 20:54:31 efeld Exp $
    *
    */

   class CTPUtil {

   public:
      // Functions extracting multiplicities when using the legacy configuration objects:
      static int getMuonMult( unsigned int word, int threshold );   //!< get Muon multiplicty in legacy mode
      static int getEMTMult( unsigned int word, int threshold );    //!< get EmTau multiplicty in legacy mode
      static int getJetMult( unsigned int word, int threshold );    //!< get Jet multiplicty in legacy mode
      static int getFJetMult( unsigned int word, int threshold );   //!< get Forward Jet multiplicty in legacy mode
      static int getJetEMult( unsigned int word, int threshold );   //!< get Jet Energy multiplicty in legacy mode
      static int getETMult( unsigned int word, int threshold );     //!< get transverse energy multiplicty in legacy mode
      static int getTMMult( unsigned int word, int threshold );     //!< get missing Et multiplicty in legacy mode

      //! extract multiplicities using new trigger configuration interface
      static int getMult( uint64_t word, unsigned int startbit, unsigned int endbit );

      //! extract multiplicities from Topo words, were the encoding is different
      static unsigned int getMultTopo( uint64_t word, unsigned int startbit, unsigned int endbit, unsigned int clock );

      // Some additional helper functions:
      static unsigned int alignBits( int value, int startbit, int endbit );            //!< align bits
      static unsigned int createMask( int startbit, int endbit );                      //!< create mask
      static std::bitset<512> convertToLargeBitset(const std::vector<uint32_t>& words);     //!< convert list of words into bitset
      static std::bitset<256> convertToBitset(const std::vector<uint32_t>& words);     //!< convert list of words into bitset
      static std::bitset<512> convertToLargeBitset(const std::vector<ROIB::CTPRoI>& words); //!< convert list of CTPRoIs into bitset
      static std::bitset<256> convertToBitset(const std::vector<ROIB::CTPRoI>& words); //!< convert list of CTPRoIs into bitset

      // helper for output
      static std::string printHex(const std::vector<uint32_t>& rhs, const std::string& delimiter = ","); //!< print vector in hex format

   }; // class CTPUtil

} // namespace LVL1CTP

#endif // TRIGT1CTP_CTPUTIL_H
