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
 * Silicon clusters RDO Raw Data Objects (class derived from ASiClusterData)
 * Parameters expected in the event file for SCTRDO datatype:
 * x0|x1|y0|y1|z0|z1|phiModule|etaModule|id|BCIDError|firstHitError|
 * formatterError|lvl1Error|preambleError|secondHitError|syncError|timeBin
 *
 */
public class ASiClusterRDOData extends AHitData
{
    private float[][] x = null;
    private float[][] y = null;
    private float[][] z = null;
    private int[] etaModule = null;
    private int[] phiModule = null;
    private int[] BCIDError = null;
    private int[] firstHitError = null;
    private int[] formatterError = null;
    private int[] lvl1Error = null;
    private int[] preambleError = null;
    private int[] secondHitError = null;
    private int[] syncError = null;
    private int[] timeBin = null;
    // calculated here
    private int[] layer;
    private float[][] rho;
    private float[][] phi;

    public static final int U_CLUSTER = -1;
    public static final int PHI_CLUSTER = 0;
    public static final int V_CLUSTER = 1;
    public static final int ENDCAP_MINUS = 0;
    public static final int BARREL = 1;
    public static final int ENDCAP_PLUS = 2;



    public String getParameterGroup()
    {
        return "SiClusterRDO";

    } // getParameterGroup() ------------------------------------------------



    public String getName()
    {
        return "SiClusterRDO";

    } // getName() ----------------------------------------------------------



    public String getNameScreenName()
    {
        return "SiClusterRDO";

    } // getNameScreenName() ------------------------------------------------



    ASiClusterRDOData(AHashMap p, AEvent e)
    {
        super(p,e);
        x = new float[][]
                        { p.getFloatArray("x0"), p.getFloatArray("x1") };
        y = new float[][]
                        { p.getFloatArray("y0"), p.getFloatArray("y1") };
        z = new float[][]
                        { p.getFloatArray("z0"), p.getFloatArray("z1") };
        etaModule = p.getIntArray("etaModule");
        phiModule = p.getIntArray("phiModule");
        BCIDError = p.getIntArray("BCIDError");
        firstHitError = p.getIntArray("firstHitError");
        formatterError = p.getIntArray("formatterError");
        lvl1Error = p.getIntArray("lvl1Error");
        preambleError = p.getIntArray("preambleError");
        secondHitError = p.getIntArray("secondHitError");
        syncError = p.getIntArray("syncError");
        timeBin = p.getIntArray("timeBin");
        rho = new float[2][numData];
        phi = new float[2][numData];
        layer = new int[numData];

        for(int i = 0; i < numData; ++i)
        {
            sub[i] = getSub(id[i]);
            layer[i] = getLayer(id[i]);
        }

    } // ASiClusterRDOData() ------------------------------------------------


    protected void calculateRhoPhi()
    {
        calculateRhoPhi(x[0], y[0], rho[0], phi[0]);
        calculateRhoPhi(x[1], y[1], rho[1], phi[1]);
        // treat wraparound
        for(int i = 0; i < numData; ++i)
        {
            if(Math.abs(phi[1][i] - phi[0][i]) > Math.PI)
            {
                if(phi[1][i] - phi[0][i] > 0)
                {
                    phi[0][i] += AMath.TWO_PI;
                }
                else
                {
                    phi[1][i] += AMath.TWO_PI;
                }
            }
        }

    } // calculateRhoPhi() --------------------------------------------------



    private void cutSubdetector()
    {
        AParameter par = parameterStore.get("CutsInDet", "SCT");
        if(par.getI() != -1)
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
        cutPhi(phi[0]);
        cutEta();

    } // applyCuts() --------------------------------------------------------



