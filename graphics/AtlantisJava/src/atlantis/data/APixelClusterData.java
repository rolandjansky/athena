package atlantis.data;

import atlantis.event.*;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.parameters.AParameter;
import atlantis.parameters.AParameterUtilities;
import atlantis.projection.AProjectionVP;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;

/**
 * PixelCluster - Pixel clusters, as SiCluster but for Pixels
 *  reads in PixelCluster datatype
 */
public class APixelClusterData extends AHitData
{
    // read in from event file
    private float[] x;
    private float[] y;
    private float[] z;
    private float[] widthx;
    private float[] widthy;
    private int[] etaModule;
    private int[] phiModule;
    private float[] eloss;

    // calculated
    private float[][] rho;
    private float[][] phi;
    private int[] layer;

    public static final int U_CLUSTER = -1;
    public static final int PHI_CLUSTER = 0;
    public static final int V_CLUSTER = 1;
    public static final int ENDCAP_MINUS = 0;
    public static final int BARREL = 1;
    public static final int ENDCAP_PLUS = 2;


    public String getParameterGroup()
    {
        return "PixelCluster";
    }

    public String getName()
    {
        return "PixelCluster";
    }

    public String getNameScreenName()
    {
        return "PixelCluster";
    }

    public float[] getX()
    {
        return x;
    }

    public float[] getY()
    {
        return y;
    }

    public float[] getZ()
    {
        return z;
    }

    APixelClusterData(AHashMap p, AEvent e)
    {
        super(p,e);
        x = p.getFloatArray("x0");
        y = p.getFloatArray("y0");
        z = p.getFloatArray("z0");
        widthx = p.getFloatArray("widthx");
        widthy = p.getFloatArray("widthy");
        eloss = p.getFloatArray("eloss");
        
        rho = new float[2][numData];
        phi = new float[2][numData];

        layer = new int[numData];
        for (int i = 0; i < numData; ++i)
        {
            sub[i] = getSub(id[i]);
            layer[i] = getLayer(id[i]);
        }

        etaModule = (p.get("etaModule") != null) ? p.getIntArray("etaModule") : null;
        phiModule = (p.get("phiModule") != null) ? p.getIntArray("phiModule") : null;

    } // APixelClusterData() ---------------------------------------------------



    protected void calculateRhoPhi()
    {
        calculateRhoPhi(x, y, rho[0], phi[0]);
        // treat wraparound
        for (int i = 0; i < numData; ++i)
        {
        	if (Math.abs(0 - phi[0][i]) > Math.PI)
            {
                if (0 - phi[0][i] > 0)
                {
                    phi[0][i] += AMath.TWO_PI;
                }
            }
        } // for
    } // calculateRhoPhi() --------------------------------------------------

    private void cutSubdetector()
    {
        AParameter par = parameterStore.get("CutsInDet", "SCT");
        if (par.getI() != -1)
        {
            cutArray(sub, par.getI(), "Barrel/Endcap");
        }
    } // cutSubdetector() ---------------------------------------------------


    protected void applyCuts()
    {
        cutIndex();
        cutOrientation();
        cutSubdetector();
        cut("CutsInDet", "Layer", "Layer", layer);
        cutSimulatedTracks();
        cutReconstructedTracks();
        cutPhi(phi[0]);
        cutEta();
    } // applyCuts() --------------------------------------------------------

