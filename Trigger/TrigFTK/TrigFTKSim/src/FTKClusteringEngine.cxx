/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKClusteringEngine.h"
#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKPMap.h"
#include "TrigFTKSim/MultiTruth.h"
#include "TrigFTKSim/ftkdefs.h"

#include <cmath>
#include <iostream>
#include <iomanip>
#include <stack>
#include <queue>

cluster::~cluster()
{
    hitlist.clear();
}


FTKClusteringEngine::FTKClusteringEngine(bool saveClusterContent, bool diagClustering,
                                         bool sctClustering, bool ibl3DRealistic,
                                         bool duplicateGanged, bool gangedPatternRecognition,
                                         bool splitBlayerModules, int pixelClusteringMode) :
    FTKLogging("FTKClusteringEngine"),
    m_saveClusterContent(saveClusterContent),
    m_diagClustering(diagClustering),
    m_sctClustering(sctClustering),
    m_ibl3DRealistic(ibl3DRealistic),
    m_duplicateGanged(duplicateGanged),
    m_gangedPatternRecognition(gangedPatternRecognition),
    m_splitBlayerModules(splitBlayerModules),
    m_pixelClusteringMode(pixelClusteringMode)
{

}

/*! Function which examines whether a hit belongs to an IBL module.
 * \param hit the hit
 * \return true if the hit is on an IBL module, false otherwide
 */
bool FTKClusteringEngine::hitOnIBLmodule(const FTKRawHit &hit)
{
    return (hit.getModuleType() == ftk::MODULETYPE_IBL3D || hit.getModuleType() == ftk::MODULETYPE_IBL_PLANAR);
}


bool FTKClusteringEngine::sortPixelInput (const std::unique_ptr<FTKRawHit>& i, const  std::unique_ptr<FTKRawHit>& j)
{

    FTK_FECoordsHit lhit1(*(i.get()));
    FTK_FECoordsHit lhit2(*(j.get()));

    if (lhit1.fe != lhit2.fe) return lhit1.fe <= lhit2.fe;
    else if (lhit1.fe == lhit2.fe) {
        if (lhit1.lcol/2 != lhit2.lcol/2) return lhit1.lcol/2 <= lhit2.lcol/2;
        else if (lhit1.lcol/2 == lhit2.lcol/2) return lhit1.lrow < lhit2.lrow;
    }

    return false;
}

bool FTKClusteringEngine::sortIBLInput(const std::unique_ptr<FTKRawHit>& i, const std::unique_ptr<FTKRawHit>& j)
{
    int firstCol  = i->getEtaStrip();
    int secondCol = j->getEtaStrip();
    int firstRow  = i->getPhiSide(); // Keisuke 20170314, start from 0
    int secondRow = j->getPhiSide(); // Keisuke 20170314, start from 0
    int firstFE  = (firstCol  < ftk::clustering::colsInFEChipPerIblModuleRow) ? 0 : 1;
    int secondFE = (secondCol < ftk::clustering::colsInFEChipPerIblModuleRow) ? 0 : 1;

    if(firstCol >= ftk::clustering::colsInFEChipPerIblModuleRow)
      firstCol = firstCol - ftk::clustering::colsInFEChipPerIblModuleRow - 1;
    if(secondCol >= ftk::clustering::colsInFEChipPerIblModuleRow)
      secondCol = secondCol - ftk::clustering::colsInFEChipPerIblModuleRow - 1;

    if (firstFE != secondFE) return firstFE > secondFE;
    else{
        if(firstCol < ftk::clustering::colsInFEChipPerIblModuleRow/2
	   || secondCol < ftk::clustering::colsInFEChipPerIblModuleRow/2){
	  if(firstCol != secondCol) {
                return firstCol < secondCol;
	  }
	  else {
                return firstRow < secondRow;
	  }
        } else {
	  if(firstCol != secondCol) {
                return firstCol > secondCol;
	  }
	  else {
                return firstRow < secondRow;
	  }
        }
    }
    return false;
}


bool FTKClusteringEngine::isSplitCluster(const cluster* clu)
{
    for (auto& hit: clu->hitlist) {
        FTK_ClusterCoordHit chit = FTK_ClusterCoordHit(*hit , clu->seed);
        if ((chit.ccol >= GRID_COL_MAX - 1) ||  (chit.crow >= (GRID_ROW_MAX - 1)) || ( chit.crow <= 0))
            return true;
    }

    return false;
}

uint32_t FTKClusteringEngine::getModuleId(const FTKRawHit &hit)
{
    return hit.getHitType() == ftk::PIXEL ? hit.getIdentifierHash() : 0x8000 + hit.getIdentifierHash();
}


bool FTKClusteringEngine::neighborhood(const FTKRawHit &hit1, const FTKRawHit &hit2)
{
    // 1st check same module
    if (hit1.getIdentifierHash() != hit2.getIdentifierHash()) {
        Error("neighborhood") << "Hits do not belong in the same cluster" << std::endl;
        return false;
    }

    switch (hit1.getHitType()) {
    case ftk::SCT:
        if (!m_sctClustering)
            return false;
        if (hit1.getEtaStrip() == hit2.getEtaStrip()+hit2.getNStrips()
            || hit1.getEtaStrip()+hit1.getNStrips() == hit2.getEtaStrip())
            return true;
        return false;

    case ftk::PIXEL:
        /* need a common edge, i.e. contiguity along diagonal is not enough (see below) */
        if ( (hit1.getEtaStrip() == hit2.getEtaStrip()+1 || hit1.getEtaStrip()+1 == hit2.getEtaStrip())
             && hit1.getPhiSide() == hit2.getPhiSide())
            return true;
        if ( (hit1.getPhiSide() == hit2.getPhiSide()+1 || hit1.getPhiSide()+1 == hit2.getPhiSide())
             && hit1.getEtaStrip() == hit2.getEtaStrip()) return true;


        if (m_diagClustering) { /* Accept contiguity along diagonal as well */
            if ( (hit1.getEtaStrip() == hit2.getEtaStrip()+1 || hit1.getEtaStrip()+1 == hit2.getEtaStrip())
                 && (hit1.getPhiSide() == hit2.getPhiSide()+1 || hit1.getPhiSide()+1 == hit2.getPhiSide()) )
                return true;
        }

        return false;
    }
    return false;
}


