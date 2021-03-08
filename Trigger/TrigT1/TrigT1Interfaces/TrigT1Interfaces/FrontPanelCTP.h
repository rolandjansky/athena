///////////////////////// -*- C++ -*- /////////////////////////////
// FrontPanelCTP.h 
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
// Header file for class FrontPanelCTP
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef TRIGT1INTERFACES_FRONTPANELCTP_H
#define TRIGT1INTERFACES_FRONTPANELCTP_H 1

// STL includes
#include <iosfwd>

#include <bitset>

// Gaudi includes
#include "GaudiKernel/DataObject.h"


namespace LVL1 {
  /**
   *  @short Front panel input class to the CTP simulation
   *
   *         This class holds the multiplicities of the inputs
   *         that are received via the front panel of the 
   *         CTPCORE+ module.
   *
   * @author R. Pöttgen
   *
   */
  
class FrontPanelCTP: public DataObject {
public: 
   
   // default constructor
   FrontPanelCTP();

   // Constructor with parameters: 
   FrontPanelCTP( uint32_t cableword0clock0,
                  uint32_t cableword0clock1,
                  uint32_t cableword1clock0,
                  uint32_t cableword1clock1,
                  uint32_t cableword2clock0,
                  uint32_t cableword2clock1 );
  
   /// Destructor: 
   ~FrontPanelCTP();


   /** @brief set the data that is sent on cable 0 */
   void setCableWord0(unsigned int clock, uint32_t data);

   /** @brief set the data that is sent on cable 1 */
   void setCableWord1(unsigned int clock, uint32_t data);

   /** @brief set the data that is sent on cable 2 */
   void setCableWord2(unsigned int clock, uint32_t data);

   /** @brief set the data that is sent on the optical cables */
   void setOptCableWord(unsigned int cable, std::bitset<128> data) { m_optcableWord[cable] = data; }



   /** @brief return the data that is sent on cable 0 */
   uint32_t cableWord0(unsigned int clock) const;
  
   /** @brief return the data that it sent on cable 1 */
   uint32_t cableWord1(unsigned int clock) const;
  
   /** @brief return the data that is sent on cable 2 */
   uint32_t cableWord2(unsigned int clock) const;

   /** @brief return the data that is sent on the optical cables by index */
   std::bitset<128> optcableWord(unsigned int cable) const { return m_optcableWord[cable]; }
  
  
  
   /////////////////////////////////////////////////////////////////// 
   // Non-const methods: 
   /////////////////////////////////////////////////////////////////// 

   /////////////////////////////////////////////////////////////////// 
   // Private data: 
   /////////////////////////////////////////////////////////////////// 
private: 
   uint32_t m_cableWord0[2] { 0, 0 };
   uint32_t m_cableWord1[2] { 0, 0 };
   uint32_t m_cableWord2[2] { 0, 0 };

   std::bitset<128> m_optcableWord[4] = { 0, 0, 0, 0 };   

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//std::ostream& operator<<( std::ostream& out, const FrontPanelCTP& o );

} //> end namespace LVL1

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LVL1::FrontPanelCTP , 33637034 , 1 )

#endif //> !TRIGT1INTERFACES_FRONTPANELCTP_H
