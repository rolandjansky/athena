package atlantis.data;

import atlantis.canvas.AWindow;
import atlantis.event.AAssociation;
import atlantis.event.AData;
import atlantis.event.AEvent;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.graphics.AGraphics;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionVP;
import atlantis.utils.A3Vector;
import atlantis.utils.A4Vector;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;

/**
 * Calo Clusters for the moment made from LAr cells
 *
 * Also contain AOD Calo Clusters from 24/10/2006
 */
public class AClusterData extends AData
{
    protected float[] eT;
    protected float[] eta;
    protected float[] phi;

    public String getParameterGroup()
    {
        return "Cluster";
    }

  @Override
    public String getName()
    {
        return "Cluster";
    }

    public String getNameScreenName()
    {
        return "Cluster";
    }

    public float getET(int index)
    {
        return eT[index];
    }

    public float getEta(int index)
    {
        return eta[index];
    }

    public float getPhi(int index)
    {
        return phi[index];
    }

    AClusterData(AHashMap p, AEvent e)
    {
        super(p,e);

        eta = p.getFloatArray("eta");
        phi = p.getFloatArray("phi");
        if (p.get("et") != null)
            eT = p.getFloatArray("et");
        else
        {
            float[] energy = p.getFloatArray("energy");
            eT = new float[numData];
            for (int i = 0; i < numData; ++i)
                eT[i] = (float) Math.abs(energy[i] * Math.cos(AMath.lambda(eta[i])));
        }

        String assocKey = getName() + getStoreGateKey();
        if (p.get("numCells") != null)
        {
            event.getAssociationManager().add(new AAssociation(assocKey, "LAr", p.getIntArray("numCells"), p.getIntArray("cells"),event));
            event.getAssociationManager().add(new AAssociation(assocKey, "TILE", p.getIntArray("numCells"), p.getIntArray("cells"),event));
        }
    }

    protected int getDrawOrFill()
    {
        return AGraphics.FILL;
    }

    protected void applyCuts()
    {
        cutIndex();
        cut("CutsCalo", "ClusterET", " |ET|", eT);
        cutPhi(phi);
        cutEta(eta);
    }
    
    protected int internalColor()
    {
        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();

        switch (colorFunction)
        {
            case 0: // by constant
                colorByConstant();
                break;
            case 1: // by index
                colorByIndex();
                break;
            case 2: // by objects
                colorByObjects();
                break;
        }
        return 3;
    }

    public String getHitInfo(int index)
    {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0){
    		String output = getNameScreenName()+" index: " + index;
    		if(simpleOutput==1 || simpleOutput==3) 
    			output+= "\n ET="+String.format("%.3f",eT[index])+" GeV\n "+
    			AMath.ETA+" = "+String.format("%.3f",eta[index])+"\n "+
    			AMath.PHI+" = "+String.format("%.3f",Math.toDegrees(phi[index]))+AMath.DEGREES;
        	if(simpleOutput==2 || simpleOutput==3) 
        		output+= "\n Ex="+String.format("%.3f",eT[index]*Math.cos(phi[index]))+" GeV "+
        		"\n Ey="+String.format("%.3f",eT[index]*Math.sin(phi[index]))+" GeV "+
        		"\n Ez="+String.format("%.3f",eT[index]*Math.sinh(eta[index]))+" GeV ";
        	return output;
    	}
    	
        String k = this.getStoreGateKey();
        String sgKey = k != null ? k : "n/a";
        StringBuffer msg = new StringBuffer(getNameScreenName());
        msg.append(" (id: " + id[index] + " index: " + index + ")");
        msg.append("\n storegate key: ");
        msg.append(sgKey);
        msg.append("\n ET = ");
        msg.append(String.format("%.3f",eT[index]));
        msg.append(" GeV\n E = ");
        msg.append(String.format("%.3f",Math.abs(eT[index]/Math.cos(AMath.lambda(eta[index])))));
        msg.append(" GeV\n ");
        msg.append(AMath.ETA);
        msg.append(" = ");
        msg.append(String.format("%.3f",eta[index]));
        msg.append("\n ");
        msg.append(AMath.PHI);
        msg.append(" = ");
        msg.append(String.format("%.3f",Math.toDegrees(phi[index])) + AMath.DEGREES);
        msg.append("  (" + String.format("%.3f",phi[index]) + " rad)");

