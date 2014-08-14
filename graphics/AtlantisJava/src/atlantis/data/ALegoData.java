package atlantis.data;

import atlantis.event.AData;
import atlantis.event.*;
import java.awt.Color;
import java.awt.geom.Point2D;

import java.util.Iterator;
import java.util.List;
import java.util.Stack;

import atlantis.canvas.AWindow;
import atlantis.graphics.AGraphics;
import atlantis.graphics.ALegoDraw;
import atlantis.parameters.APar;
import atlantis.projection.AProjectionLegoPlot;
import atlantis.utils.ALogger;
import atlantis.utils.AMath;

public class ALegoData
{
    private static ALogger logger = ALogger.getLogger(ALegoData.class);

    private static APar parameterStore = APar.instance();

    /**
     * This function loops over the cell data to find the maxEt.
     * It also rescales the height of the cells if different scales are selected
     * @param lego the tower heights
     * @return maxEt the heighest tower height
     */
    public static double findMaxEt(double[][][] lego)
    {
        double maxEt = 0.0;
        double[][] legosum = new double[lego.length][lego[0].length];
        //find colors to loop over
        int largestColourIndex=0;
        int lowestColourIndex=AProjectionLegoPlot.nLayers;
        Iterator colorIterator = AProjectionLegoPlot.colorset.iterator();
        while(colorIterator.hasNext())
        {
            int currentColor = ((Integer) colorIterator.next()).intValue();
            if(currentColor>largestColourIndex)
                largestColourIndex=currentColor;
            if(currentColor<lowestColourIndex)
                lowestColourIndex=currentColor;
        }
        for (int x = AProjectionLegoPlot.nPhiCells - 1; x >= 0; --x)
        {
            for (int y = 0; y < AProjectionLegoPlot.nEtaCells; ++y)
            {
                for(int z=largestColourIndex; z>=lowestColourIndex; z--)
                {
                    legosum[x][y] += lego[x][y][z];
                }
                if(legosum[x][y] > maxEt)
                    maxEt = legosum[x][y];
            }
        }
        if(maxEt>0)
        {
            if (AProjectionLegoPlot.defaultScale ==1 )
            {
                for (int x = AProjectionLegoPlot.nPhiCells - 1; x >= 0; --x)
                {
                    for (int y = 0; y < AProjectionLegoPlot.nEtaCells; ++y)
                    {
                        for(int z=largestColourIndex; z>=lowestColourIndex; z--)
                        {
                            if(Math.log10(legosum[x][y])>=AProjectionLegoPlot.minimumofLogScale)
                            {
                                //if multiple colors then each take linear amount of full log
                                double fraction=(lego[x][y][z]/legosum[x][y]);
                                lego[x][y][z]=fraction*(Math.log10(legosum[x][y])-AProjectionLegoPlot.minimumofLogScale);
                            }
                            else
                            {
                                //if not above minimum of log scale then don't draw
                                lego[x][y][z]=0.0;
                            }
                        }
                    }
                }
            }
            else if (AProjectionLegoPlot.defaultScale ==2 )
            {
                for (int x = AProjectionLegoPlot.nPhiCells - 1; x >= 0; --x)
                {
                    for (int y = 0; y < AProjectionLegoPlot.nEtaCells; ++y)
                    {
                        for(int z=largestColourIndex; z>=lowestColourIndex; z--)
                        {
                            //if multiple colors then each take linear amount of full sqrt
                            double fraction=(lego[x][y][z]/legosum[x][y]);
                            lego[x][y][z]=fraction*Math.sqrt(legosum[x][y]);
                        }
                    }
                }
            }
        }
        return maxEt;
    }

