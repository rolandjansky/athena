package atlantis.data;

import atlantis.event.*;
import atlantis.canvas.AWindow;
import atlantis.graphics.ACoord;
import atlantis.graphics.AGraphics;
import atlantis.parameters.AParameterUtilities;
import atlantis.projection.AProjection;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionVP;
import atlantis.projection.AProjectionXZ;
import atlantis.utils.A3Vector;
import atlantis.utils.A4Vector;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;

/**
 * Simulated particle in the muon system
 */

public class ASMTrData extends AData
{
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
    AHelix[] innerTrack;
    protected int[] type;
    private int[] index;


    public String getParameterGroup()
    {
        return "SMTr";
    }


    public String getName()
    {
        return "SMTr";
    }


    public String getNameScreenName()
    {
        return "SimMuonTrack";
    }


    ASMTrData(AHashMap p, AEvent e)
    {
        super(p,e);
        rho = new float[2][];
        phi = new float[2][];
        z = new float[2][];
        x = new float[2][];
        y = new float[2][];
        x[0] = new float[numData];
        y[0] = new float[numData];
        rho[0] = p.getFloatArray("rhoVertex");
        phi[0] = p.getFloatArray("phiVertex");
        z[0] = p.getFloatArray("zVertex");
        for(int i = 0; i < numData; ++i)
        {
            x[0][i] = (float) (rho[0][i] * Math.cos(phi[0][i]));
            y[0][i] = (float) (rho[0][i] * Math.sin(phi[0][i]));
        }
        x[1] = (float[]) x[0].clone();
        y[1] = (float[]) y[0].clone();
        z[1] = (float[]) z[0].clone();
        phi[1] = (float[]) phi[0].clone();
        rho[1] = (float[]) rho[0].clone();
        pT = p.getFloatArray("pt");
        code = p.getIntArray("code");
        type = new int[numData];
        particleType = new int[numData];
        originVertex = p.getIntArray("simulatedVertex");
        for(int i = 0; i < numData; i++)
        {
            if(Math.abs(code[i]) > 10)
                // code is being used to store particle code
                particleType[i] = APDGTable.getParticleType(code[i]);
        }
        calculateReasonableEndpoints(p.getFloatArray("phi"),
                                     p.getFloatArray("eta"));
        double[] phiD = new double[numData];
        float[] phiF = p.getFloatArray("phi");
        for(int i = 0; i < numData; i++)
            phiD[i] = phiF[i];
        index = indexBy(phiD);
    }


    protected void calculateRhoPhi()
    {
        calculateRhoPhi(x[0], y[0], rho[0], phi[0]);
        calculateRhoPhi(x[1], y[1], rho[1], phi[1]);
    }

    private void calculateReasonableEndpoints(float[] phiDir, float[] eta)
    {
        double rhoTo = 1500.;
        double zTo = 2500.;
        int maxPoints = 5;
        double[] d = new double[maxPoints];
        for(int i = 0; i < numData; i++)
        {
            int numPoints = 0;
            //initial point inside volume
            if(rho[0][i] < rhoTo && Math.abs(z[0][i]) < zTo)
            {
                d[numPoints++] = 0.;
            }
            double lambda = AMath.lambda(eta[i]);
            double x0 = x[0][i];
            double y0 = y[0][i];
            double z0 = z[0][i];
            x[1][i] = (float) x0;
            y[1][i] = (float) y0;
            z[1][i] = (float) z0;
            double u = Math.cos(lambda) * Math.cos(phiDir[i]);
            double v = Math.cos(lambda) * Math.sin(phiDir[i]);
            double w = Math.sin(lambda);
            //intersections with cylinder walls
            if(w != 0.)
            {
                d[numPoints++] = (zTo - z0) / w;
                d[numPoints++] = ( -zTo - z0) / w;
            }
            double a = u * u + v * v;
            double c = x0 * x0 + y0 * y0 - rhoTo * rhoTo;
            double b = 2 * (u * x0 + v * y0);
            double squared = b * b - 4 * a * c;
            if(squared >= 0.)
            {
                d[numPoints++] = ( -b + Math.sqrt(squared)) / (2 * a);
                d[numPoints++] = ( -b - Math.sqrt(squared)) / (2 * a);
            }
            //bubble sort points by increasing path length
            for(int j = 0; j < numPoints - 1; ++j)
                for(int k = j + 1; k < numPoints; ++k)
                    if(d[k] < d[j])
                    {
                        double temp = d[k];
                        d[k] = d[j];
                        d[j] = temp;
                    }
            for(int j = 0; j < numPoints; ++j)
                if(d[j] >= 0. && j < numPoints - 1)
                {
                    x[0][i] = (float) (x0 + d[j] * u);
                    y[0][i] = (float) (y0 + d[j] * v);
                    z[0][i] = (float) (z0 + d[j] * w);
                    x[1][i] = (float) (x0 + d[j + 1] * u);
                    y[1][i] = (float) (y0 + d[j + 1] * v);
                    z[1][i] = (float) (z0 + d[j + 1] * w);
                    break;
                }
        }
    }