int FTKClusteringEngine::buildUpCluster(hitVector& currentHits, cluster& clu)
{
    if (clu.hitlist.size()==0) {
        clu.hitlist.push_back(std::move(currentHits.front()));
        currentHits.erase(currentHits.begin());
    }

    /* Now we have a non empty cluster */
    bool newHitsAdded = false;
    int distanceis = clu.hitlist.size();
    hitVector::iterator hitP;
    for(int i = 0; i < distanceis; i++) {
        std::unique_ptr<FTKRawHit> hit = std::make_unique<FTKRawHit>(*clu.hitlist.at(i));
        for (hitP = currentHits.begin(); hitP != currentHits.end(); ) { // loop over hits in module
            if (neighborhood(*hit, **hitP)) { // if hits are neighborhood
                clu.hitlist.push_back(std::move(*hitP));
                currentHits.erase(hitP);
                distanceis++;
                hitP = currentHits.begin(); // if a hit is erased then restart the loop
                newHitsAdded = true;
            } else {
                ++hitP; // if hit is not a neighborhood then go to next hit
            }
        }
    }

    // // if at least one hit was added, check for more neighborhood hits
    if (newHitsAdded && currentHits.size())
        buildUpCluster(currentHits, clu);
    return clu.hitlist.size();
}


/*
 * Group all hits from one module into clusters
 */
void FTKClusteringEngine::makeClustersLNF(hitVector& currentHits, cluList& currentClusters)
{
    while (currentHits.size()) { // as long as we have hits
        std::unique_ptr<cluster> clu = std::make_unique<cluster>();
        clu->isSafe = false;
        clu->isKilled = false;
        buildUpCluster(currentHits, *clu.get());
        currentClusters.push_back(std::move(clu));
    }
}


void FTKClusteringEngine::makeClusterFromSeed(hitVector& currentHits, cluList& currentClusters, FTKRawHit &seed)
{
    if (currentHits.size() > 0)
    currentHits.erase(std::find_if(currentHits.begin(), currentHits.end(),
                                   [&](const std::unique_ptr<FTKRawHit>& chit) {
                                       return seed.getEtaStrip() == (chit.get())->getEtaStrip()
                                           && seed.getPhiSide() == (chit.get())->getPhiSide()
                                           && seed.getTot() == (chit.get())->getTot();}));

    std::unique_ptr<cluster> clu = std::make_unique<cluster>();
    clu->seed = seed;
    clu->hitlist.push_back(std::make_unique<FTKRawHit>(seed));
    clu->isSafe = false;
    clu->isKilled = false;
    buildUpCluster(currentHits, *clu.get());
    currentClusters.push_back(std::move(clu));
}


void FTKClusteringEngine::makeClustersGridWindow(hitVector& fifo, cluList& currentClusters)
{
    boost::circular_buffer<std::unique_ptr<FTKRawHit>> circular_buffer (256);
    hitVector gridhits;
    while (!fifo.empty() || !circular_buffer.empty()) {
        FTKRawHit seed = gridAUTH(circular_buffer, fifo, gridhits);
        makeClusterFromSeed(gridhits, currentClusters, seed);

        for(auto& hit: gridhits) {
            circular_buffer.push_back(std::move (hit));
        }

        gridhits.clear();
    }
}


bool FTKClusteringEngine::gangedHitHasNeighborhood(const FTKRawHit &hit, const cluster* clu, hitVector &connectedHits)
{
    bool hasNeighborhood = false;
    int phi = hitIsGanged(hit);
    if (phi) { // check hit is ganged
        int eta = hit.getEtaStrip();
        for (auto& chit: clu->hitlist) {
            if ( eta != chit->getEtaStrip() )
                continue;
            if ( abs(phi - chit->getPhiSide() ) != 1 )
                continue;
            hasNeighborhood = true;
            std::unique_ptr<FTKRawHit> tmpHit = std::make_unique<FTKRawHit>();
            tmpHit->setHitType( ftk::PIXEL );
            tmpHit->setModuleType( hit.getModuleType());
            tmpHit->setEtaStrip( eta );
            tmpHit->setLayer( hit.getLayer() );
            tmpHit->setPhiSide( gangedPartner(hit) );
            connectedHits.push_back( std::move(tmpHit) ); // store connected hit pointer
        } // end of nested loop over hits
    } //end of "if (phi)"
    return hasNeighborhood;
}


bool FTKClusteringEngine::findConnectedGanged(const std::unique_ptr<cluster> &clu, hitVector &connectedHits)
{
    bool hasNeighborhood = false;
    for (auto& hit: clu->hitlist) {
      if ( gangedHitHasNeighborhood(*hit, clu.get(), connectedHits) ) {
            hasNeighborhood = true;
            break;
        }
    }

    return hasNeighborhood;
}


bool FTKClusteringEngine::isKilled(const std::unique_ptr<cluster>& clu, const hitVector &connectedHits)
{
    bool isKilled = false;
    for (auto& hit: clu->hitlist) {
        if ( !hitIsGanged(*hit) ) continue;
        for (auto& chit: connectedHits) {
            if ( (*chit).getPhiSide() == (*hit).getPhiSide()
                 && (*chit).getEtaStrip() == (*hit).getEtaStrip() )
                isKilled =  true;
        }
    } // loop over hits in cluster

    return isKilled;
}


