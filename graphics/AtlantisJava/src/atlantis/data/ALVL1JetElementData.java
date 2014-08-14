package atlantis.data;

import atlantis.event.*;
import java.awt.Color;

import atlantis.canvas.AWindow;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;
import atlantis.graphics.ADrawParameters;
import atlantis.graphics.AGraphics;
import atlantis.graphics.colormap.AColorMap;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionVP;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;
import atlantis.utils.APolygon;

public class ALVL1JetElementData extends AData
{
    private float[] energy;
    private float[] eta;
    private float[] deta;
    private float[] phi;
    private float[] dphi;

    ALVL1JetElementData(AHashMap p, AEvent e)
    {
        super(p,e);

        phi = p.getFloatArray("phi");
        eta = p.getFloatArray("eta");
        // here energy is actually Et
        energy = p.getFloatArray("energy");

        deta = new float[numData];
        dphi = new float[numData];

        fillGranularity();
    }

    private void fillGranularity()
    {
        for(int i = 0; i < numData; ++i)
        {
            float positiveEta = Math.abs(eta[i]);
            if(positiveEta <= 2.4)
            {
                deta[i] = 0.1f;
                dphi[i] = (float) (Math.PI / 32.);
            }
            else if(positiveEta <= 2.7)
            {
                deta[i] = 0.15f;
                dphi[i] = (float) (Math.PI / 32.);
            }
            else if(positiveEta <= 2.9)
            {
                deta[i] = 0.1f;
                dphi[i] = (float) (Math.PI / 32.);
            }
            else if(positiveEta <= 3.2)
            {
                deta[i] = 0.15f;
                dphi[i] = (float) (Math.PI / 32.);
            }
            else
            {
                deta[i] = 0.9f;
                dphi[i] = (float) (Math.PI / 16.);
            }
        }
    }

    public double getEta(int index)
    {
        return eta[index];
    }

    public double getdEta(int index)
    {
            return deta[index];
    }

    public double getPhi(int index)
    {
        return phi[index];
    }

    public double getdPhi(int index)
    {
        return dphi[index];
    }

    public double getET(int index)
    {
        return energy[index];
    }

    public String getNameScreenName()
    {
        return "LVL1JetElement";
    }

    public String getParameterGroup()
    {
        return "LVL1JetElement";
    }

    public String getHitInfo(int index)
    {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0) return getNameScreenName()+" index: " + index+
        "\n E="+String.format("%.3f",energy[index])+" GeV\n "+
        AMath.ETA+" = "+String.format("%.3f",eta[index])+"\n "+
        AMath.PHI+" = "+String.format("%.3f",Math.toDegrees(phi[index]))+AMath.DEGREES;
        
        StringBuffer msg = new StringBuffer(getNameScreenName());
        msg.append(" (id: " + id[index] + " index: " + index + ")");
        msg.append("\n Energy = ");
        msg.append(String.format("%.3f",energy[index]));
        msg.append(" GeV\n ");
        msg.append(AMath.ETA);
        msg.append(" = ");
        msg.append(String.format("%.3f",eta[index]));
        msg.append("\n ");
        msg.append(AMath.PHI);
        msg.append(" = ");
        msg.append(String.format("%.3f",phi[index]));
        msg.append(AMath.DEGREES);
        msg.append("  (" + String.format("%.3f",phi[index]) + " rad)");

