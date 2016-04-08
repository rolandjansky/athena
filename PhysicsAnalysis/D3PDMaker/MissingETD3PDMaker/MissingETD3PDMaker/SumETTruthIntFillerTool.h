// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/SumETTruthIntFillerTool.h
 * @author jet goodson <jgoodson@cern.ch>
 * @modelled on scott snyder's MissingETFillerTool
 * @date Oct, 2009
 * @brief Block filler tool for Missing ET Int truth objects.
 */


#ifndef MISSINGETD3PPDMAKER_SUMETTRUTHINTFILLERTOOL_H
#define MISSINGETD3PPDMAKER_SUMETTRUTHINTFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "MissingETEvent/MissingEtTruth.h"
#include "StoreGate/StoreGateSvc.h"
class MissingET;


namespace D3PD {


/**
 * @brief Block filler tool for Missing ET Truth objects.
 */
class SumETTruthIntFillerTool
  : public BlockFillerTool<MissingEtTruth>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  SumETTruthIntFillerTool (const std::string& type,
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
  virtual StatusCode fill (const MissingEtTruth& p);

protected:
 MsgStream mLog;

private:
  
  //interacting met
 
  float* m_int_sumet; /// Variable: total et. 
 

 //central interacting met
 
  float* m_intcentral_sumet; /// Variable: total et. 
 

//fwd interacting met
 
  float* m_intfwd_sumet; /// Variable: total et. 
  


//outcover interacting met

  float* m_intoutcover_sumet; /// Variable: total et. 
 


//muons interacting met
 
  float* m_muons_sumet; /// Variable: total et. 
 

};


} // namespace D3PD


#endif // not MISSINGETD3PPDMAKER_SUMETTRUTHINTFILLERTOOL_H