void FTKClusteringEngine::averageCluster(std::unique_ptr<cluster>& clu)
{
    const unsigned int &nHits = clu->hitlist.size();

    FTKRawHit &av = clu->clusterEquiv; ///< get pointer to clusterEquivalent
    FTKRawHit first = *(clu->hitlist.front()); ///< get 1st hit

    /// reset values for clusterEquivalent (alias av)
    av.reset();
    av.setX(0);
    av.setY(0);
    av.setZ(0);
    av.setIdentifierHash(first.getIdentifierHash());
    av.setHitType(first.getHitType());
    av.setModuleType(first.getModuleType());
    av.setBarrelEC(first.getBarrelEC());
    av.setLayer(first.getLayer());
    av.setPhiModule(first.getPhiModule());
    av.setEtaModule(first.getEtaModule());
    av.setEtaStrip(0);
    av.setNStrips(0);
    av.setEtaWidth(0);
    av.setPhiWidth(0);
    av.setIncludesGanged(false);

    int tmp;
    int etaMax = -1;
    int etaMin = 4000;
    int phiMax = -1;
    int phiMin = 4000;
    int rowMin = 99999; //int(2*(design->width()/design->phiPitch()))+1;;
    int rowMax = 0;
    int colMin = 99999; //int(2*(design->length()/design->etaPitch()))+1;
    int colMax = 0;
    int qRowMin = 0;
    int qRowMax = 0;
    int qColMin = 0;
    int qColMax = 0;

    /// precalculate values for case m_pixelClusteringMode>0
    int BarrelEndCap = first.getBarrelEC();

    //Calculate the active layer. Start with the layer of the hit. In case
    // always count IBL as layer0 and BLayer as layer1
    int layer = first.getLayer();
    bool isIBLmodule  = hitOnIBLmodule(first);
    bool isPixelmodule = !isIBLmodule;
    if (FTKSetup::getFTKSetup().getIBLMode()==0)
        layer++;
    float radius = ftk::clustering::radii[layer];

    //Default values for a pixel module. They are modified for IBL.
    float sensorThickness = ftk::sensorThicknessPixel;
    float etaPitch = ftk::etaPitchPixel;
    float numberOfEtaPixelsInModule = ftk::numberOfEtaPixelsInPixelModule;
    float lengthOfPixelModuleInUmPixels = ftk::lengthOfPixelModuleIn400umPixels;
    float moduleActiveLength = ftk::lengthOfPixelModuleIn400umPixels*ftk::etaPitchPixel/ftk::micrometer;
    // FlagAA: 2015-01-29 making default the units of 6.25um for r-phi coordinates
    float pixYScaleFactor = ftk::clustering::yScaleFactorPixel; ///<multiply by 16 to count in unit of 25um
    float pixXScaleFactor = ftk::clustering::xScaleFactorPixel;
    float etaModule = first.getEtaModule()-6;
    // FlagAA 2013-07-31: IBL code assumes fully planar geometry as in mc12+IBL
    // This will change for the real detector!!!
    if (isIBLmodule) {
        sensorThickness = ftk::sensorThicknessIbl;
        etaPitch = ftk::etaPitchIbl;
        numberOfEtaPixelsInModule = ftk::numberOfEtaPixelsInIblModule;
        lengthOfPixelModuleInUmPixels = ftk::lengthOfIblModuleIn250umPixels;
        moduleActiveLength = ftk::lengthOfIblModuleIn250umPixels*ftk::etaPitchIbl/ftk::micrometer; // planar sensors
        pixYScaleFactor = ftk::clustering::yScaleFactorIbl; ///<multiply by 10 to count in unit of 25um
        pixXScaleFactor = ftk::clustering::xScaleFactorIbl;
        etaModule = first.getEtaModule()-8;
        if( first.getModuleType() == ftk::MODULETYPE_IBL3D && m_ibl3DRealistic ) {
            sensorThickness = 230*ftk::micrometer;
            moduleActiveLength = 80*250;
            numberOfEtaPixelsInModule = ftk::clustering::colsInFEChipPerIblModuleRow;
        }
    }
    bool hasGanged = false;

    switch (av.getHitType()) {
    case ftk::SCT: {
        int firstStrip =  99999;
        int lastStrip  = -99999;
        av.setPhiSide(first.getPhiSide());
        for (const auto &hit: clu->hitlist) {
            av.addX(hit->getX());
            av.addY(hit->getY());
            av.addZ(hit->getZ());
            if (hit->getEtaStrip() < firstStrip)
                firstStrip = hit->getEtaStrip();
            if (hit->getEtaStrip()+hit->getNStrips()-1 > lastStrip)
                lastStrip = hit->getEtaStrip() + hit->getNStrips() - 1;
            av.addEtaStrip(hit->getEtaStrip());
        }
        float stripNumberFloat = (firstStrip*1.+lastStrip)/2;
        //(av.getEtaStrip()*1.)/nHits;
        av.setEtaStrip( static_cast<int>(stripNumberFloat) );
        av.setDeltaPhi( stripNumberFloat - static_cast<int>(stripNumberFloat) );
        // The convention used for SCT is that we are providing as output
        // the cluster center minus half a strip (i.e. use the left edge of strip for a single strip cluster)
        // where output = av.setEtaStrip + av.setDeltaPhi
        av.setStripCoordinate(firstStrip+lastStrip);
        av.setColumnCoordinate( 0 );
        av.setDeltaEta(0);
        av.setNStrips(lastStrip+1-firstStrip);
        av.setEtaWidth(1);
        av.setPhiWidth(lastStrip+1-firstStrip);
        break; // end of SCT
    }
    case ftk::PIXEL: {
        av.setPhiSide(0); // eta is reset a few lines above

        if (m_pixelClusteringMode == 0) {
            for (const auto& hit: clu->hitlist) { //loop over hits in cluster
                if (m_saveClusterContent) { // if enabled the cluster also stores also the single channels
                    FTKRawHit tmpch = *hit;
                    // set the barcode of the single channel, this may allow a very refined debugging of
                    // the cluster content accounting for the single barcode of each channel
                    MultiTruth mt;
                    MultiTruth::Barcode uniquecode(tmpch.getEventIndex(),tmpch.getBarcode());
                    mt.maximize(uniquecode,tmpch.getBarcodePt());
                    tmpch.setTruth(mt);
                    av.addChannel(tmpch);
                }
                av.addX(hit->getX());
                av.addY(hit->getY());
                av.addZ(hit->getZ());
                av.addPhiSide(hit->getPhiSide());   // phi index
                // pixels are counted starting from 0 (left edge) and not 0.5 (center position)
                // if only pixel 0 is hit the output value will be 0 and not 0.5 (pixel center)
                av.addEtaStrip(hit->getEtaStrip()); // eta index
            }
            tmp = static_cast<int>(round((av.getEtaStrip()*1.)/nHits));
            av.setDeltaEta((av.getEtaStrip()*1.)/nHits-tmp);
            av.setEtaStrip(tmp);
            tmp = static_cast<int>(round((av.getPhiSide()*1.)/nHits));
            av.setDeltaPhi((av.getPhiSide()*1.)/nHits-tmp);
            av.setPhiSide(tmp);
            break;
        }

        /* For m_pixelClusteringMode > 0
         * calculate cluster center following code at line 701 of
         * https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetRecTools/SiClusterizationTool/trunk/src/MergedPixelsTool.cxx#L701
         * I.e. posStrategy == 1
         * I.e. use charge imbalance between the two outer most ros (columns) to calculate the centroid
         * In this code I'll use etaTrack instead of pixel eta for simplicity
         */

        for (auto& hit: clu->hitlist) { //loop over hits in cluster
            if (m_saveClusterContent) { // if enabled the cluster also stores also the single channels
                FTKRawHit tmpch = *hit;
                // set the barcode of the single channel, this may allow a very refined debugging of
                // the cluster content accounting for the single barcode of each channel
                MultiTruth mt;
                MultiTruth::Barcode uniquecode(tmpch.getEventIndex(),tmpch.getBarcode());
                mt.maximize(uniquecode,tmpch.getBarcodePt());
                tmpch.setTruth(mt);
                av.addChannel(tmpch);
            }

            av.addX(hit->getX());
            av.addY(hit->getY());
            av.addZ(hit->getZ());

            int row = hit->getPhiSide();
            int col = hit->getEtaStrip();
            int tot = hit->getTot(); // ToT for pixels
            if (!isIBLmodule && pixelRowIsGanged(row))
                hasGanged = true;

            if (isPixelmodule) {
                // account for 600um pixels in the centroid calculation
                // will use units of 100um and the convert to normal pixel units below
                // will also indicate the center of the pixel instead of the left edge
                // considering center of pixel along eta (left (or top if you prefer) edge along phi)
                // each FE number is worth 19*400um pixels (400um*16 + 600um*2)
                // multiply FE column by 4 to convert to 100um units
                int FEnumber = col/ftk::clustering::colsInFEChipPerPixelModuleRow;
                int FEcolumn = col%ftk::clustering::colsInFEChipPerPixelModuleRow;

                col = FEnumber*(ftk::clustering::colsInFEChipPerPixelModuleRow+1) + FEcolumn;
                col *= pixYScaleFactor;
                col += pixYScaleFactor/2; // add half a pixel to align to pixel center (assume normal 400um pixel)
                col += pixYScaleFactor/2; // assume 1<=FEcolumn<=16 add half a pixel coming from 600um pixel in FEcolumn==0
                if (FEcolumn==0) col -= pixYScaleFactor/4; // correct position for first column in FE chip
                if (FEcolumn==(ftk::clustering::colsInFEChipPerPixelModuleRow-1)) col += pixYScaleFactor/4; // correct position for last column in FE chip
            } else if (isIBLmodule) { // IBL case
                //Modifications have to be made to include 3d modules
                int orig_col = col;
                col *= pixYScaleFactor; // use units of 25um
                col += pixYScaleFactor/2; // add half a pixel to align to pixel center
                if((hit->getModuleType() == ftk::MODULETYPE_IBL3D && m_ibl3DRealistic) || !m_ibl3DRealistic){
                    if (orig_col==0) col += pixYScaleFactor/2; // add half pixel (500um pixel in col0)
                    if (orig_col>0) col += pixYScaleFactor; // add a pixel (500um pixel in col0)

                    // for 3D modules only
                    // if (orig_col==79) col += pixYScaleFactor*5/10; // add 5/10 of pixel i.e. 100um (500um pixel in col79)

                    // for planar modules only
                    if (orig_col==79) col += pixYScaleFactor*4/10; // add 4/10 of pixel i.e. 100um (450um pixel in col79)
                    if (orig_col==80) col += pixYScaleFactor*12/10; // add 12/10 of pixel i.e. 300um (450um pixel in col79 and col80)
                    if (orig_col>80) col += pixYScaleFactor*16/10; // add 16/10 of pixel i.e. 400um (450um pixel in col79 and col80)
                    if (orig_col==159) col += pixYScaleFactor/2; // add half pixel (500um pixel in col159)
                }
            }
            row *= pixXScaleFactor;

            if (row == rowMin) qRowMin += tot;
            if (row < rowMin){
                rowMin = row;
                qRowMin = tot;
            }

            if (row == rowMax) qRowMax += tot;
            if (row > rowMax){
                rowMax = row;
                qRowMax = tot;
            }

            if (col == colMin) qColMin += tot;
            if (col < colMin){
                colMin = col;
                qColMin = tot;
            }

            if (col == colMax) qColMax += tot;
            if (col > colMax){
                colMax = col;
                qColMax = tot;
            }

            int phi = hit->getPhiSide(); // ROW // JAA updated 13 March 2017 to remove +1 as above
            // int phi = (*p)->getPhiSide(); // ROW
            int eta = hit->getEtaStrip();          // COLUMN

            if (eta > etaMax) etaMax = eta;
            if (eta < etaMin) etaMin = eta;
            if (phi > phiMax) phiMax = phi;
            if (phi < phiMin) phiMin = phi;
            av.addTot(first.getTot()); // sum ToT for pixel clusters
        }
        av.setEtaWidth(etaMax-etaMin+1);
        av.setPhiWidth(phiMax-phiMin+1);

        // calculate eta index and eta delta
        double eta_average, phi_average;
        eta_average = (colMin + colMax) / 2.;
        phi_average = (rowMin + rowMax) / 2.;

        if (m_pixelClusteringMode == ftk::pcm::PIXEL_CLUSTERING_HARDWARE) {
            float etaRow = -1;
            float etaCol = -1;
            if(qRowMin+qRowMax > 0)
                etaRow = qRowMax/static_cast<float>(qRowMin+qRowMax);
            if(qColMin+qColMax > 0)
                etaCol = qColMax/static_cast<float>(qColMin+qColMax);
            ///	      double test = 0;
            int etaRow32 = 0;
            int etaCol32 =0;
            etaRow32 = lround(etaRow*32);
            etaCol32 = lround(etaCol*32);
            int posStrategy = 1;
            if(posStrategy == 1 && !hasGanged && etaRow>0 && etaCol > 0){
                if (BarrelEndCap==0) {
		  phi_average+= lround((getDeltaX1A(clu.get())+(getDeltaX2A(clu.get()))*etaRow32)/1024.); //  >>10;

                    if ( (etaModule+(colMin+colMax)/2./pixYScaleFactor/numberOfEtaPixelsInModule-0.5) > 0){
                        if ( sensorThickness*((etaModule+(colMin+colMax)/2./pixYScaleFactor/numberOfEtaPixelsInModule-0.5)
                                              * moduleActiveLength / radius)>etaPitch) {
                            ///		    test = etaPitch;
                            eta_average+= pixYScaleFactor*(etaCol-0.5);
                            eta_average = lround(eta_average);
                        }
                        else
			  eta_average+= lround((getDeltaY1A(clu.get())+getDeltaY2A(clu.get())*etaCol32
						+ getDeltaY1B(clu.get())*(colMin+colMax)
						+ getDeltaY2B(clu.get())*etaCol32*(colMin+colMax))/2048.);
                    }
                    else{
                        if ( sensorThickness*(-1*(etaModule+(colMin+colMax)/2./pixYScaleFactor/numberOfEtaPixelsInModule-0.5)
                                              * moduleActiveLength / radius)>etaPitch) {
                            eta_average += pixYScaleFactor*(etaCol - 0.5);
                            eta_average = lround(eta_average);
                        }
                        else
			  eta_average -= lround((getDeltaY1A(clu.get())+getDeltaY2A(clu.get())*etaCol32
						 + getDeltaY1B(clu.get())*(colMin+colMax)
						 + getDeltaY2B(clu.get())*etaCol32*(colMin+colMax)) / 2048.);
                    }

                }

                else{
		  phi_average +=  lround((getDeltaXEC1A(clu.get())+getDeltaXEC2A(clu.get())*etaRow32)/1024.);
		  eta_average +=  lround((getDeltaYEC1A(clu.get())+getDeltaYEC2A(clu.get())*etaCol32)/1024.);

                }

            }
        }
        //Correction to the eta_average and phi_average used for
        //m_pixelClusteringMode 1 and 100
        if (m_pixelClusteringMode <= ftk::pcm::PIXEL_CLUSTERING_MIXED) {
            float pixelEstimateCotTheta = -9999.;

            /* The next lines calculate CotTheta of a hypothetic track starting from ATLAS (0,0,0)
             * and crossing the center of the bounding box of the cluster.
             * The Z global position is estimated as: module Z position + cluster Z position within the module.
             *  The radius is a fixed costant depending on the layer. It could be better estimated accounting
             *  also for the rphi position within the module.
             */
            pixelEstimateCotTheta = (etaModule+(rowMin+rowMax)/2./pixYScaleFactor/numberOfEtaPixelsInModule-0.5)
                * moduleActiveLength / radius;
            if (m_pixelClusteringMode>=ftk::pcm::PIXEL_CLUSTERING_IDEAL_APRIL_2014_FIX) /* Fixing an error in the formula */
                pixelEstimateCotTheta = (etaModule+(colMin+colMax)/2./pixYScaleFactor/numberOfEtaPixelsInModule-0.5)
                    * moduleActiveLength / radius;


            // Compute eta for charge interpolation correction (if required)
            // Two pixels may have tot=0 (very rarely, hopefully)
            float etaRow = -1;
            float etaCol = -1;
            if(qRowMin+qRowMax > 0)
                etaRow = qRowMax/float(qRowMin+qRowMax);
            if(qColMin+qColMax > 0)
                etaCol = qColMax/float(qColMin+qColMax);

            // Charge interpolation. Very rough guess (one can do better with
            // candidate track information later) TL
            //      bool hasGanged = av.getIncludesGanged();
            float deltax = 0;
            float deltay = 0;
            if (BarrelEndCap==0) { //pixelID.is_barrel(elementID)){
                deltax = 30*ftk::micrometer*(sensorThickness/(250*ftk::micrometer));
                deltay = sensorThickness*fabs(pixelEstimateCotTheta);
                if(deltay > etaPitch ){
                    deltay = etaPitch;
                }
            } else {
                deltax = 10*ftk::micrometer*sqrt(sensorThickness/(250*ftk::micrometer));
                deltay = 10*ftk::micrometer*sqrt(sensorThickness/(250*ftk::micrometer));
            }

            // Width of the region of charge sharing. For disks assume normal incidence: delta is small, due to diffusion
            // of drifting charges in silicon. For barrel, assume 10 deg. incidence in Rphi, in z compute from pseudorapidity
            // This may be improved with better parameterization, but it is probably better to use candidate track information
            // later in reconstruction. TL
            // Values are made dependent on the sensor thickness to accomodate // different sensors layout. AA
            //    Point3D<double> globalPos = element->globalPosition(centroid);
            //    InDetDD::SiLocalPosition totCorrection(0,0,0);
            int posStrategy = 1; //Same as posStrategy == 1 in InDetRecTools/SiClusterizationTool/trunk/src/MergedPixelsTool.cxx#L701
            if(posStrategy == 1 && !hasGanged && etaRow>0 && etaCol > 0){
                phi_average += pixXScaleFactor*deltax*(etaRow-0.5)/ftk::phiPitch;
                eta_average += pixYScaleFactor*deltay*(etaCol-0.5)/etaPitch;
            }
        }

        if (phi_average<0) {
            Error("averageCluster") << "phi_average < 0" << std::endl;
            phi_average = 0;
        }
        if (eta_average<0) {
            Error("averageCluster") << "eta_average < 0" << std::endl;
            eta_average = 0;
        }

        if (BarrelEndCap != 0)
            phi_average += ftk::clustering::pixelEndCapRPhiCorrection*pixXScaleFactor;
        av.setRowCoordinate( lround(phi_average) );
        av.setColumnCoordinate( lround(eta_average) );
        av.setSplit(false);
        if (m_pixelClusteringMode >= ftk::pcm::PIXEL_CLUSTERING_MIXED && isSplitCluster(clu.get()))
            av.setSplit(true);

        eta_average *= numberOfEtaPixelsInModule/lengthOfPixelModuleInUmPixels/pixYScaleFactor;
        phi_average /= pixXScaleFactor;

        double delta;
        delta = eta_average - static_cast<int>(eta_average);
        av.setDeltaEta(delta);
        av.setEtaStrip( static_cast<int>(eta_average));
        delta = phi_average - static_cast<int>(phi_average);
        av.setDeltaPhi(delta);
        av.setPhiSide( static_cast<int>(phi_average) );

        break; // end of PIXEL
    }
    default:
        break;
    } // end of switch

    // finally divide by nHits
    av.divX(nHits);
    av.divY(nHits);
    av.divZ(nHits);

    // AB - perform geant parent matching and store with the cluster
    // data.  this involves passing all raw hits associated with the
    // cluster to a MultiTruth which will be stored with the hit
    // when it is converted to an FTKHit.
    MultiTruth mt;
    for( hitVector::iterator ihit=clu->hitlist.begin(), fhit=clu->hitlist.end(); ihit!=fhit; ++ihit ) {
        // record highest pt contribution to the combination (cluster)
        if( (*ihit)->getTruth() ) {
            mt.maximize( *((*ihit)->getTruth()) );
        } else {
            MultiTruth::Barcode uniquecode((*ihit)->getEventIndex(),
                                           (*ihit)->getBarcode());
            mt.maximize(uniquecode,(*ihit)->getBarcodePt());
        }
    } // end record truth for each raw channel in the cluster
    clu->clusterEquiv.setTruth(mt);
}