    protected void finalizeConstruction()
    {
        super.finalizeConstruction();
        // needs ZVTx
        innerTrack = new AHelix[numData];
        for(int i = 0; i < numData; ++i)
        {
            double p = AMath.getPFromPttL(pT[i],
                                          AMath.tanLambda(AParameterUtilities.eta(z[0][i],
                rho[0][i])));
            // correct for average energy loss
            double pTCorrected = pT[i] * (p + 3.7) / p;
            innerTrack[i] = new AHelix((float) 0.,
                                       (float) (parameterStore.get("Event", "ZVtx").getD()), (
                                           float) (Math.toDegrees(phi[0][i])),
                                       (float) AMath.tanLambda(AParameterUtilities.eta(z[0][i],
                rho[0][i])), (float) pTCorrected);
            // calculatePhiCorrection(i);
        }
    }


    protected void applyCuts()
    {
        cutIndex();
        cut("CutsInDet", "STrType", " STr Type", particleType);
        cut("CutsInDet", "STr", " STr id", id);
        cut("CutsInDet", "Pt", " |Pt|", pT);
        cut("CutsInDet", "SVx", " Vertex", originVertex);
        cutPhi(phi[1]);
        float[] phiStart = new float[numData];
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            int iMax = 0;
            if(rho[1][list] > rho[0][list])
                iMax = 1;
            int iMin = 1 - iMax;
            phiStart[list] = (float) phi[iMin][list];
        }
        cutPhi(phiStart);
        cutEta(rho[1], z[1]);
    }


    protected int internalColor()
    {
        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();
        if(colorFunction == 0)
            colorByConstant();
        else if(colorFunction == 1)
            colorByIndex(index);
        else if(colorFunction == 2)
            colorBy("ParticleColors", particleType);
        return 2;
    }


    public String getHitInfo(int index)
    {
        StringBuffer msg = new StringBuffer(getNameScreenName());
        msg.append(" (id: " + id[index] + " index: " + index + ")");
        msg.append("\n Type = ");
        msg.append(APDGTable.getName(code[index]));
        if (innerTrack != null)
            msg.append(innerTrack[index].toString());
        msg.append("\n pT (muon spectrometer) = ");
        msg.append(String.format("%.3f",pT[index]));
        msg.append(" GeV");

        return msg.toString();
    }


    protected void cutStartRadius()
    {
        int num = 0;
        for(int i = 0; i < numDraw; i++)
            if(Math.min(Math.abs(z[0][listdl[i]] / rho[0][listdl[i]]),
                        Math.abs(z[1][listdl[i]] / rho[1][listdl[i]]))
               < 12.3 / 9.2)
                listdl[num++] = listdl[i];
        numDraw = num;
    }


    protected void cutStartRadius2()
    {
        int num = 0;
        for(int i = 0; i < numDraw; i++)
            if(Math.max(Math.abs(z[0][listdl[i]] / rho[0][listdl[i]]),
                        Math.abs(z[1][listdl[i]] / rho[1][listdl[i]]))
               > 12.3 / 9.2)
                listdl[num++] = listdl[i];
        numDraw = num;
    }


    protected ACoord getYXUser()
    {
        makeDrawList();
        cutStartRadius();
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
    }


    protected ACoord getRZUser()
    {
        makeDrawList();
        double phiMid = Math.toRadians(parameterStore.get("RZ", "Phi").getD());
        int numSplit = 0;
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            double phiDiff = Math.abs(phi[0][list] - phiMid);
            double sign0 = -1.;
            if(phiDiff < Math.PI / 2. || phiDiff > 3 * Math.PI / 2.)
                sign0 = +1.;
            phiDiff = Math.abs(phi[1][list] - phiMid);
            double sign1 = -1.;
            if(phiDiff < Math.PI / 2. || phiDiff > 3 * Math.PI / 2.)
                sign1 = +1.;
            if(sign0 != sign1) numSplit++;
        }
        int numPoints = 100;
        double[][][] hv = new double[2][numDraw + numSplit][];
        int[] index = new int[numDraw + numSplit];
        int num = 0;
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            double phiDiff = Math.abs(phi[0][list] - phiMid);
            double sign0 = -1.;
            if(phiDiff < Math.PI / 2. || phiDiff > 3 * Math.PI / 2.)
                sign0 = +1.;
            phiDiff = Math.abs(phi[1][list] - phiMid);
            double sign1 = -1.;
            if(phiDiff < Math.PI / 2. || phiDiff > 3 * Math.PI / 2.)
                sign1 = +1.;
            if(sign0 == sign1)
            {
                hv[0][num] = AMath.splitLineIntoPieces(new double[]
                    {z[1][list], z[0][list]}, numPoints);
                double[] xx = AMath.splitLineIntoPieces(new double[]
                    {x[1][list], x[0][list]}, numPoints);
                double[] yy = AMath.splitLineIntoPieces(new double[]
                    {y[1][list], y[0][list]}, numPoints);
                hv[1][num] = new double[xx.length];
                for(int j = 0; j < xx.length; ++j)
                    hv[1][num][j] = sign0 *
                                    Math.sqrt(xx[j] * xx[j] + yy[j] * yy[j]);
                index[num++] = list;
            }
            else
            {
                //just for now....
                double a = Math.cos(phiMid);
                double b = Math.sin(phiMid);
                double x0 = x[0][list];
                double y0 = y[0][list];
                double z0 = z[0][list];
                double dx = x[1][list] - x0;
                double dy = y[1][list] - y0;
                double dz = z[1][list] - z0;
                double mag = Math.sqrt(dx * dx + dy * dy + dz * dz);
                dx /= mag;
                dy /= mag;
                dz /= mag;
                double d = -(a * x0 + b * y0) / (a * dx + b * dy);
                double xMid = x0 + d * dx;
                double yMid = y0 + d * dy;
                double zMid = z0 + d * dz;
                hv[0][num] = AMath.splitLineIntoPieces(new double[]
                    {zMid, z[0][list]}, numPoints);
                double[] xx = AMath.splitLineIntoPieces(new double[]
                    {xMid, x[0][list]}, numPoints);
                double[] yy = AMath.splitLineIntoPieces(new double[]
                    {yMid, y[0][list]}, numPoints);
                hv[1][num] = new double[xx.length];
                for(int j = 0; j < xx.length; ++j)
                    hv[1][num][j] = sign0 *
                                    Math.sqrt(xx[j] * xx[j] + yy[j] * yy[j]);
                index[num++] = list;
                hv[0][num] = AMath.splitLineIntoPieces(new double[]
                    {z[1][list], zMid}, numPoints);
                xx = AMath.splitLineIntoPieces(new double[]
                                               {x[1][list], xMid}, numPoints);
                yy = AMath.splitLineIntoPieces(new double[]
                                               {y[1][list], yMid}, numPoints);
                hv[1][num] = new double[xx.length];
                for(int j = 0; j < xx.length; ++j)
                    hv[1][num][j] = sign1 *
                                    Math.sqrt(xx[j] * xx[j] + yy[j] * yy[j]);
                index[num++] = list;
            }
        }
        return new ACoord(hv, index, this, ACoord.SMOOTH_POLYLINES);
    }


    protected ACoord getFRUser()
    {
        makeDrawList();
        cutStartRadius();
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            int iMax = 0;
            if(rho[1][list] > rho[0][list])
                iMax = 1;
            int iMin = 1 - iMax;
            double rho1 = rho[iMin][list];
            double rho2 = rho[iMax][list];
            if(rho1 > START_RADIUS)
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
        return new ACoord(hv, index, this,
            ACoord.LINES).includePhiWrapAround("FR");
    }


    protected ACoord getXZUser()
    {
        makeDrawList();
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        double phi0 = Math.toRadians(AProjectionXZ.getPhi());
        double cosPhi0 = Math.cos(phi0);
        double sinPhi0 = Math.sin(phi0);
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            for(int j = 0; j < 2; j++)
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
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            for(int j = 0; j < 2; j++)
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
        cutStartRadius2();
        double[][][] hv = new double[2][2][numDraw];
        int[] index = new int[numDraw];
        for(int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            int iMax = 0;
            if(rho[1][list] > rho[0][list])
                iMax = 1;
            int iMin = 1 - iMax;
            double rho1 = rho[iMin][list];
            if(rho1 > START_RADIUS)
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
        return new ACoord(hv, index, this,
            ACoord.LINES).includePhiWrapAround("FZ");
    }


    public void draw(AWindow window, AGraphics ag, AProjection2D projection)
    {
        if(projection instanceof AProjectionVP)
        {
            ag.draw(window.calculateDisplay(getVPUser(window, projection)));
        }
        else
            super.draw(window, ag, projection);
    }


    // a track has two arms in the VPlot

    protected ACoord getVPUser(AWindow window, AProjection projection)
    {
        makeDrawList();
       
        // make a copy of the points to draw: both arms * ??? 
        double[][][] hv = new double[2][2 * numDraw][];
        int[] index = new int[2 * numDraw];
        int num = 0;
        
        //loop over drawn objects?
        for(int j = 0; j < numDraw; ++j)
        {
            // get list item from draw index list?
            int list = listdl[j];
            // get the helix that should be drawn
            ADHelix dhelix = new ADHelix(innerTrack[list]);
            // get the helix start and end
            double s1 = dhelix.getAStart();
            double s2 = 179.;
            // hardwire projection.getMinRho() == 2. for now
            s1 = dhelix.intersectWithRadialCylinder(((AProjection2D) projection).
                getMinRho(), s1, s2);
            double sEnd = dhelix.intersectWithCylinder(true,
                AProjectionVP.getRhoVPlot(), true,
                AProjectionVP.getZVPlot());
            s2 = Math.max(Math.min(s2, sEnd), s1);
            // if the whole helix is to be drawn (which are unusual
            // helices, shorten it a little to avoid wraparound problems
            if(s1 == 0. && s2 == 180.) s2 = 179.;
            if(parameterStore.get("VP", "ShortV").getStatus() &&
               !parameterStore.get("Data", "S3D").getStatus())
                s1 = s2 - parameterStore.get("VP", "ShortV").getD() * (s2 - s1);
            if(s2 > s1)
            {
                int signMin = -1;
                int signMax = 1;
                for(int sign = signMin; sign <= signMax; sign += 2)
                {
                    // ugly must change structure at some point
                    AProjectionVP.sign = sign;
                    ACoord pointsOnHelix = dhelix.drawHelix(window,
                        (AProjection2D) projection, s1, s2);
                    hv[0][num] = pointsOnHelix.hv[0][0];
                    hv[1][num] = pointsOnHelix.hv[1][0];
                    index[num] = list;
                    num++;
                }
            }
        }
        return window.calculateUser(new ACoord(hv, index, this,
                                               ACoord.SMOOTH_POLYLINES)).
            includePhiWrapAround(projection.getName());
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
        if(phi < 0.) phi += 2 * Math.PI;
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
        for(int i = 0; i < numDraw; ++i)
        {
            sumPt += Math.abs(pT[listdl[i]]);
            double invLambda = (rho[1][listdl[i]] - rho[0][listdl[i]]) /
                               (z[1][listdl[i]] - z[0][listdl[i]]);
            sumP += Math.abs(pT[listdl[i]] / Math.cos(Math.atan(1. / invLambda)));
        }

        String msg = numDraw + " " + getNameScreenName();
        msg += "  sum(PT) = " + String.format("%.1f",sumPt) + "  sum(P) = " + String.format("%.1f",sumP);

        return msg;
    }


    public A4Vector get4Vector(int num, int[] list)
    {
        A4Vector sum = new A4Vector();
        for(int i = 0; i < num; ++i)
        {
            int k = list[i];
            A3Vector start = A3Vector.fromRhoPhiZ(rho[0][k], phi[0][k],
                z[0][k]);
            A3Vector stop = A3Vector.fromRhoPhiZ(rho[1][k], phi[1][k],
                z[1][k]);
            A3Vector v = (stop.subtract(start)).normalize();
            double p = pT[k] / Math.sqrt(1. - v.z * v.z);
            sum.add(new A4Vector(v.scale(p), 0.));
        }
        return sum;
    }
}