    public static double findAODMaxEt(AEvent event)
    {
        double AODmaxEt = 0.0;
        //From the caloCluster
        String[] colCluster=event.getActiveCollectionNames("Cluster");
        for(int i=0;i<colCluster.length;i++)
        {
            AClusterData cluster = (AClusterData) event.get(colCluster[i]);
            if (!parameterStore.get("Data", "Cluster").getStatus())
                cluster = null;
            if (cluster != null)
            {
                cluster.makeDrawList();
                for (int m = 0; m < cluster.getNumDraw(); ++m)
                {
                    int list = cluster.getDrawIndex(m);
                    float et = Math.abs(cluster.getET(list));
                    if (et > AODmaxEt)
                        AODmaxEt = et;
                }
            }
        }     
        //From the Jets - Added code to make th e jet considered in the AOD scaling      
        String[] colJet=event.getActiveCollectionNames("Jet");
        for(int i=0;i<colJet.length;i++)
        {
            AJetData jets = (AJetData) event.get(colJet[i]);
            if (!parameterStore.get("Data", "Jet").getStatus())
                jets = null;
            if (jets != null)
            {
                jets.makeDrawList();
                for (int m = 0; m < jets.getNumDraw(); ++m)
                {
                    int list = jets.getDrawIndex(m);
                    float pt = Math.abs(jets.getPt()[list]);
                    if (pt > AODmaxEt)
                        AODmaxEt = pt;
                }
            }
        }  
        // From the muons
        String[] colMuon=event.getActiveCollectionNames("Muon");
        for(int i=0;i<colMuon.length;i++)
        {
            AMuonData muons = (AMuonData) event.get(colMuon[i]);
            if (!parameterStore.get("Data", "Muon").getStatus())
                muons = null;
            if (muons != null)
            {
                muons.makeDrawList();
                for (int m = 0; m < muons.getNumDraw(); ++m)
                {
                    int list = muons.getDrawIndex(m);
                    float pt = Math.abs(muons.getPT(list));
                    if (pt > AODmaxEt)
                        AODmaxEt = pt;
                }
            }
        }
        // From the electrons
        String[] colElectron=event.getActiveCollectionNames("Electron");
        for(int i=0;i<colElectron.length;i++)
        {
            AElectronData electrons = (AElectronData) event.get(colElectron[i]);
            if (!parameterStore.get("Data", "Electron").getStatus())
                electrons = null;
            if (electrons != null)
            {

                electrons.makeDrawList();
                for (int e = 0; e < electrons.getNumDraw(); ++e)
                {
                    int list = electrons.getDrawIndex(e);
                    float pt = Math.abs(electrons.getPT(list));
                    if (pt > AODmaxEt)
                        AODmaxEt = pt;
                }
            }
        }
        // From the photons
        String[] colPhoton=event.getActiveCollectionNames("Photon");
        for(int i=0;i<colPhoton.length;i++)
        {
            APhotonData photons = (APhotonData) event.get(colPhoton[i]);
            if (!parameterStore.get("Data", "Photon").getStatus())
                photons = null;
            if (photons != null)
            {
                photons.makeDrawList();
                for (int p = 0; p < photons.getNumDraw(); ++p)
                {
                    int list = photons.getDrawIndex(p);
                    float pt = Math.abs(photons.getPT(list));
                    if (pt > AODmaxEt)
                        AODmaxEt = pt;
                }
            }
        }
        //From the compositeParticles
        String[] colCompPart=event.getActiveCollectionNames("CompositeParticle");
        for(int i=0;i<colCompPart.length;i++)
        {
            ACompositeParticleData compositeParticles = (ACompositeParticleData) event.get(colCompPart[i]);
            if (!parameterStore.get("Data", "CompositeParticle").getStatus())
                compositeParticles = null;
            if (compositeParticles != null)
            {
                compositeParticles.makeDrawList();
                for (int e = 0; e < compositeParticles.getNumDraw(); ++e)
                {
                    int list = compositeParticles.getDrawIndex(e);
                    float pt = Math.abs(compositeParticles.getPT(list));
                    if (pt > AODmaxEt)
                        AODmaxEt = pt;
                }
            }
        }

        return AODmaxEt;
    }

    public static double findMissingEt(AWindow window, double[][][] lego, AEvent event)
    {
        double met = 0.;
        List hitsAndTracks = event.getHitsAndTracks(window.getProjection());
        for (int h = 0; h < hitsAndTracks.size(); h++)
        {
            AData datatype = ((AData) hitsAndTracks.get(h));
            if (datatype.getName().equals("ETMis"))
            {
                AETMisData mis = (AETMisData) datatype;
                mis.makeDrawList();
                if (mis.getNumDraw() > 0)
                {
                    double temp = Math.sqrt(mis.getETx() * mis.getETx() + mis.getETy() * mis.getETy());
                    if(temp>met)
                        met=temp;
                }
            }
        }
        return met;
    }

    public static void fillHistograms(AEvent event, double[][][] lego)
    {
        AProjectionLegoPlot.colorset.clear();
        if (AProjectionLegoPlot.mode == 0)
        {
            // Calorimeter Lego
            fillCaloLego(event, lego);
        }
        else if (AProjectionLegoPlot.mode == 1)
        {
            // Trigger Tower Lego
            fillTriggerTowerLego(event, lego);
        }
        else
        {
            // Jet Elements Lego
            fillJetElementLego(event, lego);
        }
    }

    private static void fillCaloLego(AEvent event, double[][][] lego)
    {
        List detectors = event.getCalorimeters();
        for (int det = 0; det < detectors.size(); det++)
        {
            ACalorimeterData calorimeter = (ACalorimeterData) detectors.get(det);
            String calname=calorimeter.getName();
            if (!calname.equals("TILE") && !calname.equals("FCAL")
                    && !calname.equals("HEC") && !calname.equals("LAr")
                    && !calname.equals("MBTS"))
            {
                logger.warn("Unknown calorimeter name for LegoPlot: " + calorimeter.getName());
            }

            calorimeter.makeDrawList();
            // for coloring
            byte[] c = calorimeter.getColor();
            for (int i = 0; i < calorimeter.getNumDraw(); ++i)
            {
                int list = calorimeter.getDrawIndex(i);
                double et = calorimeter.getET(list);
                //MBTS energy is in MeV not GeV
                if(calname.equals("MBTS")) et/=1000.;
                double phi = calorimeter.getPhi(list);
                double eta = calorimeter.getEta(list);
                if (eta > 5 || eta < -5 || et == 0.0)
                    continue;
                double dphi = calorimeter.getdPhi(list);
                double deta = calorimeter.getdEta(list);
                phietabinsLoop("calo", phi, dphi, eta, deta, c[list], et, lego);
            }// loop over that calorimeter's cells
        }// loop over the calorimeters
    }

