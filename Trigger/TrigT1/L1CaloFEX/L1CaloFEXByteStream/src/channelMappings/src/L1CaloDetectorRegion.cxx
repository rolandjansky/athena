#include "channelMappings/L1CaloDetectorRegion.h"
#include "channelMappings/CaloDivision.h"

#include <iostream>

/*!
 *  \class L1CaloDetectorRegion
 * Simple class to mainly store eta/phi information (indices,
 * granularity, trigger tower coordinates) as determined by the
 * channel mappings classes. Additionally, it stores information
 * about the object and detector layer types (as enumerated types),
 * and about the validity of the mapped channel (as boolean value).
 */

//********************************************************************
//              Constructor
//********************************************************************
L1CaloDetectorRegion::L1CaloDetectorRegion()
: m_object (NONE), m_layer (None)
, m_valid (false)
, m_etaIdx (99), m_phiIdx (99)
, m_etaBinWidth (99.), m_phiBinWidth (99.)
, m_etaCoord (99.), m_phiCoord (99.)
, m_etaMin (99.), m_etaMax(99.)
, m_phiMin (99.), m_phiMax(99.)
{
}


//********************************************************************
//              Constructor
//********************************************************************
L1CaloDetectorRegion::L1CaloDetectorRegion (ObjectTypeEnum object,
                                             LayerTypeEnum layer,
                                                      bool valid,
                                                       int etaIdx,
                                                       int phiIdx,
                                                    double etaBinWidth,
                                                    double phiBinWidth,
                                                    double etaCoord,
                                                    double phiCoord)
: m_object (object), m_layer (layer)
, m_valid (valid)
, m_etaIdx (etaIdx), m_phiIdx (phiIdx)
, m_etaBinWidth (etaBinWidth), m_phiBinWidth (phiBinWidth)
, m_etaCoord (etaCoord), m_phiCoord (phiCoord)
, m_etaMin (etaCoord-0.5*etaBinWidth)
, m_etaMax (etaCoord+0.5*etaBinWidth)
, m_phiMin (phiCoord-0.5*phiBinWidth)
, m_phiMax (phiCoord+0.5*phiBinWidth)
{
}


//********************************************************************
//              Destructor
//********************************************************************
L1CaloDetectorRegion::~L1CaloDetectorRegion() {
}

//********************************************************************
//              Simple comparison operator - doesn't need to 
//                                           do everything
//********************************************************************
bool
L1CaloDetectorRegion::operator==(const L1CaloDetectorRegion& rhs) {
   if ( m_object != rhs.m_object )
      return false;
   if ( m_layer != rhs.m_layer )
      return false;
   if ( m_valid != rhs.m_valid )
      return false;
   if ( m_etaIdx != rhs.m_etaIdx )
      return false;
   if ( m_phiIdx != rhs.m_phiIdx )
      return false;
   return true;
}


//********************************************************************
//              objectTypeToString
//********************************************************************
/*!
 * Method to convert the enumerated type 'object' to string
 */
std::string L1CaloDetectorRegion::objectTypeToString() {

   std::string object;
   switch ( this->getObjectType() ) {
      case PPM: object = "PPM"; break;
      case CPM: object = "CPM"; break;
      case JEM: object = "JEM"; break;
      case EFEX: object = "EFEX"; break;
      case JFEX: object = "JFEX"; break;
      case GFEX: object = "GFEX"; break;
      case NONE: object = "NONE"; break;
   }

 return object;
}


//********************************************************************
//              layerToString
//********************************************************************
/*!
 * Method to convert the enumerated type 'layer' to string
 */
std::string L1CaloDetectorRegion::layerToString() {

   std::string layer;
   switch ( this->getLayer() ) {
      case Electromagnetic: layer = "Electromagnetic"; break;
      case Hadronic: layer = "Hadronic"; break;
      case HadFcal2: layer = "HadFcal2"; break;
      case HadFcal3: layer = "HadFcal3"; break;
      case Presampler: layer = "Presampler"; break;
      case Front: layer = "Front"; break;
      case Middle: layer = "Middle"; break;
      case Back: layer = "Back"; break;
      case Both: layer = "Both"; break;
      case None: layer = "None"; break;
   }

 return layer;
}


//********************************************************************
//              getCaloDivision
//********************************************************************
/*!
 * Method to return the calorimeter partition for this region.
 * There are sixteen possibilities, roughly following the TTC
 * partitions in the calorimeters (four Tile, six LAr) but with
 * some extra divisions for the EM barrel/endcap overlap regions
 * and with the FCAL separate from the EMEC and HEC partitions.
 */
CaloDivision L1CaloDetectorRegion::getCaloDivision() const {
   CaloDivision part = CaloDivision::Invalid;
   if (this->getLayer() == Electromagnetic) {
     if      (m_etaIdx < -32) { part = CaloDivision::LArFCAL1C; }
     else if (m_etaIdx < -15) { part = CaloDivision::LArEMECC; }
     else if (m_etaIdx < -14) { part = CaloDivision::LArOverlapC; }
     else if (m_etaIdx <   0) { part = CaloDivision::LArEMBC; }
     else if (m_etaIdx <  14) { part = CaloDivision::LArEMBA; }
     else if (m_etaIdx <  15) { part = CaloDivision::LArOverlapA; }
     else if (m_etaIdx <  32) { part = CaloDivision::LArEMECA; }
     else                     { part = CaloDivision::LArFCAL1A; }
   }
   else {
     if      (m_etaIdx < -32) { part = CaloDivision::LArFCAL23C; }
     else if (m_etaIdx < -15) { part = CaloDivision::LArHECC; }
     else if (m_etaIdx <  -9) { part = CaloDivision::TileEBC; }
     else if (m_etaIdx <   0) { part = CaloDivision::TileLBC; }
     else if (m_etaIdx <   9) { part = CaloDivision::TileLBA; }
     else if (m_etaIdx <  15) { part = CaloDivision::TileEBA; }
     else if (m_etaIdx <  32) { part = CaloDivision::LArHECA; }
     else                     { part = CaloDivision::LArFCAL23A; }
   }
   return part;
}