    private void cutEta()
    {
        AParameter par = parameterStore.get("CutsATLAS", "CutEta");
        if(!par.getStatus())return;
        double etaCut = par.getD();
        double etaMid = parameterStore.get("CutsATLAS", "EtaMiddle").getD();
        double etaLowerCut = etaMid - etaCut;
        double etaUpperCut = etaMid + etaCut;
        int num = 0;
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            double eta1 = AParameterUtilities.eta(z[0][list], rho[0][list]);
            double eta2 = AParameterUtilities.eta(z[1][list], rho[1][list]);
            double etaLower = Math.min(eta1, eta2);
            double etaUpper = Math.max(eta1, eta2);
            if(etaUpper > etaLowerCut && etaLower < etaUpperCut)
                listdl[num++] = list;
        }
        numDraw = num;

    } // cutEta() -----------------------------------------------------------



    public int getLayer(int id)
    {
        int layer = (id & 0x01e00000) >> 21;
        int sub = getSub(id);
        if(sub == BARREL)
            layer += 3;
        else
            layer += 11;
        return layer;

    } // getLayer() ---------------------------------------------------------



    public int getSub(int id)
    {
        return(id & 0x06000000) >> 25;

    } // getSub() -----------------------------------------------------------



    public int getOrientation(int id)
    {
        int side = (id & 0x400) >> 10;
        int sub = getSub(id);
        if((side == 1 && sub == BARREL) || (side == 0 && sub != BARREL))
        {
            return PHI_CLUSTER;
        }
        int layer = getLayer(id);
        if(layer % 2 == 1)return U_CLUSTER;
        return V_CLUSTER;

    } // getOrientation() ---------------------------------------------------



    protected void cutOrientation()
    {
        int num = 0;
        AParameter stereoAnglePar = parameterStore.get("SiCluster", "Stereo");
        if(!stereoAnglePar.getStatus())
        {
            return;
        }
        int orientation = stereoAnglePar.getI();
        for(int i = 0; i < numDraw; i++)
            // only works for barrel for now !!!!
            // if( getSub(id[listdl[i]])==BARREL && orientation==getOrientation(id[listdl[i]]))
            if(orientation == getOrientation(id[listdl[i]]))
            {
                listdl[num++] = listdl[i];
            }
        numDraw = num;

    } // cutOrientation() ---------------------------------------------------



    /**
     * parameter InDet -> SiClusterRDO -> Color Function:
     *    pv="Constant = 0, Error = 1, Orientation = 2"
     */
    protected int internalColor()
    {
        // it's usual to call this method from mother class, but in this case
        // it uses it's own colour types co we don't care about AHitData
        // int numColorTypes = super.internalColor();

        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();

        if(colorFunction == 0)
        {
            colorByConstant();
        }
        else if(colorFunction == 1)
        {
            colorByError();
        }
        else if(colorFunction == 2)
        {
            colorByOrientation();
        }

        // 3 colour schemes exist here
        return 3;

    } // internalColor() ----------------------------------------------------



    /**
     * colour elements by error (error subtags from the event file):
     * BCIDError, firstHitError, formatterError, lvl1Error, preambleError,
     *    secondHitError, syncError ]
     * if any of these flags is set to true, the element is considered erroneous
     * (next iteration may be to colour by error type, if they want it ...)
     */
    protected void colorByError()
    {
        int constantColor = parameterStore.get(PARAMETER_GROUP, "Constant").getI();
        int errorColor = parameterStore.get(PARAMETER_GROUP, "Error").getI();

        for(int i = 0; i < numData; i++)
        {
            if(BCIDError[i] != 0 || firstHitError[i] != 0 ||
                    formatterError[i] != 0 || lvl1Error[i] != 0 ||
                    preambleError[i] != 0 || secondHitError[i] != 0 ||
                    syncError[i] != 0)
            {
                // element is erroneous
                color[i] = (byte) errorColor;
            }
            else
            {
                color[i] = (byte) constantColor;
            }
        }

    } // colorByError() -----------------------------------------------------



    protected void colorByOrientation()
    {
        int numColors = parameterStore.get("HitColors", "Number").getI();
        numColors = Math.min(7, numColors);
        int[] col = parameterStore.getArray("HitColors", "C1", numColors);
        for(int i = 0; i < numData; i++)
        {
            color[i] = (byte) col[getOrientation(id[i]) + 1 % numColors];
        }

    } // colorByOrientation() -----------------------------------------------



    public String getHitInfo(int index)
    {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0) return getNameScreenName()+" index: " + index;
        
        String temp = getNameScreenName() + " (id: " + id[index] + " index: " + index + ")\n" +
        " orientation = " + getOrientation(id[index]) + "\n" +
        " x = " + x[0][index] + "  " + x[1][index] + "\n" +
        " y = " + y[0][index] + "  " + y[1][index] + "\n" +
        " z = " + z[0][index] + "  " + z[1][index] + "\n" +
        " " + AMath.RHO + " = " + rho[0][index] + "  " +
        rho[1][index] + "\n" +
        " " + AMath.ETA + " module = " + etaModule[index] + "\n" +
        " " + AMath.PHI + " module = " + phiModule[index] + "\n" +
        " BCID error = " + BCIDError[index] + "\n" +
        " first hit error = " + firstHitError[index] + "\n" +
        " formatter error = " + formatterError[index] + "\n" +
        " lvl1 error = " + lvl1Error[index] + "\n" +
        " preamble error = " + preambleError[index] + "\n" +
        " second hit error = " + secondHitError[index] + "\n" +
        " sync error = " + syncError[index] + "\n" +
        " time bin = " + timeBin[index];
        return temp;

    } // getHitInfo() -------------------------------------------------------



    protected ACoord getYXUser()
    {
        makeDrawList();
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            for(int j = 0; j < 2; j++)
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
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            double phiDiff = Math.abs(phi[1][list] - phiMid);
            double sign;
            if(phiDiff < Math.PI / 2. || phiDiff > 3 * Math.PI / 2.)
            {
                sign = +1.;
            }
            else
            {
                sign = -1.;
            }
            for(int j = 0; j < 2; j++)
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
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            for(int j = 0; j < 2; j++)
            {
                hv[0][j][i] = rho[j][list];
                hv[1][j][i] = Math.toDegrees(AParameterUtilities.getPhiStereo(rho[j][list], phi[j][list],
                        z[j][list]));
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
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            for(int j = 0; j < 2; j++)
            {
                hv[0][j][i] = z[j][list];
                hv[1][j][i] = Math.toDegrees(AParameterUtilities.getPhiStereo(rho[j][list], phi[j][list],
                        z[j][list]));
            }
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.LINES).includePhiWrapAround("FZ");

    } // getFZUser() --------------------------------------------------------



    protected ACoord getVPUser()
    {
        makeDrawList();
        int numTotal = 2 * numDraw;
        int numPoints = 10;
        double[][][] hv = new double[2][numTotal][numPoints];
        int[] index = new int[numTotal];
        double[] sign = new double[]
                                   { -1., 1.};
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            double xx = x[0][list];
            double delx = (x[1][list] - x[0][list]) / (numPoints - 1);
            double yy = y[0][list];
            double dely = (y[1][list] - y[0][list]) / (numPoints - 1);
            double zz = z[0][list];
            double delz = (z[1][list] - z[0][list]) / (numPoints - 1);
            for(int k = 0; k < numPoints; k++)
            {
                double xxx = xx + k * delx;
                double yyy = yy + k * dely;
                double zzz = zz + k * delz;
                double rrr = Math.sqrt(xxx * xxx + yyy * yyy);
                double ppp = Math.atan2(yyy, xxx);
                double deltaEta = AProjectionVP.getDeltaEta(rrr, zzz);
                double eta = AParameterUtilities.eta(zzz, rrr);
                for(int j = 0; j < 2; j++)
                {
                    hv[0][2 * i + j][k] = eta + sign[j] * deltaEta;
                    hv[1][2 * i + j][k] = Math.toDegrees(ppp);
                    index[2 * i + j] = list;
                }
            }
        }
        return new ACoord(hv, index, this, ACoord.POLYLINES). includePhiWrapAround("VP");

    } // getVPUser() --------------------------------------------------------


} // class ASiClusterRDOData ================================================
