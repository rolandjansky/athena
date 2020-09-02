// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenVertexFillerTool.h 639304 2015-01-13 20:08:39Z ssnyder $
/**
 * @file EventCommonD3PDMaker/src/GenVertexFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2010
 * @brief Fill the position of a vertex.
 */


#ifndef EVENTCOMMOND3PDMAKER_GENVERTEXFILLERTOOL_H
#define EVENTCOMMOND3PDMAKER_GENVERTEXFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "AtlasHepMC/GenVertex.h"


namespace D3PD {


/**
 * @brief Fill the position of a vertex.
 */
class GenVertexFillerTool
  : public BlockFillerTool<HepMC::GenVertex>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  GenVertexFillerTool (const std::string& type,
                       const std::string& name,
                       const IInterface* parent);


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const HepMC::GenVertex& p);


private:
  /// Parameter: Write vertex ID
  bool m_do_id;

  /// Variable: Vertex x position.
  float* m_x;

  /// Variable: Vertex y position.
  float* m_y;

  /// Variable: Vertex z position.
  float* m_z;

  /// Variable: Vertex barcode.
  int *m_barcode;

  /// Variable: Vertex ID
  int *m_id;

};


} // namespace D3PD


#endif // not EVENTCOMMOND3PDMAKER_GENVERTEXFILLERTOOL_H
