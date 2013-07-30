/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// TRT_CablingSvc class - interface class for TRT Mapping
//

#ifndef ITRT_CABLINGSVC_H
#define ITRT_CABLINGSVC_H

#include "GaudiKernel/IInterface.h"

#include "eformat/SourceIdentifier.h"

#include <vector>


class IdentifierHash;
class Identifier;


class ITRT_CablingSvc: virtual public IInterface
{

public:

  virtual ~ITRT_CablingSvc(){}

   static const InterfaceID& interfaceID( ) ;


   // Get Identifiers of Collections for the given ROBID
   //   virtual const std::vector<IdentifierHash>& getCollID(uint32_t rob_id) = 0 ;

   // Get Identifier for each straw from BS
   virtual Identifier getIdentifier(const eformat::SubDetector& subdetector,
			    const unsigned& rod, const int& bufferOffset,
			    IdentifierHash& hashId) const = 0 ;  

   // Get buffer offset from staw Identifier
   virtual uint32_t getBufferOffset( const Identifier &StrawId ) = 0;

   // Get ROBIDs for each Detector Element (Straw Layer)
   virtual std::vector<uint32_t> getRobID(Identifier& id) const = 0;

   // Get all RODIDs for TRT
   virtual const std::vector<uint32_t>& getAllRods() const = 0;
  
   // Get number of straws in ROD (DC, BS creation)
   //   virtual int getNumStrawInROD(uint32_t rod_id) const = 0;

   // Get ROD  for each straw (DC, BS creation)
   //   virtual uint32_t getRodID(Identifier& id) const = 0;

   // Get Straw and ROD ID (DC, BS creation)
   //   virtual void getStrawRodID(Identifier& id, uint32_t& rod_id, 
   //			      uint32_t& straw_number) const = 0;

   // Set parameters 
   //   virtual void set_parameters() = 0;

};

inline const InterfaceID& ITRT_CablingSvc::interfaceID( )
{
   static const InterfaceID IID ( "ITRT_CablingSvc", 1, 0 );

   return IID;
}

#endif     // ITRT_CABLINGSVC_H
