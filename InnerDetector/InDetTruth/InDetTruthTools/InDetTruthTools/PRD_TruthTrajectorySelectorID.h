/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRUTHTOOLS_PRD_TRUTHTRAJECTORYSELECTORID_H
#define INDETTRUTHTOOLS_PRD_TRUTHTRAJECTORYSELECTORID_H

///////////////////////////////////////////////////////////////////
// PRD_TruthTrajectorySelectorID.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkTruthTrackInterfaces/IPRD_TruthTrajectorySelector.h"

//#include "EventPrimitives/EventPrimitives.h"
//#include "TrkEventPrimitives/LocalPosition.h"
//#include "TrkEventPrimitives/GlobalPosition.h"
#include "TrkEventPrimitives/LocalParameters.h"
//#include "CLHEP/Geometry/Transform3D.h"

class ThreePointCircle {
    
    public :
		/** The constructor from three points */
        ThreePointCircle( const std::vector<Amg::Vector3D>& vecPoints);

		/** Destructor */
        ~ThreePointCircle();
        
		/** center */		
        const Amg::Vector2D& center() const;
		/** Translation */
        const Amg::Translation3D* frameTranslation() const;
        
    	/* d0, z0, eta, phi, pt */
		double d0() const;
		double z0() const;
		double phi0() const;
		double eta() const;
		double pT() const;
		double radius() const;
		
	private :
		void constructCircle(const Amg::Vector3D&, const Amg::Vector3D&, const Amg::Vector3D&);
	    // the reference point
        Amg::Translation3D*     m_translation;
	    // the parameters
		double 				m_d0, m_z0, m_phi0, m_eta, m_pt;
		double              m_radius;
		Amg::Vector2D 	m_center;
	    
};

inline double ThreePointCircle::d0() const { return m_d0; }
inline double ThreePointCircle::z0() const { return m_z0; }
inline double ThreePointCircle::phi0() const { return m_phi0; }
inline double ThreePointCircle::eta() const { return m_eta; }
inline double ThreePointCircle::pT() const { return m_pt; }
inline double ThreePointCircle::radius() const { return m_radius; }

inline const Amg::Vector2D& ThreePointCircle::center() const { return m_center; }

inline const Amg::Translation3D* ThreePointCircle::frameTranslation() const { return m_translation; }


class AtlasDetectorID;

namespace InDet {
    /** @class PRD_TruthTrajectorySelectorID
    
        simple selector for truth trajectories
        
        
        @author : Roland.Wolfgang.Jansky -at- cern.ch 
    
    */
    class PRD_TruthTrajectorySelectorID : public AthAlgTool, virtual public Trk::IPRD_TruthTrajectorySelector {

    public:
        //** Constructor with parameters */
        PRD_TruthTrajectorySelectorID( const std::string& t, const std::string& n, const IInterface* p );
        
        /** Tool Destructor */
        ~PRD_TruthTrajectorySelectorID() {}

        // Athena algtool's Hooks
        StatusCode  initialize();
        StatusCode  finalize();
  
        /** Interface method from IPRD_TruthTrajectorySelector */
        virtual bool pass( const Trk::PRD_TruthTrajectory & ) const;
    
    private:
        /**ID pixel helper*/
        const AtlasDetectorID* m_atlasId;
    };
}

#endif
