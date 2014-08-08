/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_BTAGREMOVING_H
#define BTAGGING_BTAGREMOVING_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "BTagging/IBTagRemoving.h"


#include <string>

class Jet;

/** The namespace of all packages in PhysicsAnalysis/JetTagging */
namespace Analysis
{
  class IBTagRemoving;

  class BTagRemoving : public AthAlgTool, virtual public IBTagRemoving
  {
      public:
          /** Constructors and destructors */
          BTagRemoving(const std::string&,const std::string&,const IInterface*); 
          virtual ~BTagRemoving();

          StatusCode initialize();
          StatusCode finalize();
          StatusCode  BTagRemoving_exec(Jet& myJet);

      private:

          /** to store the secondary vertices in the jet */
          bool m_storeTrackAssociation;
          bool m_storeSecondaryVertices;
          std::vector<std::string>  m_listOfTracksToRemove;
          std::vector<std::string>  m_listOfSvxToRemove;

  }; // End class

} // End namespace

#endif // BTAGREMOVING_H