void FTKClusteringEngine::atlClusteringBlayer(std::vector<FTKRawHit> &hits) {
    /*
     * Split blayer modules in half along phi
     * this effectively makes 22*2=44 modules along phi!
     */
    if(!m_splitBlayerModules) return;
    for(unsigned int i = 0; i < hits.size(); i++) {
#define HIT hits[i]
        // AK BLAYER : change # modules in barrel blayer from 22 to 44:
        if(HIT.getIsPixel() && HIT.getBarrelEC()==ftk::BARREL && HIT.getLayer()==0) {
            HIT.setPhiModule(HIT.getPhiModule()*2);
            // mid-module sector division: assume SS=20 or 10.
            // Then the division is right at SS boundary
            if(HIT.getPhiSide()>159) HIT.setPhiModule(HIT.getPhiModule()+1); // HARDCODED (328/2 rounded to SS=16)
        }
#undef HIT
    }
}


void FTKClusteringEngine::realisticPixelDecoder(hitVector &currentHits)
{
    std::stack<std::unique_ptr<FTKRawHit>> lifo;
    std::queue<std::unique_ptr<FTKRawHit>> fifo;
    bool getIsIBL = hitOnIBLmodule(*currentHits.front());
    if (getIsIBL)
        std::cout << "yep, ibl module" << std::endl;

    if(!getIsIBL) {
        if (currentHits.size() > 1)
            std::stable_sort(currentHits.begin(), currentHits.end(), sortPixelInput);

        for(auto& hit: currentHits) {
            if ((*hit).getPhiSide() < ftk::clustering::rowsInFEChipPerPixelModuleRow) {
                fifo.push( std::move(hit) );
            }
            else {
                lifo.push( std::move(hit) );
            }
        }

        currentHits.clear();

        while(!lifo.empty() && !fifo.empty()){
            if ((lifo.top().get())->getEtaStrip() <= (fifo.front().get())->getEtaStrip()) {
                currentHits.push_back( std::move(lifo.top()) );
                lifo.pop();
            }
            else {
                currentHits.push_back(std::move(fifo.front()));
                fifo.pop();
            }
        }

        while(!lifo.empty()) {
            currentHits.push_back( std::move(lifo.top()) );
            lifo.pop();
        }
        while(!fifo.empty()) {
            currentHits.push_back( std::move(fifo.front()) );
            fifo.pop();
        }
    } else {
        std::stack<std::unique_ptr<FTKRawHit>> lifo_planar;
        std::queue<std::unique_ptr<FTKRawHit>> fifo_planar;

        //invert row coordinates
        for(auto& hit: currentHits) {
            std::unique_ptr<FTKRawHit> h = std::make_unique<FTKRawHit>(*hit);
            h->setPhiSide( 335 - (*hit).getPhiSide() );
            h->setEtaStrip( (*hit).getEtaStrip()) ;
            hit.swap(h);
        }

        if (currentHits.size() > 1)
            std::stable_sort(currentHits.begin(), currentHits.end(), sortIBLInput);

        for(auto& hit: currentHits) {
            if ((*hit).getEtaStrip() < ftk::clustering::colsInFEChipPerIblModuleRow/2)
                fifo.push(std::move(hit));
            else if ((*hit).getEtaStrip() >= ftk::clustering::colsInFEChipPerIblModuleRow/2
                     && (*hit).getEtaStrip() < ftk::clustering::colsInFEChipPerIblModuleRow)
                lifo.push(std::move(hit));
            else if ((*hit).getEtaStrip() >= ftk::clustering::colsInFEChipPerIblModuleRow
                     && (*hit).getEtaStrip() < ftk::clustering::colsInFEChipPerIblModuleRow * 3/2)
                fifo_planar.push(std::move(hit));
            else if ((*hit).getEtaStrip() >= ftk::clustering::colsInFEChipPerIblModuleRow * 3/2
                    && (*hit).getEtaStrip() < 2 * ftk::clustering::colsInFEChipPerIblModuleRow)
                lifo_planar.push(std::move(hit));
    }

        currentHits.clear();

        while(!fifo.empty()){
            currentHits.push_back( std::move(fifo.front()) );
            fifo.pop();
        }

        while(!lifo.empty()) {
            currentHits.push_back(std::move (lifo.top()) );
            lifo.pop();
        }

        while(!fifo_planar.empty()) {
            currentHits.push_back(std::move(fifo_planar.front()) );
            fifo_planar.pop();
        }

        while(!lifo_planar.empty()) {
            currentHits.push_back(std::move(lifo_planar.top()) );
            lifo_planar.pop();
        }
    }

}

