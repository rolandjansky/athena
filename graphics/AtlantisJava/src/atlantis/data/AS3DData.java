package atlantis.data;

import atlantis.event.AAssociation;
import atlantis.event.AData;
import atlantis.event.AEvent;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.parameters.AParameter;
import atlantis.parameters.AParameterUtilities;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;


/*
 * The input array clusters is being used to contain two types of information
 * for pixel clusters
 *      clusters[i][0] = identifier
 *      clusters[i][1] = -1
 * for space points from 2 strip clusters it contains the identifiers of the
 * individual strips
 *      clusters[i][0] = identifier of first strip
 *      clusters[i][1] = identifier of second strip
 */
public class AS3DData extends A3DPointData
{
    private int[][] clusters = null;
    // read from the event file
    protected int[] layer = null;
    private int[] etaModule = null;
    private int[] phiModule = null;

    // for use by the filter loop, group[] is filled in by hit filter, other
    // classes contain group functionality as well but currently (2006/08)
    // hit filter takes into account only S3D (SiSpacePoint) data
    protected int[] group = null;
    private double[] eta = null;

    private boolean[] pixel = null;
    public static final int PIXEL = -1;
    public static final int CUT_INDET_SCT_ALL = -1;
    public static final int CUT_INDET_SCT_EC_NEG = 0;
    public static final int CUT_INDET_SCT_BARREL = 1;
    public static final int CUT_INDET_SCT_EC_POS = 2;



    public String getParameterGroup()
    {
        return "S3D";
    }


    public String getName()
    {
        return "S3D";
    }


    public String getNameScreenName()
    {
        return "SpacePoint";
    }


    public String getHitInfo(int index)
    {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0) return getNameScreenName()+" index: " + index+"\n"+
        AMath.ETA+" = "+String.format("%.3f",eta[index])+"\n "+
        AMath.PHI+" = "+String.format("%.3f",Math.toDegrees(phi[index]))+AMath.DEGREES;
        
        String clId = clusters[index][0] + ", " + clusters[index][1];
        String type = pixel[index]  ? " (Pixel hit)" : " (SCT hit)";

        String temp = getNameScreenName() + " (id: " + id[index] + " index: " + index + ")" +
                      type +
                      "\n cluster identifiers: " + clId +
                      "\n x = " + String.format("%.3f",x[index]) + " cm" +
                      "\n y = " + String.format("%.3f",y[index]) + " cm" +
                      "\n z = " + String.format("%.3f",z[index]) + " cm" +
                      "\n " + AMath.RHO + " = " + String.format("%.3f",rho[index]) +
                      " cm" +
                      "\n " + AMath.PHI + " = " +
                      String.format("%.3f",Math.toDegrees(phi[index])) +
                      AMath.DEGREES + "  (" + String.format("%.3f",phi[index]) + " rad)" +
                      "\n z = " + String.format("%.3f",z[index]) + " cm" +
                      "\n group = " + group[index];
        temp += "\n " + AMath.ETA + " module = ";
        temp += (etaModule != null) ? Integer.toString(etaModule[index]) : "n/a";
        temp += "\n " + AMath.PHI + " module = ";
        temp += (phiModule != null) ? Integer.toString(phiModule[index]) : "n/a";
        temp += super.getHitInfo(index); // finds barcode information

