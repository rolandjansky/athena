// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/MissingETGoodnessFillerTool.h
 * @author Max Baak <mbaak@cern.ch>
 * @date Nov, 2009
 * @brief Block filler tool for derived met objects.
 */


#ifndef MISSINGETD3PPDMAKER_MISSINGETGOODNESSFILLERTOOL_H
#define MISSINGETD3PPDMAKER_MISSINGETGOODNESSFILLERTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

//#include "D3PDMakerUtils/VoidBlockFillerTool.h"
#include "D3PDMakerUtils/BlockFillerTool.h"

#include "MissingETGoodness/Goodies.h"
#include <vector>
#include "TString.h"

class MissingET;

namespace MET {
  class EtmissGoodness;
}


namespace D3PD {

  /**
   * @brief Block filler tool for storing dummy(!) Missing ET objects.
   */
  class MissingETGoodnessFillerTool : public BlockFillerTool<MissingET> // BlockFillerTool<void>
  {
   public:
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    MissingETGoodnessFillerTool (const std::string& type,
  			         const std::string& name,
			         const IInterface* parent);

    virtual ~MissingETGoodnessFillerTool();

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
    virtual StatusCode fill (const MissingET& p);

   protected:
    MsgStream m_log;

   private:

    std::vector<int*>    m_pintVec;
    std::vector<double*> m_pdoubleVec;
    std::vector<std::vector<double>* > m_pvectorDoubleVec;
    std::vector<std::vector<int>* > m_pvectorIntVec;
    std::vector<bool*>   m_pboolVec;
    std::vector<TString> m_ginameList;
    std::vector<TString> m_gdnameList;
    std::vector<TString> m_gvdnameList;
    std::vector<TString> m_gvinameList;
    std::vector<TString> m_qualifierList;

    /// reference to MET Goodies map for storing derived met goodness quantities
    static MET::Goodies& goodies;

    int       m_dummyi;
    double    m_dummyd;
    std::vector<double>    m_dummyvD;
    std::vector<int>    m_dummyvI;
    Long64_t* m_pbitmask;
    bool      m_issecondbook;
 
    MET::EtmissGoodness* m_metgness;
  };

} // namespace D3PD

#endif 