FTKRawHit FTKClusteringEngine::gridAUTH(boost::circular_buffer<std::unique_ptr<FTKRawHit>> &cb,
                                                         hitVector &fifo, hitVector &gridhits)
{

    //seed is set from cb if there are hits and from
    //fifo if the seed is empty.
    std::unique_ptr<FTKRawHit> seed;
    if (cb.size() == 0){
        seed = std::make_unique<FTKRawHit>(*fifo.front());
    }
    else if (cb.size() != 0) {
        seed = std::make_unique<FTKRawHit>(*cb.front());
        for(auto& cbi: cb) {
            if ((*cbi).getEtaStrip() < (*seed).getEtaStrip())
                seed = std::make_unique<FTKRawHit>(*cbi);
            else if ((*cbi).getEtaStrip() == (*seed).getEtaStrip()) {
                if ((*cbi).getPhiSide() < (*seed).getPhiSide() )
                    seed = std::make_unique<FTKRawHit>(*cbi);
            }
        }
    }

    //step 2: set grid window limits
    int gridStrCol = ((*seed).getEtaStrip() % 2 != 0) ? (*seed).getEtaStrip() - 1 : (*seed).getEtaStrip();
    int gridCntRow = (*seed).getPhiSide();

    //step 3: put hits from cb to the grid window
    int dist = distance(cb.begin(), cb.end());
    for(int i = 0; i < dist; i++) {
        if (!hitColInGrid(gridStrCol, cb.front())) {
            cb.pop_front();
            continue;
        }
        if (hitRowInGrid(gridCntRow, cb.front())) {
            gridhits.push_back(std::move(cb.front()));
        }
        else {
            cb.push_back(std::move(cb.front()));
        }
        cb.pop_front();
    }

    //step 4: put hits from fifo to the grid window
    std::vector<std::unique_ptr<FTKRawHit>>::iterator hit = fifo.begin();
    while (hit != fifo.end()) {
        if (!hitColInGrid(gridStrCol, (*hit).get() )) {
            break;
        }
        if (hitRowInGrid(gridCntRow, (*hit).get())) {
            gridhits.push_back(std::move(*hit));
        }
        else {
            cb.push_back(std::move(*hit));
        }
        fifo.erase(hit);
    }

    return *seed.get();
}



