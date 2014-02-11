/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// LayerMaterialRecord.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRALGS_LAYERMATERIALRECORD_H
#define TRKDETDESCRALGS_LAYERMATERIALRECORD_H

#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/MaterialAssociationType.h"
#include "TrkGeometry/MaterialProperties.h"
#include "GeoPrimitives/GeoPrimitives.h"


namespace Trk {

    /** @class LayerMaterialRecord

     Helper Class to record the material during the GeantinoNtupleMappingProcess

     @author Andreas.Salzburger@cern.ch
     */

    class Layer;
    class AssociatedMaterial;

    class LayerMaterialRecord {

      public :
        /** Default Constructor */
        LayerMaterialRecord();

        /** Constructor */
        LayerMaterialRecord(double tlayer,
                            const BinUtility* binutils,
                            MaterialAssociationType assoc=EffectiveNumAtoms,
                            int maxSiliconHits = 0);

        /** Constructor */
        LayerMaterialRecord(const LayerMaterialRecord& lmr);

        /** Destructor */
        ~LayerMaterialRecord();

        /** Assignment operator */
        LayerMaterialRecord& operator=(const LayerMaterialRecord& lmr);

        /** adding the information about the Geantino hit */
        void associateGeantinoHit(const Amg::Vector3D& pos,
                                  double s,
                                  double x0,
                                  double l0,
                                  double a,
                                  double z,
                                  double rho,
                                  int screenoutput=0);

        /** finalize the Event */
        AssociatedMaterial* finalizeEvent(const Trk::Layer& lay, int screenoutput=0);

        /** finalize the Run */
        void finalizeRun(int screenoutput=0);

        /** return method for the LayerMaterial */
        const MaterialPropertiesMatrix& associatedLayerMaterial() const;

        /** return the BinUtility */
        const Trk::BinUtility* binUtility() const;

    private:

        double                   m_layerThickness;  //!< record the layerThickness
        BinUtility*              m_binUtility;      //!< record the BinnedArray
        int                      m_bins1;           //!< number of bins in coordinate 1
        int                      m_bins2;           //!< number of bins in coordinate 2

        //!< event normalizers
        int         m_steps;
        double      m_track_effNumAtoms;

        //!< event related information
        double      m_s;
        double      m_s_x0;
        double      m_s_l0;
        double      m_a;
        double      m_z;
        double      m_rho;

        double      m_rPosition;
        double      m_zPosition;
        double      m_phiPosition;
        double      m_thetaPosition;

        MaterialAssociationType  m_assoc;           //!< type of hit association

        //!< run normalizers
        std::vector< std::vector<int> >    m_events;

        //!< event related information
        std::vector< std::vector<double> >     m_run_t_x0;
        std::vector< std::vector<double> >     m_run_t_l0;
        std::vector< std::vector<double> >     m_run_a;
        std::vector< std::vector<double> >     m_run_z;
        std::vector< std::vector<double> >     m_run_rho;

        int                                    m_maxSiliconHits;
        int                                    m_siliconHits;

        //!< the final material properties
        mutable MaterialPropertiesMatrix       m_associatedLayerMaterial;

        static double                          s_siliconRadiationLength;

        //!< clear the material -> calls delete
        void clearMaterial();
        //!< copy from another vector
        void copyMaterial(const MaterialPropertiesMatrix& mat);

    };

    inline const MaterialPropertiesMatrix& LayerMaterialRecord::associatedLayerMaterial() const
    { return m_associatedLayerMaterial; }

    inline const Trk::BinUtility* LayerMaterialRecord::binUtility() const
    { return m_binUtility; }

}

#endif

