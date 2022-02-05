//Dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARLATOMEMAPPING_H
#define LARLATOMEMAPPING_H

#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include <vector>

/**
  * fixed number of channels on one LATOME board<br>
  *
  */
#define N_LATOME_CHANNELS 320

/**
 * @brief class to provide SC mapping
 */

class LArLATOMEMapping {

  /**
   * class which fills object from conditions DB<br>
   *
   */
  friend class LArLATOMEMappingAlg; //The conditions alg filling this object

 public:
  LArLATOMEMapping();
  
  /**
   * create a HWIdentifier from an Identifier (not inline)<br>
   *
   */
  HWIdentifier getChannelID(const unsigned int sourceID, const unsigned int chan) const;



 private:

  ///Invalid default instance of HWIdentifier
  const HWIdentifier m_hwidEmpty;

  std::vector<unsigned int>  m_sourceID;
  std::vector<std::vector< HWIdentifier> > m_onlineID;

};


//Inline methods:
inline HWIdentifier LArLATOMEMapping::getChannelID(const unsigned int sourceID, const unsigned int chan) const {
   if(chan >= N_LATOME_CHANNELS || std::find(m_sourceID.begin(), m_sourceID.end(), sourceID ) == m_sourceID.end()) {
      return m_hwidEmpty;
   } else {  
      auto it=std::find(m_sourceID.begin(),  m_sourceID.end(), sourceID );
      return m_onlineID[std::distance(m_sourceID.begin(), it)][chan];
   }
}



#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArLATOMEMapping, 219977637 , 1)
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF(  LArLATOMEMapping, 84043487  );

#endif