    private void cutEta()
    {
        AParameter par = parameterStore.get("CutsATLAS", "CutEta");
        if (!par.getStatus())
            return;
        double etaCut = par.getD();
        double etaMid = parameterStore.get("CutsATLAS", "EtaMiddle").getD();
        double etaLowerCut = etaMid - etaCut;
        double etaUpperCut = etaMid + etaCut;
        int num = 0;
        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            double eta1 = AParameterUtilities.eta(z[list], rho[0][list]);
            if (eta1 > etaLowerCut && eta1 < etaUpperCut)
                listdl[num++] = list;
        }
        numDraw = num;
    } // cutEta() -----------------------------------------------------------

    public int getLayer(int id)
    {
        int layer = (id >> 27) & 0x3;
        int sub = getSub(id);
        if (sub != BARREL)
        {
            layer += 7;
        }
        return layer;
    } // getLayer() ---------------------------------------------------------

    public int getSub(int id)
    {
    	// bitwise operation to determine cluster position from id (1,0,2 -> endcap-,barrel,endcap+)
    	return(id >> 29) & 0x3;
    } // getSub() -----------------------------------------------------------

    public int getOrientation(int id)
    {
        int side = (id & 0x400) >> 10;
        int sub = getSub(id);
        if ((side == 1 && sub == BARREL) || (side == 0 && sub != BARREL))
        {
            return PHI_CLUSTER;
        }

        int layer = getLayer(id);
        if (layer % 2 == 1)
        {
            return U_CLUSTER;
        }

        return V_CLUSTER;
    } // getOrientation() ---------------------------------------------------

    protected void cutOrientation()
    {
        int num = 0;
        AParameter stereoAnglePar = parameterStore.get("PixelCluster", "Stereo");
        if (!stereoAnglePar.getStatus())
        {
            return;
        }
        int orientation = stereoAnglePar.getI();
        for (int i = 0; i < numDraw; i++)
        {
            if (orientation == getOrientation(id[listdl[i]]))
            {
                listdl[num++] = listdl[i];
            }
        }
        numDraw = num;
    } // cutOrientation() ---------------------------------------------------

    protected int internalColor()
    {
        int numColorTypes = super.internalColor();
        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();
        if (colorFunction == numColorTypes + 1)
        {
            colorBy(layer);
        }
        else if (colorFunction == numColorTypes + 2)
        {
            colorByOrientation();
        }
        else if (colorFunction == numColorTypes + 3)
        {
        	colorByEloss();
        }

        return numColorTypes + 2;
    } // internalColor() ----------------------------------------------------

    protected void colorByOrientation()
    {
        int numColors = parameterStore.get("HitColors", "Number").getI();
        numColors = Math.min(7, numColors);
        int[] col = parameterStore.getArray("HitColors", "C1", numColors);
        for (int i = 0; i < numData; i++)
        {
            color[i] = (byte) col[getOrientation(id[i]) + 1 % numColors];
        }
    } // colorByOrientation() -----------------------------------------------

    protected void colorByEloss()
    {
    	// This method currently uses the uncorrected value for eloss as an approximation to dE/dx
    	// Need to either loop over one track collection for associated tracks in Atlantis, or do this in JiveXML..
    	
    	int numColors = parameterStore.get("HitColors", "Number").getI();
        numColors = Math.min(7, numColors);
        int[] col = parameterStore.getArray("HitColors", "C1", numColors);
        for (int i = 0; i < numData; i++)
        {
        	if(eloss[i] < 0.5)
        	{
        		color[i] = (byte) col[1 + 1 % numColors]; // blue
        	}
        	else if(eloss[i] >=0.5 && eloss[i] < 2)
        	{
        		color[i] = (byte) col[5 + 1 % numColors]; // yellow
        	}
        	else if(eloss[i] >=2)
        	{
        		color[i] = (byte) col[2 + 1 % numColors]; // red/orange
        	}
        }
    } // colorByEloss() -----------------------------------------------------
    
    public String getHitInfo(int index)
    {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0) return getNameScreenName()+" index: " + index;
        
        String temp = getNameScreenName() + " (id: " + id[index] + " index: " + index +")\n" +
                      " orientation = " + getOrientation(id[index]);
        temp += "\n x = " + x[index];
        temp += "\n y = " + y[index];
        temp += "\n z = " + z[index];
        temp += "\n " + AMath.RHO + " = " + rho[0][index] + "  " + rho[1][index];
        temp += "\n " + AMath.ETA + " module = ";
        temp += (etaModule != null) ? Integer.toString(etaModule[index]) : "n/a";
        temp += "\n " + AMath.PHI + " module = ";
        temp += (phiModule != null) ? Integer.toString(phiModule[index]) : "n/a";
        temp += "\n widthx = ";
        temp += (widthx != null) ? Float.toString(widthx[index]) : "n/a";
        temp += "\n widthy = ";
        temp += (widthy != null) ? Float.toString(widthx[index]) : "n/a";
        temp += "\n eloss = ";
        temp += (eloss != null) ? Float.toString(eloss[index]) : "n/a";

        temp += super.getHitInfo(index); // finds barcode information

        return temp;
    } // getHitInfo() -------------------------------------------------------

    protected ACoord getYXUser()
    {
        makeDrawList();
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            for (int j = 0; j < 1; j++)
            {
                hv[0][j][i] = x[list];
                hv[1][j][i] = y[list];
            }
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.SYMBOLS);
    } // getYXUser() --------------------------------------------------------

    protected ACoord getRZUser()
    {
        makeDrawList();
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        double phiMid = Math.toRadians(parameterStore.get("RZ", "Phi").getD());
        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            double phiDiff = Math.abs(phi[0][list] - phiMid);
            double sign;
            if (phiDiff < Math.PI / 2. || phiDiff > 3 * Math.PI / 2.)
            {
                sign = +1.;
            }
            else
            {
                sign = -1.;
            }

            for (int j = 0; j < 1; j++)
            {
                hv[0][j][i] = z[list];
                hv[1][j][i] = sign * rho[j][list];
            }
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.SYMBOLS);
    } // getRZUser() --------------------------------------------------------

    protected ACoord getFRUser()
    {
        makeDrawList();
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            for (int j = 0; j < 1; j++)
            {
                hv[0][j][i] = rho[j][list];
                hv[1][j][i] = Math.toDegrees(AParameterUtilities.getPhiStereo(rho[j][list], phi[j][list], z[list]));
            }
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.SYMBOLS).includePhiWrapAround("FR");
    } // getFRUser() --------------------------------------------------------

    protected ACoord getFZUser()
    {
        makeDrawList();
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            for (int j = 0; j < 1; j++)
            {
                hv[0][j][i] = z[list];
                hv[1][j][i] = Math.toDegrees(AParameterUtilities.getPhiStereo(rho[j][list], phi[j][list], z[list]));
            }
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.SYMBOLS).includePhiWrapAround("FZ");
    } // getFZUser() --------------------------------------------------------

    protected ACoord getVPUser()
    {
        makeDrawList();
        int numTotal = 2 * numDraw;
        double[][][] hv = new double[2][numTotal][2];
        int[] index = new int[numTotal];
        double[] sign = new double[] { -1., 1. };
        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];

                double deltaEta = AProjectionVP.getDeltaEta(rho[0][list], z[list]);
                double eta = AParameterUtilities.eta(z[list], rho[0][list]);
                for (int j = 0; j < 2; j++)
                {
                	// pixels displayed halfway in phi and eta, for some reason
                	// quick fix = multiply by 2
                    hv[0][2 * i + j][0] = 2*(eta + sign[j] * deltaEta);
                    hv[1][2 * i + j][0] = 2*Math.toDegrees(phi[0][list]);
                    index[2 * i + j] = list;
                }
            
        }
        return new ACoord(hv, index, this, ACoord.POLYLINES).includePhiWrapAround("VP");
    } // getVPUser() --------------------------------------------------------

} // class APixelClusterData ===================================================
