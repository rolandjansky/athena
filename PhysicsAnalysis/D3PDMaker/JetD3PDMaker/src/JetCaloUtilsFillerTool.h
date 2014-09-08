// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetCaloUtilsFillerTool.h
 * @author Jet Goodson < jgoodson AT cern.ch > 
 * @author Haifeng Li <Haifeng.Li@cern.ch>
 * @date March, 2010
 * @brief Block filler tool for Jet timing
 */

#ifndef JETCALOUTILSFILLERTOOL_H
#define JETCALOUTILSFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
class Jet;


namespace D3PD {


  /**
   * @brief Block filler tool for Jet EM Fraction.
   */
  class JetCaloUtilsFillerTool
    : public BlockFillerTool<Jet>
  {
  public:
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    JetCaloUtilsFillerTool (const std::string& type,
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
    virtual StatusCode fill (const Jet& jet);
    
    
  private:
    /// Variable: Array of Energy Samplings.
    float *m_hecf;
    float *m_tileGap3f;
    
    bool *m_isUgly;
    bool *m_isBadLooseMinus ; 
    bool *m_isBadLoose ; 
    bool *m_isBadMedium;
    bool *m_isBadTight ;
    
    typedef enum  { LooseMinusBad,  LooseBad, MediumBad,  TightBad  } BadJetCategory;
    bool isBad(BadJetCategory criteria, double quality, double NegE, double emf, 
	       double hecf, double time,double fmax, double eta, double chf, double HecQ, double AvgLArQ );
  
  };
  

} // namespace D3PD


#endif // not JETCALOUTILSFILLERTOOL_H
