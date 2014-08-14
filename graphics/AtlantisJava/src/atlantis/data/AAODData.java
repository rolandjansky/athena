package atlantis.data;

import atlantis.event.AData;
import atlantis.event.*;
import java.util.Vector;

import atlantis.canvas.AWindow;
import atlantis.graphics.ACoord;
import atlantis.graphics.AGraphics;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionVP;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;

public abstract class AAODData extends AData
{
    protected float[] eta;
    protected float[] phi;
    protected float[] pT;

    AAODData(AHashMap p, AEvent e)
    {
        super(p,e);
        eta = p.getFloatArray("eta");
        phi = p.getFloatArray("phi");
        pT = p.getFloatArray("pt");
        int[] pdgId = p.getUnsureIntArray("pdgId");
        if(pdgId != null)
        {
            for(int i=0; i<pT.length; i++)
                pT[i] *= -AMath.getSign(pdgId[i]);
        }

        if (p.get("clusterKey") != null)
        {
            int[] clusterLinkCount = p.getUnsureIntArray("clusterLinkCount");
            event.getAssociationManager().add(new AObjectsAssociation(getFullName(), "Cluster", 
                    p.getStringArray("clusterKey"), p.getIntArray("clusterIndex"), clusterLinkCount, event));
        }
        if (p.get("trackKey") != null)
        {
            int[] trackLinkCount = p.getUnsureIntArray("trackLinkCount");
            event.getAssociationManager().add(new AObjectsAssociation(getFullName(), "Track",
                    p.getStringArray("trackKey"), p.getIntArray("trackIndex"), trackLinkCount, event));
        }
    }


    public float getEta(int index)
    {
        return eta[index];
    }


    public float getPhi(int index)
    {
        return phi[index];
    }


    public float getPT(int index)
    {
        return pT[index];
    }


    public float[] getEta()
    {
        return eta;
    }


    public float[] getPhi()
    {
        return phi;
    }


    public float[] getPT()
    {
        return pT;
    }


    protected int internalColor()
    {
        int colorFunction=parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();

        if(colorFunction==0)
            colorByConstant();
        else if(colorFunction==1)
            colorByIndex();

        return 1;
    }

    public String getNameScreenName()
    {
        return getParameterGroup();
    }

    protected int getDrawOrFill()
    {
        return AGraphics.FILL;
    }

    protected void applyCuts()
    {
        cutIndex();
        cutPhi(phi);
        cutEta(eta);
    }

