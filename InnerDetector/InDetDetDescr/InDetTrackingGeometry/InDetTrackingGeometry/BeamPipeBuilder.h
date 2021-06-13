/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BeamPipeBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRY_BEAMPIPEBUILDER_H
#define INDETTRACKINGGEOMETRY_BEAMPIPEBUILDER_H

// Athena
#include "AthenaBaseComps/AthAlgTool.h"
// Trk
#include "TrkDetDescrInterfaces/ILayerBuilder.h"
// STL
#include <vector>

namespace Trk {
  class CylinderLayer;
  class DiscLayer;
  class PlaneLayer;
}

class BeamPipeDetectorManager;

namespace InDet {

  /** @class BeamPipeBuilder
      Simple LayerBuilder for the BeamPipe, 
      can be configured through jobOptions:
        - radius
        - halflength
        - thickness
        - MaterialProperties
        
      later on the slight shift/rotation of the BeamPipe can be implemented
      - make a binding to the database afterwards
       
      @author Andreas.Salzburger@cern.ch 
    */
  class BeamPipeBuilder : public AthAlgTool,
                          virtual public Trk::ILayerBuilder {
    
                            
    public:
      /** AlgTool style constructor */
      BeamPipeBuilder(const std::string&,const std::string&,const IInterface*);
      /** Destructor */
      virtual ~BeamPipeBuilder();
      
      /** AlgTool initialize method */
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
       
      /** LayerBuilder interface method - returning Barrel-like layers */
      const std::vector< const Trk::CylinderLayer* >* cylindricalLayers() const; 
      
      /** LayerBuilder interface method - returning Endcap-like layers */
      const std::vector< const Trk::DiscLayer* >*     discLayers() const; 
      
      /** LayerBuilder interface method - returning Planar-like layers */
      const std::vector< const Trk::PlaneLayer* >*    planarLayers() const; 

      /** Name identification */
      const std::string& identification() const;      
          
    private:
      
      bool                                          m_beamPipeFromDb;    //!< steer beam pipe parameters from DataBase
      const BeamPipeDetectorManager*                m_beamPipeMgr;       //!< the beam pipe manager
      std::string                                   m_beamPipeMgrName;   //!< the name of the beam pipe manager to be configured
      double                                        m_beamPipeEnvelope;  //!< radial envelope when taking the Top volume radius
      
      double                                        m_beamPipeOffsetX;    //!< beam pipe offset in x
      double                                        m_beamPipeOffsetY;    //!< beam pipe offset in y        
      double                                        m_beamPipeRadius;     //!< radius of the beam pipe
      double                                        m_beamPipeHalflength; //!< halflength of the beampipe
      double                                        m_beamPipeThickness;  //!< thickness of the beam pipe
      double                                        m_beamPipeX0;         //!< X0 of the beam pipe
      double                                        m_beamPipeL0;         //!< X0 of the beam pipe
      //double                                        m_beamPipedEdX;       //!< dEdX of the beam pipe
      double                                        m_beamPipeA;          //!< averageA of the beam pipe
      double                                        m_beamPipeZ;          //!< averageZ of the beam pipe
      double                                        m_beamPipeRho;        //!< averageRho of the beam pipe

      unsigned int                                  m_beamPipeBinsZ;      //!< number of bins in the beam pipe

      std::string                                   m_identification;     //!< string identification
      
      
  };

 inline const std::vector< const Trk::DiscLayer* >*  BeamPipeBuilder::discLayers() const
 { return 0; }
 
 inline const std::vector< const Trk::PlaneLayer* >* BeamPipeBuilder::planarLayers() const
 { return 0; }
 
 inline const std::string& BeamPipeBuilder::identification() const
 { return m_identification; } 
 
  
} // end of namespace


#endif // INDETTRACKINGGEOMETRY_BEAMPIPEBUILDER_H


