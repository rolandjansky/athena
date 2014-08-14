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
 * SiCluster - Silicon clusters, reads in STC datatype from event file
 */
public class ASiClusterData extends AHitData
{
    // read in from event file
    private float[][] x;
    private float[][] y;
    private float[][] z;
    private float[] width;
    private int[] etaModule;
    private int[] phiModule;
    private int[] side;

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
        return "SiCluster";
    }

    public String getName()
    {
        return "SiCluster";
    }

    public String getNameScreenName()
    {
        return "SCT_Cluster";
    }

    public float[][] getX()
    {
        return x;
    }

    public float[][] getY()
    {
        return y;
    }

    public float[][] getZ()
    {
        return z;
    }



    ASiClusterData(AHashMap p, AEvent e)
    {
        super(p,e);
        x = new float[][] { p.getFloatArray("x0"), p.getFloatArray("x1") };
        y = new float[][] { p.getFloatArray("y0"), p.getFloatArray("y1") };
        z = new float[][] { p.getFloatArray("z0"), p.getFloatArray("z1") };
        width = p.getFloatArray("width");

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
        side = (p.get("side") != null) ? p.getIntArray("side") : null;

    } // ASiClusterData() ---------------------------------------------------



    protected void calculateRhoPhi()
    {

        calculateRhoPhi(x[0], y[0], rho[0], phi[0]);
        calculateRhoPhi(x[1], y[1], rho[1], phi[1]);
        // treat wraparound
        for (int i = 0; i < numData; ++i)
        {
            if (Math.abs(phi[1][i] - phi[0][i]) > Math.PI)
            {
                if (phi[1][i] - phi[0][i] > 0)
                {
                    phi[0][i] += AMath.TWO_PI;
                }
                else
                {
                    phi[1][i] += AMath.TWO_PI;
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
            double eta1 = AParameterUtilities.eta(z[0][list], rho[0][list]);
            double eta2 = AParameterUtilities.eta(z[1][list], rho[1][list]);
            double etaLower = Math.min(eta1, eta2);
            double etaUpper = Math.max(eta1, eta2);
            if (etaUpper > etaLowerCut && etaLower < etaUpperCut)
                listdl[num++] = list;
        }
        numDraw = num;
    } // cutEta() -----------------------------------------------------------

    public int getLayer(int id)
    {
        int layer = (id & 0x01e00000) >> 21;
        int sub = getSub(id);
        if (sub == BARREL)
        {
            layer += 3;
        }
        else
        {
            layer += 11;
        }
        return layer;
    } // getLayer() ---------------------------------------------------------

    public int getSub(int id)
    {
        return (id & 0x06000000) >> 25;
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
        AParameter stereoAnglePar = parameterStore.get("SiCluster", "Stereo");
        if (!stereoAnglePar.getStatus())
        {
            return;
        }
        int orientation = stereoAnglePar.getI();
        for (int i = 0; i < numDraw; i++)
        {
            // only works for barrel for now !!!!
            // if( getSub(id[listdl[i]])==BARREL &&
            // orientation==getOrientation(id[listdl[i]]))
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

    public String getHitInfo(int index)
    {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0) return getNameScreenName()+" index: " + index;
        
        String temp = getNameScreenName() + " (id: " + id[index] + " index: " + index +")\n" +
                      " orientation = " + getOrientation(id[index]);
        temp += "\n x = " + x[0][index] + "  " + x[1][index];
        temp += "\n y = " + y[0][index] + "  " + y[1][index];
        temp += "\n z = " + z[0][index] + "  " + z[1][index];
        temp += "\n " + AMath.RHO + " = " + rho[0][index] + "  " + rho[1][index];
        temp += "\n " + AMath.ETA + " module = ";
        temp += (etaModule != null) ? Integer.toString(etaModule[index]) : "n/a";
        temp += "\n " + AMath.PHI + " module = ";
        temp += (phiModule != null) ? Integer.toString(phiModule[index]) : "n/a";
        temp += "\n side = ";
        temp += (side != null) ? Integer.toString(side[index]) : "n/a";
        temp += "\n width = ";
        temp += (width != null) ? Float.toString(width[index]) : "n/a";

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
            for (int j = 0; j < 2; j++)
            {
                hv[0][j][i] = x[j][list];
                hv[1][j][i] = y[j][list];
            }
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.LINES);
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
            double phiDiff = Math.abs(phi[1][list] - phiMid);
            double sign;
            if (phiDiff < Math.PI / 2. || phiDiff > 3 * Math.PI / 2.)
            {
                sign = +1.;
            }
            else
            {
                sign = -1.;
            }

            for (int j = 0; j < 2; j++)
            {
                hv[0][j][i] = z[j][list];
                hv[1][j][i] = sign * rho[j][list];
            }
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.LINES);
    } // getRZUser() --------------------------------------------------------

    protected ACoord getFRUser()
    {
        makeDrawList();
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            for (int j = 0; j < 2; j++)
            {
                hv[0][j][i] = rho[j][list];
                hv[1][j][i] = Math.toDegrees(AParameterUtilities.getPhiStereo(rho[j][list], phi[j][list], z[j][list]));
            }
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.LINES).includePhiWrapAround("FR");
    } // getFRUser() --------------------------------------------------------

    protected ACoord getFZUser()
    {
        makeDrawList();
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            for (int j = 0; j < 2; j++)
            {
                hv[0][j][i] = z[j][list];
                hv[1][j][i] = Math.toDegrees(AParameterUtilities.getPhiStereo(rho[j][list], phi[j][list], z[j][list]));
            }
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.LINES).includePhiWrapAround("FZ");
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
            for (int k = 0; k < 2; k++)
            {
                double deltaEta = AProjectionVP.getDeltaEta(rho[k][list], z[k][list]);
                double eta = AParameterUtilities.eta(z[k][list], rho[k][list]);
                for (int j = 0; j < 2; j++)
                {
                    hv[0][2 * i + j][k] = eta + sign[j] * deltaEta;
                    hv[1][2 * i + j][k] = Math.toDegrees(phi[k][list]);
                    index[2 * i + j] = list;
                }
            }
        }
        return new ACoord(hv, index, this, ACoord.POLYLINES).includePhiWrapAround("VP");
    } // getVPUser() --------------------------------------------------------

} // class ASiClusterData ===================================================
