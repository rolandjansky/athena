// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/MissingETTruthIntFillerTool.h
 * @author jet goodson <jgoodson@cern.ch>
 * @modelled on scott snyder's MissingETFillerTool
 * @date Oct, 2009
 * @brief Block filler tool for Missing ET Int truth objects.
 */


#ifndef MISSINGETD3PPDMAKER_MISSINGETTRUTHINTFILLERTOOL_H
#define MISSINGETD3PPDMAKER_MISSINGETTRUTHINTFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "MissingETEvent/MissingEtTruth.h"
#include "StoreGate/StoreGateSvc.h"
class MissingET;


namespace D3PD {


/**
 * @brief Block filler tool for Missing ET Truth objects.
 */
class MissingETTruthIntFillerTool
  : public BlockFillerTool<MissingEtTruth>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MissingETTruthIntFillerTool (const std::string& type,
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
  float* m_int_etx;/// Variable: x component. 
  float* m_int_ety;/// Variable: y component.
 

 //central interacting met
  float* m_intcentral_etx;/// Variable: x component. 
  float* m_intcentral_ety;/// Variable: y component.
 

//fwd interacting met
  float* m_intfwd_etx;/// Variable: x component. 
  float* m_intfwd_ety;/// Variable: y component.
 


//outcover interacting met
  float* m_intoutcover_etx;/// Variable: x component. 
  float* m_intoutcover_ety;/// Variable: y component.
 


//muons interacting met
  float* m_muons_etx;/// Variable: x component. 
  float* m_muons_ety;/// Variable: y component.
  

};


} // namespace D3PD


#endif // not MISSINGETD3PPDMAKER_MISSINGETFILLERTOOL_H