    private static void fillTriggerTowerLego(AEvent event, double[][][] lego)
    {
        //will draw even if data status is off
        String[] colTT=event.getCollectionNames("LVL1TriggerTower");
        for(int k=0;k<colTT.length;k++)
        {
            ALVL1TriggerTowerData lvl1TriggerTower = (ALVL1TriggerTowerData) event.get(colTT[k]);
            if (lvl1TriggerTower == null)
                continue;

            byte[] ttColor;
            for (int j = 0; j < 2; ++j)
            {
                int energyType = parameterStore.get(lvl1TriggerTower.getParameterGroup(), "EnergyType").getI();
                if((j == 0 && energyType == 1) || (j == 1 && energyType == 0))
                    continue;
                lvl1TriggerTower.makeDrawList();

                boolean emOrHad;
                if (j == 0)
                {
                    emOrHad = ALVL1TriggerTowerData.ELECTROMAGNETIC;
                    lvl1TriggerTower.applyEMCuts();
                }
                else
                {
                    emOrHad = ALVL1TriggerTowerData.HADRONIC;
                    lvl1TriggerTower.applyHadCuts();
                }
                ttColor = lvl1TriggerTower.getColor(emOrHad);
                for (int i = 0; i < lvl1TriggerTower.getNumDraw(); ++i)
                {
                    int list = lvl1TriggerTower.getDrawIndex(i);
                    double et = lvl1TriggerTower.getET(list, emOrHad);
                    double phi = lvl1TriggerTower.getPhi(list);
                    double eta = lvl1TriggerTower.getEta(list);
                    if (eta > 5 || eta < -5 || et == 0.0)
                        continue;
                    double dphi = lvl1TriggerTower.getdPhi(list, emOrHad);
                    double deta = lvl1TriggerTower.getdEta(list, emOrHad);
                    phietabinsLoop("trigger", phi, dphi, eta, deta, ttColor[list], et, lego);
                }// loop over that triggerTower's cells
            }
        }
    }

    private static void fillJetElementLego(AEvent event, double[][][] lego)
    {
        //will draw even if data status is off
        String[] colJE=event.getCollectionNames("LVL1JetElement");
        for(int k=0;k<colJE.length;k++)
        {
            ALVL1JetElementData lvl1JetElement = (ALVL1JetElementData) event.get(colJE[k]);
            if (lvl1JetElement == null)
                continue;
            byte[] jeColor = lvl1JetElement.getColor();
            lvl1JetElement.makeDrawList();
            for (int i = 0; i < lvl1JetElement.getNumDraw(); ++i)
            {
                int list = lvl1JetElement.getDrawIndex(i);
                double et = lvl1JetElement.getET(list);
                double phi = lvl1JetElement.getPhi(list);
                double eta = lvl1JetElement.getEta(list);
                if (eta > 5 || eta < -5 || et == 0.0)
                    continue;
                double dphi = lvl1JetElement.getdPhi(list);
                double deta = lvl1JetElement.getdEta(list);
                phietabinsLoop("jetElement", phi, dphi, eta, deta, jeColor[list], et, lego);
            }// loop over that jetElement's cells
        }
    }

    /**
     * Is used to loop over the phi and eta bins to fill the lego "sender"
     * histogram
     *
     * @param sender name of histogram that was sent for output message
     * @param phi
     * @param dphi
     * @param eta
     * @param deta
     * @param color
     * @param et
     * @param lego
     */
    private static void phietabinsLoop(String sender, double phi, double dphi, double eta, double deta, byte color, double et, double[][][] lego)
    {
        if (color >= AProjectionLegoPlot.nLayers)
            color = (byte) (color % AProjectionLegoPlot.nLayers);
        AProjectionLegoPlot.colorset.add(new Integer(color));

        int highphibin = 0, lowphibin = 0;

        double phibinsize = (AMath.TWO_PI / AProjectionLegoPlot.nPhiCells);
        highphibin = (int) ((phi + dphi) /phibinsize);
        lowphibin = (int) ((phi - dphi) /phibinsize);

        int highetabin = 0, lowetabin = 0;
        double etabinsize = 10.0 / AProjectionLegoPlot.nEtaCells;
        highetabin = (int) ((eta + deta + 5.0) /etabinsize);
        lowetabin = (int) ((eta - deta + 5.0) /etabinsize);
        //check eta values
        if (lowetabin < 0)
        {
            lowetabin=0;
            if (highetabin < 0)
                highetabin=0;
        }
        if (highetabin >= AProjectionLegoPlot.nEtaCells)
        {
            highetabin =AProjectionLegoPlot.nEtaCells-1;
            if (lowetabin >= AProjectionLegoPlot.nEtaCells)
                lowetabin =AProjectionLegoPlot.nEtaCells-1;
        }

        for (int phibin = lowphibin; phibin <= highphibin; ++phibin)
        {
            double phiweight = 0;
            if (lowphibin == highphibin)
                phiweight = 1.0;
            else if (phibin == lowphibin)
                phiweight = (lowphibin + 1 - ((phi - dphi) / phibinsize)) * (phibinsize/(2 * dphi));
            else if (phibin == highphibin)
                phiweight = (((phi + dphi) / phibinsize) - highphibin) * (phibinsize/(2 * dphi));
            else
                phiweight = phibinsize/(2 * dphi);
            if (phiweight > 1 || phiweight < 0)
            {
                logger.error("huh(" + sender + ")? how is phiweight...? " + phiweight);
            }

            for (int etabin = lowetabin; etabin <= highetabin; ++etabin)
            {
                double etaweight = 0;
                if (lowetabin == highetabin)
                    etaweight = 1.0;
                else if (etabin == lowetabin)
                    etaweight = (lowetabin + 1 - ((eta - deta + 5.0) / etabinsize)) * (etabinsize/(2 * deta));
                else if (etabin == highetabin)
                    etaweight = (((eta + deta + 5.0) / etabinsize) - highetabin) * (etabinsize/(2 * deta));
                else
                    etaweight = etabinsize/(2 * deta);
                if (etaweight > 1 || etaweight < 0)
                {
                    logger.error("huh(" + sender + ")? how is etaweight...? " + etaweight);
                }

                int wphibin = phibin;// for phi wrapping
                while (wphibin >= AProjectionLegoPlot.nPhiCells)
                    wphibin -= AProjectionLegoPlot.nPhiCells;
                while (wphibin < 0)
                    wphibin += AProjectionLegoPlot.nPhiCells;
                int retabin = etabin;
                if (AProjectionLegoPlot.reverse)
                    retabin = AProjectionLegoPlot.nEtaCells - etabin - 1;

                // add to the legoplot
                lego[wphibin][retabin][color] += et * phiweight * etaweight;
            }// loop over etabins
        }// loop over phibins
    }// end of phietabinsloop

