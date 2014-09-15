/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlCandidate/Utilities.h"
#include "MuGirlCandidate/BBox.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/Surface.h"
#include "TrkPrepRawData/PrepRawData.h"

using namespace MuGirlNS;

static void updateBbox(const Trk::Surface& surface, BBox& bbox, double x[4], double y[4])
{
    for (int i = 0; i < 4; i++)
    {
        
        Amg::Vector2D lp(x[i], y[i]);
        const Amg::Vector3D* gp = surface.localToGlobal(lp);
        bbox.update(*gp);
        delete gp;
    }
}

static void bounds2bbox(const Trk::SurfaceBounds& bounds, const Trk::Surface& surface, BBox& bbox)
{
    bbox.clear();
    const Trk::TrapezoidBounds* trpz = dynamic_cast<const Trk::TrapezoidBounds*>(&bounds);
    if (trpz != NULL)
    {
        double x[4] = {trpz->maxHalflengthX(), -trpz->maxHalflengthX(), -trpz->minHalflengthX(),  trpz->minHalflengthX()};
        double y[4] = {trpz->   halflengthY(),  trpz->   halflengthY(), -trpz->   halflengthY(), -trpz->   halflengthY()};
        updateBbox(surface, bbox, x, y);
        return;
    }
    const Trk::RectangleBounds* rect = dynamic_cast<const Trk::RectangleBounds*>(&bounds);
    if (rect != NULL)
    {
        double x[4] = {rect->halflengthX(), -rect->halflengthX(), -rect->halflengthX(),  rect->halflengthX()};
        double y[4] = {rect->halflengthY(),  rect->halflengthY(), -rect->halflengthY(), -rect->halflengthY()};
        updateBbox(surface, bbox, x, y);
        return;
    }
}

void MuGirlNS::DetElement2Bbox(const Trk::TrkDetElementBase* pElement, BBox& bbox)
{
    bounds2bbox(pElement->bounds(), pElement->surface(), bbox);
}

void MuGirlNS::PrepData2Bbox(const Trk::PrepRawData* pPrepData, BBox& bbox)
{
    const Trk::TrkDetElementBase* pDetElement = pPrepData->detectorElement();
    bounds2bbox(pDetElement->bounds(pPrepData->identify()), pDetElement->surface(pPrepData->identify()), bbox);
}
