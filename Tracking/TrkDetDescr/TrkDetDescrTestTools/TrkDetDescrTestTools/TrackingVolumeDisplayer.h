/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingVolumeDisplayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_TRACKINGVOLUMEDISPLAYER_H
#define TRKDETDESCRTOOLS_TRACKINGVOLUMEDISPLAYER_H

// Trk
#include "TrkDetDescrTestTools/RecursiveGeometryProcessor.h"
// STL
#include <string>
#include <fstream>


namespace Trk {

    class TrackingVolume;
    class Layer;
    class Surface;
    class MaterialProperties;

    /** @class TrackingVolumeDisplayer

        The TrackingVolumeDisplayer writes a root .C file 
        which can be executed and used the OpenGL root viewer

        @author Andreas.Salzburger@cern.ch   
     */

    class TrackingVolumeDisplayer : virtual public RecursiveGeometryProcessor {

      public:

        /** Constructor */
        TrackingVolumeDisplayer(const std::string&,const std::string&,const IInterface*);
        /** Destructor */
        virtual ~TrackingVolumeDisplayer();

        /** AlgTool initialize method */
        StatusCode initialize();
        
        /** AlgTool finalize method */
        StatusCode finalize();

      private:

        /** Current implementation: write root visualization to file stream */
        StatusCode processNode(const TrackingVolume& tvol, size_t level=0) const;

        /** Current implementation: write root visualization to file stream */
        StatusCode processNode(const Layer& lay, size_t level=0) const;

        /** Current implementation: write root visualization to file stream */
        StatusCode processNode(const Surface&, size_t level=0) const;

        void openFile(std::ofstream& output, const std::string& filename) const;  //!< File handling: open + write header
        void closeFile(std::ofstream& output) const;                              //!< File handling: write footer + close

        /** calculate the color code from the Material */
        int colorCodeFromMaterial(const Trk::MaterialProperties* prop, std::ofstream& output) const;

        mutable int                         m_volumeCounter;                      //!< volume counter

        mutable std::ofstream               m_fileVolumeOutput;                   //!< file output for visualization action
        mutable std::string                 m_fileVolumeOutputName;               //!< file name for visualization action
        mutable bool                        m_fileVolumeOutputMode;               //!< steer writing

        mutable std::ofstream               m_fileLayerOutput;                    //!< file output for visualization action
        mutable std::string                 m_fileLayerOutputName;                //!< file name for visualization action
        mutable bool                        m_fileLayerOutputMode;                //!< steer writing

        mutable std::ofstream               m_fileSurfaceOutput;                  //!< file output for visualization action
        mutable std::string                 m_fileSurfaceOutputName;              //!< file name for visualization action
        mutable bool                        m_fileSurfaceOutputMode;              //!< steer writing
        mutable bool                        m_fileSurfaceOutputSplit;             //!< use one file for each layer

        static int                          s_displaySurfaces;                    //!< static surface counter

    };

}

#endif

