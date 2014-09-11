/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    @class MultiSVTag
 ***************************************************************************/

#ifndef JETTAGTOOLS_MULTISVTAG_H
#define JETTAGTOOLS_MULTISVTAG_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetTagTools/ITagTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <vector>
#include <map>


//namespace xAOD { class TrackParticle; class TrackParticleContainer; }
namespace Trk { class VxCandidate; class RecVertex;}
//class Jet;
class StoreGateSvc;

namespace Analysis
{  
  
  class MultiSVTag : public AthAlgTool , virtual public ITagTool
    {
    public:
      MultiSVTag(const std::string&,const std::string&,const IInterface*);
      virtual ~MultiSVTag();
      StatusCode initialize();
      StatusCode finalize();
      
      void tagJet(xAOD::Jet& jetToTag);
      void setOrigin(const Trk::VxCandidate* priVtx);
      void finalizeHistos();
      
    private:      
      
      //GP: calculate the 3d significance on the fly
      double get3DSignificance(const Trk::RecVertex & priVertex,
                               std::vector<const Trk::RecVertex*> & secVertex,
                               const Amg::Vector3D jetDirection);
      
      
      std::string m_runModus; 
   
      bool m_writeInfoBase; // writes a basic info for each tagger with Pb, Pu (SVInfoBase)
      bool m_writeInfoPlus; // writes a detailed info
      std::string m_infoPlusName; // key for the SVInfoPlus

      /** information to persistify: */
      std::string m_originalTPCollectionName;
      const xAOD::TrackParticleContainer* m_originalTPCollection;
      
      const Trk::VxCandidate* m_priVtx;

      std::vector<std::string> m_jetCollectionList, m_jetWithInfoPlus;

      //name of the VKalVrt secondary vertex in Jet instance
      //(needed to retrieve the ISvxConstituent written 
      //out by the finder and attached to the ParticleJet)
      std::string m_secVxFinderName;

    }; // End class

  inline void MultiSVTag::setOrigin(const Trk::VxCandidate* priVtx) {m_priVtx = priVtx;}

} // End namespace 

#endif
