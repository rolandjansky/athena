/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// PerigeeFromVertex.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef TRKEVENTUTILS_PERIGEEFROMVERTEXCREATOR_H
#define TRKEVENTUTILS_PERIGEEFROMVERTEXCREATOR_H

#include "TrkParameters/TrackParameters.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
    /** @class PerigeeFromVertexCreator
     
        Given a vertex in global or global/local expressiong,
        a Perigee : public TrackParameters is returned

        It enhances a factory type and a copy by object return method set

        @author Tatjana.Lenz@cern.ch, Andreas.Salzburger@cern.ch */

   class PerigeeFromVertexCreator {

      public:
        /** Constructor */
        PerigeeFromVertexCreator() :
        m_field(s_magField)
        {}
        /** Constructor */
        PerigeeFromVertexCreator(double magfield) :
        m_field(magfield)
        {}
        /** ~Desctructor */
        virtual ~PerigeeFromVertexCreator()
        {}

        /** createPerigee method - returned by object */
        Trk::Perigee createPerigee(Amg::Vector3D& gp, Amg::Vector3D& gm, double charge) const;
        /** createPerigee method - returned by object */        
        Trk::Perigee createPerigee(Amg::Vector3D& gp, double phi, double theta, double qOverP) const;
        
        /** createPerigee method - Factory type */
        Trk::Perigee* createNewPerigee(Amg::Vector3D& gp, Amg::Vector3D& gm, double charge) const;
        /** createPerigee method - Factory type */        
        Trk::Perigee* createNewPerigee(Amg::Vector3D& gp, double phi, double theta, double qOverP) const;

      private:
         double m_field;                  //!< magnetic field in tesla, can be set through constructor
      
         static double s_unitConversion;   //!< unit conversion factor
         static double s_magField;         //!< default magnetic field value
                          

     };
} // end of namespace


#endif  