/*!
 * Function responsible for hit clustering and the subsequent centroid
 * calculation
 * \param hits hits which will be clustered
 * \return void
 */
void FTKClusteringEngine::atlClusteringLNF(std::vector<FTKRawHit> &hits)
{
    /*
     * Step 1: Organize the hits by the module they belong to. Furthermore, the
     * ganged hits are duplicated, if requested. The copy constructor is used,
     * instead of manually assigning each variable.
     */
    hitsByModuleMap hitsByModule; ///< store hits by module
    std::vector<FTKRawHit> gangedPixelList; ///< vector of ganged pixels
    gangedPixelList.reserve(hits.size()); // reserve memory in excess

    for(const auto& hit: hits) {
        int modId = getModuleId(hit);
        if (modId<0)
            Error("atlClusteringLNF") << "modId < 0" << std::endl;
        hitsByModule[modId].push_back( std::make_unique<FTKRawHit>(hit) );
        if (m_duplicateGanged && hitIsGanged(hit)) {
            gangedPixelList.push_back( hit );
            FTKRawHit& lastPointer = gangedPixelList.back();
            lastPointer.setPhiSide(gangedPartner( hit ));
            hitsByModule[modId].push_back( std::make_unique<FTKRawHit>(lastPointer) );
        }
    }

    /*
     * Step 2: build the list of clusters.
     * The ideal clustering is going to be used in the following cases:
     * 1. m_pixelClusteringMode is less than 100 (PIXEL_CLUSTERING_MIXED)
     * 2. The module is not a pixel module
     */
    clustersByModuleMap clustersByModule; ///< store clusters by module
    for (auto& module: hitsByModule) { // loop over modules
        int modId = module.first;
        hitVector currentHits  = std::move(module.second);

        cluList currentClusters; // instantiate cluster list

        if (currentHits.front()->getIsSCT()){
            makeClustersLNF(currentHits, currentClusters); // use ideal clustering
        }
        else if(currentHits.front()->getIsPixel()) {
	  if (m_pixelClusteringMode < ftk::pcm::PIXEL_CLUSTERING_MIXED) {
                makeClustersLNF(currentHits, currentClusters);
            }
	  else if (m_pixelClusteringMode >= ftk::pcm::PIXEL_CLUSTERING_MIXED) {
                realisticPixelDecoder(currentHits);
                makeClustersGridWindow(currentHits, currentClusters);
            }

        }

        clustersByModule[modId] = std::move( currentClusters ) ;
    }

    /*
     * Third: take the "average" of each cluster
     */
    int nTotClu = 0;
    for (auto& module: clustersByModule) {
        hitVector connectedHits; // list of ganged hits that are connected to a confirmed ganged hit
        for (auto& clu: module.second) {
            // do pattern recognition in the ganged region
            if ( (clu->hitlist.front())->getIsPixel() && findConnectedGanged(clu, connectedHits) ) // set isSafe
                clu->isSafe = true;
        }

        for (auto& clu: module.second) {
            if ( (clu->hitlist.front())->getIsPixel() && isKilled(clu, connectedHits) ) // set isKilled
                clu->isKilled = true;
            averageCluster(clu);
            nTotClu++; // count all clusters
        }
    }

    /*
     * Fourth: put clusters/hits back in the event hit list
     */
    hits.clear();

    int hitCounter=0;
    int deletedGangedClusters=0;
    for (auto& module: clustersByModule) {
        for (const auto& clu: module.second) {
            // // kill clusters with ganged hits according to ganged pattern recognition
            if ( m_gangedPatternRecognition && clu->isKilled && !clu->isSafe ) {
            //     // AA 2009-07-07 removing the "isSafe" protection does not change
            //     // number of combinations to fit for single muon events
                deletedGangedClusters++;
                continue;
            }

            // store hit by value
            FTKRawHit cluEquiv = clu->clusterEquiv;
            hits.push_back(cluEquiv);
            ++hitCounter;
        }
        // delete module.second;
    }

    /*
     * Fifth: free memory (could optimize and do this in previous step)
     */
    // The atlhit data from event is allocated and destroyed outside this function.
    // clustersByModuleMap is automatically deleted.
    clustersByModule.clear();
}


