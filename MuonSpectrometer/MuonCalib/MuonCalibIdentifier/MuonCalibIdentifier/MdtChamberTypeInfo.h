/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Muon Calibration Identifiers
 * -----------------------------------------
 *
 * Author       : Martin Woudstra
 * Creation Date: 09 May 2005
 * Last Update  : 09 May 2005
 ***************************************************************************/

/** Simple data class that holds the MDT chamber type information:
    number of multilayers, number of layers, number of tubes. */

#ifndef MUONCALIBIDENTIFIER_MDTCHAMBERTYPEINFO_H
# define MUONCALIBIDENTIFIER_MDTCHAMBERTYPEINFO_H
// std

// other packages

// this package


class MdtChamberTypeInfo {
public:
   /** Full initialise the chamber type info with number of multilayers (nMul),
       number of layers (nLay) and number of tubes (nTub) */
   MdtChamberTypeInfo( unsigned int nMul, unsigned int nLay, unsigned int nTub );
   /** comparison operator */
   bool operator==( const MdtChamberTypeInfo& rhs ) const;
   /** Return the number of multilayers in the chamber */
   unsigned int nMultilayers() const;
   /** Return the number of layers per multilayer */
   unsigned int nLayers() const;
   /** Return the number of tubes per layer */
   unsigned int nTubes() const;
   /** Return the total number of tubes in the chamber */
   unsigned int nTotalTubes() const;
private:
   unsigned int m_nMultilayers;
   unsigned int m_nLayers;
   unsigned int m_nTubes;
};

inline MdtChamberTypeInfo::MdtChamberTypeInfo( unsigned int nMul, unsigned int nLay,
					       unsigned int nTub )
   : m_nMultilayers(nMul), m_nLayers(nLay), m_nTubes(nTub) 
{}

inline bool MdtChamberTypeInfo::operator==( const MdtChamberTypeInfo& rhs ) const {
   return
      m_nMultilayers == rhs.m_nMultilayers &&
      m_nLayers == rhs.m_nLayers           &&
      m_nTubes == rhs.m_nTubes;
}

inline 
unsigned int MdtChamberTypeInfo::nMultilayers() const {
   return m_nMultilayers;
}

inline 
unsigned int MdtChamberTypeInfo::nLayers() const {
   return m_nLayers; 
}

inline 
unsigned int MdtChamberTypeInfo::nTubes() const {
   return m_nTubes;
}

inline 
unsigned int MdtChamberTypeInfo::nTotalTubes() const {
   return m_nMultilayers * m_nLayers * m_nTubes;
}

#endif // MUONCALIBIDENTIFIER_MDTCHAMBERTYPEINFO_H
