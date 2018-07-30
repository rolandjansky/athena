/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenericTruthStrategy.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_GENERICTRUTHSTRATEGY_H
#define ISF_TOOLS_GENERICTRUTHSTRATEGY_H 1

// stl includes
#include <set>
#include <vector>

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "AtlasDetDescr/AtlasRegion.h"

// ISF includes
#include "ISF_HepMC_Interfaces/ITruthStrategy.h"

namespace ISF {
  typedef std::vector<int>              VertexTypesVector;
  typedef std::set<int>                 VertexTypesSet;
  typedef std::vector<int>              PDGCodesVector;
  typedef std::set<int>                 PDGCodesSet;

  /** @class GenericTruthStrategy

      A multi-purpose implementation of an ISF TruthStrategy.
  
      @author Elmar.Ritsch -at- cern.ch
     */
  class GenericTruthStrategy final : public extends<AthAlgTool, ITruthStrategy> {
      
    public: 
     /** Constructor with parameters */
     GenericTruthStrategy( const std::string& t, const std::string& n, const IInterface* p );

     /** Destructor */
     ~GenericTruthStrategy();

     // Athena algtool's Hooks
     virtual StatusCode  initialize() override;
     virtual StatusCode  finalize() override;

     /** true if the ITruthStrategy implementation applies to the given ITruthIncident */
     virtual bool pass( ITruthIncident& incident) const override;

     virtual bool appliesToRegion(unsigned short geoID) const override;
	  private:
     /** parent kinetic energy / transverse momentum cuts
         (pT is stored as pT^2 which allows for faster comparisons) */
     bool                                   m_useParentPt;         //!< use pT or Ekin cuts?
     double                                 m_parentPt2;           //!< parent particle
     double                                 m_parentEkin;          //!< parent particle

     /** child particle kinetic energy / transverse momentum cuts
         (pT is stored as pT^2 which allows for faster comparisons) */
     bool                                   m_useChildPt;          //!< use pT or Ekin cuts?
     double                                 m_childPt2;            //!< pT momentum cut
     double                                 m_childEkin;           //!< Ekin cut
     bool                                   m_allowChildrenOrParentPass; //!< pass cuts if parent did not

     /** vertex type (physics code) checks */
     VertexTypesVector                      m_vertexTypesVector;  //!< Python property
     VertexTypesSet                         m_vertexTypes;        //!< optimized for search
     bool                                   m_doVertexRangeCheck;
     int                                    m_vertexTypeRangeLow;
     int                                    m_vertexTypeRangeHigh;
     unsigned                               m_vertexTypeRangeLength;

     /** PDG code checks */
     PDGCodesVector                         m_parentPdgCodesVector;  //!< Python property
     PDGCodesSet                            m_parentPdgCodes;        //!< optimized for search

    IntegerArrayProperty            m_regionListProperty;
   }; 
  
}


#endif //> !ISF_TOOLS_GENERICTRUTHSTRATEGY_H
