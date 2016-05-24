/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVC_BYTESTREAMATTLISTSVC_H
# define BYTESTREAMCNVSVC_BYTESTREAMATTLISTSVC_H

/**
 * @file ByteStreamAttListMetadataSvc.h
 *
 * @brief AlgTool which takes references provided by RegStream
 * finds the appropriate CORAL object in storegate and stores
 * them in a POOL collection.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Jack Cranshaw <Jack.Cranshaw@cern.ch>
 *
 * $Id: ByteStreamAttListMetadataSvc.h,v 1.11 2009-04-28 19:04:06 cranshaw Exp $
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
//#include "GaudiKernel/ToolHandle.h"

#include "GaudiKernel/IIncidentListener.h"
#include "ByteStreamCnvSvc/IByteStreamFreeMetadataSvc.h"

#include <map>
#include <vector>
#include <string>

/**
 * @class ByteStreamAttListMetadataSvc
 *
 * @brief AlgTool which takes references provided by RegStream
 * finds the appropriate CORAL object in storegate and stores
 * them in a POOL collection.
 *
 */
//static const InterfaceID IID_ByteStreamAttListMetadataSvc("ByteStreamAttListMetadataSvc", 1, 0);

class ByteStreamAttListMetadataSvc : public ::AthService, 
                                     virtual public IByteStreamFreeMetadataSvc
{
public:

  /// Standard ByteStreamAttListMetadataSvc Constructor
  ByteStreamAttListMetadataSvc( const std::string& name, ISvcLocator* pSvcLocator );

  //static const InterfaceID& interfaceID();

  /// Required of all Gaudi services:  see Gaudi documentation for details
  StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  // AthService virtual methods
  virtual StatusCode initialize();
  //virtual StatusCode stop();

  // Interface virtual methods
  StatusCode readInit      (const std::vector<std::string>& keys);
  StatusCode toBSMetadata  (const std::vector<std::string>& keys);
  StatusCode fromBSMetadata(const std::vector<std::string>& keys);

protected:

  /// Standard Destructor
  virtual ~ByteStreamAttListMetadataSvc() {}

private:
  StringProperty m_inputStoreName;
  StringProperty m_outputStoreName;

};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
/*
inline const InterfaceID& ByteStreamAttListMetadataSvc::interfaceID()
{ 
   return IID_ByteStreamAttListMetadataSvc; 
}
*/
#endif // BYTESTREAMCNVSVC_BYTESTREAMATTLISTSVC_H
