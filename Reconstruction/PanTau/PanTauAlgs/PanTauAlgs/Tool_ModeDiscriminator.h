/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class Tool_ModeDiscriminator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool for PID of TauSeeds
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef PANTAUALGS_TOOL_MODEDISCRIMINATOR_H
#define PANTAUALGS_TOOL_MODEDISCRIMINATOR_H

//! Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

//! PanTau
#include "PanTauInterfaces/ITool_ModeDiscriminator.h"
#include "PanTauInterfaces/ITool_InformationStore.h"
#include "PanTauInterfaces/ITool_HelperFunctions.h"

//! C++
#include <vector>
#include <string>


namespace TMVA{
    class Reader;
}

namespace PanTau {
    class PanTauSeed;
}



namespace PanTau {

    /** @class Tool_ModeDiscriminator
        Tool for PID of Tau Seeds
        @author Sebastian Fleischmann
        @author Christian Limbach <limbach@physik.uni-bonn.de>
    */
    class Tool_ModeDiscriminator : public AthAlgTool, virtual public PanTau::ITool_ModeDiscriminator {

    public:
        
        Tool_ModeDiscriminator(const std::string&,const std::string&,const IInterface*);
        virtual ~Tool_ModeDiscriminator ();
        
        virtual StatusCode initialize();
        virtual StatusCode finalize  ();


        virtual double getResponse(PanTau::PanTauSeed* inSeed, bool& isOK);
        virtual double getModeLikeliness(PanTau::PanTauSeed* inSeed, bool& wasSuccessful);
        
    private:
        
        void                                        updateReaderVariables(PanTau::PanTauSeed* inSeed);
        
        std::string                                 m_Name_InputAlg;
        std::string                                 m_Name_ModeCase;
        ToolHandle<PanTau::ITool_InformationStore>  m_Tool_InformationStore;
        ToolHandle<PanTau::ITool_HelperFunctions>   m_Tool_HelperFunctions;
        std::vector<TMVA::Reader*>                  m_TMVA_ReaderList;
        
        std::vector<double>                         m_BinEdges_Pt;
        std::string                                 m_ReaderOption;
        std::string                                 m_MethodName;
//         std::vector<std::string>                    m_List_WeightFiles;
        std::vector<std::string>                    m_List_BDTVariableNames;
        std::vector<float>                          m_List_BDTVariableValues;
        std::vector<double>                         m_List_BDTVariableDefaultValues;
        
    };
} // end of namespace PanTau
#endif // PANTAUALGS_TAUDISCRIMINANTTOOL_H