//********************************************************************
//              getEtaLetterBit
//********************************************************************
/*!
 * Method to return the bit number of the eta "letter".
 * Here we just use 16 bits which each roughly correspond to a 0.4
 * strip in eta except towards the endcaps.
 * In contrast to the cabling document which adds non hex letters
 * Z and G to represent the FCAL, here we lump the FCAL in with
 * the neighbouring areas (letters 0 and F).
 * So bits 0,F are +-2.9 to +-4.9, bits 1,E are +-2.4 to +-2.9
 * and all the rest are regular divisions of 0.4.
 */
unsigned int L1CaloDetectorRegion::getEtaLetterBit() const {
   unsigned int bit = 16;  // Invalid
   if      (m_etaIdx < -29) { bit =  0; }
   else if (m_etaIdx < -24) { bit =  1; }
   else if (m_etaIdx >= 29) { bit = 15; }
   else if (m_etaIdx >= 24) { bit = 14; }
   else { bit = ((m_etaIdx + 24) / 4) + 2; }
   return bit;
}


//********************************************************************
//              getPhiLetterBit
//********************************************************************
/*!
 * Method to return the bit number of the phi "letter".
 * The follows the cabling document using hex letters
 * to divide the 64 phi bins into 16 strips of 0.4 each.
 */
unsigned int L1CaloDetectorRegion::getPhiLetterBit() const {
   unsigned int bit = 16;  // Invalid
   if (m_phiIdx >= 0 && m_phiIdx < 64) { bit = m_phiIdx / 4; }
   return bit;
}


//********************************************************************
//              isDisabled
//********************************************************************
/*!
 * Method to check this region against patterns of disabled areas:
 * \a caloDisabled bit pattern of disabled divisions of the calorimeter
 * \a etaDisabled bit pattern of disabled strips in eta (mainly 0.4)
 * \a phiDisabled bit pattern of disabled strips in phi (0.4)
 */
bool L1CaloDetectorRegion::isDisabled(unsigned int caloDivisionsDisabled,
                                      unsigned int etaLetterBitsDisabled,
                                      unsigned int phiLetterBitsDisabled) const {

   unsigned int caloBit = this->getCaloDivision().getEnum();
   unsigned int etaBit  = this->getEtaLetterBit();
   unsigned int phiBit  = this->getPhiLetterBit();
   bool ret = (((1 << caloBit) & caloDivisionsDisabled) != 0) ||
              (((1 << etaBit)  & etaLetterBitsDisabled) != 0) ||
              (((1 << phiBit)  & phiLetterBitsDisabled) != 0);
   return ret;
}


//********************************************************************
//              isCaloDisabled
//********************************************************************
/*!
 * Method to check this region against patterns of disabled areas:
 * \a caloDisabled bit pattern of disabled divisions of the calorimeter
 */
bool L1CaloDetectorRegion::isCaloDisabled(unsigned int caloDivisionsDisabled) const {

   unsigned int caloBit = this->getCaloDivision().getEnum();
   bool ret = (((1 << caloBit) & caloDivisionsDisabled) != 0);
   return ret;
}


//********************************************************************
//              isEtaPhiDisabled
//********************************************************************
/*!
 * Method to check this region against patterns of disabled areas:
 * \a etaDisabled bit pattern of disabled strips in eta (mainly 0.4)
 * \a phiDisabled bit pattern of disabled strips in phi (0.4)
 */
bool L1CaloDetectorRegion::isEtaPhiDisabled(unsigned int etaLetterBitsDisabled,
                                            unsigned int phiLetterBitsDisabled) const {

   unsigned int etaBit  = this->getEtaLetterBit();
   unsigned int phiBit  = this->getPhiLetterBit();
   bool ret = (((1 << etaBit)  & etaLetterBitsDisabled) != 0) ||
              (((1 << phiBit)  & phiLetterBitsDisabled) != 0);
   return ret;
}


//********************************************************************
//              printInfo
//********************************************************************
/*!
 * Print the content of the L1CaloDetectorRegion object
 */
void L1CaloDetectorRegion::printInfo() {

   std::cout << "Region Object Type: " << objectTypeToString()
             << "  Layer: " << layerToString() << std::endl;
   std::cout << "   Valid Channel: "
             << (getValidity() ? "yes" : "no") << std::endl;

   if (m_valid) {
      std::cout << "      EtaIndex:      " << getEtaIndex()
                << "     PhiIndex: " << getPhiIndex() << std::endl;
      std::cout << "      EtaBinWidth:   " << getEtaBinWidth()
                << "   PhiBinWidth: " << getPhiBinWidth() << std::endl;
      std::cout << "      EtaCoordinate: " << getEtaCoordinate()
                << "  PhiCoordinate: " << getPhiCoordinate() << std::endl;
   }

   std::cout << std::endl;
}
