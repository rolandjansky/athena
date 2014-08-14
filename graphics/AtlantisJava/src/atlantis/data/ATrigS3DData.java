package atlantis.data;

import atlantis.event.*;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.parameters.AParameter;
import atlantis.parameters.AParameterUtilities;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;


/**
 *
 * Trigger SpacePoints
 * 
 * barcodes + numBarcodes - associations with Truth (STr)
 * clusters - associations with SiClusters and over it with reconstructed
 *   tracks (Track)
 *   
 * cleanup notes:
 * methods makeFilterDrawList(), getIntegerEta(),  getIntegerPhi() were removed
 * as these were only used by AFilter in S3D (this class was obviously the
 * origin for the others) but AFilter doesn't work with TrigS3D
 * other also unused methods are getLayer() and getSub() - what are these
 * used for?
 *
 */
public class ATrigS3DData extends A3DPointData
{
    private int[][] clusters = null;

    // data from the event file
    protected int[] layer = null;

    private boolean[] pixel = null;
    public static final int PIXEL = -1;



    public String getParameterGroup()
    {
        return "TrigS3D";
    }



    public String getName()
    {
        return "TrigS3D";
    }



    public String getNameScreenName()
    {
        return "TrigSiSpacePoint";
    }



    public String getHitInfo(int index)
    {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0) return getNameScreenName()+" index: " + index+"\n"+
        AMath.RHO+" = "+String.format("%.3f",rho[index])+"\n "+
        AMath.PHI+" = "+String.format("%.3f",Math.toDegrees(phi[index]))+AMath.DEGREES;
        
        String clId = " (" + clusters[index][0] + ", " +
                             clusters[index][1] + ")";
        String type = pixel[index]  ? " (Pixel)" : " (SCT)";

        String r = "";
        r += getNameScreenName() + " (id: " + id[index] + " index: " + index + ")" +
            clId + type + "\n " +
            "x = " + String.format("%.3f",x[index]) + " cm\n " +
            "y = " + String.format("%.3f",y[index]) + " cm\n " +
            "z = " + String.format("%.3f",z[index]) + " cm\n " +
            AMath.RHO + " = " + String.format("%.3f",rho[index]) + " cm\n " +
            AMath.PHI + " = " +
            String.format("%.3f",Math.toDegrees(phi[index])) +
            AMath.DEGREES +  "  (" + String.format("%.3f",phi[index]) + " rad)\n " +
            " layer = " + layer[index];
        r += super.getHitInfo(index); // finds barcode information

        return r;

    } // getHitInfo() -------------------------------------------------------



    ATrigS3DData(AHashMap p, AEvent e)
    {
        super(p,e);
        layer = p.getUnknownIntArray("layer");

        if(p.get("clusters") != null)
        {
            // can only distinguish between SCT and Pixel hits if clusters
            // subtag is available
            pixel = new boolean[numData];
            // save IDs from event file to be availeble with pick info
            clusters = new int[numData][];

            int[][] assocClusters = new int[numData][];
            int[] temp = p.getIntArray("clusters");
            for(int i = 0; i < numData; ++i)
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
            AAssociation assoc = new AAssociation(getName(), "SiCluster",
                                                  assocClusters,event);
            event.getAssociationManager().add(assoc);
        }

    } // ATrigS3DData() -----------------------------------------------------



    protected void finalizeConstruction()
    {
        super.finalizeConstruction();
        super.calculateAssociationViaClusters();

    } // finalizeConstruction() ---------------------------------------------



    protected int internalColor()
    {
        int numColorTypes = super.internalColor();
        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();
        if(colorFunction == numColorTypes + 1)
        {
            colorBy(layer);
        }

        return numColorTypes + 1;

    } // internalColor() ----------------------------------------------------


    protected void applyCuts()
    {
        cutIndex();
        cutSubdetector();
        cut("CutsInDet", "Layer", "Layer", layer);
        cutPhi(phi);
        cutEta(rho, z);
        cutSimulatedTracks();
        cutReconstructedTracks();

    } // applyCuts() --------------------------------------------------------


    private void cutSubdetector()
    {
        AParameter subPar = parameterStore.get("CutsInDet", "SCT");
        if(subPar.getI() != -1)
        {
            cutArray(sub, subPar.getI(), "Barrel/Endcap");
        }

    } // cutSubdetector() ---------------------------------------------------



    public int[] getLayer(int[] layer)
    {
        for(int i = 0; i < numDraw; i++)
        {
            layer[i] = this.layer[listdl[i]];
        }
        return layer;

    } // getLayer() ---------------------------------------------------------



    public int getSub(int id)
    {
        if(id > -1)
        {
            return(id >> 25) & 0x3;
        }
        else
        {
            return(id >> 29) & 0x3;
        }

    } // getSub() -----------------------------------------------------------



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

    } // getFRUser() --------------------------------------------------------



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

    } // getFZUser() --------------------------------------------------------

} // class ATrigS3DData =====================================================