int FTKClusteringEngine::hitIsGanged(const FTKRawHit &hit) {
    if (FTKSetup::getFTKSetup().getIBLMode()>=1 && hit.getLayer()==0)
        return 0; // FlagAA: IBL is never ganged
    if (hit.getIsPixel()) {
        int phi = hit.getPhiSide();
        if (pixelRowIsGanged(phi))
            return phi;
    }
    return 0;
}


int FTKClusteringEngine::gangedPartner(const FTKRawHit &hit) {
    if (hit.getIsPixel()) {
        switch (hit.getPhiSide()) {
        case 153:
            return 160;
        case 160:
            return 153;
        case 155:
            return 161;
        case 161:
            return 155;
        case 157:
            return 162;
        case 162:
            return 157;
        case 159:
            return 163;
        case 163:
            return 159;
        case 168:
            return 164;
        case 164:
            return 168;
        case 170:
            return 165;
        case 165:
            return 170;
        case 172:
            return 166;
        case 166:
            return 172;
        case 174:
            return 167;
        case 167:
            return 174;
        default:
            return hit.getPhiSide();
        }
    }
    return -1;
}


bool FTKClusteringEngine::pixelRowIsGanged(const int row)
{
    switch (row) {
    case 153:
    case 155:
    case 157:
    case 159:
    case 168:
    case 170:
    case 172:
    case 174:
        return row;        // pixel ganged found (readout channel)
    }
    if (160<=row && row<=167)
        return row;        // pixel ganged found (non readout channel)
    return false;
}


double FTKClusteringEngine::getDeltaX1A(const cluster* clu)
{
    FTKRawHit first = *(clu->hitlist.front());
    bool isIBLmodule  = hitOnIBLmodule(first);
    float pixXScaleFactor = ftk::clustering::xScaleFactorPixel; ///<multiply by 16 to count in unit of 25um
    float sensorThickness = ftk::sensorThicknessPixel;
    if (isIBLmodule) {
        sensorThickness = ftk::sensorThicknessIbl;
    }
    return lround(-(32*32*(30*ftk::micrometer*pixXScaleFactor*(sensorThickness/(250*ftk::micrometer))/ftk::phiPitch)/2.));

}


double FTKClusteringEngine::getDeltaX2A(const cluster* clu)
{
    FTKRawHit first = *(clu->hitlist.front());
    bool isIBLmodule  = hitOnIBLmodule(first);
    float sensorThickness = ftk::sensorThicknessPixel;
    float pixXScaleFactor = ftk::clustering::xScaleFactorPixel;
    if (isIBLmodule) {
        sensorThickness = ftk::sensorThicknessIbl;
    }
    return lround(32*(30*ftk::micrometer*pixXScaleFactor*(sensorThickness/(250*ftk::micrometer))/ftk::phiPitch));
}


int FTKClusteringEngine::getDeltaXEC1A(const cluster* clu)
{
    FTKRawHit first = *(clu->hitlist.front());
    bool isIBLmodule  = hitOnIBLmodule(first);
    float sensorThickness = ftk::sensorThicknessPixel;
    float pixXScaleFactor = ftk::clustering::xScaleFactorPixel;
    if (isIBLmodule) {
        sensorThickness = ftk::sensorThicknessIbl;
    }
    return lround(- (32*32*(10*ftk::micrometer*pixXScaleFactor*(sensorThickness/(250*ftk::micrometer))/ftk::phiPitch)/2.));
}


int FTKClusteringEngine::getDeltaXEC2A(const cluster* clu)
{
    FTKRawHit first = *(clu->hitlist.front());
    bool isIBLmodule  = hitOnIBLmodule(first);
    float sensorThickness = ftk::sensorThicknessPixel;
    float pixXScaleFactor = ftk::clustering::xScaleFactorPixel;
    if (isIBLmodule) {
        sensorThickness = ftk::sensorThicknessIbl;
    }
    return lround(32*(10*ftk::micrometer*pixXScaleFactor*(sensorThickness/(250*ftk::micrometer))/ftk::phiPitch));
}


int FTKClusteringEngine::getDeltaYEC1A(const cluster* clu)
{
    FTKRawHit first = *(clu->hitlist.front());
    bool isIBLmodule  = hitOnIBLmodule(first);
    float sensorThickness = ftk::sensorThicknessPixel;
    float etaPitch = ftk::etaPitchPixel;
    float pixYScaleFactor = ftk::clustering::yScaleFactorPixel;
    if (isIBLmodule) {
        sensorThickness = ftk::sensorThicknessIbl;
        etaPitch = ftk::etaPitchIbl;
    }
    return lround(- (32*32*(10*ftk::micrometer*pixYScaleFactor*(sensorThickness/(250*ftk::micrometer))/etaPitch)/2.));
}