        return msg.toString();
    }

    public String getVPHitInfo()
    {
        makeDrawList();
        if (numDraw == 0)
            return "";
        double sumE = 0.;
        double sumET = 0.;

        for (int i = 0; i < numDraw; ++i)
        {
            sumET += Math.abs(eT[listdl[i]]);
            sumE += Math.abs(eT[listdl[i]] / Math.cos(AMath.lambda(eta[listdl[i]])));
        }

        StringBuffer msg = new StringBuffer("");
        msg.append(numDraw);
        msg.append(" ");
        msg.append(getStoreGateKey() != null ? getStoreGateKey() : getNameScreenName());
        msg.append("  sum(ET) = ");
        msg.append(String.format("%.1f",sumET));
        msg.append("  sum(E) = ");
        msg.append(String.format("%.1f",sumE));

        return msg.toString();
    }

    public A4Vector get4Vector(int num, int[] list)
    {
        A4Vector sum = new A4Vector();
        for (int i = 0; i < num; ++i)
        {
            int k = list[i];
            A3Vector start = A3Vector.fromRhoPhiZ(0., 0., 0.);
            double tL = AMath.tanLambda(eta[k]);
            A3Vector stop = A3Vector.fromRhoPhiZ(1., phi[k], tL);
            A3Vector v = (stop.subtract(start)).normalize();
            double p = eT[k] / Math.sqrt(1. - v.z * v.z);
            sum.add(new A4Vector(v.scale(p), 0.));
        }

        return sum;
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

    private void drawVP(AWindow window, AGraphics ag, AProjection2D projection)
    {
        ACoord centers=window.calculateDisplay(getUser(projection));
        int[] drawlist=centers.index;
        double eLimit=0.025;
        int numPoints=25;
        double[][][] hv=new double[2][drawlist.length][numPoints];  //circle

        for(int i=0; i<drawlist.length; ++i)
        {
            int list=drawlist[i];
            double e;
            e = Math.abs(eT[list]);
            int d=(int)(Math.sqrt((e/eLimit)/Math.PI));
            if(d==0) d=1;
            for(int j=0; j<numPoints; j++)
            {
                hv[0][i][j]=centers.hv[0][0][i]+d*Math.cos(Math.PI*2*j/(numPoints-1));
                hv[1][i][j]=centers.hv[1][0][i]+d*Math.sin(Math.PI*2*j/(numPoints-1));
            }
        }
        ag.draw(new ACoord(hv, drawlist, this, ACoord.POLYLINES));
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

            // Rho range of LAr Detector (EM) is about between [148.175, 198.47]
            // Rho range of TILE Detector (Had) is about between [229, 386]
            double rhoMax = 380;
            double rhoMinus = 155;

            double maxEnergy = parameterStore.get("Projection", "EnergyMax").getD();
            double rhoPlus = rhoMax;
            if(Math.abs(eT[list]) < maxEnergy)
                rhoPlus = rhoMinus + (rhoMax - rhoMinus) * Math.abs(eT[list]) / maxEnergy;

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
        return getYXUser().convertYXToFR().includePhiWrapAround("FR");
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
            final byte LAR = 0;
            final byte LAR_ENDCAP = 1;
            final byte FCAL_EM = 2;
            byte region = LAR;
            // hard-coded value is based on the values in AGeometry.xml
            if(Math.abs(Math.tan(theta)) >= 0.0778 && Math.abs(Math.tan(theta)) < 0.4828)
                region = LAR_ENDCAP;
            else if(Math.abs(Math.tan(theta)) < 0.0778)
                region = FCAL_EM;

            double radiusMinus = 0.;
            switch(region)
            {
                // use fixed rho/z to determine the lower radius value
                case LAR:
                    radiusMinus = 155 / Math.abs(Math.sin(theta));
                    break;
                case LAR_ENDCAP:
                    radiusMinus = 380 / Math.abs(Math.cos(theta));
                    break;
                case FCAL_EM:
                    radiusMinus = 470 / Math.abs(Math.cos(theta));
                    break;
            }
            double radiusMax = radiusMinus + 200;
            double maxEnergy = parameterStore.get("Projection", "EnergyMax").getD();
            double radiusPlus = radiusMax;
            if(Math.abs(eT[list]) < maxEnergy)
                radiusPlus = radiusMinus + (radiusMax - radiusMinus) * Math.abs(eT[list]) / maxEnergy;

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
