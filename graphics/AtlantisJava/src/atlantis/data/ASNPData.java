package atlantis.data;

import atlantis.event.*;
import atlantis.graphics.ACoord;
import atlantis.parameters.AParameterUtilities;
import atlantis.projection.AProjectionXZ;
import atlantis.utils.A3Vector;
import atlantis.utils.A4Vector;
import atlantis.utils.AHashMap;
import atlantis.utils.ALogger;
import atlantis.utils.AMath;

/**
 * Simulated Neutral Particle
 */

public class ASNPData extends AData
{
    private static ALogger logger = ALogger.getLogger(ASNPData.class);
    
    private static final double START_RADIUS = 2.;

    protected int[] code;
    protected int[] particleType;
    protected int[] originVertex;
    protected float[] pT;
    protected float[][] rho;
    protected float[][] phi;
    protected float[][] z;
    protected float[][] x;
    protected float[][] y;
    protected int[] type;
    private float[] temp;
    private int[] index;

    public float[] getPt()
    {
        return pT;
    }

    public String getParameterGroup()
    {
        return "SNP";
    }

    public String getName()
    {
        return "SNP";
    }

    public String getNameScreenName()
    {
        return "SimNeutralTrack";
    }

    ASNPData(AHashMap p, AEvent e)
    {
        super(p,e);
        rho = new float[2][];
        phi = new float[2][];
        z = new float[2][];
        x = new float[2][numData];
        y = new float[2][numData];
        rho[0] = p.getFloatArray("rhoVertex");
        phi[0] = p.getFloatArray("phiVertex");
        z[0] = p.getFloatArray("zVertex");

        pT = p.getFloatArray("pt");
        code = p.getIntArray("code");
        type = new int[numData];
        particleType = new int[numData];
        originVertex = p.getIntArray("simulatedVertex");
        temp = new float[numData];
        for (int i = 0; i < numData; i++)
        {
            if (Math.abs(code[i]) > 10)
            {
                // code is being used to store particle code
                particleType[i] = APDGTable.getParticleType(code[i]);
            }
        }
        calculateReasonableEndpoints(p.getFloatArray("phi"), p.getFloatArray("eta"));

        double[] phiD = new double[numData];
        float[] phiF = p.getFloatArray("phi");

        for (int i = 0; i < numData; i++)
            phiD[i] = phiF[i];
        index = indexBy(phiD);
        calculateXY(rho[0], phi[0], x[0], y[0]);
        calculateXY(rho[1], phi[1], x[1], y[1]);
    }

    protected void calculateRhoPhi()
    {
        calculateRhoPhi(x[0], y[0], rho[0], phi[0]);
        calculateRhoPhi(x[1], y[1], rho[1], phi[1]);
    }

    public static AHashMap createSNP(AHashMap p)
    {
        float[] pt = p.getFloatArray("pt");
        float[] phi = p.getFloatArray("phi");
        float[] eta = p.getFloatArray("eta");
        float[] rhoVertex = p.getFloatArray("rhoVertex");
        float[] phiVertex = p.getFloatArray("phiVertex");
        float[] zVertex = p.getFloatArray("zVertex");
        int[] id = p.getIntArray("id");

        int[] code = p.getIntArray("code");

        int numNeutral = 0;

        for (int i = 0; i < code.length; ++i)
        {
            int charge = code[i];

            try
            {
                if (Math.abs(code[i]) >= 10)

                    // code is being used to store particle code
                    charge = APDGTable.getCharge(code[i]);
                if (charge == 0)
                    numNeutral++;
            }
            catch (APDGTable.ParticleNotFoundError pnf){
              logger.debug(pnf.getMessage());
            }
        }

        int[] simulatedVertex = ASVxData.assignVertexNumbers(phiVertex, rhoVertex, zVertex);

        int[] idN = new int[numNeutral];
        float[] rhoVertexN = new float[numNeutral];
        float[] phiVertexN = new float[numNeutral];
        float[] zVertexN = new float[numNeutral];
        float[] ptN = new float[numNeutral];
        float[] phiN = new float[numNeutral];
        float[] etaN = new float[numNeutral];
        int[] codeN = new int[numNeutral];
        int[] simulatedVertexN = new int[numNeutral];

        numNeutral = 0;
        for (int i = 0; i < code.length; ++i)
        {
            double charge = code[i];

            try
            {
                if (Math.abs(code[i]) >= 10)
                {
                    // code is being used to store particle code
                    charge = APDGTable.getCharge(code[i]);
                }
                if (charge == 0)
                {
                    idN[numNeutral] = id[i];
                    rhoVertexN[numNeutral] = (float) Math.sqrt(rhoVertex[i]*rhoVertex[i]-zVertex[i]*zVertex[i]);
                    phiVertexN[numNeutral] = phiVertex[i];
                    zVertexN[numNeutral] = zVertex[i];
                    ptN[numNeutral] = pt[i];
                    phiN[numNeutral] = phi[i];
                    etaN[numNeutral] = eta[i];
                    codeN[numNeutral] = code[i];
                    simulatedVertexN[numNeutral] = simulatedVertex[i];
                    numNeutral++;
                }
            }
            catch (APDGTable.ParticleNotFoundError pnf){
              logger.debug(pnf.getMessage());
            }
        }

        AHashMap newP = new AHashMap(10);

        newP.put("numData", new Integer(numNeutral));
        newP.put("id", idN);
        newP.put("rhoVertex", rhoVertexN);
        newP.put("phiVertex", phiVertexN);
        newP.put("zVertex", zVertexN);
        newP.put("pt", ptN);
        newP.put("phi", phiN);
        newP.put("eta", etaN);
        newP.put("code", codeN);
        newP.put("simulatedVertex", simulatedVertexN);
        return newP;
    }

