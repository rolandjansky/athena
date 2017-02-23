/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTPVERTEXHELPER_H
#define MUONTPVERTEXHELPER_H



#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "xAODTracking/VertexFwd.h"
#include "MuonTPTools/IMuonTPVertexHelper.h"

// forward declaration saves us a public use statement
namespace xAOD{
    class BPhysHypoHelper;
}


// Utility class for our vertex association related needs.
// The way it works is that the checkVertexMatch method should be called first.
// This checks the j/psi vertex container in StoreGate for a vertex containing the tag and probe ID tracks
// if it finds one, it calls the Probe::setVertex method to make the probe remember this vertex association
// The method returns true if a vertex is found, false otherwise.
// 
// Then, the other public methods can be used to extract information based on the vertex association.
// This is done using the existing decorations applied by the BPhys augmentation code (upstream)
// The decorations are accessed using the BPhysHypoHelper class. 
// An instance of this is kept as m_helper, and the switchHelper method make sure it 
// returns the properties of the vertex associated to the TP par in question (available after checkVertexMatch is called once on the pair).
// 
class MuonTPVertexHelper :
          public asg::AsgTool,
          virtual public IMuonTPVertexHelper {
    ASG_TOOL_CLASS(MuonTPVertexHelper, IMuonTPVertexHelper)
public: 

    // constructor
    MuonTPVertexHelper(std::string myname);
    // initialisation (trivial)
    virtual StatusCode initialize  ();
    // find the vertex (if any) corresponding to our TP pair. 
    // return false if no vertex found
    // otherwise, return true and call probe.setVertex(..)
    virtual bool checkVertexMatch (Probe& probe);
    // get vertex displacement in mm. 
    // if no vertex assigned to the probe or if something goes wrong, return -1e9
    virtual float getVertexDisplacement (Probe & probe, xAOD::BPhysHelper::pv_type pv_t = xAOD::BPhysHelper::PV_MIN_A0);
    // get vertex time of flight in ps. 
    // if no vertex assigned to the probe or if something goes wrong, return -1e9
    virtual float getVertexTau (Probe & probe, xAOD::BPhysHelper::pv_type pv_t = xAOD::BPhysHelper::PV_MIN_A0);
    // get chi2 of vertex fit 
    virtual float getChi2(Probe & probe);
    // get degree of freedom of vertex fit 
    virtual int getDof(Probe& probe);
    // get invariant mass of refittet track
    virtual float getInvMass(Probe& probe);

private:
    // helper method. Updates m_helper (below) to point at the vertex passed as argument
    // This way, we can profit from the caching functions of BPhysHelper.
    // This method is called automatically in all of the public methods above, so no need to do so by hand! 
    void switchHelper(const xAOD::Vertex* vx); 
    // the name of the vertex container we want to retrieve. Steered via the jobOptions
    std::string m_vertex_container_name;
    // BPhysHelper instance 
    xAOD::BPhysHypoHelper* m_helper;

};
#endif // MUONTPVERTEXHELPER_H
