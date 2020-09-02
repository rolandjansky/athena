/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILS_TRK2DDISTANCESEEDER_H
#define TRKVERTEXSEEDFINDERUTILS_TRK2DDISTANCESEEDER_H

#include "GaudiKernel/ToolHandle.h"
#include <vector>
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

namespace Trk
{

  /**
   @class Trk2dDistanceSeeder

   AlgoTool calculates the points of minimum distance between two 
   tracks on the transverse plane, 
   which is analytically well defined (intersection of two circles). In case of double intersection different possibilities 
   were considered; at the end I found reasonably good to choose the intersection point where the tracks are nearest 
   in the z coordinate...

   changes: 11-11-2007 Intersection case completely rewritten after having done analytical calculation (sign come out of calculation!)

   @author Giacinto.Piacquadio@physik.uni-freiburg.de

   */

  static const InterfaceID IID_Trk2dDistanceSeeder("Trk2dDistanceSeeder", 1, 1);
  
  class Trk2dDistanceSeeder : public AthAlgTool
  {
  public:

    static const InterfaceID& interfaceID()
    {
        return IID_Trk2dDistanceSeeder;
    };


    //default constructor due to Athena interface
    Trk2dDistanceSeeder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //
    //destructor
    virtual ~Trk2dDistanceSeeder();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    
    TwoPointOnTrack GetSeed (const TwoTracks& mytracks,
                                   TwoPoints* twopoints = nullptr) const;


  private:
    double getBField (const Perigee& p, MagField::AtlasFieldCache& cache) const;
    
    //option
    bool m_solveAmbiguityUsingZ;

    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey 
          {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
  };
}
#endif
