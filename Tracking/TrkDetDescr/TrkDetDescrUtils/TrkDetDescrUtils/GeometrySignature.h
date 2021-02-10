/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeometrySignature.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUITLS_GEOMETRYSIGNATURE_H
#define TRKDETDESCRUITLS_GEOMETRYSIGNATURE_H

namespace Trk {

  /** @class GeometrySignature
  
     An enumeration object that puts the signature
     of a GeometryBuilder to all subvolumes
      
    @author Andreas.Salzburger@cern.ch
    
    */
    
    enum GeometrySignature {
        Global                 =  0,       
        ID                     =  1,
        BeamPipe               =  2,
        Calo                   =  3,
        MS                     =  4,
        Cavern                 =  5,
        HGTD                   =  6,
        NumberOfSignatures     =  7,
        Unsigned               = 99
    };

    enum GeometryType {
        Static                  = 0,
        Dense                   = 1,
        DenseWithLayers         = 1,
        Detached                = 2,
        Master                  = 3,
        NumberOfGeometryTypes   = 3   
    };


} // end of namespace


#endif // TRKDETDESCRUITLS_GEOMETRYSIGNATURE_H
