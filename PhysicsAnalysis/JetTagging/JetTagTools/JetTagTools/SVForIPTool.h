/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JetTagTools_SVForIPTool_H
#define JetTagTools_SVForIPTool_H

/******************************************************
    @class SVForIPTool
    Package : JetTagTools
    Created : 23 February 2008
    DESCRIPTION:
    This class computes the secondary vertex direction or gives back the tracks 
    coming from V0s, the way depending on the Tagger used.
   @author Giacinto Piacquadio (giacinto.piacquadio AT physik.uni-freiburg.de)
********************************************************/

#include "GeoPrimitives/GeoPrimitives.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODBTagging/BTagging.h"

#include <vector>
#include <string>

namespace Analysis {

  static const InterfaceID IID_SVForIPTool("Analysis::SVForIPTool", 1, 0);

  class SVForIPTool : public AthAlgTool  {
    
  public:
    
    /** AlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_SVForIPTool; };

    SVForIPTool(const std::string& name,
                    const std::string& n, const IInterface* p);
    ~SVForIPTool();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize();

      /**Method to get the B flight direction from the secondary vertex info */
    void getDirectionFromSecondaryVertexInfo(Amg::Vector3D & SvxDirection,
                                             bool & canUseSvxDirection,
                                             xAOD::BTagging * BTag,
                                             const std::string & secVxFinderName,
                                             const xAOD::Vertex & priVtx);
                                             // const Trk::RecVertex & priVtx);
    

    /**Method to get the tracks from V0 from the secondary vertex info */
    void getTrkFromV0FromSecondaryVertexInfo(std::vector<const xAOD::TrackParticle*> & TrkFromV0,
                                             xAOD::BTagging * BTag,
                                             const std::string & secVxFinderName);


  private:
  
  };
  
  
}// end namespace

#endif