    private void calculateReasonableEndpoints(float[] dirPhi, float[] dirEta)
    {
        double rhoTo = parameterStore.get("RTr", "RadiusTr").getD();
        double zTo = parameterStore.get("RTr", "ZTr").getD();

        rho[1] = (float[]) rho[0].clone();
        phi[1] = (float[]) phi[0].clone();
        z[1] = (float[]) z[0].clone();
        for (int i = 0; i < numData; i++)
        {
            if (particleType[i] == 5)
            {
                // photon -> ecal entrance
                rhoTo = 150.;
                zTo = 370.;
            }
            else
            {
                // neutral hadron -> hcal entrance
                rhoTo = 228.;
                zTo = 426.2;
            }
            if (rho[0][i] > rhoTo || Math.abs(z[0][i]) > zTo)
                continue;
            double rho2 = rhoTo;
            double tanLambda = AMath.tanLambda(dirEta[i]);
            double zz = z[0][i] + (rho2 - rho[0][i]) * tanLambda;

            if (zz > zTo)
                rho2 = (zTo - z[0][i]) / tanLambda + rho[0][i];
            if (zz < -zTo)
                rho2 = (-zTo - z[0][i]) / tanLambda + rho[0][i];
            double x = rho[0][i] * Math.cos(phi[0][i]);
            double y = rho[0][i] * Math.sin(phi[0][i]);
            // y=mx+c, x*x+y*y=rho2*rho2
            double m = Math.tan(dirPhi[i]);
            double c = y - m * x;
            // ax^2+bx+cc=0
            double a = m * m + 1.;
            double b = 2. * m * c;
            double cc = c * c - rho2 * rho2;
            double d = Math.sqrt(b * b - 4 * a * cc);
            double x1 = (-b + d) / (a * 2);
            double x2 = (-b - d) / (a * 2);
            double y1 = m * x1 + c;
            double y2 = m * x2 + c;
            double xx = x1;
            double yy = y1;

            if ((x2 - x) * Math.cos(dirPhi[i]) + (y2 - y) * Math.sin(dirPhi[i]) > 0.)
            {
                xx = x2;
                yy = y2;
            }
            zz = z[0][i] + (rho2 - rho[0][i]) * tanLambda;
            rho[1][i] = (float) Math.sqrt(xx * xx + yy * yy);
            phi[1][i] = (float) Math.atan2(yy, xx);
            if (phi[1][i] < 0.)
                phi[1][i] += 2 * Math.PI;
            z[1][i] = (float) zz;
        }
    }

