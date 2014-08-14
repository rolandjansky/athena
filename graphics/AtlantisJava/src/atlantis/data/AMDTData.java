package atlantis.data;

import atlantis.event.*;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AHashMap;
import atlantis.utils.AIdHelper;
import atlantis.utils.ALogger;
import atlantis.utils.AMath;

public class AMDTData extends AMuonHitData
{
    protected float[] driftR;
    protected float[] length;
    private static final int BARREL = 1;

    private static ALogger logger = ALogger.getLogger(AMDTData.class);

    public String getParameterGroup()
    {
        return "MDT";
    }

    public String getName()
    {
        return "MDT";
    }

    public String getNameScreenName()
    {
        return "MDT";
    }

    AMDTData(AHashMap p, AEvent e)
    {
        super(p,e);
        driftR = p.getFloatArray("driftR");
        length = p.getFloatArray("length");
        // removed the fudge (CT 27/7/2004), and replaced with another
        // for(int i=0; i<numData; i++)
        // driftR[i]*=500./700.;
        for (int i = 0; i < numData; i++)
            if (driftR[i] > 1.46)
                driftR[i] = (float) 1.46; // don't go out of the drifttube !
    }

    protected int getStation(int index)
    {
        try {
            String stationName = AIdHelper.stationName(id[index]);
            
            if (stationName.charAt(1) == 'I') {
                return 0;
            } else if (stationName.charAt(1) == 'E') {
                return 1;
            } else if (stationName.charAt(1) == 'M') {
                return 2;
            } else if (stationName.charAt(1) == 'O') {
                return 3;
            }
        } catch (AAtlantisException e) {
            logger.error("Problem decoding MDT identifier", e);
        }
        
        return 0;
    }

    protected int getSub(int index)
    {
        try {
            if (AIdHelper.stationName(id[index]).charAt(0) == 'B') {
                return 1;
            } else if (AIdHelper.stationEta(id[index]) < 0) {
                return 0;
            } else {
                return 2;
            }
        } catch (AAtlantisException e) {
            logger.error("Problem decoding MDT identifier", e);
        }
        
        return 1;
    }

    public int getSector(int index)
    {
        try {
            String stationName = AIdHelper.stationName(id[index]);
            
            if (stationName.charAt(2) == 'L') {
                return 2 * (AIdHelper.stationPhi(id[index]) - 1);
            } else {
                return 2 * (AIdHelper.stationPhi(id[index]) - 1) + 1;
            }
        } catch (AAtlantisException e) {
            logger.error("Problem decoding MDT identifier", e);
        }
        
        return 0;
    }

    protected boolean getMeasuresPhi(int index)
    {
        return false;
    }

    protected void applyCuts()
    {
        super.applyCuts();
        if (parameterStore.get("CutsATLAS", "CutPhi").getStatus())
            cutPhi(phi, getDPhi());
        cutEta(rho, z);
    }

    // used to make phi cut

    private float[] getDPhi()
    {
        // only roughly correct for some special sectors
        // must create all
        float[] dphi = new float[numData];

        // need only fill for those in draw list
        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];

