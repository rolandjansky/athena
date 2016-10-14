/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_ITOOL_INFORMATIONSTORE_H
#define PANTAUALGS_ITOOL_INFORMATIONSTORE_H
// Gaudi
#include "AsgTools/IAsgTool.h"

#include <string>

//class eflowObjectContainer;

// namespace Analysis {
//     class TauJetContainer;
//     class TauDetailsContainer;
// }

//! xAOD EDM
/*
#include "xAODTau/TauJetContainer.h"

namespace Rec {
    class TrackParticleContainer;
}
*/

namespace PanTau {


    /** @class ITool_InformationStore
        @brief Interface for Tool_InformationStore
        @author Christian Limbach (limbach@physik.uni-bonn.de)
    */


    class ITool_InformationStore : virtual public asg::IAsgTool {
        ASG_TOOL_INTERFACE(ITool_InformationStore)


        public:

    virtual bool isInitialized() = 0;

    typedef std::map<std::string, std::string>                  MapString;
    typedef std::map<std::string, std::vector<std::string> >    MapVecString;
    typedef std::map<std::string, int>                          MapInt;
    typedef std::map<std::string, double>                       MapDouble;
    typedef std::map<std::string, std::vector<double> >         MapVecDouble;

#ifdef XAOD_ANALYSIS
    virtual inline void setMapString( MapString&v ) = 0 ;
    virtual inline void setMapVecString( MapVecString&v ) = 0 ;
    virtual inline void setMapInt( MapInt &v ) = 0 ;
    virtual inline void setMapDouble( MapDouble &v ) = 0 ;
    virtual inline void setMapVecDouble( MapVecDouble &v ) = 0 ;
#endif

            //virtual StatusCode updateInformation(std::string inputAlg) = 0;
            virtual StatusCode getInfo_Int(std::string varName,     int& value) = 0;
            virtual StatusCode getInfo_Double(std::string varName,  double& value) = 0;
            virtual StatusCode getInfo_VecDouble(std::string varName,  std::vector<double>& value) = 0;
            virtual StatusCode getInfo_String(std::string varName,  std::string& value) = 0;
            virtual StatusCode getInfo_VecString(std::string varName,  std::vector<std::string>& value) = 0;
            
            virtual StatusCode dumpMaps() const = 0;

            /*
            virtual const xAOD::TauJetContainer*            getContainer_TauRec() const = 0;
            virtual const Rec::TrackParticleContainer*      getContainer_TrackParticle() const = 0;
	    */
    };
    
}
#endif //PANTAUALGS_ITOOL_INFORMATIONSTORE_H

