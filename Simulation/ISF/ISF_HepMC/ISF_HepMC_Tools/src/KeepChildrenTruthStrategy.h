/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// KeepChildrenTruthStrategy.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_KEEPCHILDRENTRUTHSTRATEGY_H
#define ISF_TOOLS_KEEPCHILDRENTRUTHSTRATEGY_H 1

// stl includes
#include <set>
#include <vector>

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// ISF includes
#include "ISF_HepMC_Interfaces/ITruthStrategy.h"

namespace ISF {
  typedef std::vector<int>              VertexTypesVector;
  typedef std::set<int>                 VertexTypesSet;
  typedef std::vector<int>              PDGCodesVector;
  typedef std::set<int>                 PDGCodesSet;

  /** @class KeepChildrenTruthStrategy

      A modifier for the purposes of truth strategies defining cases
      in which we should keep all the children of an interaction.
  
      @author Zach.Marshall -at- cern.ch
     */
  class KeepChildrenTruthStrategy : public extends<AthAlgTool, ITruthStrategy> {
      
    public: 
     /** Constructor with parameters */
     KeepChildrenTruthStrategy( const std::string& t, const std::string& n, const IInterface* p );

     /** Destructor */
     ~KeepChildrenTruthStrategy();

     // Athena algtool's Hooks
     StatusCode  initialize();

     /** true if the ITruthStrategy implementation applies to the given ITruthIncident */
     bool pass( ITruthIncident& incident) const;

    private:

     /** vertex type (physics code) checks */
     VertexTypesVector                      m_vertexTypesVector;  //!< Python property
     VertexTypesSet                         m_vertexTypes;        //!< optimized for search
     bool                                   m_doVertexRangeCheck;
     int                                    m_vertexTypeRangeLow;
     int                                    m_vertexTypeRangeHigh;
     unsigned                               m_vertexTypeRangeLength;
     int                                    m_passProcessCategory;
     bool                                   m_bsmParent;             //!< Apply to BSM parents
     /** PDG code checks */
     PDGCodesVector                         m_parentPdgCodesVector;  //!< Python property
     PDGCodesSet                            m_parentPdgCodes;        //!< optimized for search
   }; 
  
}

#endif //> !ISF_TOOLS_KEEPCHILDRENTRUTHSTRATEGY_H
