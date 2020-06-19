/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeometryAsciiDumper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_GEOMETRYASCIIDUMPER_H
#define TRKDETDESCRTOOLS_GEOMETRYASCIIDUMPER_H

// Trk
#include "TrkDetDescrTestTools/RecursiveGeometryProcessor.h"
// STL
#include <algorithm>
#include <string>
#include <fstream>

class TTree;

namespace Trk {

    class TrackingVolume;
    class Layer;
    class Surface;

    /** @class GeometryAsciiDumper

        The GeometryAsciiDumper writes geometrical
        information of the tracking geomtetry 
        into an Ascii file for comparison

        @author Andreas.Salzburger@cern.ch   
     */

    class GeometryAsciiDumper : virtual public RecursiveGeometryProcessor {

      public:

        /** Constructor */
        GeometryAsciiDumper(const std::string&,const std::string&,const IInterface*);

        /** Destructor */
        virtual ~GeometryAsciiDumper();

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

        mutable std::ofstream       m_outputFile;
        std::string                 m_outputFileName;  //!< where the tree is written to  
        int                         m_outputPrecision;

    };
}

#endif

