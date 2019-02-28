/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/SVForIPTool.h"

#include "GeoPrimitives/GeoPrimitivesHelpers.h"

namespace Analysis {


  SVForIPTool::SVForIPTool(const std::string& name,
                           const std::string& n, const IInterface* p):
    AthAlgTool(name, n,p)
  {
    declareInterface<SVForIPTool>(this);
  }  

  SVForIPTool::~SVForIPTool() {}

  StatusCode SVForIPTool::initialize() {
    
    ATH_MSG_INFO(" Initialization of SVForIPTool succesful");
    return StatusCode::SUCCESS;
  }

  StatusCode SVForIPTool::finalize() {
    
    ATH_MSG_INFO(" Finalization of SVForIPTool succesful");
    return StatusCode::SUCCESS;
  }

  void SVForIPTool::getDirectionFromSecondaryVertexInfo(Amg::Vector3D & SvxDirection,
                                                        bool & canUseSvxDirection,
                                                        xAOD::BTagging* BTag,
                                                        const std::string & secVxFinderName,
                                                        const xAOD::Vertex & priVtx)
  {
    std::vector< ElementLink< xAOD::VertexContainer > > myVertices;
    BTag->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(secVxFinderName, "vertices", myVertices);
    
    if (myVertices.size() == 0) {
      ATH_MSG_DEBUG(" No secondary vertex found for getting the B flight direction (for the IP sign calculation)");
    } else {
	  if (myVertices[0].isValid())
	    {
	      canUseSvxDirection=true;
	      SvxDirection=(*myVertices[0])->position()-priVtx.position();
	      ATH_MSG_VERBOSE(" Get direction from InDetVKalVertex: phi: " << SvxDirection.phi() << 
			      " theta: " << SvxDirection.theta() );
	    }
	  else 
	    {
	      ATH_MSG_WARNING("SVX info seems usable, but no SVX available !!!");
	    }
    }	
  }
  
  
    
  void SVForIPTool::getTrkFromV0FromSecondaryVertexInfo(std::vector<const xAOD::TrackParticle*> & TrkFromV0,
							xAOD::BTagging* BTag,
                                                        const std::string & secVxFinderName)
  {
    std::vector<ElementLink<xAOD::TrackParticleContainer> > TrkFromV0_ELs;
      
    BTag->variable<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(secVxFinderName, "badTracksIP", TrkFromV0_ELs);

    for (std::vector<ElementLink<xAOD::TrackParticleContainer> >::iterator itr=TrkFromV0_ELs.begin();itr!=TrkFromV0_ELs.end();itr++)
      {
	if (itr->isValid())
	  {
	    TrkFromV0.push_back((**itr));
	  }
      }
  }
  
}//end namespace