            dphi[list] = (float) Math.abs(Math.atan2(length[list] / 2., rho[list]));
        }
        return dphi;
    }

    public String getHitInfo(int index)
    {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0) return getNameScreenName()+" index: " + index+"\n"+
        AMath.RHO+" = "+String.format("%.3f",rho[index])+"\n "+
        "z = " + String.format("%.3f",z[index]) + " cm\n"+
        AMath.PHI+" = "+String.format("%.3f",Math.toDegrees(phi[index]))+AMath.DEGREES;
        
        String temp = getNameScreenName() + " (id: " + identifier[index] +
                      " index: " + index + ")\n " +
                      AMath.RHO + " = " + String.format("%.3f",rho[index]) + " cm" + "\n " +
                      AMath.PHI + " = " + String.format("%.3f",Math.toDegrees(phi[index])) +
                      AMath.DEGREES + "  (" + String.format("%.3f",phi[index]) + " rad)\n " +
                      "z = " + String.format("%.3f",z[index]) + " cm\n " +
                      "driftR = " + String.format("%.3f",driftR[index]) + " cm";
        if (sector[index] != -1)
        {
            temp += "\n sector = " + sector[index];
        }

        return temp;
    }

    private void makeDrawListYX()
    {
        int mode = parameterStore.get("YX", "Mode").getI();

        if (mode > 0 && mode < 5)
        {
            numDraw = 0;
        }
        else
        {
            makeDrawList();
            if (mode == 0)
                cut(" Barrel ", sub, "==", BARREL);
            else
            {
                cut(" Endcaps", sub, "!=", BARREL);
                cut(" Station", station, "==", mode - 5);
            }
        }
    }

    private void makeDrawListFZ()
    {
        int mode = parameterStore.get("FZ", "Mode").getI();

        if (mode > 0 && mode < 4)
        {
            numDraw = 0;
        }
        else
        {
            makeDrawList();
            if (mode == 0)
                cut(" Endcaps", sub, "!=", BARREL);
            else
            {
                cut(" Barrel ", sub, "==", BARREL);
                if (mode == 4)
                    cut(" Station", station, "<=", mode - 3);
                else
                    cut(" Station", station, "==", mode - 3);
            }
        }
    }

    protected ACoord getYXUser()
    {
        return getYXUser(0);
    }

    private ACoord getYXUser(int flag)
    {
        makeDrawListYX();
        int numPieces = 13;
        double[][][] hv = new double[2][numDraw][];
        int[] index = new int[numDraw];

        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            double r = rho[list];
            double cosPhi = Math.cos(phi[list]);
            double sinPhi = Math.sin(phi[list]);
            double phiM = Math.rint(phi[list] / AMath.PI_BY_8) * AMath.PI_BY_8;
            double cosPhiM = Math.cos(phiM);
            double sinPhiM = Math.sin(phiM);
            double d = length[list] / 2.;
            double x = r * cosPhi;
            double y = r * sinPhi;

            hv[0][i] = AMath.splitLineIntoPieces(new double[] { x + d * sinPhiM, x - d * sinPhiM }, numPieces);
            hv[1][i] = AMath.splitLineIntoPieces(new double[] { y - d * cosPhiM, y + d * cosPhiM }, numPieces);
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.POLYLINES);
    }

    protected ACoord getFRUser()
    {
        return getYXUser(1).convertYXToFR().includePhiWrapAround("FR");
    }

    protected ACoord getRZUser()
    {
        makeDrawList();
        int[] sign = new int[numDraw];
        double phiMid = Math.toRadians(parameterStore.get("RZ", "Phi").getD());

        for (int i = 0; i < numDraw; i++)
        {
            double phiDiff = Math.abs(phi[listdl[i]] - phiMid);

            sign[i] = -1;
            if (phiDiff < Math.PI / 2. || phiDiff > 3 * Math.PI / 2.)
                sign[i] = 1;
        }
        return getXZRZUser(sign);
    }

    protected ACoord getXZUser()
    {
        makeDrawList();
        cutMuonSector(sector);
        int[] sign = new int[numDraw];
        int sect=(int)Math.round(parameterStore.get("XZ", "Phi").getD() / 22.5);

        for (int i = 0; i < numDraw; i++)
        {
            sign[i] = 1;
            if (sector[listdl[i]] != sect)
                sign[i] = -1;
        }
        return getXZRZUser(sign);
    }

    protected ACoord getXZRZUser(int[] sign)
    {
        // int muonMode=APar.get("Data.MDT").getI();
        int muonMode = parameterStore.get("MDT", "Mode").getI();
        int numPoints;

        if (muonMode == 0) // circles
            numPoints = 48;
        else
            numPoints = 2; // lines

        double[][][] hv = new double[2][numDraw][numPoints];
        int[] index = new int[numDraw];

        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            double rC = sign[i] * getSectorRho(sector[list], rho[list], phi[list]);
            double zC = z[list];
            double d = driftR[list];

            if (muonMode == 0)
            { // circles
                for (int j = 0; j < numPoints; j++)
                {
                    hv[0][i][j] = zC + d * Math.cos(Math.PI * 2 * j / (numPoints - 1));
                    hv[1][i][j] = rC + d * Math.sin(Math.PI * 2 * j / (numPoints - 1));
                }
            }
            else
            { // lines
                double radius = Math.sqrt(rC * rC + zC * zC);
                double sinTheta = rC / radius;
                double cosTheta = zC / radius;

                hv[0][i][0] = zC + d * sinTheta;
                hv[1][i][0] = rC - d * cosTheta;
                hv[0][i][1] = zC - d * sinTheta;
                hv[1][i][1] = rC + d * cosTheta;
            }
            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.POLYLINES);
    }

    protected ACoord getFZUser()
    {
        makeDrawListFZ();
        double[][][] hv = new double[2][numDraw][2];
        int[] index = new int[numDraw];

        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];

            index[i] = list;
            hv[0][i][0] = z[list];
            hv[0][i][1] = z[list];
            double deltaPhi = Math.atan2(length[list] / 2., rho[list]);

            hv[1][i][0] = Math.toDegrees(phi[list] - deltaPhi);
            hv[1][i][1] = Math.toDegrees(phi[list] + deltaPhi);
        }
        return new ACoord(hv, index, this, ACoord.POLYLINES).includePhiWrapAround("FZ");
    }

}
