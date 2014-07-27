/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOJET_T2L1CALOJETTOOL_H
#define TRIGT2CALOJET_T2L1CALOJETTOOL_H

/********************************************************************

NAME:     T2L1CaloJetCalibTool.h
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Matthew Tamsett <tamsett@cern.ch>, Carlos Sandoval <csandova@mail.cern.ch>
CREATED:  December 2010


PURPOSE: Calibration tool for L1 towers. 
         

********************************************************************/

#include "TrigT2CaloJet/T2CaloJetBaseTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgTool.h"
#include "CaloRec/ToolWithConstantsMixin.h"

class TrigT2Jet;
class Trig3Momentum;

class T2L1CaloJetCalibTool : public T2CaloJetBaseTool,
                           public CaloRec::ToolWithConstantsMixin
{
    public:
        
        T2L1CaloJetCalibTool(const std::string& type, const std::string& name, 
                           const IInterface* parent);
        
        ~T2L1CaloJetCalibTool();
        
        virtual StatusCode initialize();
        using T2CaloJetBaseTool::execute;
        virtual StatusCode execute();
        virtual StatusCode finalize();
        
        virtual StatusCode execute(TrigT2Jet* jet,double /*etamin*/, double /*etamax*/, double /*phimin*/, double /*phimax*/);
        
        using AlgTool::setProperty;
        virtual StatusCode setProperty (const std::string& propname,
        const std::string& value);  
        virtual StatusCode setProperty (const Property& p); 
    
    protected:
        /** For Logging, in all tools we should use (*m_log) to avoid recalling of the MsgStream all the time. */
        MsgStream* m_log;
        
        int GetEtaBin(double jet_eta);
        double GetCalibFactor(int etabin, double energy);        
        
        CaloRec::Array<2> m_CalibConstants;
        
        // Bins
        std::vector<double> m_eta_bin_lower_edges;
        

};

#endif // TRIGT2CALOJET_T2L1CALOJETTOOL