    // info on AOD data contained in V-Plot rubberband selection
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
            sumP += Math.abs(pT[listdl[i]] / Math.cos(AMath.lambda(eta[listdl[i]])));
        }

        StringBuffer msg = new StringBuffer("");
        msg.append(numDraw);
        msg.append(" ");
        msg.append(getFullName());
        msg.append("  sum(PT) = ");
        msg.append(String.format("%.1f", sumPt));
        msg.append("  sum(P) = ");
        msg.append(String.format("%.1f",sumP));

        return msg.toString();
    }

    public void draw(AWindow window, AGraphics ag, AProjection2D projection)
    {
        if(projection instanceof AProjectionVP)
        {
            drawVP(window, ag, projection);
        }
        else
        {
            super.draw(window, ag, projection);
        }
    }

    protected ACoord getYXUser()
    {
        makeDrawList();
        double[][][] hv = new double[2][numDraw][4];
        int[] index = new int[numDraw];

        double dphi = Math.toRadians(0.5);
        for(int i=0; i<numDraw; i++)
        {
            int list = listdl[i];
            double phiPlus = phi[list] + dphi;
            double phiMinus = phi[list] - dphi;
            double cosPlus = Math.cos(phiPlus);
            double sinPlus = Math.sin(phiPlus);
            double cosMinus = Math.cos(phiMinus);
            double sinMinus = Math.sin(phiMinus);
            //same rhoMax/Minus values used in AJetData
            double rhoMax = 1200;
            double rhoMinus = 400;
            //if no composite particles increase AOD size
            Vector<String> keys = event.getCollections().get("CompositeParticle");
            if(keys == null || !parameterStore.get("Data", "CompositeParticle").getStatus())
                rhoMax=1200;
            else if(PARAMETER_GROUP.equals("CompositeParticle"))
            {
                // display composite particle outside of normal AOD data
                rhoMax=1200;
                rhoMinus=800;
            }

            double maxEnergy = parameterStore.get("Projection", "EnergyMax").getD();
            double rhoPlus = rhoMax;
            double s = 10;
            if(Math.abs(pT[list]) < maxEnergy)
                rhoPlus = rhoMinus + (rhoMax - rhoMinus) * s *Math.abs(pT[list]) / maxEnergy;

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
        makeDrawList();
        double[][][] hv = new double[2][numDraw][4];
        int[] index = new int[numDraw];

        double dtheta = Math.toRadians(0.5);
        for(int i=0; i<numDraw; i++)
        {
            int list = listdl[i];
            double phiMid = Math.toRadians(parameterStore.get("RZ", "Phi").getD());
            double phiDiff = Math.abs(phi[list]-phiMid);
            double rSign;
            if (phiDiff > Math.PI/2. && phiDiff <= 3*Math.PI/2.)
                rSign = -1;
            else
                rSign = 1;

            // calculate theta based on the eta value
            double theta = Math.atan(Math.exp(-Math.abs(eta[list]))) * 2.0;
            if ((eta[list] > 0.) && (rSign == -1))
                theta = 2 * Math.PI - theta;
            else if ((eta[list] < 0.) && (rSign == -1))
                theta += Math.PI;
            else if ((eta[list] < 0.) && (rSign == 1))
                theta = Math.PI - theta;

            double thetaPlus = theta + dtheta;
            double thetaMinus = theta - dtheta;
            double cosPlus = Math.cos(thetaPlus);
            double sinPlus = Math.sin(thetaPlus);
            double cosMinus = Math.cos(thetaMinus);
            double sinMinus = Math.sin(thetaMinus);

            // decide the region based on the theta value
            final byte TOP_BOTTOM = 0;
            final byte LEFT_RIGHT = 1;
            byte region = TOP_BOTTOM;
            // hard-coded value is based on the values in Geometry
            if(Math.abs(Math.tan(theta)) < 0.8)
                region = LEFT_RIGHT;

            double radiusMinus = 0.;
            switch(region)
            {
                case TOP_BOTTOM:
                    if(PARAMETER_GROUP.equals("CompositeParticle"))
                        radiusMinus = 750 / Math.abs(Math.sin(theta));
                    else
                        radiusMinus = 550 / Math.abs(Math.sin(theta));
                    break;
                case LEFT_RIGHT:
                    if(PARAMETER_GROUP.equals("CompositeParticle"))
                        radiusMinus = 850 / Math.abs(Math.cos(theta));
                    else
                        radiusMinus = 700 / Math.abs(Math.cos(theta));
                    break;
            }
            //same rhoMax/Minus values used in AJetData
            double radiusMax = radiusMinus + 500;
            //if no composite particles increase AOD size
            Vector keys = (Vector) event.getCollections().get("CompositeParticle");
            if(keys == null || !parameterStore.get("Data", "CompositeParticle").getStatus())
                radiusMax+=200;
            double maxEnergy = parameterStore.get("Projection", "EnergyMax").getD();
            double radiusPlus = radiusMax;
            double s = 10;
            if(Math.abs(pT[list]) < maxEnergy)
                radiusPlus = radiusMinus + (radiusMax - radiusMinus) * s * Math.abs(pT[list]) / maxEnergy;

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

    protected ACoord getVPUser()
    {
        makeDrawList();
        double[] h = new double[numDraw];
        double[] v = new double[numDraw];
        int[] index=new int[numDraw];

        for(int i=0; i<numDraw; i++)
        {
            int list=listdl[i];
            h[i]=eta[list];
            v[i]=Math.toDegrees(phi[list]);
            index[i]=list;
        }
        return new ACoord(h, v, index, this).includePhiWrapAround("VP");
    }

    private void drawVP(AWindow window, AGraphics ag, AProjection2D projection)
    {
        ACoord centers = window.calculateDisplay(getUser(projection));
        int[] drawlist = centers.index;
        double eLimit = 0.025;
        int numPoints = 25;
        if(PARAMETER_GROUP.equals("CompositeParticle"))
            numPoints = 5;
        double[][][] hv = new double[2][drawlist.length][numPoints];  //circle
        // a cross at centre
        double[][][] hv_cross1 = new double[2][drawlist.length][2];
        double[][][] hv_cross2 = new double[2][drawlist.length][2];
        int crossLength = 6;

        for(int i=0; i<drawlist.length; ++i)
        {
            int list = drawlist[i];
            double e;
            e = Math.abs(pT[list]);
            int d = (int)(Math.sqrt((e/eLimit)/Math.PI));
            if(d == 0) d = 1;
            for(int j=0; j<numPoints; j++)
            {
                hv[0][i][j]=centers.hv[0][0][i]+d*Math.cos(Math.PI*2*j/(numPoints-1));
                hv[1][i][j]=centers.hv[1][0][i]+d*Math.sin(Math.PI*2*j/(numPoints-1));
            }
            for(int j=0; j<2; j++)
            {
                hv_cross1[0][i][j]=centers.hv[0][0][i]+crossLength/2*Math.cos(Math.PI*(1+4*j)/4);
                hv_cross1[1][i][j]=centers.hv[1][0][i]+crossLength/2*Math.sin(Math.PI*(1+4*j)/4);
                hv_cross2[0][i][j]=centers.hv[0][0][i]+crossLength/2*Math.cos(Math.PI*(3+4*j)/4);
                hv_cross2[1][i][j]=centers.hv[1][0][i]+crossLength/2*Math.sin(Math.PI*(3+4*j)/4);
            }
        }
        ag.draw(new ACoord(hv, drawlist, this, ACoord.POLYLINES));
        ag.draw(new ACoord(hv_cross1, drawlist, this, ACoord.POLYLINES));
        ag.draw(new ACoord(hv_cross2, drawlist, this, ACoord.POLYLINES));
    }

}