    /**
     * If the user has a different scale selected then this updates the value
     */
    public static double scaleValue(double value)
    {
        if(value==0)
            return 0;
        if (AProjectionLegoPlot.defaultScale == 2)
        {
            value = Math.sqrt(value);
        }
        else if (AProjectionLegoPlot.defaultScale == 1)
        {
            value = Math.log10(value) - AProjectionLegoPlot.minimumofLogScale;
        }
        return value;
    }

    public static void drawHistograms(AWindow window, AGraphics ag, AEvent event, double[][][] lego, double maxEt, double met, double AODmaxEt)
    {
        // Draw the other stuff in the event: jets, missing Et, etc.
        double _jetcircleradius = parameterStore.get("LegoPlot", "JetCircleRadius").getD();
        double _legocut = parameterStore.get("LegoPlot", "LegoCut").getD();
        _legocut=scaleValue(_legocut);
        boolean doAOD = false;
        if (AODmaxEt!=0)
            doAOD = true;

        //for ordering of ROIs
        Stack jetROI = new Stack(), emTauROI = new Stack(), muonROI = new Stack();

        List hitsAndTracks = event.getHitsAndTracks(window.getProjection());
        for (int h = 0; h < hitsAndTracks.size(); h++)
        {
            AData datatype = ((AData) hitsAndTracks.get(h));
            if (datatype.getName().equals("Jet"))
            {
                AJetData jet = (AJetData) datatype;
                jet.makeDrawList();
                byte[] colors = jet.getColor();
                for (int j = 0; j < jet.getNumDraw(); ++j)
                {
                    int list = jet.getDrawIndex(j);
                    double et = jet.getET(list);
                    Color _colorJet = AProjectionLegoPlot.defaultColorMap[colors[list]];
                    ALegoDraw.drawJet(et, jet.getEta(list), jet.getPhi(list), _colorJet, window, ag, _jetcircleradius);
                }
            }
            else if (datatype.getName().equals("ETMis"))
            {
                AETMisData mis = (AETMisData) datatype;
                mis.makeDrawList();
                if (mis.getNumDraw() > 0)
                {
                    byte[] colors = mis.getColor();
                    int phib = (int) ((mis.getPhi()) * AProjectionLegoPlot.nPhiCells / AMath.TWO_PI);
                    if (phib < 0)
                        phib += AProjectionLegoPlot.nPhiCells;
                    if (phib >= AProjectionLegoPlot.nPhiCells)
                        phib -= AProjectionLegoPlot.nPhiCells;
                    Color _colorMis = AProjectionLegoPlot.defaultColorMap[colors[0]];
                    ALegoDraw.drawMissEt(window, ag, phib, mis.getET(), _colorMis, maxEt);
                }
            }
            //save the ROIs so they can be drawn jet-em-muon
            else if (datatype.getName().equals("JetROI"))
            {
                jetROI.push(h);
            }
            else if (datatype.getName().equals("EmTauROI"))
            {
                emTauROI.push(h);
            }
            else if (datatype.getName().equals("MuonROI"))
            {
                muonROI.push(h);
            }
        }//end of loop over hitsandtracks

        //now draw the ROIs in the correct order
        drawROIs(jetROI, window, ag, event);
        drawROIs(emTauROI, window, ag, event);
        drawROIs(muonROI, window, ag, event);


        //for towers find colors to loop over
        int largestColourIndex=0;
        int lowestColourIndex=AProjectionLegoPlot.nLayers;
        Iterator colorIterator = AProjectionLegoPlot.colorset.iterator();
        while(colorIterator.hasNext())
        {
            int currentColor = ((Integer) colorIterator.next()).intValue();
            if(currentColor>largestColourIndex)
                largestColourIndex=currentColor;
            if(currentColor<lowestColourIndex)
                lowestColourIndex=currentColor;
        }
        // Draw the EM and HAD towers
        for (int y = 0; y < AProjectionLegoPlot.nEtaCells; ++y)
        {
            for (int x = AProjectionLegoPlot.nPhiCells - 1; x >= 0; --x)
            {
                double binheight = 0;
                if(AProjectionLegoPlot.getDrawEMHAD() && AProjectionLegoPlot.colorset.size()==2)
                {
                    //for Em/had option should only have 2 colors
                    int z=AProjectionLegoPlot.colorEM;
                    binheight=lego[x][y][z];
                    if (binheight > _legocut)
                    {
                        ALegoDraw.drawBox(window, ag, x, y, z, 0.0, binheight, maxEt, 0.5);
                        if(lego[x][y][AProjectionLegoPlot.colorHad]> _legocut)
                        {
                            z=AProjectionLegoPlot.colorHad;
                            ALegoDraw.drawBox(window, ag, x, y, z, binheight, lego[x][y][z], maxEt, 0.5);
                        }
                    }
                    else if(lego[x][y][AProjectionLegoPlot.colorHad]> _legocut)
                    {
                        z=AProjectionLegoPlot.colorHad;
                        ALegoDraw.drawBox(window, ag, x, y, z, 0.0, lego[x][y][z], maxEt, 0.5);
                    }
                }
                else
                {
                    //if not Em/Had then loop over colors
                    for(int z=largestColourIndex; z>=lowestColourIndex; z--)
                    {
                        if (lego[x][y][z] > _legocut)
                        {
                            ALegoDraw.drawBox(window, ag, x, y, z, binheight, lego[x][y][z], maxEt, 0.5);
                            binheight += lego[x][y][z];
                        }
                    }
                }
            }
        }

        //loop again and draw things that need to be on top of histogram
        for (int h = 0; h < hitsAndTracks.size(); h++)
        {
            AData datatype = ((AData) hitsAndTracks.get(h));
            if (datatype.getName().equals("Jet"))
            {
                AJetData jet = (AJetData) datatype;
                jet.makeDrawList();
                byte[] colors = jet.getColor();
                for (int j = 0; j < jet.getNumDraw(); ++j)
                {
                    int list = jet.getDrawIndex(j);
                    double et = jet.getET(list);
                    Color _colorJet = AProjectionLegoPlot.defaultColorMap[colors[list]];
                    if (parameterStore.get("LegoPlot", "JetText").getStatus())
                        ALegoDraw.drawJetText(et, jet.getEta(list), jet.getPhi(list), _colorJet, window, ag, _jetcircleradius);
                }
            }
        }

        if (doAOD)
        {
            //Draw the new jet Towers
            String[] colJet=event.getActiveCollectionNames("Jet");
            for(int i=0;i<colJet.length;i++)
            {
                AJetData jets = (AJetData) event.get(colJet[i]);

                if (!parameterStore.get("Data", "Jet").getStatus())
                    jets = null;
                if (jets != null)
                {
                    jets.makeDrawList();
                    byte[] colors = jets.getColor();
                    for (int j = 0; j < jets.getNumDraw(); ++j)
                    {
                        int list = jets.getDrawIndex(j);
                        Color _colorJet = AProjectionLegoPlot.defaultColorMap[colors[list]];
                        int etabin = (int) ((jets.getEta(list) + 5.0) * AProjectionLegoPlot.nEtaCells / 10.0);
                        if (AProjectionLegoPlot.reverse)
                            etabin = AProjectionLegoPlot.nEtaCells - etabin;
                        int phibin = (int) ((jets.getPhi(list)) * AProjectionLegoPlot.nPhiCells / AMath.TWO_PI);
                        double pt = Math.abs(jets.getPt()[list]);
                        pt=scaleValue(pt);
                        ALegoDraw.drawBox(window, ag, phibin, etabin, AProjectionLegoPlot.DRAW_JET,_colorJet, 0, pt, maxEt, 2.0);
                    }
                }
            }                           

            // Draw the caloClusters
            String[] colCluster=event.getActiveCollectionNames("Cluster");
            for(int i=0;i<colCluster.length;i++)
            {
                AClusterData cluster = (AClusterData) event.get(colCluster[i]);
                if (!parameterStore.get("Data", "Cluster").getStatus())
                    cluster = null;
                if (cluster != null)
                {
                    cluster.makeDrawList();
                    for (int m = 0; m < cluster.getNumDraw(); ++m)
                    {
                        int list = cluster.getDrawIndex(m);
                        int etabin = (int) ((cluster.getEta(list) + 5.0) * AProjectionLegoPlot.nEtaCells / 10.0);
                        if (AProjectionLegoPlot.reverse)
                            etabin = AProjectionLegoPlot.nEtaCells - etabin;
                        int phibin = (int) ((cluster.getPhi(list)) * AProjectionLegoPlot.nPhiCells / AMath.TWO_PI);
                        double et = Math.abs(cluster.getET(list));
                        et=scaleValue(et);
                        ALegoDraw.drawBox(window, ag, phibin, etabin, AProjectionLegoPlot.DRAW_CLUSTER, 0, et, maxEt, .5);
                    }
                }
            }

            // Draw the bjets
            String[] colBjets=event.getActiveCollectionNames("BJet");
            for(int i=0;i<colBjets.length;i++)
            {
                ABJetData bjets = (ABJetData) event.get(colBjets[i]);
                if (!parameterStore.get("Data", "BJet").getStatus())
                    bjets = null;
                if (bjets != null)
                {
                    bjets.makeDrawList();
                    for (int bj = 0; bj < bjets.getNumDraw(); ++bj)
                    {
                        int list = bjets.getDrawIndex(bj);
                        double et = bjets.getPT(list);
                        ALegoDraw.drawJet(et, bjets.getEta(list), bjets.getPhi(list), AProjectionLegoPlot.defaultColorMap[parameterStore.get("BJet", "Constant").getI()], window, ag, _jetcircleradius + _jetcircleradius / 5.0);
                    }
                }
            }

            // Draw the taujets
            String[] colTau=event.getActiveCollectionNames("TauJet");
            for(int i=0;i<colTau.length;i++)
            {
                ATauJetData tjets = (ATauJetData) event.get(colTau[i]);
                if (!parameterStore.get("Data", "TauJet").getStatus())
                    tjets = null;
                if (tjets != null)
                {
                    tjets.makeDrawList();
                    for (int tj = 0; tj < tjets.getNumDraw(); ++tj)
                    {
                        int list = tjets.getDrawIndex(tj);
                        double et = tjets.getPT(list);
                        ALegoDraw.drawJet(et, tjets.getEta(list), tjets.getPhi(list), AProjectionLegoPlot.defaultColorMap[parameterStore.get("TauJet", "Constant").getI()], window, ag, _jetcircleradius / 2.0);
                    }
                }
            }

            //Draw the muons
            String[] colMuon=event.getActiveCollectionNames("Muon");
            for(int i=0;i<colMuon.length;i++)
            {
                AMuonData muons = (AMuonData) event.get(colMuon[i]);
                if (!parameterStore.get("Data", "Muon").getStatus())
                    muons = null;
                if (muons != null)
                {
                    muons.makeDrawList();
                    for (int m = 0; m < muons.getNumDraw(); ++m)
                    {
                        int list = muons.getDrawIndex(m);
                        int etabin = (int) ((muons.getEta(list) + 5.0) * AProjectionLegoPlot.nEtaCells / 10.0);
                        if (AProjectionLegoPlot.reverse)
                            etabin = AProjectionLegoPlot.nEtaCells - etabin;
                        int phibin = (int) ((muons.getPhi(list)) * AProjectionLegoPlot.nPhiCells / AMath.TWO_PI);
                        double pt = Math.abs(muons.getPT(list));
                        pt=scaleValue(pt);
                        ALegoDraw.drawBox(window, ag, phibin, etabin, AProjectionLegoPlot.DRAW_MUON, 0, pt, maxEt, 0.5);
                    }
                }
            }

            //Draw the electrons
            String[] colElectron=event.getActiveCollectionNames("Electron");
            for(int i=0;i<colElectron.length;i++)
            {
                AElectronData electrons = (AElectronData) event.get(colElectron[i]);

                if (!parameterStore.get("Data", "Electron").getStatus())
                    electrons = null;
                if (electrons != null)
                {
                    electrons.makeDrawList();
                    for (int e = 0; e < electrons.getNumDraw(); ++e)
                    {
                        int list = electrons.getDrawIndex(e);
                        int etabin = (int) ((electrons.getEta(list) + 5.0) * AProjectionLegoPlot.nEtaCells / 10.0);
                        if (AProjectionLegoPlot.reverse)
                            etabin = AProjectionLegoPlot.nEtaCells - etabin;
                        int phibin = (int) ((electrons.getPhi(list)) * AProjectionLegoPlot.nPhiCells / AMath.TWO_PI);
                        double pt = Math.abs(electrons.getPT(list));
                        pt=scaleValue(pt);
                        ALegoDraw.drawBox(window, ag, phibin, etabin, AProjectionLegoPlot.DRAW_ELECTRON, 0, pt, maxEt, 0.5);
                    }
                }
            }

            //Draw the photons
            String[] colPhoton=event.getActiveCollectionNames("Photon");
            for(int i=0;i<colPhoton.length;i++)
            {
                APhotonData photons = (APhotonData) event.get(colPhoton[i]);
                if (!parameterStore.get("Data", "Photon").getStatus())
                    photons = null;
                if (photons != null)
                {
                    photons.makeDrawList();
                    for (int p = 0; p < photons.getNumDraw(); ++p)
                    {
                        int list = photons.getDrawIndex(p);
                        int etabin = (int) ((photons.getEta(list) + 5.0) * AProjectionLegoPlot.nEtaCells / 10.0);
                        if (AProjectionLegoPlot.reverse)
                            etabin = AProjectionLegoPlot.nEtaCells - etabin;
                        int phibin = (int) ((photons.getPhi(list)) * AProjectionLegoPlot.nPhiCells / AMath.TWO_PI);
                        double pt = Math.abs(photons.getPT(list));
                        pt=scaleValue(pt);
                        ALegoDraw.drawBox(window, ag, phibin, etabin, AProjectionLegoPlot.DRAW_PHOTON, 0, pt, maxEt, 0.5);
                    }
                }
            }

            //Draw the compositeParticles
            String[] colCompPart=event.getActiveCollectionNames("CompositeParticle");
            for(int i=0;i<colCompPart.length;i++)
            {
                ACompositeParticleData compositeParticles = (ACompositeParticleData) event.get(colCompPart[i]);
                if (!parameterStore.get("Data", "CompositeParticle").getStatus())
                    compositeParticles = null;
                if (compositeParticles != null)
                {
                    compositeParticles.makeDrawList();
                    for (int e = 0; e < compositeParticles.getNumDraw(); ++e)
                    {
                        int list = compositeParticles.getDrawIndex(e);
                        int etabin = (int) ((compositeParticles.getEta(list) + 5.0) * AProjectionLegoPlot.nEtaCells / 10.0);
                        if (AProjectionLegoPlot.reverse)
                            etabin = AProjectionLegoPlot.nEtaCells - etabin;
                        int phibin = (int) ((compositeParticles.getPhi(list)) * AProjectionLegoPlot.nPhiCells / AMath.TWO_PI);
                        double pt = Math.abs(compositeParticles.getPT(list));
                        pt=scaleValue(pt);
                        //change how drawn depending on type
                        if(Math.abs(compositeParticles.getPdgId(list))==13)
                            ALegoDraw.drawBox(window, ag, phibin, etabin, AProjectionLegoPlot.DRAW_MUON, 0, pt, maxEt, 0.5);
                        else if(Math.abs(compositeParticles.getPdgId(list))==11)
                            ALegoDraw.drawBox(window, ag, phibin, etabin, AProjectionLegoPlot.DRAW_ELECTRON, 0, pt, maxEt, 0.5);
                        else if(Math.abs(compositeParticles.getPdgId(list))==22)
                            ALegoDraw.drawBox(window, ag, phibin, etabin, AProjectionLegoPlot.DRAW_PHOTON, 0, pt, maxEt, 0.5);
                        else if(Math.abs(compositeParticles.getPdgId(list))==5)
                            ALegoDraw.drawBox(window, ag, phibin, etabin, AProjectionLegoPlot.DRAW_BJET, 0, pt, maxEt, 0.5);
                        else if(Math.abs(compositeParticles.getPdgId(list))==15)
                            ALegoDraw.drawBox(window, ag, phibin, etabin, AProjectionLegoPlot.DRAW_TAUJET, 0, pt, maxEt, 0.5);
                        else if(compositeParticles.getTypeEV(list).equals("EVParticleJet"))
                            ALegoDraw.drawBox(window, ag, phibin, etabin, AProjectionLegoPlot.DRAW_JET, 0, pt, maxEt, 0.5);
                        else
                            ALegoDraw.drawBox(window, ag, phibin, etabin, AProjectionLegoPlot.DRAW_COMPOSITEPARTICLE, 0, pt, maxEt, 0.5);
                    }
                }
            }
        }//if doAOD
    }//drawHistograms