        return msg.toString();
    }

    protected void applyCuts()
    {
        cutIndex();
        cut("CutsCalo", "LVL1TriggerET", " ET", energy);
        cutPhi(phi, dphi);
        cutEtaDEta(eta, deta);
    }

    protected int internalColor()
    {
        int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();

        if(colorFunction == 0)
            colorByConstant();

        return 1;
    }

    protected int getDrawOrFill()
    {
        return AGraphics.FILL;
    }

    protected ACoord getYXUser()
    {
        makeDrawList();

        double[][][] hv = new double[2][numDraw][4];
        int[] index = new int[numDraw];

        for(int i=0; i<numDraw; i++)
        {
            int list = listdl[i];
            double phiPlus = phi[list] + dphi[list];
            double phiMinus = phi[list] - dphi[list];
            double cosPlus = Math.cos(phiPlus);
            double sinPlus = Math.sin(phiPlus);
            double cosMinus = Math.cos(phiMinus);
            double sinMinus = Math.sin(phiMinus);

            // Rho range of Calo Detector is about between [148.175, 386]
            double rhoMax = 380;
            double rhoMinus = 155;

            double maxEnergy = parameterStore.get("Projection", "EnergyMax").getD();
            double rhoPlus = rhoMax;
            if(Math.abs(energy[list]) < maxEnergy)
                rhoPlus = rhoMinus + (rhoMax - rhoMinus) * Math.abs(energy[list]) / maxEnergy;

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
            double thetaPlus = Math.atan(Math.exp(-(Math.abs(eta[list]) - deta[list]))) * 2.0;
            double thetaMinus = Math.atan(Math.exp(-(Math.abs(eta[list]) + deta[list]))) * 2.0;

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
            if(Math.abs(energy[list]) < maxEnergy)
                radiusPlus = radiusMinus + (radiusMax - radiusMinus) * Math.abs(energy[list]) / maxEnergy;

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

    public void draw(AWindow window, AGraphics ag, AProjection2D projection)
    {
        if(projection instanceof AProjectionVP)
        {
            ACoord centers=getUser(projection);

            int[] drawlist=centers.index;
            double[] density = new double[drawlist.length];
            double maxDensity = 0.0;
            double[][][] hv=new double[2][drawlist.length][4];  // cell
            int[] index = new int[drawlist.length];
            ACoord geoCell;

            for(int i=0; i<drawlist.length; ++i)
            {
                int list=drawlist[i];
                // y coordinate of four cell corners
                hv[1][i][0] = centers.hv[1][0][i] + Math.toDegrees(getdPhi(list));
                hv[1][i][1] = centers.hv[1][0][i] - Math.toDegrees(getdPhi(list));
                hv[1][i][2] = centers.hv[1][0][i] - Math.toDegrees(getdPhi(list));
                hv[1][i][3] = centers.hv[1][0][i] + Math.toDegrees(getdPhi(list));
                // x coordinate of four cell corners
                hv[0][i][0] = centers.hv[0][0][i] - getdEta(list);
                hv[0][i][1] = centers.hv[0][0][i] - getdEta(list);
                hv[0][i][2] = centers.hv[0][0][i] + getdEta(list);
                hv[0][i][3] = centers.hv[0][0][i] + getdEta(list);

                index[i] = list;

                // calculate density for all cells, and find min and max density
                density[i] = Math.abs(energy[list]) / (getdPhi(list) * getdEta(list));
                if(i == 0)
                    maxDensity = density[i];
                else
                {
                    if (density[i] > maxDensity)
                        maxDensity = density[i];
                }
            }

            geoCell = window.calculateDisplay(new ACoord(hv, index, this, ACoord.POLYGONS));
            drawGeometry(geoCell, window, ag);
            for (int i = 0; i < density.length; i++)
            {
                double factor = Math.sqrt(density[i] / maxDensity);
                APolygon.scale(geoCell.hv[0][i], geoCell.hv[1][i], factor);
            }

            drawHits(geoCell, window, ag);
        }
        else
        {
            super.draw(window, ag, projection);
        }
    }

    private void drawGeometry(ACoord display, AWindow window, AGraphics ag)
    {
        Color[] colorMap = AColorMap.getColors();
        boolean drawCellGeometry = parameterStore.get(getParameterGroup(), "CellGeometry").getStatus();
        boolean drawCellOutline = parameterStore.get(getParameterGroup(), "CellOutline").getStatus();
        int cellGeometryColor = parameterStore.get(getParameterGroup(), "CellGeometry").getI();
        int cellOutlineColor = parameterStore.get(getParameterGroup(), "CellOutline").getI();
        boolean drawFrame = parameterStore.get(getParameterGroup(), "Frame").getStatus();
        int frameColor = parameterStore.get(getParameterGroup(), "Frame").getI();
        int frameWidth = parameterStore.get(getParameterGroup(), "FrameWidth").getI();
        //only draw frames for Grey/BW color maps if is selected to draw frames
        if(drawFrame && AColorMap.drawFrames())
            drawFrame=true;
        else
            drawFrame=false;
        
        if(!drawCellGeometry && !drawCellOutline)
            return;

        // Draw geometry
        for(int i=0; i<display.hv[0].length; ++i)
        {
            if(drawCellGeometry)
            {
                if(drawFrame)
                {
                    ag.updateDrawParameters(new ADrawParameters(true, frameColor, 1, 0, frameWidth, 0));
                    ag.drawPolygon(display.hv[0][i], display.hv[1][i], display.hv[0][i].length);
                }
                ag.updateDrawParameters(new ADrawParameters(true, cellGeometryColor, 1, 0, 0, 0));
                ag.fillPolygon(display.hv[0][i], display.hv[1][i], display.hv[0][i].length);
            }
            if(drawCellOutline)
            {
                ag.setColor(colorMap[cellOutlineColor]);
                ag.drawPolygon(display.hv[0][i], display.hv[1][i], display.hv[0][i].length);
            }
        }
    }

    private void drawHits(ACoord display, AWindow window, AGraphics ag)
    {
        int[] color = this.getColor(display.index);

        boolean drawCellGeometry = parameterStore.get(getParameterGroup(), "CellGeometry").getStatus();
        boolean drawFrame = parameterStore.get(getParameterGroup(), "Frame").getStatus();
        int frameColor = parameterStore.get(getParameterGroup(), "Frame").getI();
        int frameWidth = parameterStore.get(getParameterGroup(), "FrameWidth").getI();

        // draw frame
        if (drawFrame && !drawCellGeometry)
        {
            for (int i = 0; i < display.hv[0].length; i++)
            {
                ag.updateDrawParameters(new ADrawParameters(true, frameColor, 1, 0, frameWidth, 0));
                ag.drawPolygon(display.hv[0][i], display.hv[1][i], display.hv[0][i].length);
            }
        }

        // draw hits
        for(int i=0; i<display.hv[0].length; ++i)
        {
            int lineWidth = parameterStore.get(getParameterGroup(), "LineWidth").getI();
            ag.updateDrawParameters(new ADrawParameters(true, color[i], 1, lineWidth, 0, 0));
            ag.fillPolygon(display.hv[0][i], display.hv[1][i], display.hv[0][i].length);
        }
    }

}
