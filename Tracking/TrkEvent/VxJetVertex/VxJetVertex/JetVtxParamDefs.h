/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VtxParamDefs.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef VxJetVertex_JetVtxParamDefs_H
#define VxJetVertex_JetVtxParamDefs_H

#include <vector>
#include <string>

namespace Trk {

 /** @enum JetVtxParamDefs 

    @author Nicola.Giacinto.Piacquadio@cern.ch
    @author Christian.Weiser@cern.ch
   */



   enum JetVtxParamDefs        {
   // Enums for vertex position on the jet axis starting from the primary vertex
     jet_xv=0, jet_yv=1, jet_zv=2,                     //!< position x,y,z of primary vertex
     jet_phi=3,jet_theta=4,                        //direction of the jet axis (flight direction of B hadron),
     jet_dist=5                                //distance of vertex on jet axis from primary vertex
   };
                            
  /** @struct JetVtxParamDefsAccessor
      
      Simple struct to access the 
      JetVtxParamDefs enum with ints
      
    */
  struct JetVtxParamDefsAccessor {
     /**Constructor*/
     JetVtxParamDefsAccessor()
      { 
        pardef.push_back(jet_xv);
        pardef.push_back(jet_yv);
        pardef.push_back(jet_zv);
        pardef.push_back(jet_phi);
        pardef.push_back(jet_theta);
        pardef.push_back(jet_dist);
      }
     /**The member*/ 
     std::vector<JetVtxParamDefs> pardef;
  
  };    
      
  /** @struct JetVtxParamDefsStrings
      
      Simple struct to access the 
      strings of the JetVtxParamDefs enum names 
      for output reasons
      
    */
  struct JetVtxParamDefsStrings {
     /**Constructor*/
     JetVtxParamDefsStrings()
      { 
        pardefname.push_back("jet_xv");
        pardefname.push_back("jet_yv");
        pardefname.push_back("jet_zv");
        pardefname.push_back("jet_phi");
        pardefname.push_back("jet_theta");
	pardefname.push_back("jet_dist");
      }
     
     /**The member*/ 
     std::vector<std::string> pardefname;
  
  };                                    
                            
  
} // end of namespace

#endif // FITSECONDARYVERTEX_PARAMDEFS_H