int FTKClusteringEngine::getDeltaYEC2A(const cluster* clu)
{
    FTKRawHit first = *(clu->hitlist.front());
    bool isIBLmodule  = hitOnIBLmodule(first);
    float sensorThickness = ftk::sensorThicknessPixel;
    float etaPitch = ftk::etaPitchPixel;
    float pixYScaleFactor = ftk::clustering::yScaleFactorPixel;
    if (isIBLmodule) {
        sensorThickness = ftk::sensorThicknessIbl;
        etaPitch = ftk::etaPitchIbl;
    }
    return lround(32*(10*ftk::micrometer*pixYScaleFactor*(sensorThickness/(250*ftk::micrometer))/etaPitch));
}


int FTKClusteringEngine::getDeltaY1A(const cluster* clu)
{
    FTKRawHit first = *(clu->hitlist.front());
    int layer = first.getLayer();
    bool isIBLmodule  = hitOnIBLmodule(first);
    if (FTKSetup::getFTKSetup().getIBLMode()==0)
        layer++;
    float radius = ftk::clustering::radii[layer];
    float sensorThickness = ftk::sensorThicknessPixel;
    float etaPitch = ftk::etaPitchPixel;
    float moduleActiveLength = ftk::lengthOfPixelModuleIn400umPixels*ftk::etaPitchPixel/ftk::micrometer;
    float pixYScaleFactor = ftk::clustering::yScaleFactorPixel; ///<multiply by 16 to count in unit of 25um
    float etaModule = first.getEtaModule()-6;
    if (isIBLmodule) {
        sensorThickness = ftk::sensorThicknessIbl;etaPitch = ftk::etaPitchIbl;
        moduleActiveLength = ftk::lengthOfIblModuleIn250umPixels*ftk::etaPitchIbl/ftk::micrometer; // planar sensors
        pixYScaleFactor = ftk::clustering::yScaleFactorPixel; ///<multiply by 10 to count in unit of 25um
        etaModule = first.getEtaModule()-8;
        etaPitch = ftk::etaPitchIbl;

    }
    return lround ( - 32*32*2* pixYScaleFactor*sensorThickness* etaModule*moduleActiveLength/radius /etaPitch/2. ) + lround( 32*32*2* pixYScaleFactor*sensorThickness*moduleActiveLength/radius /etaPitch/2/2);
}
int FTKClusteringEngine::getDeltaY2A(const cluster* clu){
    FTKRawHit first = *(clu->hitlist.front());
    int layer = first.getLayer();
    bool isIBLmodule  = hitOnIBLmodule(first);
    if (FTKSetup::getFTKSetup().getIBLMode()==0)
        layer++;
    float radius = ftk::clustering::radii[layer];
    float sensorThickness = ftk::sensorThicknessPixel;
    float etaPitch = ftk::etaPitchPixel;
    float moduleActiveLength = ftk::lengthOfPixelModuleIn400umPixels*ftk::etaPitchPixel/ftk::micrometer;
    float pixYScaleFactor = ftk::clustering::yScaleFactorPixel; ///<multiply by 16 to count in unit of 25um
    float etaModule = first.getEtaModule()-6;
    if (isIBLmodule) {
        sensorThickness = ftk::sensorThicknessIbl;etaPitch = ftk::etaPitchIbl;
        moduleActiveLength = ftk::lengthOfIblModuleIn250umPixels*ftk::etaPitchIbl/ftk::micrometer; // planar sensors
        pixYScaleFactor = ftk::clustering::yScaleFactorPixel; ///<multiply by 10 to count in unit of 25um
        etaModule = first.getEtaModule()-8;
        etaPitch = ftk::etaPitchIbl;
    }
    return lround (32*2* pixYScaleFactor*sensorThickness* etaModule*moduleActiveLength/radius /etaPitch) + lround ( - 32*2* pixYScaleFactor*sensorThickness* moduleActiveLength/radius /etaPitch/2.);
}

int FTKClusteringEngine::getDeltaY1B(const cluster* clu){
    FTKRawHit first = *(clu->hitlist.front());
    int layer = first.getLayer();
    bool isIBLmodule  = hitOnIBLmodule(first);
    if (FTKSetup::getFTKSetup().getIBLMode()==0)
        layer++;
    float radius = ftk::clustering::radii[layer];
    float sensorThickness = ftk::sensorThicknessPixel;
    float etaPitch = ftk::etaPitchPixel;
    float numberOfEtaPixelsInModule = ftk::numberOfEtaPixelsInPixelModule;
    float moduleActiveLength = ftk::lengthOfPixelModuleIn400umPixels*ftk::etaPitchPixel/ftk::micrometer;
    if (isIBLmodule) {
        sensorThickness = ftk::sensorThicknessIbl;etaPitch = ftk::etaPitchIbl;
        numberOfEtaPixelsInModule = ftk::numberOfEtaPixelsInIblModule;
        moduleActiveLength = ftk::lengthOfIblModuleIn250umPixels*ftk::etaPitchIbl/ftk::micrometer; // planar sensors
        etaPitch = ftk::etaPitchIbl;
    }
    return  lround ( - 32*32* sensorThickness/numberOfEtaPixelsInModule *moduleActiveLength/radius /etaPitch/2);
}


int FTKClusteringEngine::getDeltaY2B(const cluster* clu)
{
    FTKRawHit first = *(clu->hitlist.front());
    int layer = first.getLayer();
    bool isIBLmodule  = hitOnIBLmodule(first);
    if (FTKSetup::getFTKSetup().getIBLMode()==0)
        layer++;
    float radius = ftk::clustering::radii[layer];
    float sensorThickness = ftk::sensorThicknessPixel;
    float etaPitch = ftk::etaPitchPixel;
    float numberOfEtaPixelsInModule = ftk::numberOfEtaPixelsInPixelModule;
    float moduleActiveLength = ftk::lengthOfPixelModuleIn400umPixels*ftk::etaPitchPixel/ftk::micrometer;
    if (isIBLmodule) {
        sensorThickness = ftk::sensorThicknessIbl;etaPitch = ftk::etaPitchIbl;
        numberOfEtaPixelsInModule = ftk::numberOfEtaPixelsInIblModule;
        moduleActiveLength = ftk::lengthOfIblModuleIn250umPixels*ftk::etaPitchIbl/ftk::micrometer; // planar sensors
        etaPitch = ftk::etaPitchIbl;
    }
    return lround (32 * sensorThickness/numberOfEtaPixelsInModule *moduleActiveLength/radius/etaPitch ) ;
}