    public static void drawROIs(Stack ROIstack, AWindow window, AGraphics ag, AEvent event)
    {
        List hitsAndTracks = event.getHitsAndTracks(window.getProjection());
        //loop over ROIs
        while(ROIstack.size()>0)
        {
            int h = (Integer) ROIstack.pop();
            AData datatype = ((AData) hitsAndTracks.get(h));

            byte[] ROIColor;
            double deltaPhi;
            double deltaEta;
            AROIData ROIData = (AROIData) datatype;
            ROIData.makeDrawList();
            ROIColor = datatype.getColor();
            deltaEta = parameterStore.get(datatype.getName(), "deta").getD();
            deltaPhi = parameterStore.get(datatype.getName(), "dphi").getD();
            // deltaEta and deltaPhi should not be too large
            for (int j = 0; j < ROIData.getNumDraw(); ++j)
            {
                int numPoints = 4;
                int list = ROIData.getDrawIndex(j);
                double ROIphi = ROIData.getPhi(list);
                double ROIeta = ROIData.getEta(list);
                if (AProjectionLegoPlot.reverse)
                    ROIeta = -ROIeta;
                double leftPhi = ROIphi - deltaPhi;
                double rightPhi = ROIphi + deltaPhi;
                double topEta = ROIeta + deltaEta;
                double bottomEta = ROIeta - deltaEta;
                if ((ROIColor[list] < AProjectionLegoPlot.defaultColorMap.length) && (ROIColor[list] >= 0))
                    ag.setColor(AProjectionLegoPlot.defaultColorMap[ROIColor[list]]);
                ag.setLineWidth(3);
                double ROIx = 0.0;
                double ROIy = 0.0;
                double[][] hv = new double[2][numPoints];
                if ((rightPhi <= AMath.TWO_PI) && (leftPhi >= 0.0))
                {
                    for (int p = 0; p < numPoints; p++)
                    {
                        switch (p)
                        {
                        case 0:
                            ROIx = rightPhi * 360.0 / AMath.TWO_PI;
                            ROIy = bottomEta;
                            break;
                        case 1:
                            ROIx = leftPhi * 360.0 / AMath.TWO_PI;
                            ROIy = bottomEta;
                            break;
                        case 2:
                            ROIx = leftPhi * 360.0 / AMath.TWO_PI;
                            ROIy = topEta;
                            break;
                        case 3:
                            ROIx = rightPhi * 360.0 / AMath.TWO_PI;
                            ROIy = topEta;
                            break;
                        }
                        if(ROIy <= -5.0)
                            ROIy = -5.0;
                        if(ROIy >= 5.0)
                            ROIy = 5.0;
                        //move phi value due to slant of eta axis
                        ROIx=AProjectionLegoPlot.adjustPhi(window,ROIx,ROIy);
                        hv[0][p] = ROIx;
                        hv[1][p] = ROIy;
                    }
                }
                else if ((rightPhi > AMath.TWO_PI) && (leftPhi < 0.0))
                {
                    for (int p = 0; p < numPoints; p++)
                    {
                        switch (p)
                        {
                        case 0:
                            ROIx = 360.0;
                            ROIy = bottomEta;
                            break;
                        case 1:
                            ROIx = 0.0;
                            ROIy = bottomEta;
                            break;
                        case 2:
                            ROIx = 0.0;
                            ROIy = topEta;
                            break;
                        case 3:
                            ROIx = 360.0;
                            ROIy = topEta;
                            break;
                        }
                        if(ROIy <= -5.0)
                            ROIy = -5.0;
                        if(ROIy >= 5.0)
                            ROIy = 5.0;
                        //move phi value due to slant of eta axis
                        ROIx=AProjectionLegoPlot.adjustPhi(window,ROIx,ROIy);
                        hv[0][p] = ROIx;
                        hv[1][p] = ROIy;
                    }
                }
                else if (rightPhi > AMath.TWO_PI)
                {
                    double[][] hvWrap = new double[2][numPoints];
                    double ROIxWrap = 0.0;
                    for (int p = 0; p < numPoints; p++)
                    {
                        switch (p)
                        {
                        case 0:
                            ROIx = 360.0;
                            ROIxWrap = (rightPhi - AMath.TWO_PI) * 360.0 / AMath.TWO_PI;
                            ROIy = bottomEta;
                            break;
                        case 1:
                            ROIx = leftPhi * 360.0 / AMath.TWO_PI;
                            ROIxWrap = 0.0;
                            ROIy = bottomEta;
                            break;
                        case 2:
                            ROIx = leftPhi * 360.0 / AMath.TWO_PI;
                            ROIxWrap = 0.0;
                            ROIy = topEta;
                            break;
                        case 3:
                            ROIx = 360.0;
                            ROIxWrap = (rightPhi - AMath.TWO_PI) * 360.0 / AMath.TWO_PI;
                            ROIy = topEta;
                            break;
                        }
                        if(ROIy <= -5.0)
                            ROIy = -5.0;
                        if(ROIy >= 5.0)
                            ROIy = 5.0;
                        //move phi value due to slant of eta axis
                        ROIx=AProjectionLegoPlot.adjustPhi(window,ROIx,ROIy);
                        hv[0][p] = ROIx;
                        hv[1][p] = ROIy;
                        //move phi value due to slant of eta axis
                        ROIxWrap=AProjectionLegoPlot.adjustPhi(window,ROIxWrap,ROIy);
                        hvWrap[0][p] = ROIxWrap;
                        hvWrap[1][p] = ROIy;
                    }
                    Point2D.Double p= new Point2D.Double(0,0);
                    for(int i=0; i<numPoints; i++)
                    {
                        p = window.calculateDisplay(hvWrap[0][i],hvWrap[1][i]);
                        hvWrap[0][i]=p.x;
                        hvWrap[1][i]=p.y;
                    }
                    ag.fillPolygon(hvWrap[0], hvWrap[1], numPoints);
                }
                else
                {
                    double[][] hvWrap = new double[2][numPoints];
                    double ROIxWrap = 0.0;
                    for (int p = 0; p < numPoints; p++)
                    {
                        switch (p)
                        {
                        case 0:
                            ROIx = rightPhi * 360.0  / AMath.TWO_PI;
                            ROIxWrap = 360.0;
                            ROIy = bottomEta;
                            break;
                        case 1:
                            ROIx = 0.0;
                            ROIxWrap = (leftPhi + AMath.TWO_PI) * 360.0 / AMath.TWO_PI;
                            ROIy = bottomEta;
                            break;
                        case 2:
                            ROIx = 0.0;
                            ROIxWrap = (leftPhi + AMath.TWO_PI) * 360.0 / AMath.TWO_PI;
                            ROIy = topEta;
                            break;
                        case 3:
                            ROIx = rightPhi * 360.0 / AMath.TWO_PI;
                            ROIxWrap = 360.0;
                            ROIy = topEta;
                            break;
                        }
                        if(ROIy <= -5.0)
                            ROIy = -5.0;
                        if(ROIy >= 5.0)
                            ROIy = 5.0;
                        //move phi value due to slant of eta axis
                        ROIx=AProjectionLegoPlot.adjustPhi(window,ROIx,ROIy);
                        hv[0][p] = ROIx;
                        hv[1][p] = ROIy;
                        //move phi value due to slant of eta axis
                        ROIxWrap=AProjectionLegoPlot.adjustPhi(window,ROIxWrap,ROIy);
                        hvWrap[0][p] = ROIxWrap;
                        hvWrap[1][p] = ROIy;
                    }
                    Point2D.Double p= new Point2D.Double(0,0);
                    for(int i=0; i<numPoints; i++)
                    {
                        p = window.calculateDisplay(hvWrap[0][i],hvWrap[1][i]);
                        hvWrap[0][i]=p.x;
                        hvWrap[1][i]=p.y;
                    }
                    ag.fillPolygon(hvWrap[0], hvWrap[1], numPoints);
                }
                Point2D.Double p= new Point2D.Double(0,0);
                for(int i=0; i<numPoints; i++)
                {
                    p = window.calculateDisplay(hv[0][i],hv[1][i]);
                    hv[0][i]=p.x;
                    hv[1][i]=p.y;
                }
                ag.fillPolygon(hv[0], hv[1], numPoints);
            }//end of loop over ROI data
        }

    }
}