        return temp;

    } // getHitInfo() ---------------------------------------------------------



    AS3DData(AHashMap p, AEvent e)
    {
        super(p,e);

        etaModule = (p.get("etaModule") != null) ? p.getIntArray("etaModule") : null;
        phiModule = (p.get("phiModule") != null) ? p.getIntArray("phiModule") : null;

        // use this for now as pixel id have bug i.e. sometimes > 0
        layer = p.getUnknownIntArray("layer");
        if(p.get("layer") == null || p.get("sub") == null)
        {
            calculateLayerSubdetector();
        }
        group = new int[numData];
        eta = new double[numData];

        if(p.get("clusters") != null)
        {
            // can only distinguish between SCT and Pixel hits if clusters
            // subtag is available
            pixel = new boolean[numData];
            // save IDs from event file to be available with pick info
            clusters = new int[numData][];
            int[][] assocClusters = new int[numData][];
            int[] temp = p.getIntArray("clusters");
            for(int i = 0; i < numData; i++)
            {
                // save all clusters IDs
                clusters[i] = new int[] { temp[2 * i], temp[2 * i + 1] };

                // check if the second number, PIXEL (-1) designates pixel hit
                if(temp[2 * i + 1] != PIXEL)
                {
                    // this is SCT spacepoint - has got two clusters
                    assocClusters[i] = new int[] { temp[2 * i], temp[2 * i + 1] };
                }
                else
                {
                    // this is a pixel spacepoint (second cluster number is -1)
                    pixel[i] = true;

                    // to ignore associtiation between pixel clusters and spacepoints:
                    // assocClusters[i] = null;

                    // taking pixel cluster into account (pixel spacepoint has got
                    // only one cluster) (where as SCT spacepoint has got two clusters)
                    assocClusters[i] = new int[] { temp[2 * i] };
                }
            }
            event.getAssociationManager().add(new AAssociation(getFullName(), "SiCluster", assocClusters, event));
            event.getAssociationManager().add(new AAssociation(getFullName(), "PixelCluster", assocClusters, event));
        }
    }



   protected void finalizeConstruction()
   {
       super.finalizeConstruction();
       super.calculateAssociationViaClusters();

   } // finalizeConstruction() ----------------------------------------------



    protected int internalColor()
    {
        int numColorTypes = super.internalColor();
        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();
        if(colorFunction == numColorTypes + 1)
        {
            colorByGroup();
        }
        else if(colorFunction == numColorTypes + 2)
        {
            colorBy(layer);
        }
        return numColorTypes + 2;

    } // internalColor() ----------------------------------------------------


    protected void colorByGroup()
    {
        byte ungroupedColor;
        if(parameterStore.get(PARAMETER_GROUP, "Ungrouped").getStatus())
            ungroupedColor = (byte) parameterStore.get(PARAMETER_GROUP, "Ungrouped").getI();
        else
            ungroupedColor = (byte) parameterStore.get(PARAMETER_GROUP, "Unconnected").
                             getI();
        int numColors = (byte) parameterStore.get("HitColors", "Number").getI();
        numColors = Math.min(7, numColors);
        int[] col = parameterStore.getArray("HitColors", "C1", numColors);
        for(int i = 0; i < numData; i++)
        {
            if(group[i] == 0)
                color[i] = ungroupedColor;
            else
                color[i] = (byte) col[(group[i] - 1) % numColors];
        }
    }



    /**
     * define noise and good hits type = 0 means noise
     * noise hits - hits associated neither with Track not STr
     * good hits - hits associated either with Track or with STr plus
     *   group information - set by the AFilter (hit filter)
     */
    protected void setType()
    {
        int[][] assocSTr = event.getAssociationManager().get(getFullName(), "STr");
        int[][] assocRTr = event.getAssociationManager().get(getFullName(), getReconstructedTracks());
        for(int i = 0; i < numData; i++)
        {
            if((assocSTr != null && assocSTr[i] != null) ||
               (assocRTr != null && assocRTr[i] != null) || group[i] > 0)
            {
                type[i] = 1;
            }
            else
            {
                type[i] = 0;
            }
        }

    } // setType() ----------------------------------------------------------



    protected void applyCuts()
    {
        cutIndex();
        cutSubdetector();
        cut("CutsInDet", "Layer", " Layer", layer);
        cutPhi(phi);
        cutEta(rho, z);
        cutSimulatedTracks();
        cutReconstructedTracks();
        cut("CutsInDet", "Group", " Group", group);
        if(etaModule != null)
        {
            cut("CutsInDet", "EtaModule", " EtaModule", etaModule);
        }
        if(phiModule != null)
        {
            cut("CutsInDet", "PhiModule", " PhiModule", phiModule);
        }
    }

    private void cutSubdetector()
    {
        AParameter subPar = parameterStore.get("CutsInDet", "SCT");
        if(subPar.getI() != -1)
            cutArray(sub, subPar.getI(), "Barrel/Endcap");
    }


    public int getLayer(int id)
    {
        int layer;
        if(id > -1)
            layer = (id >> 21) & 0xF;
        else
            layer = (id >> 27) & 0x3;
        int sub = getSub(id);
        if(id > -1)
        {
            // strips
            if(sub == ASiClusterData.BARREL)
                layer += 3;
            else
                layer += 11;
        }
        else
        {
            // pixels
            if(sub != ASiClusterData.BARREL)
                layer += 7;
        }
        return layer;
    }


    public int getSub(int id)
    {
        if(id > -1)
            return(id >> 25) & 0x3;
        else
            return(id >> 29) & 0x3;
    }


    private void calculateLayerSubdetector()
    {
        final double rhoPixelMax = 24.;
        final double zBarrelPixelMax = 41.;
        final double zBarrelStripsMax = 78.;
        final double[] zEndcapPixel =
                                      {51., 64., 90.};
        final double[] zEndcapStrips =
                                       {88., 98., 116., 134., 156., 190., 233.,
                                       263.};
        final double[] rhoBarrel =
                                   {6.74, 11.5, 23., 34., 40., 48.};
        for(int i = 0; i < numData; i++)
        {
            double z = Math.abs(this.z[i]);
            double r = rho[i];
            if(r < rhoPixelMax)
            {
                // .............................................. pixels
                if(z < zBarrelPixelMax)
                {
                    // ............................................ barrel
                    sub[i] = 1;
                    for(int l = 0; l < rhoBarrel.length; l++)
                        if(r < rhoBarrel[l])
                        {
                            layer[i] = l;
                            break;
                        }
                }
                else
                {
                    // ............................................ end cap
                    layer[i] = 10;
                    for(int l = 0; l < zEndcapPixel.length; l++)
                    {
                        if(z < zEndcapPixel[l])
                        {
                            layer[i] = 7 + l;
                            break;
                        }
                    }
                    if(this.z[i] < 0)
                        sub[i] = 0;
                    else
                        sub[i] = 2;
                }
            }
            else
            {
                // .............................................. strips

                if(z < zBarrelStripsMax)
                {
                    // ............................................ BARREL
                    sub[i] = 1;
                    layer[i] = 6;
                    for(int l = 0; l < rhoBarrel.length; l++)
                        if(r < rhoBarrel[l])
                        {
                            layer[i] = l;
                            break;
                        }
                }
                else
                {
                    // ............................................ end cap
                    layer[i] = 19;
                    for(int l = 0; l < zEndcapStrips.length; l++)
                    {
                        if(z < zEndcapStrips[l])
                        {
                            layer[i] = 11 + l;
                            break;
                        }
                    }
                    if(this.z[i] < 0)
                        sub[i] = 0;
                    else
                        sub[i] = 2;
                }
            }
        }
    }


    // drawlist to be used as input to the filter
    public int makeFilterDrawList(double etaRange)
    {
        calculateRhoPhi();

        // overwrite the input array to improve speed
        numDraw = 0;
        for(int i = 0; i < numData; i++)
        {
            double eta = AParameterUtilities.eta(z[i], rho[i]);
            this.eta[i] = eta;
            if(eta > -etaRange && eta < etaRange)
            {
                listdl[numDraw++] = i;
            }
        }

        return numDraw;
    }


    public int[] getIntegerEta(int numBins, double etaRange, int[] integerEta)
    {
        // overwrite the input array to improve speed
        double binWidth = 2 * etaRange / numBins;
        double etaRangeOptimised = etaRange + binWidth;
        for(int i = 0; i < numDraw; i++)
            // implicit (int) conversion only does correct thing if positive
            integerEta[i] = (int) ((eta[listdl[i]] + etaRangeOptimised) /
                                   binWidth);
        return integerEta;
    }


    public int[] getIntegerPhi(int numBins, double skew, int[] integerPhi)
    {
        calculateRhoPhi();
        // overwrite the input array to improve speed
        double binWidth = 2 * Math.PI / numBins;
        for(int i = 0; i < numDraw; i++)
        {
            // can only be positive
            // add one to treat wraparound efficiently
            double p = phi[listdl[i]] - skew * rho[listdl[i]];
            if(p < 0.)
                p += 2 * Math.PI;
            else if(p > 2 * Math.PI)
                p -= 2 * Math.PI;
            integerPhi[i] = (int) (p / binWidth) + 1;
        }
        return integerPhi;
    }


    public int[] getLayer(int[] layer)
    {
        for(int i = 0; i < numDraw; i++)
            layer[i] = this.layer[listdl[i]];
        return layer;
    }


    public void setGroup(int[] group)
    {
        for(int i = 0; i < numData; i++)
            this.group[i] = 0;
        for(int i = 0; i < numDraw; i++)
            this.group[listdl[i]] = group[i];
    }


    protected ACoord getFRUser()
    {
        if(!parameterStore.get("SiCluster", "Stereo").getStatus())return super.getFRUser();
        makeDrawList();
        double[] h = new double[numDraw];
        double[] v = new double[numDraw];
        int[] index = new int[numDraw];
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            h[i] = rho[list];
            v[i] = Math.toDegrees(AParameterUtilities.getPhiStereo(rho[list], phi[list], z[list]));
            index[i] = list;
        }
        return new ACoord(h, v, index, this).includePhiWrapAround("FR");
    }


    protected ACoord getFZUser()
    {
        if(!parameterStore.get("SiCluster", "Stereo").getStatus())return super.getFZUser();
        makeDrawList();
        double[] h = new double[numDraw];
        double[] v = new double[numDraw];
        int[] index = new int[numDraw];
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            h[i] = z[list];
            v[i] = Math.toDegrees(AParameterUtilities.getPhiStereo(rho[list], phi[list], z[list]));
            index[i] = list;
        }
        return new ACoord(h, v, index, this).includePhiWrapAround("FZ");
    }

} // class AS3DData =========================================================
