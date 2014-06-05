/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JetTagD3PDMaker/src/JetTagPixelModCondFillerTool.h
 * @author Georges Aad
 * @date Jun, 2011
 * @brief filler for dead pixel modules
 */

#ifndef JetTagD3PDMaker_JetTagPixelModCondFillerTool_H
#define JetTagD3PDMaker_JetTagPixelModCondFillerTool_H

#include "D3PDMakerUtils/BlockFillerTool.h"

#include <vector>

namespace InDetDD{
  class SiDetectorElement;
  class SiDetectorElementCollection;
}


class PixelID;
class IInDetConditionsSvc;

namespace D3PD {


class JetTagPixelModCondFillerTool
  : public BlockFillerTool<InDetDD::SiDetectorElementCollection>
{
public:

  JetTagPixelModCondFillerTool (const std::string& type,
			   const std::string& name,
			   const IInterface* parent);
  

  /// Book variables for this block.
  virtual StatusCode book();
  virtual StatusCode initialize();

  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const InDetDD::SiDetectorElementCollection& p);
  
  
private:


  bool selectModule(const InDetDD::SiDetectorElement*);

  const PixelID* m_pixId;
  ServiceHandle< IInDetConditionsSvc > m_pixelCondSummarySvc;

  std::vector<short> *m_idHash;
  int *m_ndead;
  int m_maxDeadModules;

};


} // namespace D3PD


#endif /// JetTagD3PDMaker_JetTagPixelModCondFillerTool_H
