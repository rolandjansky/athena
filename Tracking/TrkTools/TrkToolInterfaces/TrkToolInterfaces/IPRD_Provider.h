/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPRD_Provider.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_TRUTHTRACKINTERFACES_ITTRUTHPRDPROVIDER_H
#define TRK_TRUTHTRACKINTERFACES_ITTRUTHPRDPROVIDER_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"
 
class Identifier; 
class IdentifierHash;
 
namespace Trk {
 
  class PrepRawData;

  static const InterfaceID IID_IPRD_Provider("IPRD_Provider", 1, 0);
   
  /**
   @class IPRD_Provider

   @brief Simple interface that takes an identifier and find the associated 
   PRD from an Identifyable container
       
   @author Andreas.Salzburger -at- cern.ch
   */
     
  class IPRD_Provider : virtual public IAlgTool {

     public:     
       /** Virtual destructor */
       virtual ~IPRD_Provider(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IPRD_Provider; }

       /** return the Prd given the Identifier - returns the number of degrees of freedom */
       virtual const Trk::PrepRawData* prdFromIdentifier(const Identifier& ide, size_t& ndof ) const = 0;
       
       /** call to retrieve the collections (can't be Incident) */
       virtual StatusCode retrieveCollection() = 0;

  };

} // end of namespace

#endif // TRK_TRUTHTRACKINTERFACES_ITTRUTHPRDPROVIDER_H