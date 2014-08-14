package atlantis.data;

import atlantis.event.*;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.graphics.AGraphics;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;

/**
 * Base class for EmTauROI, JetROI and MuonROI
 *
 * @author Gary Taylor, Zdenek Maxa, Qiang Lu
 */

abstract public class AROIData extends AData
{
    protected float[] eta;
    protected float[] phi;
    protected float[] energy;
    // energy variables for EmTauROIs
    protected float[] energyEM;
    protected float[] energyTAU;

    private int drawOrFill = AGraphics.FILL;

    AROIData(AHashMap p, AEvent e)
    {
        super(p,e);
        eta = p.getFloatArray("eta");
        phi = p.getFloatArray("phi");
        energy = p.getFloatArray("energy");
        if(this instanceof AEmTauROIData)
        {
        	energyEM = p.getFloatArray("energyEM");
        	energyTAU = p.getFloatArray("energyTAU");
        }
    }

    public float getEnergy(int index)
    {
        return energy[index];
    }
    public float getEnergyEM(int index)
    {
    	return energyEM[index];
    }
    public float getEnergyTAU(int index)
    {
    	return energyTAU[index];
    }

    public float getEta(int index)
    {
        return eta[index];
    }

    public float getPhi(int index)
    {
        return phi[index];
    }

    public float[] getEnergy()
    {
        return energy;
    }
    
    public float[] getEnergyEM()
    {
        return energyEM;
    }
    
    public float[] getEnergyTAU()
    {
        return energyTAU;
    }

    public float[] getEta()
    {
        return eta;
    }

    public float[] getPhi()
    {
        return phi;
    }

    protected int getDrawOrFill()
    {
        return drawOrFill;
    }

    public String getHitInfo(int index)
    {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0) return getNameScreenName()+" index: " + index+
        "\n Energy="+String.format("%.3f",energy[index])+" GeV\n "+
        AMath.ETA+" = "+String.format("%.3f",eta[index])+"\n "+
        AMath.PHI+" = "+String.format("%.3f",Math.toDegrees(phi[index]))+AMath.DEGREES;
        
        double deta = parameterStore.get(PARAMETER_GROUP, "deta").getD();
        double dphi = parameterStore.get(PARAMETER_GROUP, "dphi").getD();

        StringBuffer msg = new StringBuffer(getNameScreenName());
        msg.append(" (id: " + id[index] + " index: " + index + ")");
        msg.append("\n energy = ");
        msg.append(String.format("%.3f",energy[index]));
        msg.append(" GeV\n ");
        if(this instanceof AEmTauROIData)
        {
        	if(energyEM != null)
        	{
        	 msg.append("energyEM = ");
             msg.append(String.format("%.3f",energyEM[index]));
             msg.append(" GeV\n ");
        	}
        	if(energyTAU != null)
        	{
             msg.append("energyTAU = ");
             msg.append(String.format("%.3f",energyTAU[index]));
             msg.append(" GeV\n ");
        	}
        }
        msg.append(AMath.ETA);
        msg.append(" = ");
        msg.append(String.format("%.3f",eta[index]));
        msg.append("\n ");
        msg.append(AMath.PHI);
        msg.append(" = ");
        msg.append(String.format("%.3f",Math.toDegrees(phi[index])));
        msg.append(AMath.DEGREES);
        msg.append("  (" + String.format("%.3f",phi[index]) + " rad)");
        msg.append("\n deta = ");
        msg.append(String.format("%.3f",deta));
        msg.append("\n dphi = ");
        msg.append(String.format("%.3f",Math.toDegrees(dphi)));
        msg.append(AMath.DEGREES);
        msg.append("  (" + String.format("%.3f",dphi) + " rad)");