    protected void applyCuts()
    {
        cutIndex();
        cut("CutsInDet", "STrType", " STr Type", particleType);
        cut("CutsInDet", "STr", " STr id", id);
        cut("CutsInDet", "Pt", " |Pt|", pT);
        cut("CutsInDet", "SVx", " Vertex", originVertex);
        cut("CutsInDet", "d0", " |d0|", getD0());
        cut("CutsInDet", "d0Loose", " |d0Loose|", getD0());
        cut("CutsInDet", "z0-zVtx", " |z0-zVtx|", getZ0Primary());
        cut("CutsInDet", "z0", " |z0|", getZ0());
        cutPhi(phi[1]);
        float[] phiStart = new float[numData];

        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            int iMax = 0;

            if (rho[1][list] > rho[0][list])
                iMax = 1;
            int iMin = 1 - iMax;

            if (rho[iMin][list] > START_RADIUS)
                phiStart[list] = phi[iMin][list];
            else
            {
                double[] rpz = calculateRhoPhiZAtStart(list);

                phiStart[list] = (float) rpz[1];
            }
        }
        cutPhi(phiStart);
        cutEta(rho[1], z[1]);
    }

    protected int internalColor()
    {
        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();

        if (colorFunction == 0)
            colorByConstant();
        else if (colorFunction == 1)
            colorByIndex(index);
        else if (colorFunction == 2)
            colorBy(getPt());
        else if (colorFunction == 3)
            colorBy("ParticleColors", particleType);
        else if (colorFunction == 4)
            colorBy(originVertex);
        return 3;
    }

    public String getHitInfo(int index)
    {
        StringBuffer msg = new StringBuffer(getNameScreenName());
        msg.append(" (id: " + id[index] + " index: " + index + ")");
        msg.append("\n Type = ");
        msg.append(APDGTable.getName(code[index]));
        msg.append("\n PT = ");
        msg.append(String.format("%.3f",pT[index]));
        msg.append(" GeV");

        return msg.toString();
    }

    protected void cutStartRadius()
    {
        int num = 0;

        for (int i = 0; i < numDraw; i++)
            if (Math.max(rho[0][listdl[i]], rho[1][listdl[i]]) > START_RADIUS)
                listdl[num++] = listdl[i];
        numDraw = num;
    }

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
    }

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

            // this is not quite correct need to split some tracks
            if (phiDiff < Math.PI / 2. || phiDiff > 3 * Math.PI / 2.)
                sign = +1.;
            else
                sign = -1.;
            for (int j = 0; j < 2; j++)
            {
                hv[0][j][i] = z[j][list];
                hv[1][j][i] = sign * rho[j][list];
            }
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.LINES);
    }

    protected ACoord getFRUser()
    {
        makeDrawList();
        cutStartRadius();
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];

        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            int iMax = 0;

            if (rho[1][list] > rho[0][list])
                iMax = 1;
            int iMin = 1 - iMax;
            double rho1 = rho[iMin][list];
            double rho2 = rho[iMax][list];

            if (rho1 > START_RADIUS)
            {
                hv[0][0][i] = rho1;
                hv[1][0][i] = Math.toDegrees(phi[iMin][list]);
            }
            else
            {
                double[] rpz = calculateRhoPhiZAtStart(list);

                hv[0][0][i] = rpz[0];
                hv[1][0][i] = Math.toDegrees(rpz[1]);
            }
            hv[0][1][i] = rho2;
            hv[1][1][i] = Math.toDegrees(phi[iMax][list]);
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.LINES).includePhiWrapAround("FR");
    }

    protected ACoord getXZUser()
    {
        makeDrawList();
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        double phi0 = Math.toRadians(AProjectionXZ.getPhi());
        double cosPhi0 = Math.cos(phi0);
        double sinPhi0 = Math.sin(phi0);

        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];

            for (int j = 0; j < 2; j++)
            {
                hv[0][j][i] = z[j][list];
                hv[1][j][i] = x[j][list] * cosPhi0 + y[j][list] * sinPhi0;
            }
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.LINES);
    }

    protected ACoord getYZUser()
    {
        makeDrawList();
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        double phi0 = Math.toRadians(AProjectionXZ.getPhi());
        double cosPhi0 = Math.cos(phi0);
        double sinPhi0 = Math.sin(phi0);

        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];

            for (int j = 0; j < 2; j++)
            {
                hv[0][j][i] = z[j][list];
                hv[1][j][i] = y[j][list] * cosPhi0 - x[j][list] * sinPhi0;
            }
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.LINES);
    }

    protected ACoord getFZUser()
    {
        makeDrawList();
        cutStartRadius();
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];

        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            int iMax = 0;

            if (rho[1][list] > rho[0][list])
                iMax = 1;
            int iMin = 1 - iMax;
            double rho1 = rho[iMin][list];

            if (rho1 > START_RADIUS)
            {
                hv[0][0][i] = z[iMin][list];
                hv[1][0][i] = Math.toDegrees(phi[iMin][list]);
            }
            else
            {
                double[] rpz = calculateRhoPhiZAtStart(list);

                hv[0][0][i] = rpz[2];
                hv[1][0][i] = Math.toDegrees(rpz[1]);
            }
            hv[0][1][i] = z[iMax][list];
            hv[1][1][i] = Math.toDegrees(phi[iMax][list]);
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.LINES).includePhiWrapAround("FZ");
    }

    protected ACoord getVPUser()
    {
        makeDrawList();
        double[] h = new double[numDraw];
        double[] v = new double[numDraw];
        int[] index = new int[numDraw];

        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];

            h[i] = AParameterUtilities.eta(z[1][list], rho[1][list]);
            v[i] = Math.toDegrees(phi[1][list]);
            index[i] = list;
        }
        return new ACoord(h, v, index, this).includePhiWrapAround("VP");
    }

    private double[] calculateRhoPhiZAtStart(int index)
    {
        double[] rpz = new double[3];
        double rho1 = rho[0][index];
        double phi1 = phi[0][index];
        double z1 = z[0][index];
        double rho2 = rho[1][index];
        double phi2 = phi[1][index];
        double z2 = z[1][index];
        double x1 = rho1 * Math.cos(phi1);
        double y1 = rho1 * Math.sin(phi1);
        double x2 = rho2 * Math.cos(phi2);
        double y2 = rho2 * Math.sin(phi2);
        double fract = (START_RADIUS - rho1) / (rho2 - rho1);
        double x = x1 + fract * (x2 - x1);
        double y = y1 + fract * (y2 - y1);
        double z = z1 + fract * (z2 - z1);
        double phi = Math.atan2(y, x);

        if (phi < 0.)
            phi += 2 * Math.PI;
        rpz[0] = START_RADIUS;
        rpz[1] = phi;
        rpz[2] = z;
        return rpz;
    }

    public String getVPHitInfo()
    {
        makeDrawList();
        if (numDraw == 0)
            return "";
        double sumP = 0.;
        double sumPt = 0.;

        for (int i = 0; i < numDraw; ++i)
        {
            sumPt += Math.abs(pT[listdl[i]]);
            double invLambda = (rho[1][listdl[i]] - rho[0][listdl[i]]) / (z[1][listdl[i]] - z[0][listdl[i]]);
            sumP += Math.abs(pT[listdl[i]] / Math.cos(Math.atan(1. / invLambda)));
        }

        String msg = numDraw + " " + getNameScreenName();
        msg += "  sum(PT) = " + String.format("%.1f",sumPt) + "  sum(P) = " + String.format("%.1f",sumP);

        return msg;
    }

    public A4Vector get4Vector(int num, int[] list)
    {
        A4Vector sum = new A4Vector();
        for (int i = 0; i < num; ++i)
        {
            int k = list[i];
            A3Vector start = A3Vector.fromRhoPhiZ(rho[0][k], phi[0][k], z[0][k]);
            A3Vector stop = A3Vector.fromRhoPhiZ(rho[1][k], phi[1][k], z[1][k]);
            A3Vector v = (stop.subtract(start)).normalize();
            double p = pT[k] / Math.sqrt(1. - v.z * v.z);
            sum.add(new A4Vector(v.scale(p), 0.));
        }

        return sum;
    }

    private float[] getZ0()
    {
        for (int i = 0; i < numDraw; ++i)
        {
            int list = listdl[i];
            double[] poca = getPoca(list);
            temp[list] = (float) poca[2];
        }
        return temp;
    }

    private float[] getZ0Primary()
    {
        double[] primary = event.getPrimaryVertex();
        double c = primary[2];
        for (int i = 0; i < numDraw; ++i)
        {
            int list = listdl[i];
            double[] poca = getPoca(list);
            temp[list] = (float) (poca[2] - c);
        }
        return temp;
    }

    private float[] getD0()
    {
        double[] primary = event.getPrimaryVertex();
        double a = primary[0];
        double b = primary[1];
        for (int i = 0; i < numDraw; ++i)
        {
            int list = listdl[i];
            double[] poca = getPoca(list);
            temp[list] = (float) Math.hypot(a-poca[0], b-poca[1]);
        }
        return temp;
    }

    private double[] getPoca(int i)
    {
        double[] primary = event.getPrimaryVertex();
        double a = primary[0];
        double b = primary[1];
        double xMin;
        double yMin;
        double zMin;
        double dx = x[1][i] - x[0][i];
        if (dx == 0.)
        {
            xMin = x[0][i];
            yMin = b;
        }
        else
        {
            double m = (y[1][i] - y[0][i]) / dx;
            double c = y[0][i] - m * x[0][i];
            xMin = (a + (b - c) * m) / (m * m + 1);
            yMin = m * xMin + c;
        }
        double dz = z[1][i] - z[0][i];
        if (dz == 0.)
        {
            zMin = z[0][i];
        }
        else
        {
            double mx = (x[1][i] - x[0][i]) / dz;
            if (mx != 0.)
                zMin = z[0][i] + (xMin - x[0][i]) / mx;
            else
            {
                double my = (y[1][i] - y[0][i]) / dz;
                if (my != 0.)
                    zMin = z[0][i] + (yMin - y[0][i]) / my;
                else
                    zMin = z[0][i];
            }
        }
        return new double[] { xMin, yMin, zMin };
    }

}
