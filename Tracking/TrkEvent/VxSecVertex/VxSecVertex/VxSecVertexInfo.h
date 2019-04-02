/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************

    VxSecVertexInfo.h - Description
           -------------------

    begin: March 2007
    authors: CSC vertexing note group members :-)
    work started by: Giacinto Piacquadio (University of Freiburg)
    e-mail: giacinto.piacquadio@physik.uni-freiburg.de
    changes: new!

This class represents the base information about secondary vertices, which is 
given by a vector of VxCandidate*. The class OWNS the VxCandidate. Pointers
 are used to allow inheritance of the extended classes from the VxCandidate.


    April 2014: Trk::VxCandidate is replaced by xAOD::Vertex

    May 2014:  VxSecVertexInfo doesn't own the corresponding secondary vertices anymore by default.
               Beware - this leads to a strong memory leak if SV are not explictly deleted before VxSecVertexInfo destructor.
	       VxSecVertexInfo gets back the SV ownership if getSVOwnership(true) is called.  

*****************************************/

#ifndef VXSECVERTEX_VXSECVERTEXINFO_H
#define VXSECVERTEX_VXSECVERTEXINFO_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "xAODTracking/Vertex.h"
#include <vector>


namespace Trk {

  class VxSecVertexInfo 
  {

  public:

    /* default constructor for persistency */
    VxSecVertexInfo();

    /* constructor with list of VxCandidate. The VxSecVertexInfo takes 
       care of the ownership of the VxCandidate*. Don't delete them 
       afterwards */
    VxSecVertexInfo(const std::vector<xAOD::Vertex*> &);

    /* clone method */
    virtual VxSecVertexInfo* clone() const;

    /* Destructor */
    virtual ~VxSecVertexInfo();
    
    /* Copy constructor */
    VxSecVertexInfo(const VxSecVertexInfo &);
    VxSecVertexInfo(VxSecVertexInfo &&) noexcept = default;

    /* Assignment operator */
    VxSecVertexInfo& operator= (const VxSecVertexInfo &);
    VxSecVertexInfo& operator= (VxSecVertexInfo &&) noexcept = default;

    /* get the list of Vertices */
    const std::vector<xAOD::Vertex*> & vertices() const;

    /* set the list of Vertices */
    void setVertices(const std::vector<xAOD::Vertex*> &);

    void getSVOwnership(bool Ownership) const { m_SVOwnership=Ownership; }

  protected:

    std::vector<xAOD::Vertex*> m_vertices;
    mutable bool m_SVOwnership;

  };



  inline VxSecVertexInfo* VxSecVertexInfo::clone() const {
    return new VxSecVertexInfo(*this);
  }


}//end namespace

namespace Trk
{
  typedef DataVector<Trk::VxSecVertexInfo> VxSecVertexInfoContainer;
}


CLASS_DEF( Trk::VxSecVertexInfo , 170608128 , 1 )
CLASS_DEF( Trk::VxSecVertexInfoContainer , 1284088780 , 1 )



#endif
