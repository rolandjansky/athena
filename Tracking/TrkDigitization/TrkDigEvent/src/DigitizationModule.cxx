//////////////////////////////////////////////////////////////////
// DigitizationModule.cxx, ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkDigEvent/DigitizationModule.h"

Trk::DigitizationModule::DigitizationModule(std::shared_ptr<const Segmentation> moduleSegmentation,
                                            double halfThickness, 
                                            int readoutDirection, 
                                            double lorentzAngle) :
   
   m_halfThickness(halfThickness),
   m_readoutDirection(readoutDirection),
   m_lorentzAngle(lorentzAngle),
   m_tanLorentzAngle(tan(lorentzAngle)),
   m_segmentation(moduleSegmentation),
   m_boundarySurfaces(),           
   m_segmentationSurfacesX(),
   m_segmentationSurfacesY()
{
    m_segmentation->createSegmenationSurfaces(m_boundarySurfaces,
                                              m_segmentationSurfacesX,
                                              m_segmentationSurfacesY,
                                              halfThickness,
                                              readoutDirection, 
                                              lorentzAngle);
}
        
const std::vector< std::shared_ptr<const Trk::Surface> > Trk::DigitizationModule::segmentationSurfaces(const Trk::DigitizationCell& entryCids, const Trk::DigitizationCell& exitCids) const
{
    std::vector< std::shared_ptr<const Trk::Surface> > sSurfaces;
    
    int startbinX = entryCids.first;
    int endbinX   = exitCids.first;
    // swap if needed
    if (startbinX > endbinX) std::swap(startbinX,endbinX);
    // now cash in the rest
    for ( ; startbinX <= endbinX; ++startbinX)
        sSurfaces.push_back(m_segmentationSurfacesX[startbinX]);
    
    // start bin, end bin
    int startbinY = entryCids.second;
    int endbinY   = exitCids.second;
    // swap if needed
    if (startbinY > endbinY) std::swap(startbinY,endbinY);
    // now cash in the rest
    for ( ; startbinY <= endbinY; ++startbinY)
        sSurfaces.push_back(m_segmentationSurfacesY[startbinY]);

    // return what you have 
    return sSurfaces;  
}  

const std::vector< std::shared_ptr<const Trk::Surface> > Trk::DigitizationModule::stepSurfaces(const Amg::Vector3D& start, const Amg::Vector3D& end) const 
{
    // prepare the return vector
    std::vector< std::shared_ptr<const Trk::Surface> > stepSurfaces;
    
    const Trk::DigitizationCell startCell = m_segmentation->cell(start);
    const Trk::DigitizationCell endCell   = m_segmentation->cell(end);
    
    // go along x - first with the naive binning (i.e. w.o lorentz angle)
    size_t sCellX = startCell.first;
    size_t eCellX = endCell.first;
    if (sCellX > eCellX) std::swap(sCellX,eCellX);
    // now take the boundaries as well
    if (sCellX > 0) --sCellX;
    ++eCellX;
    // the surfaces along Y are easy, just the bin surfaces
    size_t sCellY = startCell.second; 
    size_t eCellY = endCell.second;  
    if (sCellY > eCellY) std::swap(sCellY,eCellY);
    // reserve - be safe
    stepSurfaces.reserve((eCellY-sCellY)+(eCellX-sCellX)+2);
    // now fill the x surfaces
    for ( ; sCellX <= eCellX && sCellX < m_segmentationSurfacesX.size(); ++sCellX)
        stepSurfaces.push_back(m_segmentationSurfacesX[sCellX]);
    // end fill the y surfaces
    for ( ; sCellY <= eCellY && sCellY < m_segmentationSurfacesY.size(); ++sCellY)
        stepSurfaces.push_back(m_segmentationSurfacesY[sCellY]);
    // return the lot
    return  stepSurfaces;   
}


                                                
