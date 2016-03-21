/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_ALIGNDBSVC_H
#define ITRT_ALIGNDBSVC_H

/** @file ITRT_AlignDbSvc.h
 * @brief abstract interface to Service to manage TRT alignment conditions
 * @author John Alison, Peter Hansen <phansen@nbi.dk>, Wouter Hulsbergen
 */


#include <string>
#include "GaudiKernel/IService.h"
#include "AthenaKernel/IOVSvcDefs.h" // For IOVSVC_CALLBACK_ARGS macro def.
// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

class AlignableTransform;
class Identifier; 

/** @class ITRTAlignDbSvc
    abstract interface to Service to manage TRT alignment data
*/
class ITRT_AlignDbSvc: virtual public IService
{
 public:
  
  /**  access to interfaceID */
  static const InterfaceID& interfaceID() ;
  
  /** Call back function for alignment folders */
  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS) =0;
  
  /** write AlignableTransforms to flat text file */
  virtual StatusCode writeAlignTextFile(std::string) const =0;
  virtual StatusCode writeStrawAlignTextFile(std::string file) const = 0;
  virtual StatusCode writeGlobalFolderFile(std::string file) const = 0;
  
  /** read AlignableTransforms from text file into TDS */
  virtual StatusCode readAlignTextFile(std::string) =0;

  /** write the alignment objects to output */
  virtual StatusCode streamOutAlignObjects () const =0;

  /** register alignment objects with the IoV service */
  virtual StatusCode registerAlignObjects(std::string, int, int, int, int) const =0;

  /** set AlignableTransform for an identifier */
  virtual StatusCode setAlignTransform(Identifier ident, Amg::Transform3D trans, unsigned int level) =0;
    
  /** tweak AlignableTransform for an identifier */
  virtual StatusCode tweakAlignTransform(Identifier, Amg::Transform3D, unsigned int level) =0;

  /** get AlignableTransform for an identifier */
  virtual const Amg::Transform3D getAlignmentTransform(Identifier const& ident, unsigned int level) const =0;
  virtual const Amg::Transform3D* getAlignmentTransformPtr(Identifier const& ident, unsigned int level) const =0;
};


inline const InterfaceID& ITRT_AlignDbSvc::interfaceID() {
  static const InterfaceID id("ITRT_AlignDbSvc",1,0);
  return id;
}
#endif 
