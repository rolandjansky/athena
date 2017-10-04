/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_DistortionsTool_H
#define SCT_DistortionsTool_H

#include "SCT_ModuleDistortions/ISCT_ModuleDistortionsTool.h"
//#include "CLHEP/Geometry/Vector3D.h"
//#include "CLHEP/Geometry/Point3D.h"

#include <string>
#include <map>
#include "AthenaBaseComps/AthAlgTool.h"

class SCT_ID;

// SCT class to correct for module distortions
class SCT_DistortionsTool : public AthAlgTool, virtual public ISCT_ModuleDistortionsTool {
    public:
  //virtual ~SCT_DistortionsTool();
    SCT_DistortionsTool (const std::string&, const std::string&, const IInterface*);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    // Find Z shift for x,y hit
    virtual float zShift( const double xhit, const double yhit,const  std::vector<float>* ZVec ) const;
    // Identify Assembly Region
    virtual int identifyRegion(IdentifierHash id) const;
    // Load Common Profile
    virtual const std::vector<float>* readDistortions(int RegionID, int Side) const;

    // Correct the rescontruction position 
    virtual double correctReconstruction(const Trk::TrackParameters & trackPar,
						       const InDetDD::SiDetectorElement & EL, 
						       Trk::LocalParameters & locpar,  
						       const Amg::Vector2D & loct) const; 
    // Correct the simulation position 
    virtual Amg::Vector2D correctSimulation(IdentifierHash id, 
					 double xhit ,
					 double yhit ,
					 double cEta , 
					 double cPhi ,
					 double cDep  ) const; 

    // Get just the correction
    virtual Amg::Vector2D correction(IdentifierHash id, 
				  const Amg::Vector2D & locpos, 
				  const Amg::Vector3D & direction) const; 

    private:  

    //! identifier-helper
    const SCT_ID*                     m_sctID;

    std::string m_textFileNameJ1;
    std::string m_textFileNameJ2;
    std::string m_textFileNameUK;
    std::string m_textFileNameUSA;
    std::string m_textFileNameScand;
    std::string m_textFileNameProfiles;

    float m_distortionsScale;

    mutable std::map<int,int> m_moduleSites;
    
    std::vector<float>* m_dataJap1_S0;
    std::vector<float>* m_dataJap2_S0;
    std::vector<float>* m_dataUK_S0;
    std::vector<float>* m_dataUSA_S0;
    std::vector<float>* m_dataScand_S0;

    std::vector<float>* m_dataJap1_S1;
    std::vector<float>* m_dataJap2_S1;
    std::vector<float>* m_dataUK_S1;
    std::vector<float>* m_dataUSA_S1;
    std::vector<float>* m_dataScand_S1;
    
    bool loadData() const;

};

#endif // SCT_DistortionsTool_H


