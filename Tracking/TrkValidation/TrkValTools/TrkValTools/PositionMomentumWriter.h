/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// PositionMomentumWriter.h
///////////////////////////////////////////////////////////////////

#ifndef TRK_POSITIONMOMENTUMWRITER_H
#define TRK_POSITIONMOMENTUMWRITER_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkValInterfaces/IPositionMomentumWriter.h"

class TTree;

namespace Trk {

/** @class PositionMomentumWriter

    Position momentum writer

    @author Andreas.Salzburger -at- cern.ch
*/

  class PositionMomentumWriter : public AthAlgTool, virtual public IPositionMomentumWriter  {
    public:
      /** standard AlgTool constructor / destructor */
      PositionMomentumWriter(const std::string&,const std::string&,const IInterface*);
      ~PositionMomentumWriter(){}
      
      /** standard Athena methods */
      StatusCode initialize();
      StatusCode finalize();
      
      /** Initialize State */
      virtual void initializeTrack(const Amg::Vector3D& pos,
                                   const Amg::Vector3D& mom,
                                   double m, int pdg) const override;
      
      
      /** Record a single TrackState */
      virtual void recordTrackState(const Amg::Vector3D& pos, const Amg::Vector3D& mom) const override;
      
      /** Finalization State */
      virtual void finalizeTrack() const;
    
  private: 
         
      std::string                       m_treeName;
      std::string                       m_treeFolder;  
      std::string                       m_treeDescription;
      mutable TTree*                    m_tree;

      mutable float                     m_pM;
      mutable float                     m_pEta;
      mutable float                     m_pPhi;
      mutable float                     m_pE;
      mutable float                     m_eEta;
      mutable float                     m_ePhi;
      mutable float                     m_eE;
      mutable int                       m_pPdg;
      mutable std::vector< float >*     m_pPositionX;
      mutable std::vector< float >*     m_pPositionY;      
      mutable std::vector< float >*     m_pPositionZ;
      mutable std::vector< float >*     m_pPositionR;
      mutable std::vector< float >*     m_pMomentumX;
      mutable std::vector< float >*     m_pMomentumY;      
      mutable std::vector< float >*     m_pMomentumZ;
      mutable std::vector< float >*     m_pMomentumMag;
      mutable std::vector< float >*     m_pMomentumEta;
      mutable std::vector< float >*     m_pMomentumPhi;


};


} // end of namespace

#endif // TRK_POSITIONMOMENTUMWRITER_H
