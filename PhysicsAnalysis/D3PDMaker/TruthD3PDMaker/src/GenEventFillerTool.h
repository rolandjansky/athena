// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenEventFillerTool.h 651255 2015-03-03 16:29:01Z ssnyder $
/**
 * @file EventCommonD3PDMaker/src/GenEventFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Block filler tool for @c GenEvent information.
 */


#ifndef TRUTHD3PPDMAKER_GENEVENTFILLERTOOL_H
#define TRUTHD3PPDMAKER_GENEVENTFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "AtlasHepMC/GenEvent.h"


namespace D3PD {


/**
 * @brief Block filler tool for @c GenEvent information.
 */
class GenEventFillerTool
  : public BlockFillerTool<HepMC::GenEvent>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  GenEventFillerTool (const std::string& type,
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
  virtual StatusCode fill (const HepMC::GenEvent& p);


private:
  /// Variable: Signal process ID.
  int* m_signal_process_id;

  /// Variable: Event number.
  int* m_event_number;

  /// Variable: Event scale.
  double* m_event_scale;

  /// Variable: alpha_QCD.
  double* m_alphaQCD;

  /// Variable: alpha_QED.
  double* m_alphaQED;

  /// Variable: pdf id1
  int* m_pdf_id1;

  /// Variable: pdf id2
  int* m_pdf_id2;

  /// Variable: pdf x1
  double* m_pdf_x1;

  /// Variable: pdf x2
  double* m_pdf_x2;

  /// Variable: pdf scale
  double* m_pdf_scale;

  /// Variable: pdf name 1
  double* m_pdf1;

  /// Variable: pdf name 2
  double* m_pdf2;

  /// Variable: MC event weights
	std::vector<double>* m_weight;

  // vertex?
  // weights?

};


} // namespace D3PD


#endif // not TRUTHD3PPDMAKER_GENEVENTFILLERTOOL_H

