/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class TauImpactParameterExtractionTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool to store information needed in PanTau Algorithms
///////////////////////////////////////////////////////////////////
// limbach@physik.uni-bonn.de
///////////////////////////////////////////////////////////////////

#ifndef PANTAUALGS_TOOL_INFORMATIONSTORE_H
#define PANTAUALGS_TOOL_INFORMATIONSTORE_H


//! C++
#include <map>
#include <string>

//! Gaudi
#include "AthenaBaseComps/AthAlgTool.h"

//! PanTau
#include "PanTauInterfaces/ITool_InformationStore.h"

namespace Rec {
    class TrackParticleContainer;
}


class eflowObjectContainer;
class StoreGateSvc;

// namespace Analysis {
//     class TauJetContainer;
// }

#include "xAODTau/TauJetContainer.h"

namespace PanTau {

/** @class Tool_InformationStore
    Tool to store information needed in PanTau Algorithms
    @author Christian Limbach (limbach@physik.uni-bonn.de)
*/
class Tool_InformationStore : public AthAlgTool, virtual public PanTau::ITool_InformationStore {
    
    
    typedef std::map<std::string, std::string>                  MapString;
    typedef std::map<std::string, std::vector<std::string> >    MapVecString;
    typedef std::map<std::string, int>                          MapInt;
    typedef std::map<std::string, double>                       MapDouble;
    typedef std::map<std::string, std::vector<double> >         MapVecDouble;
    
    
    
    public:
        
        Tool_InformationStore(const std::string&,const std::string&,const IInterface*);
        virtual ~Tool_InformationStore ();
        
        virtual StatusCode initialize();
//         virtual StatusCode finalize  ();
        
        virtual StatusCode updateInformation(std::string inputAlg);
        virtual StatusCode getInfo_Int(std::string varName,     int& value);
        virtual StatusCode getInfo_Double(std::string varName,  double& value);
        virtual StatusCode getInfo_VecDouble(std::string varName,  std::vector<double>& value);
        virtual StatusCode getInfo_String(std::string varName,  std::string& value);
        virtual StatusCode getInfo_VecString(std::string varName,  std::vector<std::string>& value);
        
        StatusCode  dumpMaps() const;
        
        void checkEFOContainer(const eflowObjectContainer* inputContainer, eflowObjectContainer* outputContainer);
        
        virtual const eflowObjectContainer*             getContainer_eflowRec() const;
//         virtual const Analysis::TauJetContainer*        getContainer_TauRec() const;
        virtual const xAOD::TauJetContainer*            getContainer_TauRec() const;
        virtual const Rec::TrackParticleContainer*      getContainer_TrackParticle() const;
        
    private:
        
        //! named strings, ints etc... for configuration
        MapString       m_Infos_String;
        MapVecString    m_Infos_VecString;
        MapInt          m_Infos_Int;
        MapDouble       m_Infos_Double;
        MapVecDouble    m_Infos_VecDouble;
        
        StoreGateSvc*   m_sgSvc;
        
        //!other information
        // input containers
        std::string                       m_Name_Container_eflowRec;
        std::string                       m_Name_Container_TauRec;
        std::string                       m_Name_Container_Tracks;
        
        eflowObjectContainer*                   m_Container_eflowRec;
        //const eflowObjectContainer*             m_Container_eflowRecFromSG;
//         const Analysis::TauJetContainer*        m_Container_TauRec;
        const xAOD::TauJetContainer*            m_Container_TauRec;
        const Rec::TrackParticleContainer*      m_Container_TrackParticle;
        
};


} // end of namespace PanTau
#endif // PANTAUALGS_TAUIMPACTPARAMETEREXTRACTIONTOOL_H
 