        return msg.toString();
    }

    protected int internalColor()
    {
        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();

        if (colorFunction == 0)
        {
            colorByConstant();
        }
        else if (colorFunction == 1)
        {
            colorByIndex();
        }

        return 1;
    }

    protected void applyCuts()
    {
        cutIndex();
        cutPhi(phi);
        cutEta(eta);
    }

    protected ACoord getVPUser()
    {
        drawOrFill = AGraphics.DRAW;

        double deta = parameterStore.get(PARAMETER_GROUP, "deta").getD() / 2;
        double dphiDeg = Math.toDegrees(parameterStore.get(PARAMETER_GROUP, "dphi").getD()) / 2;
        double phiDeg = 0;

        makeDrawList();
        double[][][] hv = new double[2][numDraw][];
        int[] index = new int[numDraw];

        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            phiDeg = Math.toDegrees(phi[list]);

            // without the cross in the middle
            // hv[0][i] = AMath.xBox(eta[list], deta); // x-dx,x-dx,x+dx,x+dx
            // hv[1][i] = AMath.yBox(phiDeg, dphiDeg); // y-dy,y+dy,y+dy,y-dy

            hv[0][i] = new double[] { eta[list] - deta, eta[list] - deta,
                    eta[list] + deta, eta[list] - deta, eta[list] + deta,
                    eta[list] - deta, eta[list] + deta, eta[list] + deta };
            hv[1][i] = new double[] { phiDeg - dphiDeg, phiDeg + dphiDeg,
                    phiDeg - dphiDeg, phiDeg - dphiDeg, phiDeg + dphiDeg,
                    phiDeg + dphiDeg, phiDeg - dphiDeg, phiDeg + dphiDeg };
            index[i] = list;
        }

        return new ACoord(hv, index, this).includePhiWrapAround("VP");
    }

    protected ACoord getYXUser()
    {
        drawOrFill = AGraphics.FILL;
        double dphi = parameterStore.get(PARAMETER_GROUP, "dphi").getD() / 2;

        makeDrawList();
        double[][][] hv = new double[2][numDraw][4];
        int[] index = new int[numDraw];

        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            double phiPlus = phi[list] + dphi;
            double phiMinus = phi[list] - dphi;
            double cosPlus = Math.cos(phiPlus);
            double sinPlus = Math.sin(phiPlus);
            double cosMinus = Math.cos(phiMinus);
            double sinMinus = Math.sin(phiMinus);

            double rhoPlus = 1150;
            double rhoMinus = 1130;

            // 4 corners of the cell area
            // x0, y0
            hv[0][i][0] = rhoMinus * cosPlus;
            hv[1][i][0] = rhoMinus * sinPlus;
            // x1, y1
            hv[0][i][1] = rhoPlus * cosPlus;
            hv[1][i][1] = rhoPlus * sinPlus;
            // x2, y2
            hv[0][i][2] = rhoPlus * cosMinus;
            hv[1][i][2] = rhoPlus * sinMinus;
            // x3, y3
            hv[0][i][3] = rhoMinus * cosMinus;
            hv[1][i][3] = rhoMinus * sinMinus;

            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.POLYGONS);
    }

    protected ACoord getFRUser()
    {
        ACoord coordFR = getYXUser().convertYXToFR().includePhiWrapAround("FR");
        return coordFR;
    }

    protected ACoord getRZUser()
    {
        drawOrFill = AGraphics.FILL;
        double deta = parameterStore.get(PARAMETER_GROUP, "deta").getD() / 2;

        makeDrawList();
        double[][][] hv = new double[2][numDraw][4];
        int[] index = new int[numDraw];

        for (int i = 0; i < numDraw; i++)
        {
            int list = listdl[i];
            double phiMid = Math.toRadians(parameterStore.get("RZ", "Phi").getD());
            double phiDiff = Math.abs(phi[list] - phiMid);
            double rSign;
            if (phiDiff > Math.PI / 2. && phiDiff <= 3 * Math.PI / 2.)
                rSign = -1;
            else
                rSign = 1;

            // calculate theta based on the eta value
            double theta = Math.atan(Math.exp(-Math.abs(eta[list]))) * 2.0;
            double thetaPlus = Math.atan(Math.exp(-(Math.abs(eta[list]) - deta))) * 2.0;
            double thetaMinus = Math.atan(Math.exp(-(Math.abs(eta[list]) + deta))) * 2.0;

            if ((eta[list] > 0.) && (rSign == -1))
            {
                theta = 2 * Math.PI - theta;
                thetaPlus = 2 * Math.PI - thetaPlus;
                thetaMinus = 2 * Math.PI - thetaMinus;
            }
            else if ((eta[list] < 0.) && (rSign == -1))
            {
                theta += Math.PI;
                thetaPlus += Math.PI;
                thetaMinus += Math.PI;
            }
            else if ((eta[list] < 0.) && (rSign == 1))
            {
                theta = Math.PI - theta;
                thetaPlus = Math.PI - thetaPlus;
                thetaMinus = Math.PI - thetaMinus;
            }

            double cosPlus = Math.cos(thetaPlus);
            double sinPlus = Math.sin(thetaPlus);
            double cosMinus = Math.cos(thetaMinus);
            double sinMinus = Math.sin(thetaMinus);

            // decide the region based on the theta value
            final byte TOP_BOTTOM = 0;
            final byte LEFT_RIGHT = 1;
            byte region = TOP_BOTTOM;
            // hard-coded value is based on the values in Geometry
            if (Math.abs(Math.tan(theta)) < 0.8)
                region = LEFT_RIGHT;

            double radiusMinus = 0.;
            switch (region)
            {
                case TOP_BOTTOM:
                    radiusMinus = 1330 / Math.abs(Math.sin(theta));
                    break;
                case LEFT_RIGHT:
                    radiusMinus = 1630 / Math.abs(Math.cos(theta));
                    break;
            }
            double radiusPlus = radiusMinus + 20;

            // 4 corners of the cell area
            // x0, y0
            hv[0][i][0] = radiusMinus * cosPlus;
            hv[1][i][0] = radiusMinus * sinPlus;
            // x1, y1
            hv[0][i][1] = radiusPlus * cosPlus;
            hv[1][i][1] = radiusPlus * sinPlus;
            // x2, y2
            hv[0][i][2] = radiusPlus * cosMinus;
            hv[1][i][2] = radiusPlus * sinMinus;
            // x3, y3
            hv[0][i][3] = radiusMinus * cosMinus;
            hv[1][i][3] = radiusMinus * sinMinus;

            index[i] = list;
        }
        return new ACoord(hv, index, this, ACoord.POLYGONS);
    }

}
