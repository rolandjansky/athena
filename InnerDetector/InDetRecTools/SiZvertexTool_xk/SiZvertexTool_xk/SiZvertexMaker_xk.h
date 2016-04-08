/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiZvertexMaker_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for primary vertices z-coordinates generation (xKalman algorithm)
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiZvertexMaker_xk_H
#define SiZvertexMaker_xk_H

#include <list>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetRecToolInterfaces/ISiZvertexMaker.h"  
#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"

class MsgStream;

namespace InDet{

  class SiZvertexMaker_xk : virtual public ISiZvertexMaker, public AthAlgTool
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      SiZvertexMaker_xk
	(const std::string&,const std::string&,const IInterface*);
      virtual ~SiZvertexMaker_xk();
      virtual StatusCode initialize ();
      virtual StatusCode finalize   ();

      ///////////////////////////////////////////////////////////////////
      // Methods to initialize tool for new event or region
      ///////////////////////////////////////////////////////////////////

      virtual void newEvent();
      virtual void newRegion
	(const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&);
      virtual void newRegion
	(const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&,const IRoiDescriptor&);

      ///////////////////////////////////////////////////////////////////
      // Methods for primary vertices production
      ///////////////////////////////////////////////////////////////////

      virtual const std::list<Trk::Vertex>&  getVertices();

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump          (MsgStream   & out) const;
      std::ostream& dump          (std::ostream& out) const;
      
    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected data and methods
      ///////////////////////////////////////////////////////////////////
      
      int*                                       m_histogram     ;
      double*                                    z_histogram     ;
      int                                        m_outputlevel   ;
      int                                        m_nprint        ;
      int                                        m_nspoint       ;
      int                                        m_histsize      ;
      int                                        m_mincontent    ;
      int                                        m_maxcontent    ;
      double                                     m_zmin          ;
      double                                     m_zmax          ;
      double                                     m_ratio         ;
      std::list<Trk::Vertex>                     m_vertex        ;
      ToolHandle<InDet::ISiSpacePointsSeedMaker> m_seedsgenerator; 

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

      void production();
      MsgStream&    dumpConditions(MsgStream   & out) const;
      MsgStream&    dumpEvent     (MsgStream   & out) const;
   };
  MsgStream&    operator << (MsgStream&   ,const SiZvertexMaker_xk&);
  std::ostream& operator << (std::ostream&,const SiZvertexMaker_xk&); 
}

#endif // SiZvertexMaker_xk_H

