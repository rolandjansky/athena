/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDiphotonVertexID_h
#define IDiphotonVertexID_h

#include "GaudiKernel/IAlgTool.h"
#include <vector>

class egamma;
namespace Trk { class VxCandidate; }

static const InterfaceID IID_IDiphotonVertexID("IDiphotonVertexID", 1, 0);

class IDiphotonVertexID : virtual public IAlgTool
{
  public :
  
    virtual ~IDiphotonVertexID() {};

    static const InterfaceID& interfaceID() { return IID_IDiphotonVertexID; };
    
    /** Return a vector of the vertex candidates with decreasing probability order **/
    virtual std::vector< const Trk::VxCandidate* > getVertex(egamma *photon1, 
                                                             egamma *photon2, 
                                                             bool ignoreConv=false) =0;

    /* Last case treated */
    virtual int getCase() =0;

    /* Get Zcommon between photons */
    virtual float getZcommon() =0;

    /* Get Zerror (on Zcommon) */
    virtual float getZerror()  =0;
    
    /* Get MLP values */
    virtual std::vector<float> getMLP() =0;

};
  
#endif 
